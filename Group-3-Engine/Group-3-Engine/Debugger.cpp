#include "Debugger.h"
#ifdef DEBUGGER
#include "comboFilter.h"
#include "GameObject.h"
#include "Collider.h"
#include "ComponentPtr.h"
#include "glm/gtc/type_ptr.hpp"
#include "Scene.h"
#include <string>
#include "Application.h"
#include "ProceduralGeneration.h"
#include "PreBuild.h"
#include <typeinfo>
#include <type_traits>
#include <fstream>

#include "AIAgent.h"
#include "AnimatorComponent.h"
#include "AudioComponent.h"
#include "CameraComponent.h"
#include "Collider.h"
#include "ColliderRenderComponent.h"
#include "CollisionManager.h"
#include "InputObserverComponent.h"
#include "NavigationComponent.h"
#include "PhysicsComponent.h"
#include "PostProcessRenderComponent.h"
#include "RotatorComponent.h"
#include "SpriteRenderComponent.h"
#include "SteeringBehaviourComponent.h"
#include "TextRenderComponent.h"
#include "TilemapRenderComponent.h"
#include "TimerManager.h"
#include "TransformComponent.h"
#include "UnusedClearComponent.h"
#include "VirtualButtonComponent.h"
#include "PreBuild.h"

bool Debugger::m_gamePaused = false;
bool Debugger::m_uiPaused = false;
bool Debugger::m_loggerWindowOpen = false;
bool Debugger::m_sceneWindowOpen = false;
bool Debugger::m_tilemapWindowOpen = false;
bool Debugger::m_tilemapTypeChooserOpen = false;
bool Debugger::m_assetsWindowOpen = false;
bool Debugger::m_statsWindowOpen = false;
bool Debugger::m_gameObjectWindow = false;
bool Debugger::m_animationEditorOpen = false;
bool Debugger::m_sceneSelectorWindowOpen = false;
bool Debugger::m_cameraControlMode = false;
bool Debugger::m_tilemapSelector = false;

int Debugger::m_aISpawnPosGridCoords[2] = { 0, 0 };

// Testing for Audio------------------------------------
bool Debugger::m_audioWindowOpen = false;
std::string Debugger::m_audioClipName = "";
//------------------------------------------------------

Vector2 Debugger::m_aISelectedPos;
int Debugger::m_gameobjectCount = 0;
int Debugger::m_indexedGameobject = 0;
GameObject* Debugger::m_gameobjectSelected = nullptr;

Scene* Debugger::m_scene = nullptr;
Input* Debugger::m_input = nullptr;
Application* Debugger::m_app = nullptr;
std::string Debugger::m_sceneNameToLoad = "";
std::string Debugger::m_sceneNameLoaded = "";

float Debugger::m_fps = 0;
float Debugger::m_fpsLastUpdate = 0;
float Debugger::m_fpsAverage = 0;
int Debugger::m_frameCount = 0;
std::vector<float> Debugger::m_frameTimes;
float Debugger::m_fpsOnePercentLows = 0.0f;
float Debugger::m_fpsOnePercentHighs = 0.0f;
float Debugger::m_deltaTimeOnePercentLows = 0.0f;
float Debugger::m_deltaTimeOnePercentHighs = 0.0f;
int Debugger::buffersize = 1000;
float Debugger::m_fpsMax = 0;
float Debugger::m_fpsMin = 1000000;
float Debugger::m_deltaTimesSeconds = 0;
float Debugger::m_sampleInterval = 1.0f;
float Debugger::m_deltaTimesSecondsAverage = 0.0f;
float Debugger::m_deltaTimesSecondsSum = 0.0f;
int Debugger::tileMapType = 1;
std::vector<std::string> Debugger::m_assests;
std::vector<std::string> Debugger::m_componentList;
std::vector<Cell> Debugger::m_newGrid;
std::vector<int> Debugger::m_aIAgents;

// Testing for Audio------------------------------------
float Debugger::m_listenerPosX = 0.0f;
float Debugger::m_listenerPosY = 0.0f;
float Debugger::m_sourcePosX = 0.0f;
float Debugger::m_sourcePosY = 0.0f;
bool Debugger::m_isDraggingListener = false;
bool Debugger::m_isDraggingSource = false;
std::unordered_set<std::string> Debugger::selectedTags;

bool Debugger::compareAudioClipNames(const std::string& a, const std::string& b)
{
    static bool sortAToZ = true; // Define sortAToZ as a static variable
    return sortAToZ ? a < b : a > b;
}

bool Debugger::isAudioClipSelected(const std::string& clipName)
{
    auto clipTags = AudioManager::GetInstance().GetAudioClipTags(clipName);
    return Debugger::GetSelectedTags().empty() || std::any_of(clipTags.begin(), clipTags.end(), [](const std::string& tag) {
        return Debugger::GetSelectedTags().find(tag) != Debugger::GetSelectedTags().end();
        });
}
//------------------------------------------------------

std::vector<std::string> Debugger::m_messages;
std::map<std::string, unsigned int> Debugger::m_componentStringIDMap;

#ifndef ANDROID
void Debugger::Init(GLFWwindow* window, Scene* scene)
#else
void Debugger::Init(ANativeWindow* window, Scene* scene)
#endif
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();
#ifndef ANDROID
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
#else
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui_ImplAndroid_Init(window);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    ImGui::GetStyle().ScaleAllSizes(5.0f);

#endif


    m_componentStringIDMap = GetComponentStringIDMap();


    std::string path = "assets/scenes/";
#ifndef ANDROID
    for (auto& entry : std::filesystem::directory_iterator(path))
        m_assests.push_back(entry.path().string());
#else
    auto dir = AAssetManager_openDir(Resources::GetAssetManager(), "scenes/");
    while(true) //search inside folder
    {
        auto name = AAssetDir_getNextFileName(dir);
        if (name == nullptr) //nothing left
            break;
        else
        {
            std::string filePath = name;
            filePath = "assets/scenes/"+filePath;
            m_assests.push_back(filePath);
        }
    }
    AAssetDir_close(dir);
#endif


    m_scene = scene;

}

