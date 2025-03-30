#include "AudioManager.h"
#include <fstream>
#include <vector>
#include <iostream>
#include "Resources.h"


// LOG macro for logging messages
// It outputs the provided message to the standard output stream (std::cout)
#define LOG(message) std::cout << message << std::endl

// Conditional compilation for debug logging
// If the _DEBUG macro is defined (in debug builds),
// the DEBUG_LOG macro is defined to output a debug message
// with a "[DEBUG]" prefix to the standard output stream

// If _DEBUG is not defined (in release builds),
// the DEBUG_LOG macro is defined as an empty statement,
// effectively disabling debug logging
#ifdef _DEBUG
#define DEBUG_LOG(message) std::cout << "[DEBUG] " << message << std::endl
#else
#define DEBUG_LOG(message)
#endif

AudioManager::AudioManager() {
    // Constructor implementation
    // Initialize OpenAL and any other necessary setup
    InitializeOpenAL();

    // Initialize currentAudioClip to an empty string
    // This variable stores the ID of the currently selected audio clip
    currentAudioClip = "";
}

AudioManager::~AudioManager() {
    // Destructor implementation
    // Clean up OpenAL and any other resources
    ShutdownOpenAL();
}

AudioManager& AudioManager::GetInstance()
{
    // Implementation of the singleton pattern
    // Create a static instance of AudioManager
    // This ensures that only one instance of AudioManager exists throughout the program
    static AudioManager instance;

    // Return a reference to the static instance
    return instance;
}

void AudioManager::InitializeOpenAL()
{
    // OpenAL initialization code

    // Open the default OpenAL device
    device = alcOpenDevice(nullptr);
    if (!device)
    {
        // Error handling
        // If the device couldn't be opened, return from the function
        return;
    }

    // Create an OpenAL context
    context = alcCreateContext(device, nullptr);
    if (!context)
    {
        // Error handling
        // If the context couldn't be created, close the device and return from the function
        alcCloseDevice(device);
        return;
    }

    // Make the created context the current context
    alcMakeContextCurrent(context);
}

void AudioManager::ShutdownOpenAL()
{
    // OpenAL shutdown code

    // Make the current context NULL
    alcMakeContextCurrent(nullptr);

    // Destroy the OpenAL context
    alcDestroyContext(context);

    // Close the OpenAL device
    alcCloseDevice(device);
}

/*
std::string AudioManager::ShowFileOpenDialog() {


    // Declare a variable to store the selected file name
    wchar_t fileName[MAX_PATH] = L"";

    // Declare and initialize an OPENFILENAME structure
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = L"WAV Files (*.wav)\0*.wav\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"wav";

    // Show the file open dialog
    if (GetOpenFileName(&ofn)) {
        // If a file was selected

        // Convert the wide string file name to a UTF-8 encoded string
        std::wstring wideStr(fileName);
        std::string utf8Str(wideStr.begin(), wideStr.end());

        // Log the selected file path
        LOG("Selected file: " << utf8Str);

        // Return the selected file path
        return utf8Str;
    }

    // If no file was selected, log a message and return an empty string
    LOG("No file selected.");
    return std::string();
}
*/

// Preallocate a buffer for audio data
std::vector<char> audioDataBuffer;

void AudioManager::LoadAudioClip(const std::string& audioClipId, const std::string& filePath) {

    std::string path = filePath;
    //need to format for android
    path = Resources::FormatPath(path); 

    // Log a debug message indicating the audio clip being loaded and its file path
    DEBUG_LOG("Loading audio clip: " << audioClipId << " from file: " << path);

#ifdef ANDROID
    AAsset* asset = AAssetManager_open(Resources::GetAssetManager(), path.c_str(), AASSET_MODE_UNKNOWN);
    if (asset == NULL)
    {
        __android_log_print(ANDROID_LOG_ERROR, "File Load Error", "%s", path.c_str());
        return;
    }
    unsigned char* charData;
    size_t size = (size_t)AAsset_getLength(asset);
    charData = (unsigned char*)malloc(sizeof(unsigned char)*size);
    AAsset_read(asset, charData, size);
    AAsset_close(asset);

#else
    // Open the audio file in binary mode
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        
        // If the file fails to open, log an error message and return
        LOG("Failed to open audio file: " << path);
        return;
    }
