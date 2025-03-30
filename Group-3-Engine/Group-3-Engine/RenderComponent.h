#pragma once
#include "BaseComponent.h"
#include "RenderData.h"
#include "ConstantBuffer.h"
#include "ComponentPtr.h"
#include "TransformComponent.h"
#include "CameraComponent.h"

class RenderComponent : public BaseComponent
{
protected:
	RenderData* m_renderData = nullptr;
	ConstantBuffer m_buffer{};
	ComponentPtr<TransformComponent> m_transform;
	ComponentPtr<CameraComponent> m_camera;
	friend class Debugger;
	friend class DebuggerFunctions;
	friend class DebuggerMenus;
public:
	virtual void Init() override;
	virtual void Render() override;
	virtual void Cleanup() override;
	void RenderComponentMenu(int ID) override;

	CreateComponentID(RenderComponent);
	ConstantBuffer* GetConstantBuffer() { return &m_buffer; }

	RenderData* GetRenderData() { return m_renderData; }

	glm::vec4 GetColor() { return m_buffer.color; }
	void SetColor(glm::vec4 col) { m_buffer.color = col; }
	void SetCamera(ComponentPtr<CameraComponent> camera) { m_camera = camera; }
	ComponentPtr<CameraComponent> GetCamera() { return m_camera; };

	std::string GetName() override { return "RenderComponent"; };
};
