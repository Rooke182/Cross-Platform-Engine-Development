#include "Animation.h"

Animation Animation::CreateBasic(std::string textureName, float frameWidth, float frameHeight, int frameCount, float fps)
{
    Animation anim{};
    anim.m_fps = fps;

    auto texture = Resources::GetTexture(textureName);
    anim.m_textures.push_back(texture);

    float x = 0.0f;
    float y = 0.0f;
    float textureWidth = (float)texture->m_width;

    for (int i = 0; i < frameCount; i++)
    {
        Frame frame{};

        frame.m_frameIndex = i;
        frame.m_x = x;
        frame.m_y = y;
        frame.m_width = frameWidth;
        frame.m_height = frameHeight;
        frame.m_offsetX = 0.0f;
        frame.m_offsetY = 0.0f;
        frame.m_textureIndex = (int)anim.m_textures.size() - 1;

        x += frameWidth;
        if (x >= textureWidth) //new row
        {
            x = 0.0f;
            y += frameHeight;
        }
        anim.AddFrame(frame);
    }
    return anim;
}

nlohmann::json Animation::to_json() const
{
    nlohmann::json json;

    json["m_frames"] = m_frames;
    json["m_fps"] = m_fps;

    nlohmann::json textureJson;
    for (const auto& texture : m_textures)
    {
        textureJson.push_back(texture->m_path);
    }
    json["m_textures"] = textureJson;

    return json;
}

void Animation::from_json(const nlohmann::json& json)
{
    m_frames = json["m_frames"];
    m_fps = json["m_fps"];

    for (const auto& texture : json["m_textures"])
    {
        m_textures.push_back(Resources::GetTexture(texture));
    }
}

void to_json(nlohmann::json& json, const Frame& frame)
{
    json["m_frameIndex"] = frame.m_frameIndex;
    json["m_x"] = frame.m_x;
    json["m_y"] = frame.m_y;
    json["m_width"] = frame.m_width;
    json["m_height"] = frame.m_height;
    json["m_offsetX"] = frame.m_offsetY;
    json["m_frameIndex"] = frame.m_frameIndex;
    json["m_textureIndex"] = frame.m_textureIndex;
}

void from_json(const nlohmann::json& json, Frame& frame)
{
    frame.m_frameIndex = json["m_frameIndex"];
    frame.m_x = json["m_x"];
    frame.m_y = json["m_y"];
    frame.m_width = json["m_width"];
    frame.m_height = json["m_height"];
    frame.m_offsetX = json["m_offsetX"];
    frame.m_frameIndex = json["m_frameIndex"];
    frame.m_textureIndex = json["m_textureIndex"];
}

void to_json(nlohmann::json& json, const Animation& animation)
{
    json = animation.to_json();
}

void from_json(const nlohmann::json& json, Animation& animation)
{
    animation.from_json(json);
}
