//
// Created by Elec332 on 28/10/2021.
//

#ifndef NATIVESDR_WAV_READER_H
#define NATIVESDR_WAV_READER_H

#include <algorithm>

uint8_t RIFF[] = {'R', 'I', 'F', 'F'};
uint8_t WAVE[] = {'W', 'A', 'V', 'E'};
uint8_t FMT[] = {'f', 'm', 't', ' '};
uint8_t AUXI[] = {'a', 'u', 'x', 'i'};
uint8_t DATA[] = {'d', 'a', 't', 'a'};


typedef struct sample_data {
    uint16_t AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,257=IBM Mu-Law,258=IBM A-Law,259=ADPCM
    uint16_t NumOfChan;      // Number of channels 1=Mono 2=Stereo
    uint32_t SamplesPerSec;  // Sampling Frequency in Hz
    uint32_t bytesPerSec;    // bytes per second
    uint16_t blockAlign;     // 2=16-bit mono, 4=16-bit stereo
    uint16_t bitsPerSample;  // Number of bits per sample
} sample_data;


typedef struct file_data {
    sample_data sampleData{};
    uint32_t centerFreq = -1;
    uint32_t length = 0;
} file_data;

inline bool checkFourBytes(FILE* file, uint8_t* buf, uint8_t* check) {
    size_t bytesRead = fread(buf, 1, 4, file);
    if (bytesRead != 4 || memcmp(buf, check, 4) != 0) {
        return false;
    }
    return true;
}

std::shared_ptr<file_data> readWAV(FILE* file) {
    if (file == nullptr) {
        return nullptr;
    }
    uint32_t buf_len = 64;
    auto buf = (uint8_t*) malloc(buf_len);
    if (!checkFourBytes(file, buf, RIFF)) {
        return nullptr;
    }
    uint32_t length;
    size_t readAmt = fread(&length, 4, 1, file);
    if (readAmt != 1 || !checkFourBytes(file, buf, WAVE) || !checkFourBytes(file, buf, FMT)) {
        return nullptr;
    }
    length -= 8;
    std::shared_ptr<file_data> ret = std::make_shared<file_data>();
    readAmt = fread(buf, 1, 4, file);
    readAmt += fread(&ret->sampleData, sizeof(sample_data), 1, file) * sizeof(sample_data);
    length -= readAmt;
    if (readAmt != sizeof(sample_data) + 4) {
        return nullptr;
    }
    readAmt = fread(buf, 1, 4, file);
    length -= readAmt;
    if (readAmt != 4) {
        return nullptr;
    }
    if (memcmp(buf, AUXI, 4) == 0) {
        uint32_t au_len;
        fread(&au_len, 4, 1, file);
        fread(buf, 1, 16, file);
        fread(buf, 1, 16, file);
        fread(&ret->centerFreq, 4, 1, file);
        length -= (au_len + 8);
        au_len -= 36;
        while (au_len > 0) {
            uint32_t read = std::min(au_len, buf_len);
            fread(buf, 1, read, file);
            au_len -= read;
        }
        readAmt = fread(buf, 1, 4, file);
        if (readAmt != 4) {
            return nullptr;
        }
    }
    readAmt = fread(&ret->length, 4, 1, file);
    if (readAmt != 1 || memcmp(buf, DATA, 4) != 0 || ret->length != (length - readAmt * 4)) {
        return nullptr;
    }
    return ret;
}

#endif //NATIVESDR_WAV_READER_H