void Debugger::Render(Time _time)
{
    RunPerformance(_time);

    ImGui_ImplOpenGL3_NewFrame();
#ifndef ANDROID
    ImGui_ImplGlfw_NewFrame();
#else
    ImGui_ImplAndroid_NewFrame();
#endif
    ImGui::NewFrame();
    //TODO: Fix rendering over everything else
    //ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::Button("Save"))
        {
            m_scene->WriteToJson(Debugger::m_sceneNameLoaded);
        }
        if (ImGui::Button("Scene Selector"))
        {
            m_sceneSelectorWindowOpen = true;
        }
        if (ImGui::Button("Logger")) {
            m_loggerWindowOpen = true;
        }
        if (ImGui::Button("Stats"))
        {
            m_statsWindowOpen = true;
        }
        if (ImGui::Button("Scene"))
        {
            m_sceneWindowOpen = true;
        }
        if (ImGui::Button("Tilemap Editor"))
        {
            m_tilemapWindowOpen = true;
        }
        if (ImGui::Button("Tilemap Type Chooser"))
        {
            m_tilemapTypeChooserOpen = true;
        }
        if (ImGui::Button("Assets"))
        {
            m_assetsWindowOpen = true;
        }
        ImGui::Checkbox("Camera Control", &m_cameraControlMode);
        //Test for Audio-------------------------------------------------------------------
        if (ImGui::Button("Audio"))
        {
            m_audioWindowOpen = true;
        }
        //---------------------------------------------------------------------------------
        
        if (ImGui::Button("Pause")) 
        {
            m_gamePaused = true;
            Debugger::AddError("Error", "game state: paused", false);
        }
        if (ImGui::Button("Play"))
        {
            m_gamePaused = false; 
            Debugger::AddError("Warning", "game state: unpaused", false);
            Debugger::AddLog("Variable Writing proof: %f", m_fps);
        }

        if (ImGui::Button("UI on"))m_uiPaused = false;
        if (ImGui::Button("UI off"))m_uiPaused = true;


        ImGui::EndMainMenuBar();
    }

    if (m_loggerWindowOpen) DebuggerMenus::LoggerWindow();
    if (m_sceneWindowOpen) DebuggerMenus::SceneWindow();
    if (m_statsWindowOpen) DebuggerMenus::StatsWindow();
    if (m_gameObjectWindow) DebuggerMenus::GameObjectWindow();
    if (m_assetsWindowOpen) DebuggerMenus::AssetViewerMenu();
    if (m_tilemapWindowOpen) DebuggerMenus::TilemapEditor();
    if (m_tilemapTypeChooserOpen) DebuggerMenus::TileMapSelector();
    if (m_tilemapTypeChooserOpen) DebuggerMenus::TileMapRendererFunctionality();
    if (m_animationEditorOpen) DebuggerMenus::AnimatorMenu();
    if (m_cameraControlMode) DebuggerMenus::CameraControl();
    if (m_sceneSelectorWindowOpen) DebuggerMenus::SceneSelector();

    DebuggerMenus::SetAITargetPos();

    //Testing for Audio-----------------------------------------------------------------------------------------------------------------------------



    if (m_audioWindowOpen)
    {
        if (ImGui::Begin("Audio", &m_audioWindowOpen))
        {
            static std::string filePath;

            /*if (ImGui::Button("Import"))
            {
                ImGui::OpenPopup("Enter Audio Clip Name");
            }*/

            ImGui::InputText("Audio Clip Name", &m_audioClipName);
            if (ImGui::BeginDragDropTarget())
            {
                std::string file = DebuggerFunctions::AssetDropTarget("Audio"); //use image type to only accept image files
                if (file != "")
                {
                    m_audioClipName = file;
                    AudioManager::GetInstance().LoadAudioClip(m_audioClipName, m_audioClipName);
                    AudioManager::GetInstance().SetCurrentAudioClip(m_audioClipName);
                }
                ImGui::EndDragDropTarget();
            }

            //ImGui::Text("Drop Audio Here");

            //ImGui::SameLine();

            static bool sortAToZ = true;
            if (ImGui::Button(sortAToZ ? "Sort (A to Z)" : "Sort (Z to A)"))
            {
                sortAToZ = !sortAToZ;
            }

            ImGui::SameLine();

            static bool filterPopupOpen = false;
            if (ImGui::Button("Filter"))
            {
                filterPopupOpen = true;
                ImGui::OpenPopup("Filter Popup");
            }

            if (ImGui::BeginPopup("Filter Popup"))
            {
                auto audioTags = AudioManager::GetInstance().GetAudioTags();

                for (const auto& pair : audioTags) {
                    const std::string& tag = pair.first;
                    bool isSelected = Debugger::GetSelectedTags().find(tag) != Debugger::GetSelectedTags().end();

                    if (ImGui::Checkbox(("#" + tag).c_str(), &isSelected)) {
                        if (isSelected) {
                            Debugger::GetSelectedTags().insert(tag);
                        }
                        else {
                            Debugger::GetSelectedTags().erase(tag);
                        }
                    }
                }

                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal("Enter Audio Clip Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::InputText("Audio Clip Name", &m_audioClipName);

                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    std::string audioClipName(m_audioClipName);
                    AudioManager::GetInstance().LoadAudioClip(audioClipName, filePath);
                    AudioManager::GetInstance().SetCurrentAudioClip(audioClipName);
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();

                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            // Master volume control
            float masterVolume = AudioManager::GetInstance().GetMasterVolume() * 100.0f;
            ImGui::PushItemWidth(200.0f);
            if (ImGui::SliderFloat("Master Volume", &masterVolume, 0.0f, 100.0f, "%.0f%%"))
            {
                AudioManager::GetInstance().SetMasterVolume(masterVolume / 100.0f);
            }
            ImGui::PopItemWidth();

            // Display loaded audio clips and control playback
            ImGui::Text("Loaded Audio Clips:");
            auto loadedClips = AudioManager::GetInstance().GetLoadedAudioClips();

            // Sort audio clips based on name
            std::sort(loadedClips.begin(), loadedClips.end(), Debugger::compareAudioClipNames);

            for (auto it = loadedClips.begin(); it != loadedClips.end(); ++it)
            {
                const std::string& clipName = *it;

                // Filter audio clips based on selected tags
                if (!Debugger::isAudioClipSelected(clipName)) {
                    continue;
                }

                ImGui::BulletText("%s", clipName.c_str());

                // Display assigned tags with "#" prefix
                auto clipTags = AudioManager::GetInstance().GetAudioClipTags(clipName);
                for (const auto& tag : clipTags) {
                    ImGui::SameLine();
                    ImGui::Text("#%s", tag.c_str());
                }

                ImGui::SameLine();
                if (ImGui::Button(("Add Tag##" + clipName).c_str())) {
                    ImGui::OpenPopup(("Tag Popup##" + clipName).c_str());
                }

                if (ImGui::BeginPopup(("Tag Popup##" + clipName).c_str())) {
                    auto audioTags = AudioManager::GetInstance().GetAudioTags();

                    static char newTagBuffer[256] = "";
                    ImGui::InputText("New Tag", newTagBuffer, IM_ARRAYSIZE(newTagBuffer));
                    ImGui::SameLine();
                    if (ImGui::Button("Add New Tag")) {
                        std::string newTag(newTagBuffer);
                        if (!newTag.empty()) {
                            AudioManager::GetInstance().AddAudioClipTag(clipName, newTag);
                            newTagBuffer[0] = '\0';
                        }
                    }

                    for (const auto& pair : audioTags) {
                        const std::string& tag = pair.first;
                        bool isAssigned = std::find(clipTags.begin(), clipTags.end(), tag) != clipTags.end();

                        if (ImGui::Checkbox(("#" + tag).c_str(), &isAssigned)) {
                            if (isAssigned) {
                                AudioManager::GetInstance().AddAudioClipTag(clipName, tag);
                            }
                            else {
                                AudioManager::GetInstance().RemoveAudioClipTag(clipName, tag);
                            }
                        }
                    }

                    ImGui::SameLine();
                    if (ImGui::Button(("Remove Tag##" + clipName).c_str()))
                    {
                        ImGui::OpenPopup(("Remove Tag Popup##" + clipName).c_str());
                    }

                    if (ImGui::BeginPopup(("Remove Tag Popup##" + clipName).c_str()))
                    {
                        for (const auto& tag : clipTags)
                        {
                            if (ImGui::Button(("Remove " + tag + "##" + clipName).c_str()))
                            {
                                AudioManager::GetInstance().RemoveAudioClipTag(clipName, tag);
                            }
                        }

                        ImGui::EndPopup();
                    }

                    ImGui::EndPopup();
                }

                ImGui::NewLine();

                if (ImGui::Button(("Play##" + clipName).c_str()))
                {
                    if (AudioManager::GetInstance().IsAudioClipPaused(clipName))
                    {
                        AudioManager::GetInstance().ResumeAudioClip(clipName);
                    }
                    else
                    {
                        AudioManager::GetInstance().PlayAudioClip(clipName);
                    }
                }

                ImGui::SameLine();
                if (ImGui::Button(("Stop##" + clipName).c_str()))
                {
                    AudioManager::GetInstance().StopAudioClip(clipName);
                }

                ImGui::SameLine();
                if (ImGui::Button(("Pause##" + clipName).c_str()))
                {
                    AudioManager::GetInstance().PauseAudioClip(clipName);
                }

                ImGui::SameLine();
                bool isLooping = AudioManager::GetInstance().IsAudioClipLooping(clipName);
                if (ImGui::Checkbox(("Loop##" + clipName).c_str(), &isLooping))
                {
                    AudioManager::GetInstance().SetAudioClipLooping(clipName, isLooping);
                }

                ImGui::SameLine();
                if (ImGui::Button(("Unload##" + clipName).c_str()))
                {
                    AudioManager::GetInstance().UnloadAudioClip(clipName);
                    it = loadedClips.erase(it);
                    if (it == loadedClips.end())
                        break;
                }

                // Audio clip volume control
                float audioClipVolume = AudioManager::GetInstance().GetAudioClipVolume(clipName) * 100.0f;
                ImGui::PushItemWidth(200.0f);
                if (ImGui::SliderFloat(("Volume##" + clipName).c_str(), &audioClipVolume, 0.0f, 100.0f, "%.0f%%"))
                {
                    AudioManager::GetInstance().SetAudioClipVolume(clipName, audioClipVolume / 100.0f);
                }
                ImGui::PopItemWidth();
            }

            ImGui::End();
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------------------------------------

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#ifdef _WIN32
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
#endif
}

void Debugger::Cleanup()
{
    m_messages.clear();
    SaveLogToFile();
    ImGui_ImplOpenGL3_Shutdown();
#ifndef ANDROID
    ImGui_ImplGlfw_Shutdown();
#else
    ImGui_ImplAndroid_Shutdown();
#endif
    ImGui::DestroyContext();

}

void Debugger::AddError(std::string LogType, const std::string& msg, bool exit)
{
    std::string log;
    //why no switch work?
    if (LogType == "Warning" || LogType == "1")
    {
        std::cout << "[WARNING]: " << msg << std::endl;
        log = "Warning: " + msg;
    }
    else if (LogType == "Error" || LogType == "2")
    {
        std::cout << "[ERROR]: " << msg << std::endl;
        log = "ERROR: " + msg;
    }
    else
    {
        log = msg;
    }
    m_messages.push_back(log);
    if (exit)
    {
        StackTrace();
        SaveLogToFile();
        abort();
    }

}

void Debugger::AddLog(const std::string& msg)
{
    m_messages.push_back(msg);
}

void Debugger::AddLog(const char* format, ...)
{
    int _buffer = 512;
    char _message[512];

    va_list _arguments;
    va_start(_arguments, format);
    vsnprintf(_message, _buffer, format, _arguments);
    va_end(_arguments);

    m_messages.push_back(std::string(_message));
}

void Debugger::SaveLogToFile()
{
#ifndef ANDROID
    const std::string folderPath = "DebugLogs"; // Specify the folder path

    if (!std::filesystem::exists(folderPath))
    {
        // Create the folder if it doesn't exist
        if (std::filesystem::create_directory(folderPath))
        {
            std::cout << "DebugLogs folder created successfully." << std::endl;
        }
        else
        {
            std::cerr << "Failed to create DebugLogs folder." << std::endl;
        }
    }

    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);

    std::tm now_tm;

    localtime_s(&now_tm, &now_time_t);

    char filename[80];
    strftime(filename, sizeof(filename), "DebugLogs\\debug_log%Y-%m-%d_%H-%M-%S.txt", &now_tm);

    std::ofstream fileStream(filename, std::ios::out | std::ios::app);
    if (!fileStream.is_open())
    {
        std::cerr << "Failed to open log file for writing: " << filename << std::endl;
        return;
    }

    for (const auto& msg : m_messages)
    {
        fileStream << msg << std::endl;
    }

    fileStream.close();
#endif
}

void Debugger::RunPerformance(Time _time)
{
    m_frameCount++;
    m_deltaTimesSeconds = _time.m_deltaTime / 1000.0f;
    m_fpsLastUpdate += m_deltaTimesSeconds;
    m_deltaTimesSecondsSum += m_deltaTimesSeconds;

    if (m_fpsLastUpdate > m_sampleInterval)
    {
        m_fps = 1.0f / m_deltaTimesSeconds;
        if (m_fpsMax < m_fps) m_fpsMax = m_fps;
        if (m_fpsMin > m_fps) m_fpsMin = m_fps;

        m_deltaTimesSecondsAverage = m_deltaTimesSecondsSum / m_frameCount;

        m_fpsLastUpdate = 0.0f;
        m_fpsAverage = (float)m_frameCount / (_time.m_timeSinceStart / 1000.0f);
        if (m_frameTimes.size() >= buffersize)
        {
            std::sort(m_frameTimes.begin(), m_frameTimes.end());

            // Calculate indices for both 1% lows and highs
            auto onePercentLowIndex = static_cast<size_t>(std::ceil(m_frameTimes.size() * 0.99)) - 1;
            auto onePercentHighIndex = m_frameTimes.size() - onePercentLowIndex - 1; // Adjusted for zero-based indexing

            // Access the frame times at these indices
            m_deltaTimeOnePercentLows = (float)m_frameTimes[onePercentLowIndex];
            m_deltaTimeOnePercentHighs = (float)m_frameTimes[onePercentHighIndex];

            m_fpsOnePercentLows = 1.0f / m_deltaTimeOnePercentLows;
            m_fpsOnePercentHighs = 1.0f / m_deltaTimeOnePercentHighs;
        }
    }
    m_frameTimes.push_back(m_deltaTimesSeconds);
}

void Debugger::ResetStats()
{
    //m_fps = 0;
    m_fpsLastUpdate = 0;
    m_fpsAverage = 0;
    m_frameCount = 0;
    m_frameTimes.clear();
    m_fpsOnePercentLows = 0.0f;
    m_fpsOnePercentHighs = 0.0f;
    m_deltaTimeOnePercentLows = 0.0f;
    m_deltaTimeOnePercentHighs = 0.0f;
    buffersize = 1000;
    m_fpsMax = 0;
    m_fpsMin = 1000000;
    m_deltaTimesSeconds = 0;
    m_sampleInterval = 1.0f;
}

void Debugger::StackTrace()
{
#ifndef ANDROID
    m_messages.push_back(cpptrace::generate_trace().to_string());
#endif
}

//------------------------------------------

glm::vec3 DebuggerFunctions::Vec3Input(const char* label, const char* inputLabel, glm::vec3 value, bool inputX, bool inputY, bool inputZ)
{
    std::string _newLabel = inputLabel + std::to_string(Debugger::m_indexedGameobject);
    float _x = value.x;
    float _y = value.y;
    float _z = value.z;

    std::string xLabel = _newLabel + "_x";
    std::string yLabel = _newLabel + "_y";
    std::string zLabel = _newLabel + "_z";

    float _windowWidth = ImGui::GetWindowWidth();

    float _split = 0;
    if (inputX) _split++;
    if (inputY) _split++;
    if (inputZ) _split++;
    ImGui::Text("%s", label); ImGui::SameLine(_windowWidth / 3);
    ImGui::PushItemWidth((_windowWidth / 3 * 2) / _split);
    if (inputX) { ImGui::DragFloat(xLabel.c_str(), &_x); }
    if (inputY) { if (inputX)ImGui::SameLine(); ImGui::DragFloat(yLabel.c_str(), &_y); }
    if (inputZ) { if (inputX || inputY)ImGui::SameLine(); ImGui::DragFloat(zLabel.c_str(), &_z); }
    ImGui::PopItemWidth;

    value = glm::vec3(_x, _y, _z);

    return value;
}

bool DebuggerFunctions::TextInput(const char* label, const char* inputLabel, std::string* text)
{
    std::string _newLabel = inputLabel + std::to_string(Debugger::m_indexedGameobject);

    float _windowWidth = ImGui::GetWindowWidth();
    ImGui::Text("%s", label); ImGui::SameLine(_windowWidth / 3);
    ImGui::PushItemWidth((_windowWidth / 3 * 2));

    if (ImGui::InputText(_newLabel.c_str(), text))
    {
        //AddLog("hi");
        return true;
    }
    return false;
}

void DebuggerFunctions::TextOutput(const char* label, std::string text)
{
    std::string _newLabel = label + std::to_string(Debugger::m_indexedGameobject);

    float _windowWidth = ImGui::GetWindowWidth();
    ImGui::Text("%s", label); ImGui::SameLine((_windowWidth + 11.5) / 3 * 2);
    ImGui::PushItemWidth((_windowWidth / 3));

    ImGui::BeginDisabled();
    if (ImGui::InputText(_newLabel.c_str(), &text)) {}
    ImGui::EndDisabled();
}

float DebuggerFunctions::FloatInput(const char* label, const char* inputLabel, float value)
{
    std::string _newLabel = inputLabel + std::to_string(Debugger::m_indexedGameobject);
    float _windowWidth = ImGui::GetWindowWidth();

    ImGui::Text("%s", label); ImGui::SameLine((_windowWidth + 11.5) / 3 * 2);
    ImGui::PushItemWidth((_windowWidth / 3));
    ImGui::DragFloat(_newLabel.c_str(), &value);


    return value;
}

float DebuggerFunctions::IntInput(const char* label, const char* inputLabel, float value)
{

    std::string _newLabel = inputLabel + std::to_string(Debugger::m_indexedGameobject);
    float _windowWidth = ImGui::GetWindowWidth();

    ImGui::Text("%s", label); ImGui::SameLine((_windowWidth + 11.5) / 3 * 2);
    ImGui::PushItemWidth((_windowWidth / 3));
    ImGui::DragFloat(_newLabel.c_str(), &value, 1.0, 1, 200, "%.0f");
    return value;
}

float DebuggerFunctions::IntArrowInput(const char* label, const char* inputLabel, float value)
{



    std::string _newLabel = inputLabel + std::to_string(Debugger::m_indexedGameobject);
    float _windowWidth = ImGui::GetWindowWidth();

    ImGui::Text("%s", label); ImGui::SameLine((_windowWidth + 11.5) / 3 * 2 - 54);
    ImGui::PushItemWidth((_windowWidth / 3));

    std::string _decrease = _newLabel + std::string("decrease");
    std::string _increase = _newLabel + std::string("increase");
    if (ImGui::ArrowButton(_decrease.c_str(), 0))
    {
        value--;
        if (value < 1)
            value = 1;
        //tilemap->CheckTexture();
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton(_increase.c_str(), 1))
    {
        value++;

        //tilemap->CheckTexture();
    }
    ImGui::SameLine((_windowWidth + 11.5) / 3 * 2);

    ImGui::DragFloat(_newLabel.c_str(), &value, 1.0, 1, 200, "%.0f");

    ImGui::PopItemWidth();
    return value;
}

std::string DebuggerFunctions::DropDownPicker(const char* label, const char* inputLabel, std::string* currentlySelected, std::vector<std::string> slecteable, bool changeVariable)
{
    std::string _newLabel = inputLabel + std::string(" ") + std::to_string(Debugger::m_indexedGameobject);
    float _windowWidth = ImGui::GetWindowWidth();
    const char* _tempp = currentlySelected->c_str();
    ImGui::Text("%s", label); ImGui::SameLine(_windowWidth / 3);
    ImGui::PushItemWidth((_windowWidth / 3 * 2));
    bool _selected = false;
    if (ImGui::BeginCombo(_newLabel.c_str(), _tempp))
    {
        for (int n = 0; n < slecteable.size(); n++)
        {
            bool is_selected = (_tempp == slecteable[n]);
            if (ImGui::Selectable(slecteable[n].c_str(), is_selected))
            {
                _tempp = slecteable[n].c_str();
                _selected = true;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    if (changeVariable)
        *currentlySelected = _tempp;
    if (_selected) return _tempp;
    return "";
}

std::string DebuggerFunctions::GameObjectDropDownPicker(const char* label, const char* inputLabel, std::string currentlySelected)
{
    std::vector<std::string> gameObjectNames;
    std::vector<std::string> gameObjectGUIDs;

    for (auto& obj : Debugger::m_scene->m_gameObjects)
    {
        std::string name = obj.GetName() + " (" + obj.GetGuid().ToString() + ")";
        gameObjectNames.push_back(name);
        gameObjectGUIDs.push_back(obj.GetGuid().ToString());
        if (currentlySelected == obj.GetGuid().ToString())
            currentlySelected = name;
    }

    std::string selected = DropDownPicker(label, inputLabel, &currentlySelected, gameObjectNames);
    if (selected != "")
    {
        for (int i = 0; i < gameObjectNames.size(); i++)
        {
            if (gameObjectNames[i] == selected)
                return gameObjectGUIDs[i];
        }

    }

    return "";
}

bool DebuggerFunctions::Button(const char* label)
{
    float _windowWidth = ImGui::GetWindowWidth();
    //ImGui::SameLine(_windowWidth / 3);
    //ImGui::PushItemWidth(((_windowWidth-60) / 3 * 1));
    const ImVec2& size = ImVec2(((_windowWidth - 30) / 3 * 1), 0);
    if (ImGui::Button(label, size))
    {
        return true;
    }
    //ImGui::PopItemWidth();
    return false;
}

std::string DebuggerFunctions::AssetDropTarget(std::string assetType)
{
    float _windowWidth = ImGui::GetColumnWidth();
    std::string asset = "";
    assetType = "AssetPath" + assetType;
    auto payload = ImGui::AcceptDragDropPayload(assetType.c_str());
    if (payload != nullptr) //check if payload exists
    {
        const char* str = (const char*)payload->Data;
        asset.assign(str);
        asset.resize(payload->DataSize);
    }
    return asset;
}

std::string DebuggerFunctions::Dropper(const char* label, const char* inputLabel, std::string* filePath, std::string assestType)
{

    //ImGui::Text(label); ImGui::SameLine(); ImGui::InputText(inputLabel, filePath);
    TextInput(label, inputLabel, filePath);

    std::string newPath = "";
    if (ImGui::BeginDragDropTarget())
    {
        newPath = AssetDropTarget(assestType);

        ImGui::EndDragDropTarget();
    }
    if (newPath != "")
    {
        return newPath;
    }
    else return *filePath;
}

void DebuggerFunctions::OpenTileMapEditor()
{
    Debugger::m_tilemapWindowOpen = true;
}

void DebuggerFunctions::OpenAnimatorEditor()
{
    Debugger::m_animationEditorOpen = true;
}

void DebuggerFunctions::ReadSceneFolder() 
{
    Debugger::m_assests.clear();
    std::string path = "assets/scenes/";
#ifndef ANDROID
    for (auto& entry : std::filesystem::directory_iterator(path))
        Debugger::m_assests.push_back(entry.path().string());
#else
    auto dir = AAssetManager_openDir(Resources::GetAssetManager(), "scenes/");
    while(true) //search inside folder
    {
        auto name = AAssetDir_getNextFileName(dir);
        if (name == nullptr) //nothing left
            break;
        else
        {
            std::string filePath = name;
            filePath = "assets/scenes/"+filePath;
            Debugger::m_assests.push_back(filePath);
        }
    }
    AAssetDir_close(dir);
#endif
}

//---------------------------------------------

void DebuggerMenus::SpriteRendererMenu()
{
    auto _spriteRenderer = Debugger::m_gameobjectSelected->GetComponentOfType<SpriteRenderComponent>();
    if (_spriteRenderer == nullptr) return;

    if (Debugger::m_gameobjectSelected->GetComponentOfType<AnimatorComponent>() != nullptr)
        return; //has animator so ignore

    ImGui::Text("Texture:"); ImGui::SameLine(); ImGui::InputText("##spritetexture", &_spriteRenderer->m_texturePath);
    if (ImGui::BeginDragDropTarget())
    {
        std::string newTex = DebuggerFunctions::AssetDropTarget("Image"); //use image type to only accept image files
        if (newTex != "")
        {
            _spriteRenderer->UpdateTexturePath(newTex);
        }
        ImGui::EndDragDropTarget();
    }
}

void DebuggerMenus::LoggerWindow()
{
    ImGuiWindowFlags window_flags = 0;
    window_flags = ImGuiWindowFlags_NoResize;


    if (ImGui::Begin("Logs", &Debugger::m_loggerWindowOpen))
    {
        if (ImGui::Button("Save Log"))
        {
            Debugger::SaveLogToFile();
        }

        for (const auto& msg : Debugger::m_messages)
        {
            if (msg.rfind("Warning", 0) == 0)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                ImGui::TextUnformatted(msg.c_str());
                ImGui::PopStyleColor();
            }
            else if (msg.rfind("ERROR", 0) == 0)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                ImGui::TextUnformatted(msg.c_str());
                ImGui::PopStyleColor();
            }
            else
            {
                ImGui::TextUnformatted(msg.c_str());
            }
        }
    }
    ImGui::End();
}

void DebuggerMenus::SceneWindow()
{
    if (ImGui::Begin("Scene", &Debugger::m_sceneWindowOpen))
    {
        ImGui::Text("GameObject Count: %d", Debugger::m_scene->m_gameObjects.size());// m_GameObjects.size());
        for (int i = 0; i < Debugger::m_scene->m_gameObjects.size(); i++)
        {
            std::string buttonLabel = Debugger::m_scene->m_gameObjects[i].GetName() + "##" + std::to_string(i);
            if (ImGui::Button(buttonLabel.c_str()))
            {
                Debugger::m_gameobjectSelected = &(Debugger::m_scene->m_gameObjects[i]);
                Debugger::m_indexedGameobject = i;
                Debugger::m_gameObjectWindow = true;
                break;
            }
            // //if (Button("Init")) _tileMapRenderer->Init();
            // ImGui::Text("Path:"); ImGui::SameLine(); ImGui::InputText("##tilemapPath", &_tileMapRenderer->m_tilemapData.m_lastCSVPath);
            // if (ImGui::BeginDragDropTarget())
            // {
            //     std::string newPath = AssetDropTarget("Text");
            //     if (newPath != "") _tileMapRenderer->m_tilemapData.m_lastCSVPath = newPath;
            //     ImGui::EndDragDropTarget();
            // } 
        }
        std::string temp = std::to_string(Debugger::m_indexedGameobject);
        ImGui::Text("%s", temp.c_str());
        DebuggerMenus::AddGameobject();
    }
    ImGui::End();
}

void DebuggerMenus::AddGameobject()
{
    if (DebuggerFunctions::Button("AddGameObject"))
    {
        int _temp = Debugger::m_scene->AddGameObject(GameObject(std::vector<ComponentPtr<BaseComponent>>{}, nullptr, nullptr, Debugger::m_scene));

        Debugger::m_scene->m_gameObjects[_temp].Setup();
        for (auto comp : Debugger::m_scene->m_gameObjects[_temp].m_components)
        {
            comp->Init();
        }

        ComponentPtr<TransformComponent> comp = Debugger::m_scene->MakeComponent<TransformComponent>();
        Debugger::m_scene->m_gameObjects[_temp].AddComponent(comp);
        comp->Init();
        Debugger::m_scene->m_gameObjects[_temp].m_transform = comp;
    }
}

void DebuggerMenus::DeleteGameObjcet()
{
    if (DebuggerFunctions::Button("delete"))
    {




        for (auto comp : Debugger::m_gameobjectSelected->m_components)
        {
            comp->DeleteComponent();
        }

        for (int x = Debugger::m_indexedGameobject+1; x < Debugger::m_scene->m_gameObjects.size(); x++)
        {
            auto name = Debugger::m_scene->m_gameObjects[x].GetName();
            Debugger::m_scene->m_gameObjects[x-1].SetName(name.c_str());
        }

        auto it = Debugger::m_scene->m_gameObjects.begin() + Debugger::m_indexedGameobject;
        
        std::vector<GameObject> temp;
        
        int count = 0;
        for (auto go : Debugger::m_scene->m_gameObjects) 
        {
            if (count != Debugger::m_indexedGameobject) 
            {
                temp.push_back(Debugger::m_scene->m_gameObjects[count]);
            }
            count++;
        }


        Debugger::m_scene->m_gameObjects = temp;

        Debugger::m_indexedGameobject = 0;
        Debugger::m_gameobjectSelected = nullptr;

        
        //
        
        //KEEP JUST INCASE
        //for (auto& comp : Debugger::m_gameobjectSelected->m_components)
        //{
        //    comp->m_parent = nullptr;
        //    //Debugger::m_gameobjectSelected->m_components.erase(std::find(Debugger::m_gameobjectSelected->m_components.begin(), Debugger::m_gameobjectSelected->m_components.end(), comp));
        //
        //    //comp->DeleteComponent();
        //    //Debugger::m_scene->m_components.Deallocate(&comp);
        //}
        //
        //auto it = Debugger::m_scene->m_gameObjects.begin() + Debugger::m_indexedGameobject;
        //Debugger::m_indexedGameobject = 0;
        //Debugger::m_gameobjectSelected = nullptr;
        //
        //Debugger::m_scene->m_gameObjects.erase(it);
        //
        //
        ////Debugger::m_app->m_scene->EndFrame();
        ////close down old scene
        //Debugger::m_app->m_scene->WriteToJson("temp.json");
        //
        ////Debugger::m_input = new Input();
        //Debugger::m_app->m_scene->Cleanup();
        //
        ////Debugger::m_sceneNameLoaded = Debugger::m_sceneNameToLoad;
        //Debugger::m_app->m_scene = Scene::LoadFromJson("temp.json");
        //Debugger::m_app->m_scene->SetInput(&Debugger::m_app->m_input);
        //Debugger::m_app->m_scene->Init();
        //Debugger::m_app->InitInput();
        //Debugger::m_app->m_scene->Render();
        //Debugger::m_scene = Debugger::m_app->m_scene;
    }
}

void DebuggerMenus::StatsWindow()
{
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize;



    if (ImGui::Begin("Stats", &Debugger::m_statsWindowOpen, window_flags))
    {
        Debugger::m_sampleInterval = DebuggerFunctions::FloatInput("SampleInterval: ", "##SampleInterval: ", Debugger::m_sampleInterval);
        DebuggerFunctions::TextOutput("Frames per second: ", std::to_string(Debugger::m_fps));
        DebuggerFunctions::TextOutput("Average Time between frames: ", std::to_string(Debugger::m_deltaTimesSecondsAverage));
        DebuggerFunctions::TextOutput("Average Fps: ", std::to_string(Debugger::m_fpsAverage));
        DebuggerFunctions::TextOutput("1 percent lows Fps: ", std::to_string(Debugger::m_fpsOnePercentLows));
        DebuggerFunctions::TextOutput("1 percent Highs Fps: ", std::to_string(Debugger::m_fpsOnePercentHighs));
        DebuggerFunctions::TextOutput("1 percent lows Delta Time: ", std::to_string(Debugger::m_deltaTimeOnePercentLows));
        DebuggerFunctions::TextOutput("1 percent Highs Delta Time: ", std::to_string(Debugger::m_deltaTimeOnePercentHighs));
        DebuggerFunctions::TextOutput("Max Fps: ", std::to_string(Debugger::m_fpsMax));
        DebuggerFunctions::TextOutput("Min Fps: ", std::to_string(Debugger::m_fpsMin));
        ImGui::Text("Reset Stats: ");
        ImGui::SameLine();
        if (ImGui::Button("Reset")) Debugger::ResetStats();
    }
    ImGui::End();
}

void DebuggerMenus::GameObjectWindow()
{
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("GameObjects", &Debugger::m_gameObjectWindow, window_flags))
    {

        if(Debugger::m_gameobjectSelected == nullptr)
        {
            ImGui::End();
            return;
        }
        DebuggerFunctions::TextInput("Name", "##goname", &Debugger::m_gameobjectSelected->m_name);
        int count = 0;
        auto trans = Debugger::m_gameobjectSelected->GetComponentOfType<TransformComponent>();
        //if (trans != nullptr) trans->RenderComponentMenu(count);
        count++;
        if (Debugger::m_gameobjectSelected != nullptr)
        {
            if (Debugger::m_gameobjectSelected->GetComponentOfType<TransformComponent>() != nullptr) {
                auto trans = Debugger::m_gameobjectSelected->GetComponentOfType<TransformComponent>();
                count++;
            }
            for (auto& temp : Debugger::m_gameobjectSelected->m_components)
            {
                temp->RenderComponentMenu(count);
                count++;
            }

            AddComponent();
            if (Debugger::m_indexedGameobject != 0) DeleteGameObjcet();
        }

    }

    if (DebuggerFunctions::Button("Duplicate"))
    {
        int _temp = Debugger::m_scene->AddGameObject(GameObject(std::vector<ComponentPtr<BaseComponent>>{}, nullptr, nullptr, Debugger::m_scene));


        Debugger::m_scene->m_gameObjects[_temp].SetName(Debugger::m_gameobjectSelected->GetName().c_str());

        auto IDMap = GetComponentStringIDMap();

        for (auto& component : Debugger::m_gameobjectSelected->m_components)
        {
            //auto id = component->GetId();
            ComponentPtr<BaseComponent> comp = CreateComponent(*Debugger::m_scene, component->GetId());
            nlohmann::json json;

            auto compID = comp->GetGuid();

            component->Serialise(json);
            comp->Deserialise(json);

            comp->m_guid = compID;

            Debugger::m_scene->m_gameObjects[_temp].AddComponent(comp);
            //comp->Init();
        }

        Debugger::m_scene->m_gameObjects[_temp].Setup();

        for (auto& component : Debugger::m_scene->m_gameObjects[_temp].m_components)
            component->Init();



    }
    ImGui::End();
}

static int currentItem = 0;

void DebuggerMenus::AddComponent()
{
    if (ImGui::CollapsingHeader("Add Component"))
    {
        auto possibleComps = GetAllComponentTypeNames();
        auto IDMap = GetComponentStringIDMap();
        possibleComps.erase(std::find(possibleComps.begin(), possibleComps.end(), "TransformComponent"));
        possibleComps.erase(std::find(possibleComps.begin(), possibleComps.end(), "CameraComponent"));
        for (auto comp : possibleComps)
        {
            auto id = IDMap[comp];

        }

        if (ImGui::ComboWithFilter("Components: ", &currentItem, possibleComps, 7))
        {
            ComponentPtr<BaseComponent> comp = CreateComponent(*Debugger::m_scene, IDMap[possibleComps[currentItem]]);
            Debugger::m_gameobjectSelected->AddComponent(comp);
            comp->Init();
        }
    }
}

//ToDo: fix this i dont know how i even would do this at this point im kinda lost on how to remove it
void DebuggerMenus::DeleteGameObject()
{
    if (ImGui::CollapsingHeader("DeleteGameObject"))
    {
        if (ImGui::Button("Delete This Game Object"))
        {

            //Debugger::m_scene->m_gameObjects.erase(std::remove(Debugger::m_scene->m_gameObjects.begin(), Debugger::m_scene->m_gameObjects.end(), Debugger::m_gameobjectSelected), Debugger::m_scene->m_gameObjects.end());
            //delete(Debugger::m_gameobjectSelected);

        }
    }
}

void DebuggerMenus::AnimatorMenu()
{
    auto _component = Debugger::m_scene->m_gameObjects[Debugger::m_indexedGameobject].GetComponentOfType<AnimatorComponent>();
    if (_component == nullptr) return;

    static bool animationEditorOpen = false;
    static int curFrame = 0;
    static std::string curAnimName = "";


    if (ImGui::Begin("Animation Editor", &Debugger::m_animationEditorOpen))
    {

        _component->Pause();

        curAnimName = _component->m_currentAnimationName;

        std::string curFrameStr = "Current Frame: " + std::to_string(curFrame) + " / " + std::to_string(_component->GetAnimation(curAnimName).GetFrameCount() - 1);
        ImGui::Text("%s", curFrameStr.c_str());
        ImGui::SameLine(); if (ImGui::ArrowButton("#frameLeft", 0))
            curFrame--;
        ImGui::SameLine(); if (ImGui::ArrowButton("#frameRight", 1))
            curFrame++;



        if (ImGui::Button("Add Frame"))
        {
            Frame frame{};
            frame.m_frameIndex = _component->GetAnimation(curAnimName).GetFrameCount();
            frame.m_x = 0;
            frame.m_y = 0;
            auto tex = _component->GetFrameTexture();
            frame.m_width = tex->m_width;
            frame.m_height = tex->m_height;
            frame.m_offsetX = 0.0f;
            frame.m_offsetY = 0.0f;
            frame.m_textureIndex = 0;
            _component->m_animations[curAnimName].m_frames.push_back(frame);
        }
        if (ImGui::Button("Remove Frame"))
        {
            _component->m_animations[curAnimName].m_frames.erase(_component->m_animations[curAnimName].m_frames.begin() +
                _component->m_animations[curAnimName].m_frames.size() - 1);
        }

        //if (_component->GetAnimation(curAnimName).m_frames.empty())
       // {
        //    ImGui::End();
        //    return;
        //}
            


        //bound current frame
        if (curFrame < 0)
            curFrame = 0;
        if (curFrame > _component->GetAnimation(curAnimName).GetFrameCount() - 1)
            curFrame = _component->GetAnimation(curAnimName).GetFrameCount() - 1;
        auto _Frame = &_component->m_animations[curAnimName].m_frames[curFrame];
        _Frame->m_x = DebuggerFunctions::FloatInput("Frame X", "##Frame X", _Frame->m_x);
        _Frame->m_y = DebuggerFunctions::FloatInput("Frame Y", "##Frame Y", _Frame->m_y);
        _Frame->m_width = DebuggerFunctions::FloatInput("Frame Width", "##Frame Width", _Frame->m_width);
        _Frame->m_height = DebuggerFunctions::FloatInput("Frame Height", "##Frame Height", _Frame->m_height);

        /*std::string curTexture = _component->m_animations[curAnimName].m_texturePaths[_component->m_animations[curAnimName].m_frames[curFrame].m_textureIndex];
        ImGui::Text("Texture:"); ImGui::SameLine(); ImGui::InputText("##frameTexture", &curTexture);
        if (ImGui::BeginDragDropTarget())
        {
            std::string newTex = AssetDropTarget("Image"); //use image type to only accept image files
            if (newTex != "")
            {

            }
            ImGui::EndDragDropTarget();
        }*/

        _component->m_currentFrameIndex = curFrame;

        auto tex = _component->GetFrameTexture();
        auto frame = _component->GetCurrentFrame();
        float uvX = frame.m_x / (float)tex->m_width;
        float uvY = frame.m_y / (float)tex->m_height;
        float uvRight = (frame.m_x + frame.m_width) / (float)tex->m_width;
        float uvBottom = (frame.m_y + frame.m_height) / (float)tex->m_height;

        ImGui::Image((void*)(intptr_t)tex->m_textureID, ImVec2(256, 256),
            ImVec2(uvX, uvY), ImVec2(uvRight, uvBottom));

        if (ImGui::BeginDragDropTarget())
        {
            std::string newTex = DebuggerFunctions::AssetDropTarget("Image"); //use image type to only accept image files
            if (newTex != "")
            {
                bool animExists = false;
                for (int i = 0; i < _component->m_animations[curAnimName].m_textures.size(); i++)
                {
                    if (_component->m_animations[curAnimName].m_textures[i]->m_path == newTex)
                    {
                        _Frame->m_textureIndex = i;
                        animExists = true;
                        break;
                    }
                }

                if (!animExists) //need to add new texture to list
                {
                    auto tex = Resources::GetTexture(newTex);
                    _component->m_animations[curAnimName].m_textures.push_back(tex);
                    _Frame->m_textureIndex = _component->m_animations[curAnimName].m_textures.size() - 1;
                    _Frame->m_width = tex->m_width;
                    _Frame->m_height = tex->m_height;
                }

            }
            ImGui::EndDragDropTarget();
        }



    }
    ImGui::End();

}

void DebuggerMenus::TilemapEditor()
{
    auto tilemap = Debugger::m_scene->GetComponentOfType<TilemapRenderComponent>();
    if (tilemap == nullptr) return;

    ImGui::SetNextWindowPos(ImVec2(0.0, 20.0));
    ImGui::SetNextWindowSize(ImVec2(400.0, 700.0));
    if (ImGui::Begin("Tilemap Editor", &Debugger::m_tilemapWindowOpen, ImGuiWindowFlags_NoMove))
    {
        static std::shared_ptr<TilemapData> tilemap;
        static std::vector<std::shared_ptr<TilemapData>> undos;
        static std::vector<std::shared_ptr<TilemapData>> redos;
        static std::string tilemapName = "assets/tilemaps/tilemapEditorTest.txt";
        static std::shared_ptr<Texture> tex;
        if (tilemap == nullptr)
        {
            tilemap = std::make_shared<TilemapData>();
            tilemap->InitCells(25, 25);
            tilemap->CheckTexture();
            undos = {};
            tex = Resources::GetTexture(tilemap->m_texturePath);
        }

        int gridSizeX = tilemap->m_tileWidth;
        int gridSizeY = tilemap->m_tileHeight;
        static float gridScale = 0.5;
        float scaledGridSizeX = (float)gridSizeX * gridScale;
        float scaledGridSizeY = (float)gridSizeY * gridScale;
        static int selectedTileType = 1;
        static float lastMouseX = 0.0f;
        static float lastMouseY = 0.0f;
        static float tilemapOffsetX = 0;
        static float tilemapOffsetY = 0;
        static bool unsaved = false;

        auto saveUndo = [&]()
            {
                redos.clear();
                std::shared_ptr<TilemapData> data = std::make_shared<TilemapData>();
                tilemap->CopyOnto(*data);
                undos.push_back(data);

                if (undos.size() > 50)
                    undos.erase(undos.begin());
            };


        //bound movement
        if (tilemapOffsetX > 250.0f)
            tilemapOffsetX = 250.0f;
        if (tilemapOffsetY > 250.0f)
            tilemapOffsetY = 250.0f;

        if (tilemapOffsetX < -scaledGridSizeX * tilemap->m_gridWidthInCells + 250.0f)
            tilemapOffsetX = -scaledGridSizeX * tilemap->m_gridWidthInCells + 250.0f;
        if (tilemapOffsetY < -scaledGridSizeY * tilemap->m_gridHeightInCells + 250.0f)
            tilemapOffsetY = -scaledGridSizeY * tilemap->m_gridHeightInCells + 250.0f;



        ImGui::SetNextWindowPos(ImVec2(400.0, 20.0));
        ImGui::SetNextWindowSize(ImVec2(880.0, 700.0));
        auto flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        std::string title = unsaved ? "* Tilemap View" : "Tilemap View";
        if (ImGui::Begin(title.c_str(), 0, flags))
        {
            auto mousePos = ImGui::GetMousePos(); //get local mouse pos
            auto windowPos = ImGui::GetWindowPos();
            mousePos.x -= windowPos.x;
            mousePos.y -= windowPos.y;

            float curX = tilemapOffsetX;
            float curY = ImGui::GetCursorPosY() + tilemapOffsetY;
            //calculate which grid is hovered
            int selectedGridX = (int)floor((mousePos.x - curX) / scaledGridSizeX);
            int selectedGridY = (int)floor((mousePos.y - curY) / scaledGridSizeY);

            //draw tilemap bounds
            ImGui::SetCursorPos(ImVec2(curX, curY));
            ImGui::GetWindowDrawList()->AddRect(ImVec2(windowPos.x + ImGui::GetCursorPosX(), windowPos.y + ImGui::GetCursorPosY()),
                ImVec2(windowPos.x + ImGui::GetCursorPosX() + scaledGridSizeX * tilemap->m_gridWidthInCells, windowPos.y + ImGui::GetCursorPosY() + scaledGridSizeY * tilemap->m_gridHeightInCells),
                ImGui::ColorConvertFloat4ToU32(ImVec4(1.0, 1.0, 1.0, 1.0)), 1.0f, 0, 5.0f * gridScale);

            //draw tilemap
            for (auto& tile : tilemap->m_grid)
            {
                //draw at this pos
                ImGui::SetCursorPos(ImVec2(curX + (tile.m_gridX * scaledGridSizeX), curY + (tile.m_gridY * scaledGridSizeY)));

                if (tile.m_type != 0)
                {
                    float U = tilemap->GetUVColumn(tile.m_type) * gridSizeX;
                    float V = tilemap->GetUVRow(tile.m_type) * gridSizeY;

                    //float gridX = tile.m_gridX * (float)gridSize;
                    //float gridY = -tile.m_gridY * (float)gridSize;

                    float u0 = static_cast<float>(U) / (float)tex->m_width;
                    float v0 = static_cast<float>(V) / (float)tex->m_height;
                    float u1 = static_cast<float>(U + gridSizeX) / (float)tex->m_width;
                    float v1 = static_cast<float>(V + gridSizeY) / (float)tex->m_height;



                    ImGui::Image((void*)(intptr_t)tex->m_textureID, ImVec2(scaledGridSizeX, scaledGridSizeY), ImVec2(u0, v0), ImVec2(u1, v1));
                }
                else
                {
                    //ImGui::Image(0, ImVec2(50, 50));
                }
            }


            //draw the box for the currently hovered grid
            if (selectedGridX >= 0 && selectedGridY >= 0 && selectedGridX < tilemap->m_gridWidthInCells && selectedGridY < tilemap->m_gridHeightInCells)
            {
                ImGui::SetCursorPos(ImVec2(curX + (selectedGridX * scaledGridSizeX), curY + (selectedGridY * scaledGridSizeY)));
                ImGui::GetWindowDrawList()->AddRect(ImVec2(windowPos.x + ImGui::GetCursorPosX(), windowPos.y + ImGui::GetCursorPosY()),
                    ImVec2(windowPos.x + ImGui::GetCursorPosX() + scaledGridSizeX, windowPos.y + ImGui::GetCursorPosY() + scaledGridSizeY), ImGui::ColorConvertFloat4ToU32(ImVec4(1.0, 1.0, 1.0, 1.0)), 1.0f, 0, 5.0f * gridScale);

                auto& tile = tilemap->m_grid[tilemap->GetTileIndex(selectedGridX, selectedGridY)];
                std::string tileInfo = std::to_string(tile.m_gridX) + "," + std::to_string(tile.m_gridY);
                tileInfo = tileInfo + "\n" + "Type: " + std::to_string(tile.m_type);

                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + scaledGridSizeX + (5.0f * gridScale), ImGui::GetCursorPosY()));
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(windowPos.x + ImGui::GetCursorPosX(), windowPos.y + ImGui::GetCursorPosY()),
                    ImVec2(windowPos.x + ImGui::GetCursorPosX() + scaledGridSizeX, windowPos.y + ImGui::GetCursorPosY() + scaledGridSizeY), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0, 0.0, 0.0, 0.5)), 1.0f, 0);
                ImGui::Text("%s", tileInfo.c_str());

                if (ImGui::IsMouseClicked(0))
                {
                    unsaved = true;
                    saveUndo();
                }
                /*if (ImGui::IsMouseClicked(2))
                {
                    unsaved = true;
                    saveUndo();
                    tilemap->FloodFillAtTile(selectedGridX, selectedGridY, selectedTileType);
                }*/

                if (ImGui::IsMouseDown(0))
                {
                    tilemap->m_grid[tilemap->GetTileIndex(selectedGridX, selectedGridY)].m_type = selectedTileType;
                    Debugger::tileMapType = selectedTileType;
                }
            }

            //tilemap drag
            if (ImGui::IsMouseDragging(1))
            {
                //auto drag = ImGui::GetMouseDragDelta(1);
                tilemapOffsetX += mousePos.x - lastMouseX;
                tilemapOffsetY += mousePos.y - lastMouseY;

            }
            //tilemap zoom
            auto wheel = ImGui::GetIO().MouseWheel;
            if (ImGui::IsWindowHovered() && wheel != 0.0f)
            {
                gridScale += wheel * 0.1f;
                if (gridScale < 0.1f)
                    gridScale = 0.1f;
            }

            //store for next frame
            lastMouseX = mousePos.x;
            lastMouseY = mousePos.y;
        }
        ImGui::End();


        //save and undos
        if (ImGui::IsKeyPressed(ImGuiKey_S) && ImGui::IsKeyDown(ImGuiKey_ModCtrl))
        {
            tilemap->SaveCSV(tilemapName);
            unsaved = false;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Z) && ImGui::IsKeyDown(ImGuiKey_ModCtrl))
        {
            if (undos.size() > 0)
            {

                undos[undos.size() - 1]->CopyOnto(*tilemap);
                undos.erase(undos.begin() + undos.size() - 1);
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Y) && ImGui::IsKeyDown(ImGuiKey_ModCtrl))
        {
            std::cout << "redo (not added yet)" << std::endl;
        }

        ImGui::SeparatorText("Info");

        tilemapName = DebuggerFunctions::Dropper("Path: ", "##tilemapname", &tilemapName, "Text");


        if (DebuggerFunctions::Button("Save"))
        {
            tilemap->SaveCSV(tilemapName);
            unsaved = false;
        }
        ImGui::SameLine();
        if (DebuggerFunctions::Button("Load"))
        {
            tilemap->LoadCSV(tilemapName, false);
            unsaved = false;
            undos.clear();
            tilemap->CheckTexture();
            tex = Resources::GetTexture(tilemap->m_texturePath);
        }

        ImGui::SeparatorText("Properties");

        //grid increase/decreases

        int _DesiredWidth = DebuggerFunctions::IntArrowInput("Grid Width: ", "##GridWidthInput:", tilemap->m_gridWidthInCells);

        while (_DesiredWidth != (tilemap->m_gridWidthInCells))
        {
            if ((tilemap->m_gridWidthInCells) > _DesiredWidth) {
                tilemap->DecreaseGridWidth();
            }
            if ((tilemap->m_gridWidthInCells) < _DesiredWidth)
            {
                tilemap->IncreaseGridWidth();
            }
        }

        int _DesiredHeight = DebuggerFunctions::IntArrowInput("Grid Height: ", "##GridHeightInput:", tilemap->m_gridHeightInCells);

        while (_DesiredHeight != (tilemap->m_gridHeightInCells))
        {
            if ((tilemap->m_gridHeightInCells) > _DesiredHeight) {
                tilemap->DecreaseGridHeight();
            }
            if ((tilemap->m_gridHeightInCells) < _DesiredHeight)
            {
                tilemap->IncreaseGridHeight();
            }
        }

        //change texture

        tilemap->m_texturePath = DebuggerFunctions::Dropper("Texture: ", "##TileMapTexture", &tilemap->m_texturePath, "Image");

        if (DebuggerFunctions::Button("Load"))
        {
            tilemap->LoadCSV(tilemapName, true);
            unsaved = false;
            undos.clear();
            tilemap->CheckTexture();
            tex = Resources::GetTexture(tilemap->m_texturePath);
        }

        ImGui::SeparatorText("Tiles");

        //tile width/height
        tilemap->m_tileWidth = DebuggerFunctions::IntArrowInput("Tile Width: ", "##TileWidthInput:", tilemap->m_tileWidth);

        tilemap->m_tileHeight = DebuggerFunctions::IntArrowInput("Tile Height: ", "##TileHeightInput:", tilemap->m_tileHeight);

        ImGui::SeparatorText("Tiles");

        //tile width/height
        std::string tileWidth = "Tile Width: " + std::to_string(tilemap->m_tileWidth);
        ImGui::Text("%s", tileWidth.c_str()); ImGui::SameLine();

        if (ImGui::ArrowButton("##Tile Width Decrease", 0))
        {
            unsaved = true;
            saveUndo();
            tilemap->m_tileWidth--;
            if (tilemap->m_tileWidth < 1)
                tilemap->m_tileWidth = 1;
            tilemap->CheckTexture();
        }
        ImGui::SameLine();

        if (ImGui::ArrowButton("##Tile Width Increase", 1))
        {
            unsaved = true;
            saveUndo();
            tilemap->m_tileWidth++;
            tilemap->CheckTexture();
        }

        std::string tileHeight = "Tile Height: " + std::to_string(tilemap->m_tileHeight);
        ImGui::Text("%s", tileHeight.c_str()); ImGui::SameLine();

        if (ImGui::ArrowButton("##Tile Height Decrease", 0))
        {
            unsaved = true;
            saveUndo();
            tilemap->m_tileHeight--;
            if (tilemap->m_tileHeight < 1)
                tilemap->m_tileHeight = 1;
            tilemap->CheckTexture();
        }

        ImGui::SameLine();

        if (ImGui::ArrowButton("##Tile Height Increase", 1))
        {
            unsaved = true;
            saveUndo();
            tilemap->m_tileHeight++;
            tilemap->CheckTexture();
        }

        //tile selection

        ImGui::BeginChild("Texture", ImVec2(300, 300), 0, ImGuiWindowFlags_HorizontalScrollbar);

        auto imageY = ImGui::GetCursorPosY();

        ImGui::Image((void*)(intptr_t)tex->m_textureID, ImVec2((float)tex->m_width, (float)tex->m_height));

        auto windowPos = ImGui::GetWindowPos();
        auto pos = ImVec2(windowPos.x + ImGui::GetCursorPosX() - ImGui::GetScrollX(), windowPos.y + imageY - ImGui::GetScrollY());

        int U = (int)floor(tilemap->GetUVColumn(selectedTileType)) * gridSizeX;
        int V = (int)floor(tilemap->GetUVRow(selectedTileType)) * gridSizeY;

        if (selectedTileType != 0)
            ImGui::GetWindowDrawList()->AddRect(ImVec2(pos.x + U, pos.y + V),
                ImVec2(pos.x + gridSizeX + U, pos.y + gridSizeY + V), ImGui::ColorConvertFloat4ToU32(ImVec4(1.0, 1.0, 1.0, 1.0)), 1.0f, 0, 5.0f);

        if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
        {
            auto mousePos = ImGui::GetMousePos();
            mousePos.x -= pos.x; mousePos.y -= pos.y;

            int tileGridX = (int)floor(mousePos.x / gridSizeX);
            int tileGridY = (int)floor(mousePos.y / gridSizeY);

            if (tileGridX < 0)
                tileGridX = 0;
            if (tileGridY < 0)
                tileGridY = 0;
            if (tileGridX > (int)tilemap->m_tilemapColCountUV)
                tileGridX = (int)tilemap->m_tilemapColCountUV;
            if (tileGridY > (int)tilemap->m_tilemapRowCountUV)
                tileGridY = (int)tilemap->m_tilemapRowCountUV;

            selectedTileType = (tileGridY * tilemap->m_tilemapColCountUV) + tileGridX + 1;
        }

        ImGui::EndChild();
        if (ImGui::Button("Eraser"))
            selectedTileType = 0;
    }
    ImGui::End();
}

