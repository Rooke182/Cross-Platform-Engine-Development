#pragma once
#include "BaseComponent.h"
#include "Observer.h"
#include "AudioManager.h"
#include "PreBuild.h"
class FlappyCatController : public BaseComponent, public Observer
{
private:
    float m_yVelocity = 0;
    int m_pipesPassed = 0;
    float m_speed = 1;
public:
    virtual void KeyNotify(int key, int action) override;
    void Jump();
    virtual void CursorNotify(double xPos, double yPos) override {};
    virtual void MouseButtonNotify(int mouseButton, int action) override {};
    virtual void ScrollWheelNotify(double xOffset, double yOffset) override {}
#ifdef _WIN32
    virtual void ControllerNotify(int cid, GLFWgamepadstate* state, GLFWgamepadstate* prevState) override {};
#endif
#ifdef ANDROID
    virtual void AndroidGryoNotify(float x, float y, float z) override {}
    virtual void AndroidTouchNotify(bool down, float x, float y, int touchIndex) { if (down) Jump(); }
    virtual void AndroidMotionNotify(float x, float y, int touchIndex) {}
#endif

    virtual void Init() override;
    virtual void Cleanup() override;
    virtual void Update(Time time) override;

    void Reset();

    CreateComponentID(FlappyCatController);

    void RenderComponentMenu(int ID) override;
};

