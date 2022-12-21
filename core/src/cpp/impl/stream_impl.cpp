//
// Created by Elec332 on 12/11/2021.
//

#include <nativesdr/pipeline/datastream.h>
#include <iostream>
#include <set>
#include <nativesdr/dsp/malloc.h>
#include <stdexcept>

template<class T>
class stream_impl : public pipeline::datastream<T> {

public:

    explicit stream_impl(int size, size_t count) {
        writeBuf = dsp::malloc(count * size);
        readBuf = dsp::malloc(count * size);
        bufCount = count;
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
        if (len > bufCount) {
            std::cout << ("Stream write too large! ") << readers << std::endl;
            throw std::runtime_error("Stream write too large!");
        }
        {
            std::unique_lock<std::mutex> raii(writeMutex);
            { //Keep cycling to avoid SDR overflows
                std::lock_guard<std::mutex> raii2(readCountMutex);
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
        }
        {
            std::lock_guard<std::mutex> raii2(readMutex);
            writeDone = true;
        }
        readWait.notify_all();
        return true;
    }

    bool read(const std::function<void(const T*, int)>& reader) override {
        if (stopped) {
            return false;
        }
        auto rVal = (size_t) &reader;
        {
            std::unique_lock<std::mutex> raii(readMutex);
            readWait.wait(raii, [&] {
                if (stopped) {
                    return true;
                }
                std::lock_guard<std::mutex> raii3(readCountMutex);
                if (readFinished.count(rVal)) {
                    return false;
                }
                return writeDone;
            });
            if (stopped) {
                return false;
            }
        }

        reader(readBuf, writeSize);
        {
            std::lock_guard<std::mutex> raii3(readCountMutex);
            readFinished.insert(rVal);
            if (readFinished.size() != readers) {
                return true;
            }
        }
        {
            std::lock_guard<std::mutex> raii(readMutex);
            std::lock_guard<std::mutex> raii3(readCountMutex);
            readFinished.clear();
            writeDone = false;
        }
        {
            std::lock_guard<std::mutex> raii2(writeMutex);
            readDone = true;
        }
        writeWait.notify_all();
        return true;
    }

    void setReaders(int readerCount) override {
        std::lock_guard<std::mutex> raii(readCountMutex);
        readers = readerCount;
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

    [[nodiscard]] size_t getBufferCount() const override {
        return bufCount;
    }

private:

    T* writeBuf;
    T* readBuf;
    size_t bufCount = 0;
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

pipeline::datastream<void>* pipeline::createUnknownStream(int size, size_t count) {
    return new stream_impl<void>(size, count);
}

void pipeline::deleteUnknownStream(pipeline::datastream<void>* stream) {
    delete (stream_impl<void>*) stream;
}
