#pragma once
#include "RenderComponent.h"
#include "Resources.h"
#include "PostProcess.h"
class Application;


class PostProcessRenderComponent : public RenderComponent
{
private:
	std::vector<PostProcess*> m_postProcesses;
	int m_lastWidth = 0;
	int m_lastHeight = 0;
public:
	virtual void Init() override;
	virtual void PreRender() override;
	virtual void Render() override { /*do nothing lol*/ }
	virtual void PostRender() override;
	virtual void Cleanup() override;

	void AddPostProcess(std::string name, std::string vertexPath, std::string fragmentPath, std::vector<IPostProcessValue*> postProcessValues);
	PostProcess* GetPostProcess(std::string name);

	CreateComponentID(PostProcessRenderComponent);


	std::string GetName() override { return "PostProcessRenderComponent"; };
};

