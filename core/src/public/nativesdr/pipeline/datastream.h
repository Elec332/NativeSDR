//
// Created by Elec332 on 10/10/2021.
//

#ifndef NATIVESDR_DATASTREAM_H
#define NATIVESDR_DATASTREAM_H

#include <nativesdr/core_export.h>
#include <cstdlib>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace pipeline {

    const size_t BUFFER_COUNT = 1024 * 128;

    template<class T>
    class datastream {

    public:

        /**
         * Writes data to this stream, async
         *
         * @param writer The data writer (must return length of data written)
         * @return Whether the data was read (false if e.g. stream stopped)
         */
        virtual bool write(const std::function<int(T*)>& writer) = 0;

        /**
         * Sets the amount of stream readers, all input data will be voided if the stream has 0 readers
         *
         * @param readers The amount of stream readers
         */
        virtual void setReaders(int readers) = 0;

        /**
         * Read data from this stream, async
         *
         * @param reader The data reader
         * @return Whether the data was read (false if e.g. stream stopped)
         */
        virtual bool read(const std::function<void(const T*, int)>& reader) = 0;

        /**
         * (Re)starts the stream
         */
        virtual void start() = 0;

        /**
         * Stops the stream, kicks all readers & writers out of their waiting queue
         */
        virtual void stop() = 0;

        /**
         * Returns the maximum amount of elements in the stream buffer
         *
         * @return The maximum amount of elements in the stream buffer
         */
        virtual size_t getBufferCount() const = 0;

        /**
         * Auxiliary stream data, type of data is dependant on the stream type "T".
         */
        void* auxData = nullptr;

    };

    CORE_EXPORT datastream<void>* createUnknownStream(int size, size_t count);

    /**
     * Returns a new stream for the specified type.
     * WARNING: The returned stream will void any data received until a receiver count has been set!
     *
     * @return A new stream
     */
    template<class T>
    datastream<T>* createStream() {
        return (datastream<T>*) createUnknownStream(sizeof(T), BUFFER_COUNT);
    }

    CORE_EXPORT void deleteUnknownStream(datastream<void>* stream);

    /**
     * Deletes a data stream
     *
     * @param stream The stream to be deleted
     */
    template<class T>
    void deleteStream(datastream<T>* stream) {
        deleteUnknownStream((datastream<void>*) stream);
    }

}

#endif //NATIVESDR_DATASTREAM_H