std::vector<Cell> Debugger::GetGrid()
{
    ComponentPtr<TilemapRenderComponent> tilemapRC;
    for (GameObject gO : m_scene->m_gameObjects)
    {
        if (gO.GetComponentOfType<TilemapRenderComponent>() != NULL)
        {
            tilemapRC = gO.GetComponentOfType<TilemapRenderComponent>();
        }
    }

    std::vector<Cell> grid = tilemapRC->GetGrid();

    return grid;
}

ComponentPtr<TilemapRenderComponent> tilemapp;
void DebuggerMenus::TileMapRendererFunctionality()
{

    auto tilemap = Debugger::m_scene->GetComponentOfType<TilemapRenderComponent>();
    if (tilemap == nullptr) return;
    for (GameObject gO : Debugger::m_scene->m_gameObjects)
    {
        if (gO.GetComponentOfType<TilemapRenderComponent>() != NULL)
        {
            tilemapp = gO.GetComponentOfType<TilemapRenderComponent>();
        }
    }

    if (tilemapp == nullptr)
        return;

    double mouseXPos = Application::m_mouseXpos;
    double mouseYPos = Application::m_mouseYpos;

    int width = Application::tilemapWidth;
    int height = Application::tilemapHeight;

    if (Application::isMouse0Down)
    {
        if (Debugger::m_tilemapSelector == true)
        {
            std::vector<Cell> grid = tilemapp->GetGrid();
            Vector2 id = tilemapp->GetCellWorld(Vector2(mouseXPos, mouseYPos));

            if (id.x < 0 || id.x >= width || id.y < 0 || id.y >= height)
            {
                return;
            }

            grid[id.x + id.y * height].m_isSelected = true;
            Debugger::m_aISpawnPosGridCoords[0] = id.x;
            Debugger::m_aISpawnPosGridCoords[1] = id.y;

            tilemapp->SetGrid(grid);
            tilemapp->Cleanup();
            tilemapp->Init();
        }
        if (Debugger::m_tilemapSelector == false)
        {
            std::vector<Cell> grid = tilemapp->GetGrid();
            Vector2 id = tilemapp->GetCellWorld(Vector2(mouseXPos, mouseYPos));

            if (id.x < 0 || id.x >= width || id.y < 0 || id.y >= height)
            {
                return;
            }

            grid[id.x + id.y * height].m_isWalkable = false;
            grid[id.x + id.y * height].m_type = Debugger::tileMapType;

            tilemapp->SetGrid(grid);
            tilemapp->Cleanup();
            tilemapp->Init();
        }
    }

    if (Application::isMouse2Down)
    {
        std::vector<Cell> grid = tilemapp->GetGrid();
        Vector2 id = tilemapp->GetCellWorld(Vector2(mouseXPos, mouseYPos));

        if (id.x < 0 || id.x >= width || id.y < 0 || id.y >= height)
        {
            return;
        }

        grid[id.x + id.y * height].m_isWalkable = true;
        grid[id.x + id.y * height].m_type = 0;

        tilemapp->SetGrid(grid);
        tilemapp->Cleanup();
        tilemapp->Init();
    }
}

