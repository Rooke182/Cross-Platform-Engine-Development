#include "AnimatorComponent.h"
#include "SpriteRenderComponent.h"
#include "Scene.h"
#include "Animation.h"

void AnimatorComponent::Update(Time time)
{
	if (m_paused)
		return;

	if (ContainsAnimation(m_currentAnimationName) && !m_animationFinished)
	{
		auto anim = GetAnimation(m_currentAnimationName);


		if (m_currentFrameIndex < anim.GetFrameCount()) //if anim not finished
		{
			float frameTime = 1000.0f / anim.GetFrameRate();
			m_currentFrameElapsedTime += time.m_deltaTime * m_speed;

			if (m_currentFrameElapsedTime >= frameTime) //advance frame
			{
				m_currentFrameElapsedTime = 0.0;
				m_currentFrameIndex++;


					

				if (m_loopAnimation && m_currentFrameIndex >= anim.GetFrameCount())
				{
					m_currentFrameIndex = 0; //restart animation
					m_animationFinished = false;
				}
				else
				{
					if (m_currentFrameIndex >= anim.GetFrameCount()) //check if above max frame count
					{
						m_currentFrameIndex = anim.GetFrameCount() - 1;
						m_animationFinished = true;
					}
				}
			}
		}
	}

}

void AnimatorComponent::AddAnimation(std::string name, Animation anim)
{
	m_animations[name] = anim;
}

void AnimatorComponent::PlayAnimation(std::string name, bool isLooped, int startingFrame)
{
	if (ContainsAnimation(name))
	{
		if (GetAnimation(name).GetFrameCount() <= 0)
			return; //the animation has no frames!

		bool alreadyPlaying = m_currentAnimationName == name;

		m_currentAnimationName = name;
		m_loopAnimation = isLooped;
		m_animationFinished = false;
		if (!alreadyPlaying)
		{
			m_currentFrameIndex = startingFrame;
			m_currentFrameElapsedTime = 0.0f;
		}

		m_paused = false; //unpause

		if (m_currentFrameIndex >= GetAnimation(name).GetFrameCount()) //check if above max frame count
			m_currentFrameIndex = GetAnimation(name).GetFrameCount() - 1;
	}
}

inline bool AnimatorComponent::ContainsAnimation(std::string name)
{
	return m_animations.find(name) != m_animations.end();
}

inline Animation AnimatorComponent::GetAnimation(std::string name)
{
	return m_animations[name];
}

Frame AnimatorComponent::GetCurrentFrame()
{
	if (ContainsAnimation(m_currentAnimationName))
		return GetAnimation(m_currentAnimationName).GetFrame(m_currentFrameIndex);
	return Frame{};
}

std::shared_ptr<Texture> AnimatorComponent::GetFrameTexture()
{
	if (ContainsAnimation(m_currentAnimationName))
	{
		auto anim = GetAnimation(m_currentAnimationName);
		return anim.GetTextureFromIndex(anim.GetFrame(m_currentFrameIndex).m_textureIndex);
	}
	return nullptr;
}

bool AnimatorComponent::IsCurrentAnimationValid()
{
	return ContainsAnimation(m_currentAnimationName) && GetAnimation(m_currentAnimationName).GetFrameCount() > 0;
}

void AnimatorComponent::Init()
{
	if (m_parent->GetComponentOfType<SpriteRenderComponent>() != nullptr)
		m_parent->GetComponentOfType<SpriteRenderComponent>()->m_animator = m_parent->GetComponentOfType<AnimatorComponent>();
	BaseComponent::Init();
}

void AnimatorComponent::Pause()
{
	m_paused = true;
}

void AnimatorComponent::Resume()
{
	m_paused = false;
}

void AnimatorComponent::RenderComponentMenu(int ID)
{
#ifdef DEBUGGER

	if (m_parent->GetComponentOfType<SpriteRenderComponent>() == nullptr)
		return;

	std::string label = "Animator Component##" + std::to_string(ID);
	if (ImGui::CollapsingHeader(label.c_str()))
	{
		std::string info = "";
		DebuggerFunctions::TextOutput("Current Animation: ", m_currentAnimationName);// info += "Current Animation:" + m_currentAnimationName + "\n";
		DebuggerFunctions::TextOutput("Frame Index: ", std::to_string(m_currentFrameIndex)); //info += "Frame Index:" + std::to_string(m_currentFrameIndex) + "\n";
		DebuggerFunctions::TextOutput("Frame current time: ", std::to_string(m_currentFrameElapsedTime));// info += "Frame current time:" + std::to_string(m_currentFrameElapsedTime) + "\n";
		ImGui::Text("%s", info.c_str());

		static std::string curAnimName = "";

		if (IsCurrentAnimationValid())
		{
			auto tex = GetFrameTexture();
			auto frame = GetCurrentFrame();
			if (tex != nullptr) {

				float uvX = frame.m_x / (float)tex->m_width;
				float uvY = frame.m_y / (float)tex->m_height;
				float uvRight = (frame.m_x + frame.m_width) / (float)tex->m_width;
				float uvBottom = (frame.m_y + frame.m_height) / (float)tex->m_height;

				ImGui::Image((void*)(intptr_t)tex->m_textureID, ImVec2(256, 256),
					ImVec2(uvX, uvY), ImVec2(uvRight, uvBottom));
			}
			if (DebuggerFunctions::Button("Pause"))
				Pause();
			ImGui::SameLine();
			if (DebuggerFunctions::Button("Resume"))
				Resume();

			
		}

		std::vector<std::string> animList;
		for (auto& kv : m_animations)
			animList.push_back(kv.first);
		std::string newanim = "";
		newanim = DebuggerFunctions::DropDownPicker("Animations: ", "##anim picker", &m_currentAnimationName, animList);
		if (newanim != "")
		{
			curAnimName = newanim;
			PlayAnimation(newanim);
		}

		DebuggerFunctions::TextInput("Name: ", "##animname", &curAnimName);
		if (DebuggerFunctions::Button("Add"))
		{
			Animation anim{};
			anim.m_fps = 24;
			anim.m_textures.push_back(Resources::GetTexture(m_parent->GetComponentOfType<SpriteRenderComponent>()->m_texturePath));

			Frame frame{};
			frame.m_frameIndex = 0;
			frame.m_textureIndex = 0;
			frame.m_width = anim.m_textures[0]->m_width;
			frame.m_height = anim.m_textures[0]->m_height;
			frame.m_x = 0;
			frame.m_y = 0;
			anim.m_frames.push_back(frame);
			//auto texture = Resources::GetTexture(textureName);
			//anim.m_textures.push_back(texture);
			AddAnimation(curAnimName, anim);
		}
		ImGui::SameLine();
		if (DebuggerFunctions::Button("Remove"))
		{

		}

		if (ImGui::Button("Open Editor"))
		{
			DebuggerFunctions::OpenAnimatorEditor();
		}
		RemoveComponentMenu(ID);
	}
#endif
}
