#ifdef ANDROID
#include <jni.h>
#include <string>
#include <android/asset_manager_jni.h>
#include <android/native_window.h>
#include <android/native_activity.h>
#include <android/input.h>
#include <android/keycodes.h>
#include <android/native_window_jni.h>
#include "Resources.h"
#include "Application.h"

Application* app;

extern "C" JNIEXPORT jstring

JNICALL
Java_com_example_group_13_1engine_1android_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    return env->NewStringUTF(hello.c_str());
}

extern "C" void
Java_com_example_group_13_1engine_1android_MainActivity_EngineInit(JNIEnv *env, jclass clazz) {

    if (app == nullptr)
        app = new Application();

    ///gladLoadGLES2Loader()

    LOGV("Initializing Application");
    app->Init();
    //app->Loop();
    //app->Cleanup();



    //act->callbacks->onInputQueueCreated = *onInputQueueCreated;
}



extern "C"
JNIEXPORT void JNICALL
Java_com_example_group_13_1engine_1android_MainActivity_EngineDraw(JNIEnv *env, jclass clazz) {
    app->Loop();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_group_13_1engine_1android_MainActivity_EngineResize(JNIEnv *env, jclass clazz, int width, int height) {
    if (app == nullptr)
        app = new Application();
    app->OnAndroidResize(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_group_13_1engine_1android_MainActivity_AssetsInit(JNIEnv *env, jclass clazz, jobject assetManager) {
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    Resources::SetAssetManager(mgr);
    LOGV("added asset manager");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_group_13_1engine_1android_MainActivity_WindowInit(JNIEnv *env, jclass clazz, jobject surface) {
    ANativeWindow* win = ANativeWindow_fromSurface(env, surface);
    if (app == nullptr)
        app = new Application();
    app->m_window = win;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_group_13_1engine_1android_MainActivity_OnMotionEvent(JNIEnv *env, jclass clazz, jobject event, jint pointerID) {
    const AInputEvent* e = AMotionEvent_fromJava(env, event);
    if (app != nullptr)
        app->OnAndroidInput(e, pointerID);
    //LOGV("motion");
    AInputEvent_release(e);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_group_13_1engine_1android_MainActivity_OnKeyEvent(JNIEnv *env, jclass clazz, jobject event) {
    const AInputEvent* e = AKeyEvent_fromJava(env, event);
    if (app != nullptr)
        app->OnAndroidInput(e, 0);
    AInputEvent_release(e);

}

#endif
extern "C"
JNIEXPORT void JNICALL
Java_com_example_group_13_1engine_1android_MainActivity_OnGyroEvent(JNIEnv *env, jclass clazz,
                                                                    jfloat x, jfloat y, jfloat z) {
    if (app != nullptr)
        app->OnAndroidGyro(x, y, z);
}