void DebuggerMenus::TileMapSelector()
{

    auto tilemap = Debugger::m_scene->GetComponentOfType<TilemapRenderComponent>();
    if (tilemap == nullptr) return;
    ImGui::SetNextWindowPos(ImVec2(0.0, 20.0));
    ImGui::SetNextWindowSize(ImVec2(400.0, 700.0)); \
        if (ImGui::Begin("Tilemap Editor", &Debugger::m_tilemapTypeChooserOpen))
        {
            static std::shared_ptr<TilemapData> tilemap;
            static std::string tilemapName = "assets/tilemaps/tilemapEditorTest.txt";
            static std::shared_ptr<Texture> tex;

            if (tilemap == nullptr)
            {
                tilemap = std::make_shared<TilemapData>();
                tilemap->CheckTexture();
                tex = Resources::GetTexture(tilemap->m_texturePath);
            }

            int gridSizeX = tilemap->m_tileWidth;
            int gridSizeY = tilemap->m_tileHeight;
            static float gridScale = 0.5;
            float scaledGridSizeX = (float)gridSizeX * gridScale;
            float scaledGridSizeY = (float)gridSizeY * gridScale;
            static int selectedTileType = 1;
            static float tilemapOffsetX = 0;
            static float tilemapOffsetY = 0;

            //bound movement
            if (tilemapOffsetX > 250.0f)
                tilemapOffsetX = 250.0f;
            if (tilemapOffsetY > 250.0f)
                tilemapOffsetY = 250.0f;

            // ImGui button to toggle tilemap selector
            if (ImGui::Button("Select"))
            {
                Debugger::m_tilemapSelector = true;
                selectedTileType = 0; // Clear highlighted tilemap texture
                Debugger::tileMapType = selectedTileType;
            }
            if (ImGui::InputInt2("Spawn Pos", Debugger::m_aISpawnPosGridCoords))
            {

            }
            if (ImGui::Button("Spawn Agent"))
            {
                int _temp = Debugger::m_scene->AddGameObject(GameObject(std::vector<ComponentPtr<BaseComponent>>{}, Debugger::m_scene->MakeComponent<TransformComponent>(),
                    Debugger::m_scene->MakeComponent<SpriteRenderComponent>("assets/textures/HuhCar.png"), Debugger::m_scene));

                Debugger::m_scene->m_gameObjects[_temp].Setup();
                for (auto comp : Debugger::m_scene->m_gameObjects[_temp].m_components)
                {
                    comp->Init();
                }
                auto camera = Debugger::m_scene->GetComponentOfType<CameraComponent>();
                Debugger::m_scene->m_gameObjects[_temp].GetTransform()->m_position = glm::vec3(Debugger::m_aISpawnPosGridCoords[0] * 5, Debugger::m_aISpawnPosGridCoords[1] * 5 + 360, 0);
                Debugger::m_scene->m_gameObjects[_temp].GetTransform()->m_scale = glm::vec3(0.1f, 0.1f, 0);

                Debugger::m_aIAgents.push_back(_temp);
            }
            if (ImGui::InputInt2("Spawn Pos", Debugger::m_aISpawnPosGridCoords))
            {

            }

            //tile selection

            ImGui::BeginChild("Texture", ImVec2(300, 300), 0, ImGuiWindowFlags_HorizontalScrollbar);

            auto imageY = ImGui::GetCursorPosY();

            ImGui::Image((void*)(intptr_t)tex->m_textureID, ImVec2((float)tex->m_width, (float)tex->m_height));

            auto windowPos = ImGui::GetWindowPos();
            auto pos = ImVec2(windowPos.x + ImGui::GetCursorPosX() - ImGui::GetScrollX(), windowPos.y + imageY - ImGui::GetScrollY());

            int U = (int)floor(tilemap->GetUVColumn(selectedTileType)) * gridSizeX;
            int V = (int)floor(tilemap->GetUVRow(selectedTileType)) * gridSizeY;

            if (selectedTileType != 0)
                ImGui::GetWindowDrawList()->AddRect(ImVec2(pos.x + U, pos.y + V),
                    ImVec2(pos.x + gridSizeX + U, pos.y + gridSizeY + V), ImGui::ColorConvertFloat4ToU32(ImVec4(1.0, 1.0, 1.0, 1.0)), 1.0f, 0, 5.0f);

            if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
            {
                auto mousePos = ImGui::GetMousePos();
                mousePos.x -= pos.x;
                mousePos.y -= pos.y;

                int tileGridX = (int)floor(mousePos.x / gridSizeX);
                int tileGridY = (int)floor(mousePos.y / gridSizeY);

                if (tileGridX >= 0 && tileGridY >= 0 && tileGridX < tilemap->m_tilemapColCountUV && tileGridY < tilemap->m_tilemapRowCountUV)
                {
                    selectedTileType = (tileGridY * tilemap->m_tilemapColCountUV) + tileGridX + 1;

                    Debugger::m_tilemapSelector = false;

                    Debugger::tileMapType = selectedTileType;
                }
            }

            if (ImGui::Button("Procedural Generation"))
            {
                ProcGen();
            }
            ImGui::EndChild();
        }
    ImGui::End();
}

