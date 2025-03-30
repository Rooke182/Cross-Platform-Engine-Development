#include "TextRenderComponent.h"
#include "UIWidget.h"

FT_Library TextRenderComponent::ft;
bool TextRenderComponent::startedFT = false;

void TextRenderComponent::Init()
{
	if (!startedFT) //init library
	{
		if (FT_Init_FreeType(&ft))
		{
#ifdef ANDROID
            LOGV("Could not init freetype");
#endif
#ifdef DEBUGGER
			Debugger::AddError("Warning", "Could not init FreeType!", true);
#endif
		}
		startedFT = true;
	}

	std::string fontPath = Resources::FormatPath(m_font);
#ifdef ANDROID
    AAsset* asset = AAssetManager_open(Resources::GetAssetManager(), fontPath.c_str(), AASSET_MODE_UNKNOWN);
    if (asset == NULL)
    {
        __android_log_print(ANDROID_LOG_ERROR, "File Load Error", "%s", fontPath.c_str());
    }
    char* charData;
    size_t size = (size_t)AAsset_getLength(asset);
    charData = (char*)malloc(sizeof(char)*size);
    AAsset_read(asset, charData, size);
    AAsset_close(asset);

    if (FT_New_Memory_Face( ft,(const FT_Byte*)charData,size,0,&m_face ))
    {
        LOGV("could not load font");
    }
#else
    if (FT_New_Face(ft, fontPath.c_str(), 0, &m_face)) //load default font
	{
        #ifdef DEBUGGER
                Debugger::AddError("Warning", "Could not load font: " + m_font, true);
        #endif
	}
#endif


	auto texture = Resources::GetFontTexture(m_font, m_face, m_fontSize);
	m_renderData = new RenderData();
	m_fontTexture = texture;
	
	#ifdef OPENGL
	m_renderData->m_bufferType = GL_DYNAMIC_DRAW;
	#endif
	std::vector<Vertex> vertices = texture->GenerateVertices(m_text);
	std::vector<unsigned int> indices;
	m_renderData->m_useIndices = false;
	m_renderData->GenerateBuffers();
	m_renderData->BindVertexData(vertices, indices);
	#ifdef OPENGL
	m_renderData->m_shaderProgram = Resources::GetShaderProgram("assets/shaders/base", "assets/shaders/text");
	#endif



	m_renderData->m_texture = texture;

	if (m_parent != nullptr)
	{
		m_parent->GetTransform()->m_scale.x = 2;
		m_parent->GetTransform()->m_scale.y = 2;
	}
	else if (m_UIparent != nullptr)
	{
		m_UIparent->GetTransformComponent()->m_scale.x = 2;
		m_UIparent->GetTransformComponent()->m_scale.y = 2;
	}

	RenderComponent::Init();
}

void TextRenderComponent::Cleanup()
{
	FT_Done_Face(m_face); //make sure to clear the font
	RenderComponent::Cleanup();
}

void TextRenderComponent::SetText(std::string t)
{
	if (m_text != t)
	{
		m_dirtyText = true;
		m_text = t;
	}

	if (m_fontTexture == nullptr)
		return;
		

	if (m_dirtyText) //only update if needed
	{
		std::vector<Vertex> vertices = m_fontTexture->GenerateVertices(m_text);
		std::vector<unsigned int> indices;
		m_renderData->BindVertexData(vertices, indices);
		m_dirtyText = false;
	}
}

void TextRenderComponent::UpdateText() 
{
	std::vector<Vertex> vertices = m_fontTexture->GenerateVertices(m_text);
	std::vector<unsigned int> indices;
	m_renderData->BindVertexData(vertices, indices);
	m_dirtyText = false;
}

void TextRenderComponent::SetFont(std::string f)
{
	if (m_font != f)
	{
		m_dirtyText = true;
		m_font = f;
	}
		
	if (m_dirtyText)
	{
		FT_Done_Face(m_face);
		std::string fontPath = Resources::FormatPath(f);

#ifdef ANDROID
        AAsset* asset = AAssetManager_open(Resources::GetAssetManager(), fontPath.c_str(), AASSET_MODE_UNKNOWN);
        if (asset == NULL)
        {
            __android_log_print(ANDROID_LOG_ERROR, "File Load Error", "%s", fontPath.c_str());
        }
        char* charData;
        size_t size = (size_t)AAsset_getLength(asset);
        charData = (char*)malloc(sizeof(char)*size);
        AAsset_read(asset, charData, size);
        AAsset_close(asset);

        if (FT_New_Memory_Face( ft,(const FT_Byte*)charData,size,0,&m_face ))
        {
            LOGV("could not load font");
            return;
        }
#else
        if (FT_New_Face(ft, fontPath.c_str(), 0, &m_face)) //load font
		{
#ifdef DEBUGGER
			Debugger::AddError("Warning", "Could not load font: " + f, true);
#endif
			return;
		}
#endif


		m_font = f;
		auto texture = Resources::GetFontTexture(m_font, m_face, m_fontSize);
		m_fontTexture = texture;
		m_renderData->m_texture = texture;

		SetText(m_text);
	}
}

void TextRenderComponent::SetFontSize(unsigned int s)
{
	if (m_fontSize != s)
	{
		if (s > 0)
		{
			m_fontSize = s;
			m_dirtyText = true;
			SetFont(m_font);
		}
	}
}

void TextRenderComponent::RenderComponentMenu(int ID)
{
#ifdef DEBUGGER
	std::string label = "Text Renderer Component##" + std::to_string(ID);
	if (ImGui::CollapsingHeader(label.c_str()))
	{
		if (DebuggerFunctions::TextInput("Text:", "##Text", &m_text)) UpdateText();

		if (unsigned int _temp = (unsigned int)DebuggerFunctions::IntInput("Font Size:", "##FontSize", (float)m_fontSize)) SetFontSize(_temp);

		RemoveComponentMenu(ID);
	}
#endif
}
