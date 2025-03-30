#pragma once
#ifdef DEBUGGER
#include "Debugger.h"
#endif
class Application;
#include "Observer.h"
#include "UIWidget.h"
#include "ComponentBuffer.h"
#include "ComponentPtr.h"
#include "Structs.h"

class Application;

	class UIScene : public Observer {
    protected:
        std::vector<UIWidget *> m_widgets;

        friend class Debugger;

    private:
        ComponentBuffer<16 * 1024> m_components;
    public:
        UIScene() {}

        virtual void Init();

        virtual void Update(Time time);

        virtual void Render();

        virtual void Cleanup();

        virtual void EndFrame();


        Vector2 WorldToScreen(glm::vec3 worldPos);

        void CursorNotify(double xPos, double yPos) override;

        void MouseButtonNotify(int mouseButton, int action) override;

        void KeyNotify(int key, int action) override {}

        void ScrollWheelNotify(double xOffset, double yOffset) override {}


#ifdef _WIN32
    virtual void ControllerNotify(int cid, GLFWgamepadstate* state, GLFWgamepadstate* prevState) override {}
#endif
#ifdef ANDROID

    virtual void AndroidGryoNotify(float x, float y, float z) {}

    virtual void AndroidMotionNotify(float x, float y, int touchIndex) {}

#endif

#ifdef ANDROID


    virtual void AndroidTouchNotify(bool down, float x, float y, int touchIndex) {
        for (auto widget: m_widgets) {
            if (widget->GetType() == BUTTON) {
                //convert transform to screenspace
                Vector2 screenCoords = WorldToScreen(widget->GetTransformComponent()->m_position);
                float width = widget->GetRenderComponent()->GetRenderData()->m_texture->m_width;
                float height = widget->GetRenderComponent()->GetRenderData()->m_texture->m_height;

                //button boundaries
                if (x > screenCoords.x && x < screenCoords.x + width) {
                    if (y > screenCoords.y && y < screenCoords.y + height) {
                        widget->OnClicked();
                    }
                }
            }
        }
    }
#endif

        UIWidget *GetWidget(UIType type);
        void OnClicked();
        void OnRelease();

        int AddWidget(UIWidget &&widget);


        template<typename T, typename... Args>
        ComponentPtr<T> MakeComponent(Args... args);

        template<typename T>
        ComponentPtr<T> GetComponentOfType();

    };

template<typename T, typename... Args>
inline ComponentPtr<T> UIScene::MakeComponent(Args... args)
{
    return m_components.CreateComponent<T>(args...);
}

template<typename T>
inline ComponentPtr<T> UIScene::GetComponentOfType()
{
    return m_components.GetComponentOfType<T>();
}