void DebuggerMenus::SetAITargetPos()
{
    if (Application::isMouse0Down)
    {
        for (GameObject gO : Debugger::m_scene->m_gameObjects)
        {
            if (gO.GetComponentOfType<TilemapRenderComponent>() != NULL)
            {
                tilemapp = gO.GetComponentOfType<TilemapRenderComponent>();
            }
        }

        if (tilemapp == NULL)
        {
            return;
        }

        int width = Application::tilemapWidth;
        int height = Application::tilemapHeight;

        double mouseXPos = Application::m_mouseXpos;
        double mouseYPos = Application::m_mouseYpos;

        Vector2 aiPos = tilemapp->GetCellWorld(Vector2(mouseXPos, mouseYPos));

        if (aiPos.x < 0 || aiPos.x >= width || aiPos.y < 0 || aiPos.y >= height)
        {
            return;
        }

        Debugger::m_aISelectedPos = aiPos;
    }
}

void DebuggerMenus::ProcGen()
{
    for (int i = 0; i < Debugger::m_scene->m_gameObjects.size(); i++)
    {
        if (Debugger::m_scene->m_gameObjects[i].GetComponentOfType<TilemapRenderComponent>() != NULL)
        {
            tilemapp = Debugger::m_scene->m_gameObjects[i].GetComponentOfType<TilemapRenderComponent>();
        }
        if (Debugger::m_scene->m_gameObjects[i].GetComponentOfType<AIAgent>() != NULL)
        {
            Debugger::m_aIAgents.push_back(i);
        }
    }

    if (tilemapp == NULL)
    {
        return;
    }

    if (Debugger::m_newGrid.size() != 0)
    {
        RemoveGrids(tilemapp->GetGrid(), Debugger::m_newGrid);
    }

    ProceduralGeneration procGenGO = { 200, 200 };
    Debugger::m_newGrid = procGenGO.InitialiseStartingRoom(50, 50, 200, 200);

    AddGrids(tilemapp->GetGrid(), Debugger::m_newGrid);
    tilemapp->Cleanup();
    tilemapp->Init();

    if (Debugger::m_aIAgents.size() != 0)
    {
        for (int i : Debugger::m_aIAgents)
        {
            if (Debugger::m_scene->m_gameObjects[i].GetComponentOfType<AIAgent>() != NULL)
            {
                Debugger::m_scene->m_gameObjects[i].GetComponentOfType<AIAgent>()->m_pos = Vector2(260 + (i * 5), 255);
                Debugger::m_scene->m_gameObjects[i].GetComponentOfType<AIAgent>()->m_steeringComponent->SetPos(Vector2(260 + (i * 5), 255 ));
            }
        }
    }
}

