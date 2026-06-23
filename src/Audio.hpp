#pragma once
#include <windows.h>
#include <xaudio2.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdint>
class Audio
{
  public:
    static inline bool Initialize()
    {
        if (initialized)
            return true;
        HRESULT hr = XAudio2Create(&xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
        if (FAILED(hr))
        {
            std::cerr << "Failed to create XAudio2" << std::endl;
            return false;
        }
        hr = xaudio2->CreateMasteringVoice(&masteringVoice);
        if (FAILED(hr))
        {
            std::cerr << "Failed to create mastering voice" << std::endl;
            xaudio2->Release();
            xaudio2 = nullptr;
            return false;
        }
        initialized = true;
        std::cout << "Audio engine initialized" << std::endl;
        return true;
    }
    static inline void Shutdown()
    {
        if (!initialized)
            return;
        StopAllSounds();
        for (auto& pair : sounds)
        {
            CleanupSound(pair.second);
        }
        sounds.clear();
        if (masteringVoice)
        {
            masteringVoice->DestroyVoice();
            masteringVoice = nullptr;
        }
        if (xaudio2)
        {
            xaudio2->Release();
            xaudio2 = nullptr;
        }
        initialized = false;
        std::cout << "Audio engine shutdown" << std::endl;
    }
    static inline bool IsInitialized()
    {
        return initialized;
    }
    static inline void AddWav(const char* pathToWave, const char* name = nullptr)
    {
        if (!initialized)
        {
            std::cerr << "Audio engine not initialized!" << std::endl;
            return;
        }
        std::string soundName;
        if (name && name[0] != '\0')
        {
            soundName = name;
        }
        else
        {
            std::string path(pathToWave);
            size_t lastSlash = path.find_last_of("/\\");
            size_t lastDot = path.find_last_of('.');
            if (lastSlash != std::string::npos)
                lastSlash++;
            else
                lastSlash = 0;
            soundName = path.substr(lastSlash, lastDot - lastSlash);
        }
        if (sounds.find(soundName) != sounds.end())
        {
            std::cerr << "Sound already exists: " << soundName << std::endl;
            return;
        }
        SoundData sound;
        sound.name = soundName;
        sound.volume = 1.0f;
        sound.pitch = 1.0f;
        sound.isLooped = false;
        sound.isPlaying = false;
        if (!LoadWavFile(pathToWave, sound))
        {
            return;
        }
        sounds[soundName] = std::move(sound);
        std::cout << "Added sound: " << soundName << std::endl;
    }
    static inline void PlaySound(const char* name, bool isLooped = false)
    {
        if (!initialized)
        {
            std::cerr << "Audio engine not initialized!" << std::endl;
            return;
        }
        auto it = sounds.find(name);
        if (it == sounds.end())
        {
            std::cerr << "Sound not found: " << name << std::endl;
            return;
        }
        SoundData& sound = it->second;
        if (sound.voice)
        {
            sound.voice->Stop(0);
            sound.voice->DestroyVoice();
            sound.voice = nullptr;
        }
        sound.isLooped = isLooped;
        if (!CreateVoice(sound))
        {
            std::cerr << "Failed to create voice for: " << name << std::endl;
            return;
        }
        HRESULT hr = sound.voice->Start(0);
        if (FAILED(hr))
        {
            std::cerr << "Failed to start voice for: " << name << std::endl;
            return;
        }
        sound.isPlaying = true;
    }
    static inline void StopSound(const char* name)
    {
        auto it = sounds.find(name);
        if (it == sounds.end())
            return;
        CleanupSound(it->second);
    }
    static inline void StopAllSounds()
    {
        for (auto& pair : sounds)
        {
            CleanupSound(pair.second);
        }
    }
    static inline void SetSoundVolume(float volume)
    {
        masterVolume = std::clamp(volume, 0.0f, 1.0f);
        if (masteringVoice)
        {
            masteringVoice->SetVolume(masterVolume);
        }
        for (auto& pair : sounds)
        {
            if (pair.second.voice)
            {
                pair.second.voice->SetVolume(pair.second.volume * masterVolume);
            }
        }
    }
    static inline void SetSoundVolume(const char* name, float volume)
    {
        auto it = sounds.find(name);
        if (it == sounds.end())
            return;
        it->second.volume = std::clamp(volume, 0.0f, 1.0f);
        if (it->second.voice)
        {
            it->second.voice->SetVolume(it->second.volume * masterVolume);
        }
    }
    static inline void SetSoundPitch(const char* name, float pitch)
    {
        auto it = sounds.find(name);
        if (it == sounds.end())
            return;
        it->second.pitch = std::clamp(pitch, 0.125f, 8.0f);
        if (it->second.voice)
        {
            it->second.voice->SetFrequencyRatio(it->second.pitch);
        }
    }
    static inline void PauseSound(const char* name)
    {
        auto it = sounds.find(name);
        if (it == sounds.end() || !it->second.voice)
            return;
        it->second.voice->Stop(0);
        it->second.isPlaying = false;
    }
    static inline void ResumeSound(const char* name)
    {
        auto it = sounds.find(name);
        if (it == sounds.end() || !it->second.voice)
            return;
        it->second.voice->Start(0);
        it->second.isPlaying = true;
    }
    static inline bool IsSoundPlaying(const char* name)
    {
        auto it = sounds.find(name);
        if (it == sounds.end() || !it->second.voice)
            return false;
        XAUDIO2_VOICE_STATE state;
        it->second.voice->GetState(&state);
        return state.BuffersQueued > 0 || it->second.isPlaying;
    }
    static inline bool SoundExists(const char* name)
    {
        return sounds.find(name) != sounds.end();
    }
    static inline void Update()
    {
        if (!initialized)
            return;
        for (auto& pair : sounds)
        {
            SoundData& sound = pair.second;
            if (sound.voice && !sound.isLooped)
            {
                XAUDIO2_VOICE_STATE state;
                sound.voice->GetState(&state);
                if (state.BuffersQueued == 0 && sound.isPlaying)
                {
                    sound.isPlaying = false;
                }
            }
        }
    }

  private:
    Audio() = delete;
    ~Audio() = delete;
    struct SoundData
    {
        std::vector<unsigned char> audioData;
        WAVEFORMATEX wavFormat = {};
        uint32_t bufferSize = 0;
        std::string name;
        IXAudio2SourceVoice* voice = nullptr;
        bool isPlaying = false;
        bool isLooped = false;
        float volume = 1.0f;
        float pitch = 1.0f;
        XAUDIO2_BUFFER GetBuffer() const
        {
            XAUDIO2_BUFFER buffer = {};
            buffer.AudioBytes = bufferSize;
            buffer.pAudioData = audioData.data();
            buffer.Flags = XAUDIO2_END_OF_STREAM;
            buffer.LoopCount = isLooped ? XAUDIO2_LOOP_INFINITE : 0;
            return buffer;
        }
    };
    static inline bool LoadWavFile(const std::string& filePath, SoundData& outSound)
    {
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Failed to open WAV file: " << filePath << std::endl;
            return false;
        }
        struct RIFFHeader
        {
            char chunkId[4];
            uint32_t chunkSize;
            char format[4];
        } riffHeader;
        struct FMTChunk
        {
            char subChunkId[4];
            uint32_t subChunkSize;
            uint16_t audioFormat;
            uint16_t numChannels;
            uint32_t sampleRate;
            uint32_t byteRate;
            uint16_t blockAlign;
            uint16_t bitsPerSample;
        } fmtChunk;
        struct DataChunk
        {
            char subChunkId[4];
            uint32_t subChunkSize;
        } dataChunk;
        file.read(reinterpret_cast<char*>(&riffHeader), sizeof(riffHeader));
        if (riffHeader.chunkId[0] != 'R' || riffHeader.chunkId[1] != 'I' || riffHeader.chunkId[2] != 'F' ||
            riffHeader.chunkId[3] != 'F')
        {
            std::cerr << "Invalid RIFF header in: " << filePath << std::endl;
            return false;
        }
        file.read(reinterpret_cast<char*>(&fmtChunk), sizeof(fmtChunk));
        if (fmtChunk.subChunkId[0] != 'f' || fmtChunk.subChunkId[1] != 'm' || fmtChunk.subChunkId[2] != 't' ||
            fmtChunk.subChunkId[3] != ' ')
        {
            std::cerr << "Invalid FMT chunk in: " << filePath << std::endl;
            return false;
        }
        outSound.wavFormat.wFormatTag = fmtChunk.audioFormat;
        outSound.wavFormat.nChannels = fmtChunk.numChannels;
        outSound.wavFormat.nSamplesPerSec = fmtChunk.sampleRate;
        outSound.wavFormat.nAvgBytesPerSec = fmtChunk.byteRate;
        outSound.wavFormat.nBlockAlign = fmtChunk.blockAlign;
        outSound.wavFormat.wBitsPerSample = fmtChunk.bitsPerSample;
        outSound.wavFormat.cbSize = 0;
        while (true)
        {
            file.read(reinterpret_cast<char*>(&dataChunk), sizeof(dataChunk));
            if (dataChunk.subChunkId[0] == 'd' && dataChunk.subChunkId[1] == 'a' && dataChunk.subChunkId[2] == 't' &&
                dataChunk.subChunkId[3] == 'a')
            {
                break;
            }
            file.seekg(dataChunk.subChunkSize, std::ios::cur);
            if (file.eof())
                break;
        }
        if (file.eof())
        {
            std::cerr << "Data chunk not found in: " << filePath << std::endl;
            return false;
        }
        outSound.bufferSize = dataChunk.subChunkSize;
        outSound.audioData.resize(dataChunk.subChunkSize);
        file.read(reinterpret_cast<char*>(outSound.audioData.data()), dataChunk.subChunkSize);
        return true;
    }
    static inline bool CreateVoice(SoundData& sound)
    {
        if (!xaudio2 || !masteringVoice)
            return false;
        if (sound.voice)
        {
            sound.voice->Stop(0);
            sound.voice->DestroyVoice();
            sound.voice = nullptr;
        }
        HRESULT hr = xaudio2->CreateSourceVoice(&sound.voice, &sound.wavFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr);
        if (FAILED(hr))
        {
            std::cerr << "Failed to create source voice for: " << sound.name << std::endl;
            return false;
        }
        XAUDIO2_BUFFER buffer = sound.GetBuffer();
        hr = sound.voice->SubmitSourceBuffer(&buffer);
        if (FAILED(hr))
        {
            sound.voice->DestroyVoice();
            sound.voice = nullptr;
            std::cerr << "Failed to submit buffer for: " << sound.name << std::endl;
            return false;
        }
        sound.voice->SetVolume(sound.volume * masterVolume);
        sound.voice->SetFrequencyRatio(sound.pitch);
        return true;
    }
    static inline void CleanupSound(SoundData& sound)
    {
        if (sound.voice)
        {
            sound.voice->Stop(0);
            sound.voice->DestroyVoice();
            sound.voice = nullptr;
        }
        sound.isPlaying = false;
    }
    static bool initialized;
    static IXAudio2* xaudio2;
    static IXAudio2MasteringVoice* masteringVoice;
    static std::unordered_map<std::string, SoundData> sounds;
    static float masterVolume;
};

bool Audio::initialized = false;
IXAudio2* Audio::xaudio2 = nullptr;
IXAudio2MasteringVoice* Audio::masteringVoice = nullptr;
std::unordered_map<std::string, Audio::SoundData> Audio::sounds;
float Audio::masterVolume = 1.0f;
