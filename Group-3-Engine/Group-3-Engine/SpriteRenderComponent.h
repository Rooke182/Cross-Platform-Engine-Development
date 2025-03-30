#pragma once
#include "RenderComponent.h"
#include "AnimatorComponent.h"
#include "Vertex.h"
#include "Resources.h"
#include <vector>
class GameObject;

class SpriteRenderComponent : public RenderComponent
{
public:
	SpriteRenderComponent(std::string texturePath = "")
	{
		m_texturePath = texturePath;
	}
	virtual void Init() override;
	virtual void Render() override;

	CreateComponentID(SpriteRenderComponent);

	void UpdateTexturePath(std::string texturePath = "");
	std::string m_texturePath = "";

	std::string GetName() override { return "SpriteRenderComponent"; };

private:

	ComponentPtr<AnimatorComponent> m_animator;
	friend class AnimatorComponent;
};

