//
// Created by Elec332 on 12/11/2021.
//

#include <pipeline/datastream.h>
#include <iostream>
#include <set>
#include <dsp/malloc.h>

template<class T>
class stream_impl : public pipeline::datastream<T> {

public:

    explicit stream_impl(int size) {
        writeBuf = dsp::malloc(pipeline::BUFFER_COUNT * size);
        readBuf = dsp::malloc(pipeline::BUFFER_COUNT * size);
    }

    ~stream_impl() {
        dsp::free(writeBuf);
        dsp::free(readBuf);
    }

    bool write(const std::function<int(T*)>& writer) override {
        if (stopped) {
            return false;
        }
        int len = writer(writeBuf);
        std::unique_lock<std::mutex> raii(writeMutex);
        { //Keep cycling to avoid SDR overflows
            std::unique_lock<std::mutex> raii2(readCountMutex);
            if (!readDone && readers == 0) {
                return true;
            }
        }
        writeWait.wait(raii, [&] {
            return readDone || stopped;
        });
        if (stopped) {
            return false;
        }

        T* temp = readBuf;
        readBuf = writeBuf;
        writeSize = len;
        writeBuf = temp;
        readDone = false;
        {
            std::unique_lock<std::mutex> raii2(readMutex);
            writeDone = true;
        }
        readWait.notify_all();
        return true;
    }

    bool read(const std::function<void(const T*, int)>& reader) override {
        if (stopped) {
            return false;
        }
        {
            std::unique_lock<std::mutex> raii(readMutex);
            readWait.wait(raii, [&] {
                return writeDone || stopped;
            });
            if (stopped) {
                return false;
            }
        }

        reader(readBuf, writeSize);
        {
            std::unique_lock<std::mutex> raii3(readCountMutex);
            readFinished.insert((size_t) &reader);
            if (readFinished.size() != readers) {
                return true;
            }
        }
        std::unique_lock<std::mutex> raii(readMutex);
        {
            std::unique_lock<std::mutex> raii2(writeMutex);
            readDone = true;
        }
        readFinished.clear();
        writeDone = false;
        writeWait.notify_all();
        return true;
    }

    void setReaders(int readerCount) override {
        std::unique_lock<std::mutex> raii(readCountMutex);
        readers = readerCount;
        std::cout << "outputs stream: " << readers << std::endl;
    }

    void start() override {
        stopped = false;
    }

    void stop() override {
        std::unique_lock<std::mutex> raii(readMutex);
        std::unique_lock<std::mutex> raii2(writeMutex);
        stopped = true;
        readWait.notify_all();
        writeWait.notify_all();
    }

private:

    T* writeBuf;
    T* readBuf;
    int writeSize = 0;

    std::mutex writeMutex;
    std::condition_variable writeWait{};
    bool readDone = true;

    std::mutex readMutex;
    std::condition_variable readWait{};
    bool writeDone = false;

    bool stopped = false;

    std::mutex readCountMutex;
    size_t readers = 0;
    std::set<size_t, std::greater<>> readFinished;

};

pipeline::datastream<void>* pipeline::createUnknownStream(int size) {
    return new stream_impl<void>(size);
}

void pipeline::deleteUnknownStream(pipeline::datastream<void>* stream) {
    delete (stream_impl<void>*) stream;
}
