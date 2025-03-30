#include "FlappyCatController.h"
#include "Scene.h"
#include "GameObject.h"
#include "TextRenderComponent.h"
#include "Input.h"
#include "Collider.h"

void FlappyCatController::KeyNotify(int key, int action)
{
#ifdef _WIN32
	if (key == GLFW_KEY_SPACE)
	{
		Jump();
	}
#endif
}

void FlappyCatController::Jump()
{
	m_parent->GetTransform()->m_angle.z = -360;
	m_yVelocity = 4500;
	AudioManager::GetInstance().StopAudioClip("meow");
	AudioManager::GetInstance().PlayAudioClip("meow");
}

void FlappyCatController::Init()
{
	auto input = m_parent->GetScene()->GetInput();
	if (input != nullptr)
		input->AddObserver(this, "FlappyCat");

	AudioManager::GetInstance().LoadAudioClip("meow", "assets/sounds/meow.wav");
	AudioManager::GetInstance().LoadAudioClip("hiss", "assets/sounds/hiss.wav");

	Reset();
	m_parent->GetComponentOfType<Collider>()->m_onCollide = [&](ComponentPtr<Collider> col)
	{
		AudioManager::GetInstance().PlayAudioClip("hiss");
		Reset();
	};
}

void FlappyCatController::Cleanup()
{
	auto input = m_parent->GetScene()->GetInput();
	if (input != nullptr)
		input->RemoveObserver(this);
}

void FlappyCatController::Update(Time time)
{

	auto transform = m_parent->GetTransform();
	float delta = time.m_deltaTime * 0.001;

	transform->m_position.y -= m_yVelocity * delta;
	transform->m_position.y += 3000 * delta;

	if (m_yVelocity > 0.0f)
		m_yVelocity -= 2500 * delta;
	else
		m_yVelocity = 0.0f;

	auto bottomPipes = m_parent->GetScene()->GetGameObjectsByName("bottomPipe");
	auto topPipes = m_parent->GetScene()->GetGameObjectsByName("topPipe");
	m_speed += 1000.0f * delta;
	for (int i = 0; i < topPipes.size(); i++)
	{
		topPipes[i]->GetTransform()->m_position.x -= (1000.0f+ m_speed) * delta;
		if (topPipes[i]->GetTransform()->m_position.x < -3000.0f)
		{
			topPipes[i]->GetTransform()->m_position.x += 9000.0f;
			m_pipesPassed++;
			std::string text = "Score: " + std::to_string(m_pipesPassed);
			m_parent->GetScene()->GetGameObjectByName("ScoreText")->GetComponentOfType<TextRenderComponent>()->SetText(text);
			m_parent->GetScene()->GetGameObjectByName("ScoreText")->GetTransform()->m_scale = { 10.0f, 10.0f, 10.0f };

			float randOffset = (float)((std::rand() / RAND_MAX) * 300.0f);
			topPipes[i]->GetTransform()->m_position.y = randOffset;
			bottomPipes[i]->GetTransform()->m_position.y = 3000.0f + randOffset;
		}
	}
	for (int i = 0; i < bottomPipes.size(); i++)
		bottomPipes[i]->GetTransform()->m_position.x = topPipes[i]->GetTransform()->m_position.x;

	m_parent->GetTransform()->m_angle.z = std::lerp(m_parent->GetTransform()->m_angle.z, 0, delta * 5.0f);


	//collision
	if (transform->m_position.y > 5000.0f)
		Reset();
	if (transform->m_position.y < -2000.0f)
		Reset();
}

void FlappyCatController::Reset()
{
	m_pipesPassed = 0;
	m_speed = 1;
	auto bottomPipes = m_parent->GetScene()->GetGameObjectsByName("bottomPipe");
	auto topPipes = m_parent->GetScene()->GetGameObjectsByName("topPipe");
	for (int i = 0; i < topPipes.size(); i++)
	{
		topPipes[i]->GetTransform()->m_position.x = (3000 * i) + 6000.0f;
	}
	std::string text = "Score: " + std::to_string(m_pipesPassed);
	m_parent->GetScene()->GetGameObjectByName("ScoreText")->GetComponentOfType<TextRenderComponent>()->SetText(text);
	m_parent->GetScene()->GetGameObjectByName("ScoreText")->GetTransform()->m_scale = { 10.0f, 10.0f, 10.0f };
	m_parent->GetTransform()->m_position.x = 2000.0f;
	m_parent->GetTransform()->m_position.y = 2000.0f;
}

void FlappyCatController::RenderComponentMenu(int ID)
{
#ifdef DEBUGGER
	std::string label = "Flappy Cat Component##" + std::to_string(ID);
	if (ImGui::CollapsingHeader(label.c_str()))
	{

		RemoveComponentMenu(ID);
	}
#endif
}