#endif

    // Define a struct to represent the WAV header
    struct WAVHeader {
        char chunkID[4];
        unsigned int chunkSize;
        char format[4];
        char subchunk1ID[4];
        unsigned int subchunk1Size;
        unsigned short audioFormat;
        unsigned short numChannels;
        unsigned int sampleRate;
        unsigned int byteRate;
        unsigned short blockAlign;
        unsigned short bitsPerSample;
        char subchunk2ID[4];
        unsigned int subchunk2Size;
    };

    // Read the WAV header from the file
    WAVHeader header;
    #ifdef ANDROID
    header = *(WAVHeader*)charData;
    #else
    file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
    #endif



    // Check if the WAV format is supported (only PCM is supported)
    if (header.audioFormat != 1) {
        LOG("Unsupported WAV format. Only PCM is supported.");
        #ifndef ANDROID
        file.close();
        #endif
        return;
    }

    // Determine the OpenAL buffer format based on the number of channels and bits per sample
    ALenum format;
    if (header.numChannels == 1 && header.bitsPerSample == 8) {
        format = AL_FORMAT_MONO8;
    }
    else if (header.numChannels == 1 && header.bitsPerSample == 16) {
        format = AL_FORMAT_MONO16;
    }
    else if (header.numChannels == 2 && header.bitsPerSample == 8) {
        format = AL_FORMAT_STEREO8;
    }
    else if (header.numChannels == 2 && header.bitsPerSample == 16) {
        format = AL_FORMAT_STEREO16;
    }
    else {
        LOG("Unsupported WAV format. Only mono or stereo, 8-bit or 16-bit formats are supported.");
        #ifndef ANDROID
        file.close();
        #endif
        return;
    }

    // Resize the preallocated buffer to the required size
    audioDataBuffer.resize(header.subchunk2Size);

    // Read the audio data into the preallocated buffer
    #ifndef ANDROID
    file.read(audioDataBuffer.data(), header.subchunk2Size);
    file.close();
    #else
    memcpy(audioDataBuffer.data(), charData + sizeof(WAVHeader), header.subchunk2Size); //copy data into buffer
    #endif

    // Generate an OpenAL buffer
    ALuint buffer;
    alGenBuffers(1, &buffer);
    // Copy the audio data into the OpenAL buffer
    alBufferData(buffer, format, audioDataBuffer.data(), header.subchunk2Size, header.sampleRate);

    // Check for OpenAL errors
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        LOG("Failed to load audio data into OpenAL buffer. Error code: " << error);
        alDeleteBuffers(1, &buffer);
        return;
    }

    // Check if an audio clip with the same ID is already loaded
    auto it = audioClips.find(audioClipId);
    if (it != audioClips.end()) {
        // If an audio clip with the same ID exists, unload it first
        UnloadAudioClip(audioClipId);
    }

    // Store the OpenAL buffer, default volume, looping state, and audio data in the audioClips map
    audioClips[audioClipId] = { buffer, 0.5f, false, std::move(audioDataBuffer) };

    // Add the audio clip ID to the list of loaded audio clips
    loadedAudioClips.push_back(audioClipId);

    // Log a message indicating successful loading of the audio clip
    LOG("Audio clip loaded successfully: " << audioClipId);
}

void AudioManager::UnloadAudioClip(const std::string& audioClipId) {
    // Log a debug message indicating the audio clip being unloaded
    DEBUG_LOG("Unloading audio clip: " << audioClipId);

    // Find the audio clip in the audioClips map
    auto it = audioClips.find(audioClipId);
    if (it != audioClips.end()) {
        // Get the OpenAL buffer associated with the audio clip
        ALuint buffer = it->second.buffer;

        // Stop the audio clip if it is currently playing
        auto sourceIt = audioSources.find(audioClipId);
        if (sourceIt != audioSources.end()) {
            ALuint source = sourceIt->second;
            alSourceStop(source);
            alDeleteSources(1, &source);
            audioSources.erase(sourceIt);
            if (currentPlayingAudioClip == audioClipId) {
                currentPlayingAudioClip = "";
            }
        }

        // Delete the OpenAL buffer
        alDeleteBuffers(1, &buffer);

        // Check for OpenAL errors
        ALenum error = alGetError();
        if (error != AL_NO_ERROR) {
            LOG("Failed to delete OpenAL buffer. Error code: " << error);
        }

        // Clear the audio data vector to free memory
        it->second.data.clear();
        it->second.data.shrink_to_fit();

        // Remove the audio clip from the audioClips map
        audioClips.erase(it);

        // Remove the audio clip ID from the loaded audio clips list
        auto listIt = std::find(loadedAudioClips.begin(), loadedAudioClips.end(), audioClipId);
        if (listIt != loadedAudioClips.end()) {
            loadedAudioClips.erase(listIt);
        }

        // Log a message indicating successful unloading of the audio clip
        LOG("Audio clip unloaded: " << audioClipId);
    }
}