void DebuggerMenus::AddGrids(std::vector<Cell>& oldGrid, std::vector<Cell>& newGrid)
{
    for (size_t i = 0; i < oldGrid.size(); i++)
    {
        if (newGrid[i].m_type != 0)
        {
            if (newGrid[i].m_type != oldGrid[i].m_type)
            {
                oldGrid[i].m_type = newGrid[i].m_type;
            }
        }
    }
}

void DebuggerMenus::RemoveGrids(std::vector<Cell>& oldGrid, std::vector<Cell>& newGrid)
{
    for (size_t i = 0; i < oldGrid.size(); i++)
    {
        if (newGrid[i].m_type != 0)
        {
            oldGrid[i].m_type = 0;
        }
    }
}

void DebuggerMenus::AssetViewerMenu()
{
#ifndef ANDROID
    static std::string testAsset;
    if (ImGui::Begin("Asset Viewer", &Debugger::m_assetsWindowOpen))
    {
        static std::string assetPath = ".\\assets\\";
        auto folder = std::filesystem::directory_iterator(assetPath);

        //back button
        if (ImGui::Button("Back"))
        {
            assetPath.erase(assetPath.begin() + assetPath.size() - 1);
            while (true) //remove end directory to go back
            {
                if (assetPath.empty())
                {
                    assetPath = ".\\";
                    break;
                }

                if (assetPath.at(assetPath.size() - 1) == '/' || assetPath.at(assetPath.size() - 1) == '\\')
                {
                    break;
                }
                assetPath.erase(assetPath.begin() + assetPath.size() - 1); //keep removing until it hits '/' or is empty
            }
        }

        for (auto& file : folder)
        {
            if (file.is_directory())
            {
                if (ImGui::Button(file.path().filename().string().c_str()))
                {
                    assetPath = file.path().string().c_str();
                }
            }
            else
            {
                ImGui::Text(file.path().filename().string().c_str());

                std::string assetType = "AssetPathDefault"; //store a type so the drop target knows which assets to accept
                if (file.path().extension().string() == ".png" || file.path().extension().string() == ".jpg")
                {
                    assetType = "AssetPathImage";
                }
                else if (file.path().extension().string() == ".txt")
                {
                    assetType = "AssetPathText";
                }
                else if (file.path().extension().string() == ".wav")
                {
                    assetType = "AssetPathAudio";
                }

                //setup drop source
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    testAsset = file.path().string();
                    ImGui::SetDragDropPayload(assetType.c_str(), testAsset.c_str(), testAsset.length());
                    ImGui::Text(file.path().string().c_str());
                    ImGui::EndDragDropSource();
                }

            }
        }
    }
    ImGui::End();
