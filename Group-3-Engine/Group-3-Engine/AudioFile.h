#pragma once
#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

template <typename T>
class AudioFile {
public:
    bool load(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }

        // Load WAV File Header
        char chunkId[4];
        file.read(chunkId, 4);
        if (std::string(chunkId, 4) != "RIFF") {
            std::cerr << "Not a valid WAV file: " << filename << std::endl;
            return false;
        }

        uint32_t chunkSize;
        file.read(reinterpret_cast<char*>(&chunkSize), 4);

        char format[4];
        file.read(format, 4);
        if (std::string(format, 4) != "WAVE") {
            std::cerr << "Not a valid WAV file: " << filename << std::endl;
            return false;
        }

        char subchunk1Id[4];
        file.read(subchunk1Id, 4);
        if (std::string(subchunk1Id, 4) != "fmt ") {
            std::cerr << "Not a valid WAV file: " << filename << std::endl;
            return false;
        }

        uint32_t subchunk1Size;
        file.read(reinterpret_cast<char*>(&subchunk1Size), 4);

        uint16_t audioFormat;
        file.read(reinterpret_cast<char*>(&audioFormat), 2);
        if (audioFormat != 1) {
            std::cerr << "Unsupported audio format: " << filename << std::endl;
            return false;
        }

        uint16_t numChannels;
        file.read(reinterpret_cast<char*>(&numChannels), 2);

        uint32_t sampleRate;
        file.read(reinterpret_cast<char*>(&sampleRate), 4);
        this->sampleRate = sampleRate;

        uint32_t byteRate;
        file.read(reinterpret_cast<char*>(&byteRate), 4);

        uint16_t blockAlign;
        file.read(reinterpret_cast<char*>(&blockAlign), 2);

        uint16_t bitsPerSample;
        file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

        char subchunk2Id[4];
        file.read(subchunk2Id, 4);
        if (std::string(subchunk2Id, 4) != "data") {
            std::cerr << "Not a valid WAV file: " << filename << std::endl;
            return false;
        }

        uint32_t subchunk2Size;
        file.read(reinterpret_cast<char*>(&subchunk2Size), 4);

        // Loading Audio File
        samples.resize(numChannels);
        for (uint16_t channel = 0; channel < numChannels; ++channel) {
            samples[channel].resize(subchunk2Size / blockAlign);
        }

        for (size_t i = 0; i < subchunk2Size / blockAlign; ++i) {
            for (uint16_t channel = 0; channel < numChannels; ++channel) {
                int16_t sample;
                file.read(reinterpret_cast<char*>(&sample), 2);
                samples[channel][i] = static_cast<T>(sample) / 32768.0;
            }
        }

        return true;
    }

    size_t getNumChannels() const {
        return samples.size();
    }

    size_t getNumSamplesPerChannel() const {
        return samples[0].size();
    }

    int getSampleRate() const {
        return sampleRate;
    }

    std::vector<std::vector<T>> samples;

private:
    int sampleRate;
};

#endif // AUDIOFILE_H