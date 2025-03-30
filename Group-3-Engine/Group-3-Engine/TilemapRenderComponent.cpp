#include "TilemapRenderComponent.h"
#include "Application.h"
#include "GameObject.h"

TilemapRenderComponent::TilemapRenderComponent(TilemapData m_tilemapData) { this->m_tilemapData = m_tilemapData; }


void TilemapRenderComponent::Init()
{

	//m_tilemapData.LoadCSV(m_tilemapData.m_lastCSVPath, true);

	m_renderData = new RenderData();
	if (m_tilemapData.m_texturePath != "")
	{
		auto texture = Resources::GetTexture(m_tilemapData.m_texturePath);
		m_renderData->m_texture = texture;
	}

	float textureWidth = static_cast<float>(m_renderData->m_texture->m_width);
	float textureHeight = static_cast<float>(m_renderData->m_texture->m_height);
	m_renderData->m_useIndices = false;

	m_vertices.clear();
	for (auto& tile : m_tilemapData.m_grid)
	{
		tile.m_isWalkable = true;
		if (tile.m_type != 0)
		{

			tile.m_isWalkable = false;

			float U = m_tilemapData.GetUVColumn(tile.m_type) * m_tilemapData.m_tileWidth;
			float V = m_tilemapData.GetUVRow(tile.m_type) * m_tilemapData.m_tileHeight;

			float gridX = tile.m_gridX * m_width;
			float gridY = tile.m_gridY * m_width;

			float u0 = U / textureWidth;
			float v0 = V / textureHeight;
			float u1 = (U + m_tilemapData.m_tileWidth) / textureWidth;
			float v1 = (V + m_tilemapData.m_tileHeight) / textureHeight;

			// Push vertices
			m_vertices.push_back({ gridX, gridY, 0.0f, u0, v0 });
			m_vertices.push_back({ gridX + m_width, gridY, 0.0f, u1, v0 });
			m_vertices.push_back({ gridX, gridY + m_width, 0.0f, u0, v1 });

			m_vertices.push_back({ gridX, gridY + m_width, 0.0f, u0, v1 });
			m_vertices.push_back({ gridX + m_width, gridY, 0.0f, u1, v0 });
			m_vertices.push_back({ gridX + m_width, gridY + m_width, 0.0f, u1, v1 });
		}
	}

	m_parent->GetTransform()->m_width = static_cast<float>(m_tilemapData.m_tileWidth);
	m_parent->GetTransform()->m_height = static_cast<float>(m_tilemapData.m_tileWidth);//keep same aspect ratio

	m_renderData->GenerateBuffers();
	m_renderData->BindVertexData(m_vertices, m_indices);
#ifdef OPENGL
	m_renderData->m_shaderProgram = Resources::GetShaderProgram("assets/shaders/base", "assets/shaders/base");
#endif


	RenderComponent::Init();
}


void TilemapRenderComponent::RenderComponentMenu(int ID)
{
#ifdef DEBUGGER
    std::string label = "Tile Map Renderer Component##" + std::to_string(ID);
    if (ImGui::CollapsingHeader(label.c_str()))
    {
		m_tilemapData.m_lastCSVPath = DebuggerFunctions::Dropper("Path: ", "##TileMapPath", &m_tilemapData.m_lastCSVPath, "Text");

        if (DebuggerFunctions::Button("TileMapEditor"))
        {
            DebuggerFunctions::OpenTileMapEditor();
        }
        ImGui::SameLine();
        if (DebuggerFunctions::Button("Reload"))
        {
			m_tilemapData.LoadCSV(m_tilemapData.m_lastCSVPath, true);
            Cleanup();
            Init();
        }
		if (ImGui::Checkbox("Serialize grid?", &m_tilemapData.m_serializeGrid))
		{

		}
		RemoveComponentMenu(ID);
    }
#endif
}

Vector2 TilemapRenderComponent::GetCellWorld(Vector2 pos)
{
	auto aspectScale = glm::vec3((float)Application::m_windowWidth / m_camera->m_width, (float)Application::m_windowHeight / m_camera->m_height, 1.0f);
	auto aspect = glm::scale(glm::mat4(1.0), aspectScale);

	// Adjust mouse coordinates based on camera zoom level
	pos.x /= m_camera->m_zoom;
	pos.y /= m_camera->m_zoom;
	pos.x /= aspectScale.x;
	pos.y /= aspectScale.y;	

	//// Adjust mouse coordinates based on camera position and screen dimensions
	float screenWidth = 1280 / m_camera->m_zoom; // Adjust screen width based on zoom level
	pos.x -= (m_camera->m_x + screenWidth / 2);  // Adjust for camera X position and screen center


	float screenHeight = 720 / m_camera->m_zoom; // Adjust screen height based on zoom level
	pos.y -= (m_camera->m_y + screenHeight / 2);

	return m_tilemapData.FindNearestCell(pos);
}

