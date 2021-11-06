//
// Created by Elec332 on 10/10/2021.
//

#ifndef NATIVESDR_DATASTREAM_H
#define NATIVESDR_DATASTREAM_H

#include <cstdlib>
#include <mutex>
#include <condition_variable>
#include <functional>

template<class T>
class datastream {

public:

    virtual bool write(std::function<int(T*)> writer) = 0;

    virtual bool read(std::function<void(T*, int)> reader) = 0;

};

template<class T>
class stream_impl : public datastream<T> {

    stream_impl() {
        writeBuf = malloc(100);
        readBuf = malloc(100);
    }

    ~stream_impl() {
        free(writeBuf);
        free(readBuf);
    }

    bool write(std::function<int(T*)> writer) override {
        std::unique_lock<std::mutex> raii(writeMutex);
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
        readWait.notify_all();
        return true;
    }

    bool read(std::function<void(T*, int)> reader) override {
        std::unique_lock<std::mutex> raii(readMutex);
        readWait.wait(raii, [&] {
            return writeDone || stopped;
        });
        if (stopped) {
            return false;
        }

        reader(readBuf, writeSize);
        readDone = true;
        writeWait.notify_all();
        return true;
    }

private:

    T* writeBuf;
    T* readBuf;
    int writeSize = 0;

    std::mutex writeMutex{};
    std::condition_variable writeWait{};
    bool writeDone = false;

    std::mutex readMutex{};
    std::condition_variable readWait{};
    bool readDone = true;

    bool stopped = false;

};

#endif //NATIVESDR_DATASTREAM_H