void AudioManager::PlayAudioClip(const std::string& audioClipId, bool loop) {
    // Log a debug message indicating the audio clip being played
    DEBUG_LOG("Playing audio clip: " << audioClipId);

    // Find the audio clip in the audioClips map
    auto it = audioClips.find(audioClipId);

    if (it != audioClips.end()) {
        // Get the OpenAL buffer associated with the audio clip
        ALuint buffer = it->second.buffer;

        // Check if the audio clip is already playing or paused
        auto sourceIt = audioSources.find(audioClipId);
        if (sourceIt != audioSources.end()) {
            ALuint source = sourceIt->second;
            ALint state;
            alGetSourcei(source, AL_SOURCE_STATE, &state);

            if (state == AL_PLAYING) {
                // Audio clip is already playing, do nothing
                return;
            }
            else if (state == AL_PAUSED) {
                // Audio clip is paused, resume playback
                alSourcePlay(source);
                LOG("Audio clip resumed: " << audioClipId);
                return;
            }
            else {
                // Audio clip is not playing or paused, stop and delete the existing source
                alSourceStop(source);
                alDeleteSources(1, &source);
                audioSources.erase(sourceIt);
            }
        }

        // Create a new source for the audio clip
        ALuint source;
        alGenSources(1, &source);
        alSourcei(source, AL_BUFFER, buffer);

        // Set the looping state based on the value stored in the audioClips map
        auto clipIt = audioClips.find(audioClipId);
        if (clipIt != audioClips.end()) {
            alSourcei(source, AL_LOOPING, clipIt->second.isLooping ? AL_TRUE : AL_FALSE);
        }

        // Set the initial volume based on the master volume and audio clip volume
        float volume = masterVolume * GetAudioClipVolume(audioClipId);
        alSourcef(source, AL_GAIN, volume);

#ifdef DEBUGGER
        // Set the spatial position of the audio source based on the Player's position
        float playerX = Debugger::GetSourcePosX();
        float playerY = Debugger::GetSourcePosY();
        alSource3f(source, AL_POSITION, playerX, playerY, 0.0f);
#endif

        // Set the spatial orientation of the audio source (assuming 2D, so no orientation)
        alSource3f(source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);

        // Set the velocity of the audio source (assuming no velocity in 2D)
        alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);

        // Set the reference distance and maximum distance for attenuation
        alSourcef(source, AL_REFERENCE_DISTANCE, 10.0f);
        alSourcef(source, AL_MAX_DISTANCE, 1000.0f);

        // Set the rolloff factor for attenuation
        alSourcef(source, AL_ROLLOFF_FACTOR, 0.5f);

        // Play the audio source
        alSourcePlay(source);

        // Store the source in the audioSources map
        audioSources[audioClipId] = source;
        currentPlayingAudioClip = audioClipId;

        // Log a message indicating that the audio clip is playing
        LOG("Audio clip playing: " << audioClipId);
    }
    else {
        // If the audio clip is not found, log an error message
        LOG("Audio clip not found: " << audioClipId);
    }
}

void AudioManager::StopAudioClip(const std::string& audioClipId) {
    // Log a debug message indicating the audio clip being stopped
    DEBUG_LOG("Stopping audio clip: " << audioClipId);

    // Check if the specified audio clip is currently playing
    if (audioClipId == currentPlayingAudioClip) {
        // Find the audio source associated with the audio clip
        auto it = audioSources.find(audioClipId);
        if (it != audioSources.end()) {
            // Get the OpenAL source associated with the audio clip
            ALuint source = it->second;
            alSourceStop(source);
            alDeleteSources(1, &source);
            audioSources.erase(it);
            currentPlayingAudioClip = "";
            LOG("Audio clip stopped");
        }
        else {
            // If the audio source is not found, log an error message
            LOG("Audio source not found: " << audioClipId);
        }
    }
    else {
        // If the audio clip is not currently playing, log a message
        LOG("Audio clip not currently playing: " << audioClipId);
    }
}

void AudioManager::PauseAudioClip(const std::string& audioClipId) {
    // Log a debug message indicating the audio clip being paused
    DEBUG_LOG("Pausing audio clip: " << audioClipId);

    // Find the audio source associated with the audio clip
    auto it = audioSources.find(audioClipId);
    if (it != audioSources.end()) {
        // Get the OpenAL source associated with the audio clip
        ALuint source = it->second;
        alSourcePause(source);
        LOG("Audio clip paused: " << audioClipId);
    }
    else {
        // If the audio source is not found, log an error message
        LOG("Audio source not found: " << audioClipId);
    }
}

