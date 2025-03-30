#pragma once
#include "Defines.h"
#ifdef DEBUGGER
#include "libraries/ImGUI/imconfig.h"
#ifndef ANDROID
#include "libraries/ImGUI/imgui_impl_glfw.h"
#else
#include "libraries/ImGUI/imgui_impl_android.h"
#include "libraries/ImGUI/imgui.h"
#include "android/native_window.h"
#endif
#include "libraries/ImGUI/imgui_impl_opengl3.h"
#include "libraries/ImGUI/imgui_stdlib.h"
#include "Input.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <list>
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <filesystem>
#include "Time.h"
#include "Structs.h"
#include "glm/glm.hpp"
#include "Vector2.h"
//#include "ComponentPtr.h"
#ifndef ANDROID
#include "cpptrace/cpptrace.hpp"
#include "Input.h"
#endif

#include "AudioManager.h"
#include <unordered_set>

class Scene;
class Application;
class AIAgent;
class GameObject;

	class DebuggerFunctions
	{
	public:
		friend class Debugger;
		friend class DebuggerMenus;
		friend class BaseComponent;
		static glm::vec3 Vec3Input(const char* label, const char* inputLabel, glm::vec3 value, bool inputX, bool inputY, bool inputZ);
		static bool TextInput(const char* label, const char* inputLabel, std::string* text);

		static void TextOutput(const char* label, std::string text);
		static float m_deltaTimesSeconds;
		static float m_deltaTimesSecondsAverage;
		static float m_deltaTimesSecondsSum;
		static float m_deltaTimeOnePercentLows;
		static float m_deltaTimeOnePercentHighs;
		static int buffersize; 
		static int tileMapType;
		static float m_sampleInterval;

		static float FloatInput(const char* label, const char* inputLabel, float value);

		static float IntInput(const char* label, const char* inputLabel, float value);

		static float IntArrowInput(const char* label, const char* inputLabel, float value);

		static bool Button(const char* label);

		static std::string DropDownPicker(const char* label, const char* inputLabel, std::string* currentlySelected, std::vector<std::string> slecteable, bool changeVariable = false);
		static std::string GameObjectDropDownPicker(const char* label, const char* inputLabel, std::string currentlySelected);
		static std::string AssetDropTarget(std::string assetType);

		static std::string Dropper(const char* label, const char* inputLabel, std::string* filePath, std::string assestType);

		static void OpenTileMapEditor();
		static void OpenAnimatorEditor();
		static void ReadSceneFolder();
	};

	class DebuggerMenus
	{
	public:
		friend class Debugger;
		friend class Debuggerfunctions;
		friend class BaseComponent;

		static void SpriteRendererMenu();

	private:

		static void AnimatorMenu();

		static void TileMapSelector();
		static void ProcGen();
		static void TileMapRendererFunctionality();
		static void SetAITargetPos();

		static void AddGrids(std::vector<Cell>& oldGrid, std::vector<Cell>& newGrid);
		static void RemoveGrids(std::vector<Cell>& oldGrid, std::vector<Cell>& newGrid);
		static void TilemapEditor();


		static void AssetViewerMenu();

		static void AddComponent();

		static void RemoveComponent();

		static void DeleteGameObject();

		//static void RemoveComponent(ComponentPtr<BaseComponent> comp);

		//static void RemoveComponent();

		static void CameraControl();

		static void SceneSelector();

		static void LoggerWindow();

		static void SceneWindow();

		static void AddGameobject();

		static void DeleteGameObjcet();

		static void StatsWindow();

		static void GameObjectWindow();
	};

	class Debugger
	{
	private:
		static bool m_gamePaused; 
		static bool m_uiPaused;
		static bool m_loggerWindowOpen;
		static bool m_sceneWindowOpen;
		static int m_indexedGameobject;
		static bool m_statsWindowOpen;
		static bool m_gameObjectWindow;
		static bool m_tilemapWindowOpen;
		static bool m_tilemapTypeChooserOpen;
		static bool m_animationEditorOpen;
		static bool m_audioWindowOpen;
		static std::string m_audioClipName;
		static bool m_sceneSelectorWindowOpen;

		static std::map<std::string, unsigned int> m_componentStringIDMap;

		static int tileMapType;

		static bool m_assetsWindowOpen;
		static bool m_cameraControlMode;
		static bool m_tilemapSelector;

		static float m_fps;
		static float m_fpsLastUpdate;
		static int m_frameCount;
		static std::vector<float> m_frameTimes;
		static float m_fpsAverage;
		static float m_fpsMax;
		static float m_fpsMin;
		static float m_fpsOnePercentLows;
		static float m_fpsOnePercentHighs;

		static float m_deltaTimesSeconds;
		static float m_deltaTimesSecondsAverage;
		static float m_deltaTimesSecondsSum;
		static float m_deltaTimeOnePercentLows;
		static float m_deltaTimeOnePercentHighs;
		static int buffersize;
		static float m_sampleInterval;

		static float m_listenerPosX;
		static float m_listenerPosY;
		static float m_sourcePosX;
		static float m_sourcePosY;
		static bool m_isDraggingListener;
		static bool m_isDraggingSource;

		static int m_aISpawnPosGridCoords[2];
		static std::unordered_set<std::string> selectedTags;

		static std::vector<std::string> m_messages;

		static Application* m_app;
		static Input* m_input;
		static std::string m_sceneNameToLoad;
		static std::string m_sceneNameLoaded;

		static void RunPerformance(Time _time);
		static void ResetStats();

		static std::vector<std::string> m_assests;
		static std::vector<std::string> m_componentList;
		static std::vector<Cell> m_newGrid;
		static std::vector<int> m_aIAgents;
	public:
		friend class DebuggerMenus;
		friend class DebuggerFunctions;
		friend class Application;
		friend class BaseComponent;
		static Scene* m_scene;
		static int m_gameobjectCount;
		static Vector2 m_aISelectedPos;

		static GameObject* m_gameobjectSelected;

#ifndef ANDROID
		static void Init(GLFWwindow* window, Scene* scene);
#else
		static void Init(ANativeWindow* window, Scene* scene);
#endif
		static void Render(Time _time);
		static void Cleanup();
		static void AddError(std::string LogType, const std::string& msg, bool exit);
		static void AddLog(const std::string& msg);
		static void AddLog(const char* format, ...);
		static void SaveLogToFile();
		static void StackTrace();

		//Test for Audio------------------------------------------------------------------
		static float GetListenerPosX() { return m_listenerPosX; }
		static float GetListenerPosY() { return m_listenerPosY; }
		static float GetSourcePosX() { return m_sourcePosX; }
		static float GetSourcePosY() { return m_sourcePosY; }


		static std::unordered_set<std::string>& GetSelectedTags() { return selectedTags; }

		static bool compareAudioClipNames(const std::string& a, const std::string& b);

		static bool isAudioClipSelected(const std::string& clipName);
		//--------------------------------------------------------------------------------
		static std::vector<Cell> GetGrid();

	};

#endif



