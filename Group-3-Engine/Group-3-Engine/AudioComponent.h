#pragma once
#include "BaseComponent.h"
#include <string>

class AudioComponent : public BaseComponent {
public:
    AudioComponent();
    virtual ~AudioComponent();

    void PlaySound(const std::string& audioClipName);
    void StopSound(const std::string& audioClipName);
    void SetVolume(float volume);
    void SetPitch(float pitch);
    // Add more function declarations as needed

    CreateComponentID(AudioComponent);

    std::string GetName() override { return "AudioComponent"; };
};