void AudioManager::ResumeAudioClip(const std::string& audioClipId) {
    // Log a debug message indicating the audio clip being resumed
    DEBUG_LOG("Resuming audio clip: " << audioClipId);

    // Find the audio source associated with the audio clip
    auto it = audioSources.find(audioClipId);
    if (it != audioSources.end()) {
        // Get the OpenAL source associated with the audio clip
        ALuint source = it->second;
        alSourcePlay(source);
        LOG("Audio clip resumed: " << audioClipId);
    }
    else {
        // If the audio source is not found, log an error message
        LOG("Audio source not found: " << audioClipId);
    }
}

void AudioManager::SetAudioClipLooping(const std::string& audioClipId, bool loop) {
    auto it = audioClips.find(audioClipId);
    if (it != audioClips.end()) {
        it->second.isLooping = loop;

        // Find the audio source associated with the audio clip
        auto sourceIt = audioSources.find(audioClipId);
        if (sourceIt != audioSources.end()) {
            ALuint source = sourceIt->second;
            alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
        }
    }
}

bool AudioManager::IsAudioClipLooping(const std::string& audioClipId) const {
    auto it = audioClips.find(audioClipId);
    if (it != audioClips.end()) {
        return it->second.isLooping;
    }
    return false;
}

bool AudioManager::IsAudioClipPaused(const std::string& audioClipId) const {
    auto it = audioSources.find(audioClipId);
    if (it != audioSources.end()) {
        ALint state;
        alGetSourcei(it->second, AL_SOURCE_STATE, &state);
        return state == AL_PAUSED;
    }
    return false;
}

void AudioManager::SetCurrentAudioClip(const std::string& audioClipId) {
    currentAudioClip = audioClipId;
}

const std::vector<std::string>& AudioManager::GetLoadedAudioClips() const {
    return loadedAudioClips;
}

void AudioManager::SetMasterVolume(float volume) {
    masterVolume = volume;
    UpdateAudioVolumes();
}

float AudioManager::GetMasterVolume() const {
    return masterVolume;
}

void AudioManager::SetAudioClipVolume(const std::string& audioClipId, float volume) {
    auto it = audioClips.find(audioClipId);
    if (it != audioClips.end()) {
        it->second.volume = volume;
        UpdateAudioVolumes();
    }
}

float AudioManager::GetAudioClipVolume(const std::string& audioClipId) const {
    auto it = audioClips.find(audioClipId);
    if (it != audioClips.end()) {
        return it->second.volume;
    }
    return 0.5f;
}

void AudioManager::UpdateAudioVolumes() {
    for (const auto& pair : audioSources) {
        const std::string& audioClipId = pair.first;
        ALuint source = pair.second;
        float volume = masterVolume * GetAudioClipVolume(audioClipId);
        alSourcef(source, AL_GAIN, volume);
    }
}


void AudioManager::AddAudioClipTag(const std::string& audioClipId, const std::string& tag) {
    audioTags[tag].push_back(audioClipId);
}

void AudioManager::RemoveAudioClipTag(const std::string& audioClipId, const std::string& tag) {
    auto it = audioTags.find(tag);
    if (it != audioTags.end()) {
        auto& clipIds = it->second;
        auto clipIt = std::find(clipIds.begin(), clipIds.end(), audioClipId);
        if (clipIt != clipIds.end()) {
            clipIds.erase(clipIt);
        }
    }
}

std::vector<std::string> AudioManager::GetAudioClipTags(const std::string& audioClipId) const {
    std::vector<std::string> tags;
    for (const auto& pair : audioTags) {
        const std::string& tag = pair.first;
        const auto& clipIds = pair.second;
        if (std::find(clipIds.begin(), clipIds.end(), audioClipId) != clipIds.end()) {
            tags.push_back(tag);
        }
    }
    return tags;
}

const std::unordered_map<std::string, std::vector<std::string>>& AudioManager::GetAudioTags() const
{
    return audioTags;
}

void AudioManager::Update() {
    // Get the positions of Listener and Audio Source from Debugger class
#ifdef DEBUGGER
    float listenerX = Debugger::GetListenerPosX();
    float listenerY = Debugger::GetListenerPosY();
    float sourceX = Debugger::GetSourcePosX();
    float sourceY = Debugger::GetSourcePosY();

    // Set the Listener position by OpenAL
    alListener3f(AL_POSITION, listenerX, listenerY, 0.0f);
#endif

    // If there is a playing clip, update the position of the audio source
    if (!currentPlayingAudioClip.empty()) {
        ALuint source = audioSources[currentPlayingAudioClip];
#ifdef DEBUGGER
        alSource3f(source, AL_POSITION, sourceX, sourceY, 0.0f);
#endif
    }
}
