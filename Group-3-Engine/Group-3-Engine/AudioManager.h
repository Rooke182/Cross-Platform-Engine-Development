#pragma once

// Include necessary headers
#include <string>
#include <unordered_map>
#include <vector>
#include <al.h>
#include <alc.h>
#include "Debugger.h"

// Structure to store audio clip data
struct AudioClipData {
    ALuint buffer;
    float volume;
    bool isLooping;
    std::vector<char> data;

    //AudioClipData();
    //AudioClipData(ALuint buffer, float volume, bool isLooping, std::vector<char>&& data);
};

class AudioManager {
public:
    // Get the singleton instance of the AudioManager
    static AudioManager& GetInstance();

    // Initialize the OpenAL library
    void InitializeOpenAL();
    // Shut down the OpenAL library
    void ShutdownOpenAL();

    // Load an audio clip from a file
    void LoadAudioClip(const std::string& audioClipId, const std::string& filePath);
    // Unload an audio clip
    void UnloadAudioClip(const std::string& audioClipId);

    // Play an audio clip
    void PlayAudioClip(const std::string& audioClipId, bool loop = false);
    // Stop playing an audio clip
    void StopAudioClip(const std::string& audioClipId);
    // Pause an audio clip
    void PauseAudioClip(const std::string& audioClipId);
    // Resume playing a paused audio clip
    void ResumeAudioClip(const std::string& audioClipId);
    // Set the looping state of an audio clip
    void SetAudioClipLooping(const std::string& audioClipId, bool loop);

    // Update audio sources (e.g., position, velocity)
    void Update();

    // Show a file open dialog to select an audio file
    //std::string ShowFileOpenDialog();

    // Set the current audio clip (for playback)
    void SetCurrentAudioClip(const std::string& audioClipId);
    // Get a list of loaded audio clips
    const std::vector<std::string>& GetLoadedAudioClips() const;

    // Check if an audio clip is set to loop
    bool IsAudioClipLooping(const std::string& audioClipId) const;
    // Check if an audio clip is paused
    bool IsAudioClipPaused(const std::string& audioClipId) const;

    // Set the master volume
    void SetMasterVolume(float volume);
    // Set the volume of an audio clip
    void SetAudioClipVolume(const std::string& audioClipId, float volume);
    // Get the master volume
    float GetMasterVolume() const;
    // Get the volume of an audio clip
    float GetAudioClipVolume(const std::string& audioClipId) const;

    // Add a tag to an audio clip
    void AddAudioClipTag(const std::string& audioClipId, const std::string& tag);
    // Remove a tag from an audio clip
    void RemoveAudioClipTag(const std::string& audioClipId, const std::string& tag);
    // Get the tags associated with an audio clip
    std::vector<std::string> GetAudioClipTags(const std::string& audioClipId) const;
    // Get all audio tags
    const std::unordered_map<std::string, std::vector<std::string>>& GetAudioTags() const;

private:
    // Private constructor and destructor
    AudioManager();
    ~AudioManager();

    // OpenAL device and context
    ALCdevice* device;
    ALCcontext* context;



    // Map to store loaded audio clips and their data
    std::unordered_map<std::string, AudioClipData> audioClips;

    // List of loaded audio clip IDs
    std::vector<std::string> loadedAudioClips;

    // Map to store active audio sources
    std::unordered_map<std::string, ALuint> audioSources;

    // Map to store audio tags and associated audio clip IDs
    std::unordered_map<std::string, std::vector<std::string>> audioTags;

    // ID of the current audio clip
    std::string currentAudioClip;

    // ID of the currently playing audio clip
    std::string currentPlayingAudioClip;

    // Master volume
    float masterVolume = 0.5f;

    // Update audio volumes based on master volume and individual clip volumes
    void UpdateAudioVolumes();
};