#endif
}

void DebuggerMenus::CameraControl()
{
    //if (!ImGui::IsAnyItemHovered())
    //{
    auto camera = Debugger::m_scene->GetComponentOfType<CameraComponent>();
    if (camera == nullptr)
        return;
    static ImVec2 lastMousePos = ImVec2(0.0f, 0.0f);
    ImVec2 mousePos = ImGui::GetMousePos();
    mousePos.x -= ImGui::GetMainViewport()->Pos.x;
    mousePos.y -= ImGui::GetMainViewport()->Pos.y;
    ImVec2 windowOffset = ImGui::GetWindowPos();
#ifdef ANDROID
    static bool androidHold = false;
    if (ImGui::IsMouseClicked(0))
        androidHold = true;
    if (ImGui::IsMouseReleased(0))
        androidHold = false;
    if (androidHold)
#else
    if (ImGui::IsMouseDragging(1))
#endif
    {
        // LOGV("drag");
         //auto drag = ImGui::GetMouseDragDelta(1);
        camera->m_x += mousePos.x - lastMousePos.x;
        camera->m_y += mousePos.y - lastMousePos.y;
}
    //tilemap zoom
    auto wheel = ImGui::GetIO().MouseWheel;
    if (wheel != 0.0f)
    {
        camera->m_zoom += wheel * 0.1f;
        if (camera->m_zoom < 0.1f)
            camera->m_zoom = 0.1f;
    }
    lastMousePos = mousePos;
    auto temp = ImGui::GetMainViewport()->Pos;
    auto width = camera->m_width + temp.x;
    auto height = camera->m_height + temp.y;
    auto view = glm::translate(glm::mat4(1.0), glm::vec3(camera->m_width * 0.5f, camera->m_height * 0.5f, 0.0)) //center the camera
        * glm::scale(glm::mat4(1.0), glm::vec3(camera->m_zoom, camera->m_zoom, camera->m_zoom)) //zoom
        * glm::rotate(glm::mat4(1.0), glm::radians(camera->m_angle), glm::vec3(0.0f, 0.0f, 1.0f)) //rotate
        * glm::translate(glm::mat4(1.0), glm::vec3(camera->m_x, camera->m_y, 0.0)); //translate
    auto aspectScale = glm::vec3((float)Application::m_windowWidth / camera->m_width, (float)Application::m_windowHeight / camera->m_height, 1.0f);
    auto aspect = glm::scale(glm::mat4(1.0), aspectScale);
    if (ImGui::IsMouseDoubleClicked(0))
    {
        for (int i = 0; i < Debugger::m_scene->m_gameObjects.size(); i++)
        {
            if (Debugger::m_scene->m_gameObjects[i].GetTransform() != nullptr)
            {
                //sorry for messy calculation but it works
                auto transform = aspect * view * Debugger::m_scene->m_gameObjects[i].GetTransform()->GetTransformMatrix();
                auto scale = Debugger::m_scene->m_gameObjects[i].GetTransform()->m_scale *
                    glm::vec3{ Debugger::m_scene->m_gameObjects[i].GetTransform()->m_width, Debugger::m_scene->m_gameObjects[i].GetTransform()->m_height, 1.0 };
                auto renderer = Debugger::m_scene->m_gameObjects[i].GetRenderer();
                if (renderer == nullptr) continue;
                glm::vec4 translatedPos = glm::inverse(transform) * glm::vec4(mousePos.x, mousePos.y, 1.0, 1.0);
                translatedPos *= glm::vec4(camera->m_width * 0.5f, camera->m_height * 0.5f, 1.0, 1.0);
                bool didHit = (translatedPos.x >= renderer->m_renderData->m_vertexMinX && translatedPos.x < scale.x * renderer->m_renderData->m_vertexMaxX) &&
                    translatedPos.y >= renderer->m_renderData->m_vertexMinY && translatedPos.y < scale.y * renderer->m_renderData->m_vertexMaxY;
                if (didHit)
                {
                    Debugger::m_gameobjectSelected = &(Debugger::m_scene->m_gameObjects[i]);
                    Debugger::m_indexedGameobject = i;
                    Debugger::m_sceneWindowOpen = true;
                    Debugger::m_gameObjectWindow = true;
                    break;
                }
            }
        }
    }
    if (Debugger::m_gameobjectSelected != nullptr)
    {
        if (Debugger::m_gameobjectSelected->GetComponentOfType<TransformComponent>() == nullptr) return;
        if (Debugger::m_gameobjectSelected->GetTransform() == nullptr)
            return;
        auto temp = ImGui::GetMainViewport()->Pos;
        auto transform = aspect * view * Debugger::m_gameobjectSelected->GetTransform()->GetTransformMatrix();
        auto renderer = Debugger::m_gameobjectSelected->GetRenderer();
        //auto scale = m_gameobjectSelected->GetTransform()->m_scale *
            //glm::vec3{ m_gameobjectSelected->GetTransform()->m_width, m_gameobjectSelected->GetTransform()->m_height, 1.0 };
        if (renderer == nullptr) return;
        glm::vec4 topLeft = transform * glm::vec4(renderer->m_renderData->m_vertexMinX, renderer->m_renderData->m_vertexMinY, 0.0f, 1.0f);
        topLeft.x += temp.x;
        topLeft.y += temp.y;
        glm::vec4 bottomRight = transform * glm::vec4(renderer->m_renderData->m_vertexMaxX, renderer->m_renderData->m_vertexMaxY, 1.0f, 1.0f);
        bottomRight.x += temp.x;
        bottomRight.y += temp.y;
        glm::vec4 topRight = transform * glm::vec4(renderer->m_renderData->m_vertexMaxX, renderer->m_renderData->m_vertexMinY, 0.0f, 1.0f);
        topRight.x += temp.x;
        topRight.y += temp.y;
        glm::vec4 bottomLeft = transform * glm::vec4(renderer->m_renderData->m_vertexMinX, renderer->m_renderData->m_vertexMaxY, 1.0f, 1.0f);
        bottomLeft.x += temp.x;
        bottomLeft.y += temp.y;
        glm::vec4 center = transform * glm::vec4((renderer->m_renderData->m_vertexMaxX - renderer->m_renderData->m_vertexMinX) * 0.5f,
            (renderer->m_renderData->m_vertexMaxY - renderer->m_renderData->m_vertexMinY) * 0.5f, 1.0f, 1.0f);
        //ImGui::GetForegroundDrawList()->AddRect(ImVec2(topLeft.x, topLeft.y), ImVec2(bottomRight.x, bottomRight.y),
           // ImGui::ColorConvertFloat4ToU32(ImVec4(1.0, 1.0, 1.0, 1.0)), 1.0f, 0, 5.0f);
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(topLeft.x, topLeft.y), ImVec2(topRight.x, topRight.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0, 0.0, 1.0, 1.0)), 5.0f);
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(topRight.x, topRight.y), ImVec2(bottomRight.x, bottomRight.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0, 0.0, 1.0, 1.0)), 5.0f);
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottomRight.x, bottomRight.y), ImVec2(bottomLeft.x, bottomLeft.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0, 0.0, 1.0, 1.0)), 5.0f);
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottomLeft.x, bottomLeft.y), ImVec2(topLeft.x, topLeft.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0, 0.0, 1.0, 1.0)), 5.0f);
        bool isHoveringCenter = ImGui::IsMouseHoveringRect(ImVec2(center.x + temp.x - 10.0f, center.y + temp.y - 10.0f), ImVec2(center.x + temp.x + 10.0f, center.y + temp.y + 10.0f), false);
        ImGui::GetForegroundDrawList()->AddRect(ImVec2(center.x + temp.x - 10.0f, center.y + temp.y - 10.0f), ImVec2(center.x + temp.x + 10.0f, center.y + temp.y + 10.0f),
            isHoveringCenter ? ImGui::ColorConvertFloat4ToU32(ImVec4(1.0, 1.0, 1.0, 1.0)) : ImGui::ColorConvertFloat4ToU32(ImVec4(0.5, 0.5, 0.5, 1.0)), 1.0f, 0, 5.0f);
        static bool grabbingObject = false;
        if (isHoveringCenter)
        {
            if (ImGui::IsMouseDown(0))
                grabbingObject = true;
        }
        if (!ImGui::IsMouseDown(0))
            grabbingObject = false; //let go
        if (grabbingObject)
        {
            auto scale = Debugger::m_gameobjectSelected->GetTransform()->m_scale *
                glm::vec3{ Debugger::m_gameobjectSelected->GetTransform()->m_width, Debugger::m_gameobjectSelected->GetTransform()->m_height, 1.0 };
            glm::vec4 newPosition = glm::inverse(view) * (glm::vec4(mousePos.x / aspectScale.x, mousePos.y / aspectScale.y, 1.0, 1.0));
            newPosition -= glm::vec4((renderer->m_renderData->m_vertexMaxX - renderer->m_renderData->m_vertexMinX) * 0.5f * scale.x,
                (renderer->m_renderData->m_vertexMaxY - renderer->m_renderData->m_vertexMinY) * 0.5f * scale.y, 1.0f, 1.0f);
            Debugger::m_gameobjectSelected->GetTransform()->m_position = newPosition;
        }
    }
    // }
}

