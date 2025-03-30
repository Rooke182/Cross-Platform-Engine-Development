package com.example.group_3_engine_android;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;


import android.opengl.EGLContext;
import android.opengl.EGL15;
import android.opengl.GLES30;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.Surface;

class EngineRenderer implements GLSurfaceView.Renderer
{
    public SurfaceView parent;
    static {
        System.loadLibrary("group_3_engine_android");
    }
    public void onSurfaceCreated(GL10 unused, EGLConfig config) {
        // Set the background frame color
        GLES30.glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
        MainActivity.WindowInit(parent.getHolder().getSurface()); //needed for ImGUI
        MainActivity.EngineInit();
    }
    public void onDrawFrame(GL10 unused) {

        // Redraw background color
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT);
        MainActivity.EngineDraw();
    }
    public void onSurfaceChanged(GL10 unused, int width, int height) {
        GLES30.glViewport(0, 0, width, height);
        MainActivity.EngineResize(width, height);
    }
}

class SurfaceView extends GLSurfaceView
{
    private final EngineRenderer renderer;

    public SurfaceView(Context context){
        super(context);






        //setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);

        renderer = new EngineRenderer();
        renderer.parent = this;

        // Set the Renderer for drawing on the GLSurfaceView
        setRenderer(renderer);

        MainActivity.AssetsInit(context.getAssets());
    }
}

