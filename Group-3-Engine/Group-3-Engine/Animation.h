#pragma once
#include "Resources.h"
#include "BaseComponent.h"
#include <string>
#include <vector>

struct Frame
{
	int m_frameIndex;
	float m_x;
	float m_y;
	float m_width;
	float m_height;
	float m_offsetX;
	float m_offsetY;
	int m_textureIndex = 0;
};

class Animation
{
protected:
	std::vector<Frame> m_frames;
	std::vector<std::shared_ptr<Texture>> m_textures;
	float m_fps = 24;
public: 	
	inline void AddFrame(Frame frame) { m_frames.push_back(frame); }
	inline Frame GetFrame(unsigned int index) { return m_frames[index]; }
	inline unsigned int GetFrameCount() { return static_cast<unsigned int>(m_frames.size()); }
	inline float GetFrameRate() { return m_fps; }
	inline std::shared_ptr<Texture> GetTextureFromIndex(unsigned int index) { return m_textures[index]; }

	static Animation CreateBasic(std::string textureName, float frameWidth, float frameHeight, int frameCount, float fps = 24);

	friend class Debugger;

	nlohmann::json to_json() const;
	void from_json(const nlohmann::json& json);
	friend class AnimatorComponent;
	friend class DebuggerFunctions;
	friend class DebuggerMenus;
};

void to_json(nlohmann::json& json, const Frame& frame);
void from_json(const nlohmann::json& json, Frame& frame);

void to_json(nlohmann::json& json, const Animation& animation);
void from_json(const nlohmann::json& json, Animation& animation);