std::string FileName = "";

std::string newFile = R"(
{
    "components": [
    {
        "id": 4361531,
            "m_angle" : 0.0,
            "m_guid" : {
            "guid": "3d9153c3808e427febaffb3f27c6950e25cfb255b42eb1475337257c43f385dc"
        },
            "m_height" : 720.0,
            "m_width" : 1280.0,
            "m_x" : 0.0,
            "m_y" : 0.0,
            "m_zoom" : 0.5
    }
    ],
    "gameobjects": [
    {
        "m_children": [] ,
            "m_components" : [
        {
            "m_component": {
                "guid": "3d9153c3808e427febaffb3f27c6950e25cfb255b42eb1475337257c43f385dc"
            }
        }
            ] ,
            "m_guid": {
            "guid": "1d53e67a0306d2e18e68e82b4e9468e99c4a9a8f313908d6f17eea5e0868e298"
        },
            "m_name" : "Camera",
            "m_parent" : {
            "guid": "0000000000000000000000000000000000000000000000000000000000000000"
        }
    }
    ]
}
)";

void DebuggerMenus::SceneSelector()
{
    if (ImGui::Begin("SceneSelector", &Debugger::m_sceneSelectorWindowOpen))
    {
        /*for (auto ob : Debugger::m_input->m_subjInst.m_observers)
        {
            Debugger::m_input->RemoveObserver(ob);
        }*/
        //Debugger::m_input->ListObservers();
        DebuggerFunctions::DropDownPicker("Scenes: ", "##Scenes:", &Debugger::m_sceneNameToLoad, Debugger::m_assests, true);
        //Debug::Error(scenePath);
        if (DebuggerFunctions::Button("Load"))
        {
            if (Debugger::m_sceneNameLoaded != Debugger::m_sceneNameToLoad)
            {
                Debugger::m_indexedGameobject = 0;
                Debugger::m_gameobjectSelected = nullptr;
                Debugger::m_app->m_scene->EndFrame();
                //close down old scene
                Debugger::m_app->m_scene->WriteToJson(Debugger::m_sceneNameLoaded);
                //delete( Debugger::m_scene->m_input);
                Debugger::m_input->ListObservers();
                Debugger::m_input->RemoveObserver(Debugger::m_app);
                Debugger::m_input->RemoveObserver(Debugger::m_app->m_scene);
                Debugger::m_input->ListObservers();
                for (auto ob : Debugger::m_input->m_subjInst.m_observers)
                {
                    Debugger::m_input->RemoveObserver(ob);
                }
                Debugger::m_input->ListObservers();
                //Debugger::m_input = new Input();
                Debugger::m_app->m_scene->Cleanup();
                Resources::ClearUnusedTextures();
                Debugger::m_sceneNameLoaded = Debugger::m_sceneNameToLoad;
                Debugger::m_app->m_scene = Scene::LoadFromJson(Debugger::m_sceneNameLoaded);
                Debugger::m_app->m_scene->SetInput(&Debugger::m_app->m_input);
                Debugger::m_app->m_scene->Init();
                Debugger::m_app->InitInput();
                Debugger::m_app->m_scene->Render();
                Debugger::m_scene = Debugger::m_app->m_scene;

                DebuggerFunctions::ReadSceneFolder();
            }
        }

        ImGui::SameLine();

        if (DebuggerFunctions::Button("New")) 
        {
            ImGui::OpenPopup("My Popup");
        }

        ImGui::SameLine();

#ifndef ANDROID //cant do this on android
        if (DebuggerFunctions::Button("Delete")) 
        {
            if (Debugger::m_sceneNameToLoad != Debugger::m_sceneNameLoaded) 
            {
                if (std::filesystem::exists(Debugger::m_sceneNameToLoad)) {
                    // Attempt to delete the file
                    if (std::filesystem::remove(Debugger::m_sceneNameToLoad)) {
                        //std::cout << "File deleted successfully." << std::endl;
                        Debugger::m_sceneNameToLoad = Debugger::m_sceneNameLoaded;

                        DebuggerFunctions::ReadSceneFolder();
                    }
                    else {
                        //std::cerr << "Failed to delete the file." << std::endl;
                    }
                }
                else {
                    //std::cerr << "File does not exist." << std::endl;
                }
            }
        }
#endif

        // Render the popup
        if (ImGui::BeginPopupModal("My Popup", NULL))
        {
            DebuggerFunctions::TextInput("File Name: ", "##File Name: ", &FileName);

            if (DebuggerFunctions::Button("Confirm"))
            {
                if (FileName != "")
                {
                    // Create and open a file stream
                    std::ofstream outFile("assets\\scenes\\" + FileName + ".json");

                    if (!outFile) {
                        return;
                    }
                    Debugger::m_sceneNameToLoad = "assets\\scenes\\" + FileName + ".json";
                    // Write the JSON string to the file
                    outFile << newFile;

                    // Close the file
                    outFile.close();


                    Debugger::m_indexedGameobject = 0;
                    Debugger::m_gameobjectSelected = nullptr;
                    Debugger::m_app->m_scene->EndFrame();
                    //close down old scene
                    Debugger::m_app->m_scene->WriteToJson(Debugger::m_sceneNameLoaded);
                    //delete( Debugger::m_scene->m_input);
                    Debugger::m_input->ListObservers();
                    Debugger::m_input->RemoveObserver(Debugger::m_app);
                    Debugger::m_input->RemoveObserver(Debugger::m_app->m_scene);
                    Debugger::m_input->ListObservers();
                    for (auto ob : Debugger::m_input->m_subjInst.m_observers)
                    {
                        Debugger::m_input->RemoveObserver(ob);
                    }
                    Debugger::m_input->ListObservers();
                    //Debugger::m_input = new Input();
                    Debugger::m_app->m_scene->Cleanup();
                    Resources::ClearUnusedTextures();
                    Debugger::m_sceneNameLoaded = Debugger::m_sceneNameToLoad;
                    Debugger::m_app->m_scene = Scene::LoadFromJson(Debugger::m_sceneNameLoaded);
                    Debugger::m_app->m_scene->SetInput(&Debugger::m_app->m_input);
                    Debugger::m_app->m_scene->Init();
                    Debugger::m_app->InitInput();
                    Debugger::m_app->m_scene->Render();
                    Debugger::m_scene = Debugger::m_app->m_scene;

                    DebuggerFunctions::ReadSceneFolder();

                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::SameLine();
            if (DebuggerFunctions::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
    ImGui::End();
}
#endif
