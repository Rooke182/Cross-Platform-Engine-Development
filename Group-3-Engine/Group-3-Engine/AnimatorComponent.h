#pragma once
#include "BaseComponent.h"
#include "Animation.h"
#include <functional>
#include <string>
#include <map>
class AnimatorComponent : public BaseComponent
{
private: 
	std::map<std::string, Animation> m_animations;
	float m_speed = 1.0;
	bool m_paused = true;
	bool m_loopAnimation = true;
	bool m_animationFinished = false;

	std::string m_currentAnimationName = "";
	unsigned int m_currentFrameIndex = 0;
	float m_currentFrameElapsedTime = 0.0f;
	friend class Debugger;
	friend class DebuggerFunctions;
	friend class DebuggerMenus;
	
public:

	std::string GetName() override { return "AnimatorComponent"; };

	CreateComponentID(AnimatorComponent);
	SerialiseVariables(8, m_animations, m_speed, m_paused, m_loopAnimation, m_animationFinished, m_currentAnimationName, m_currentFrameIndex, m_currentFrameElapsedTime);

	virtual void Update(Time time) override;
	void AddAnimation(std::string name, Animation anim);
	void PlayAnimation(std::string name, bool isLooped = true, int startingFrame = 0);
	inline bool ContainsAnimation(std::string name);
	inline Animation GetAnimation(std::string name);
	std::string GetCurrentAnimationName() { return m_currentAnimationName; }
	bool IsAnimationFinished() { return m_animationFinished; }
	Frame GetCurrentFrame();
	std::shared_ptr<Texture> GetFrameTexture();
	bool IsCurrentAnimationValid();
	void Init();
	void Pause();
	void Resume();
	void RenderComponentMenu(int ID) override;


};

