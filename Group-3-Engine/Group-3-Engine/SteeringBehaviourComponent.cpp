#include "SteeringBehaviourComponent.h"
#include "GameObject.h"
#include "AIAgent.h"

SteeringBehaviourComponent::SteeringBehaviourComponent(ComponentPtr<NavigationComponent> navCom)
{
	m_navCom = navCom;
}

void SteeringBehaviourComponent::Init()
{
	BaseComponent::Init();
}

void SteeringBehaviourComponent::Update(Time time)
{
	if (m_wander)
	{
		if (waitTime > 0)
		{
			waitTime -= time.m_deltaTime;
		}
		else if (m_positionStack.size() == 0)
		{
			Wander();
		}
	}

	if (m_positionStack.size() != 0)
	{
		Vector2 a = ((m_positionStack.top() - m_pos) * 10);

		if (a.magnitude() > m_maxSpeed)
		{
			a = a.normalized() * m_maxSpeed;
		}

		m_pos.x += a.x;
		m_pos.y += a.y;
		if (glm::distance(m_pos.toGlmVec2(), m_positionStack.top().toGlmVec2()) < 0.1f)
		{
			m_positionStack.pop();
		}
	}
}


void SteeringBehaviourComponent::Seek(Vector2 seekPos)
{
	//Clear Position Stack
	for (size_t i = 0; i < m_positionStack.size(); i++)
	{
		m_positionStack.pop();
	}

	//Calculate Path
	std::vector<Cell> path = m_navCom->Pathfinding(Vector2(m_pos.x / 5, m_pos.y / 5), seekPos);

	//Set Position Stack To Path
	for (Cell posPoint : path)
	{
		m_positionStack.push(Vector2(posPoint.m_gridX * 5, posPoint.m_gridY * 5));
	}
}

void SteeringBehaviourComponent::Flee(Vector2 fleePos)
{
	for (GameObject gO : m_parent->GetScene()->m_gameObjects)
	{
		if (gO.GetComponentOfType<TilemapRenderComponent>() != NULL)
		{
			m_tilemapRC = gO.GetComponentOfType<TilemapRenderComponent>();
		}
	}

	std::vector<Vector2> tempStackVec;

	Vector2 direction = Vector2(m_pos.x / 5, m_pos.y  / 5) - fleePos;
	direction.x = glm::normalize(direction.toGlmVec3()).x;
	direction.y = glm::normalize(direction.toGlmVec3()).y;
	direction = direction * 0.5f;
	for (size_t i = 0; i < 1000; i++)
	{
		Cell currentCell = m_tilemapRC->GetTilemap().GetTile(Vector2(m_pos.x / 5, m_pos.y / 5) + (direction * i));
		if (currentCell.m_type == 0)
		{
			tempStackVec.push_back(Vector2(currentCell.m_gridX * 5, currentCell.m_gridY * 5));
		}
		else
		{
			std::reverse(tempStackVec.begin(), tempStackVec.end());
			for (Vector2 temp : tempStackVec)
			{
				m_positionStack.push(temp);
			}
			return;
		}
	}
}

void SteeringBehaviourComponent::WanderHelper()
{
	m_wander = !m_wander;

	if (!m_wander)
	{
		return;
	}

	Wander();
}

void SteeringBehaviourComponent::Wander()
{
	for (GameObject gO : m_parent->GetScene()->m_gameObjects)
	{
		if (gO.GetComponentOfType<TilemapRenderComponent>() != NULL)
		{
			m_tilemapRC = gO.GetComponentOfType<TilemapRenderComponent>();
		}
	}
	int xPos = m_pos.x / 5;
	int yPos = (m_pos.y) / 5;

	int movementType = rand() % ENUM_COUNT;
	if (movementType == MovementType::Wait)
	{
		waitTime = rand() % 2500;
	}
	else if (movementType == MovementType::Move)
	{
		int direction = rand() % 4;
		int moveLength = rand() % 6;
		switch (direction)
		{
		case 0:
			for (size_t i = 0; i < moveLength; i++)
			{
				Cell currentCell = m_tilemapRC->GetTilemap().GetTile(Vector2(xPos, yPos + 1));
				if (currentCell.m_type == 0)
				{
					m_positionStack.push(Vector2(currentCell.m_gridX * 5, currentCell.m_gridY * 5 ));
				}
				else
				{
					return;
				}
			}
			break;
		case 1:
			for (size_t i = 0; i < moveLength; i++)
			{
				Cell currentCell = m_tilemapRC->GetTilemap().GetTile(Vector2(xPos + 1, yPos));
				if (currentCell.m_type == 0)
				{
					m_positionStack.push(Vector2(currentCell.m_gridX * 5, currentCell.m_gridY * 5 ));
				}
				else
				{
					return;
				}
			}
			break;
		case 2:
			for (size_t i = 0; i < moveLength; i++)
			{
				Cell currentCell = m_tilemapRC->GetTilemap().GetTile(Vector2(xPos, yPos - 1));
				if (currentCell.m_type == 0)
				{
					m_positionStack.push(Vector2(currentCell.m_gridX * 5, currentCell.m_gridY * 5));
				}
				else
				{
					return;
				}
			}
			break;
		case 3:
			for (size_t i = 0; i < moveLength; i++)
			{
				Cell currentCell = m_tilemapRC->GetTilemap().GetTile(Vector2(xPos - 1, yPos));
				if (currentCell.m_type == 0)
				{
					m_positionStack.push(Vector2(currentCell.m_gridX * 5, currentCell.m_gridY * 5 ));
				}
				else
				{
					return;
				}
			}
			break;
		default:
			break;
		}
	}
}