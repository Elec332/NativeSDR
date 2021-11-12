//
// Created by Elec332 on 12/11/2021.
//

#include <pipeline/datastream.h>
#include <iostream>
#include <set>

template<class T>
class stream_impl : public pipeline::datastream<T> {

public:

    explicit stream_impl(int size) {
        writeBuf = malloc(100 * size);
        readBuf = malloc(100 * size);
    }

    ~stream_impl() {
        free(writeBuf);
        free(readBuf);
    }

    bool write(const std::function<int(T*)>& writer) override {
        if (stopped) {
            return false;
        }
        std::unique_lock<std::mutex> raii(writeMutex);
        {
            std::unique_lock<std::mutex> raii2(readCountMutex); //Keep cycling to avoid SDR overflows
            if (!readDone && readers == 0) {
                readDone = true;
                writeDone = false;
            }
        }
        int len = writer(writeBuf);
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
        writeDone = true;
        {
            std::unique_lock<std::mutex> raii2(readMutex);
            readDone = false;
        }
        readWait.notify_all();
        return true;
    }

    bool read(const std::function<void(T*, int)>& reader) override {
        if (stopped) {
            return false;
        }
        std::unique_lock<std::mutex> raii(readMutex);
        readWait.wait(raii, [&] {
            return writeDone || stopped;
        });
        if (stopped) {
            return false;
        }

        reader(readBuf, writeSize);
        std::unique_lock<std::mutex> raii3(readCountMutex);
        readFinished.insert((size_t) &reader);
        if (readFinished.size() != readers) {
            return true;
        }
        {
            std::unique_lock<std::mutex> raii2(writeMutex);
            writeDone = false;
        }
        readFinished.clear();
        readDone = true;
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
    bool writeDone = false;

    std::mutex readMutex;
    std::condition_variable readWait{};
    bool readDone = true;

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

