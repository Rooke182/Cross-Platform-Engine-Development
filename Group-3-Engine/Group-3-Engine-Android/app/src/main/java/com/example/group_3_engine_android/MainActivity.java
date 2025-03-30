package com.example.group_3_engine_android;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.res.AssetManager;
import android.hardware.Sensor;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.hardware.SensorEvent;
import android.view.DragEvent;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.View;

import com.example.group_3_engine_android.databinding.ActivityMainBinding;



public class MainActivity extends AppCompatActivity {

    // Used to load the 'group_3_engine_android' library on application startup.
    static {
        System.loadLibrary("group_3_engine_android");
    }

    private ActivityMainBinding binding;

    private SurfaceView gLView;

    private SensorManager sensorManager;
    private Sensor sensor;

    private SensorEventListener sensorEventListener;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //need to do some checks for gles version i think

        //binding = ActivityMainBinding.inflate(getLayoutInflater());
        //setContentView(binding.getRoot());
        gLView = new SurfaceView(this);
        setContentView(gLView);




        // Example of a call to a native method
        //TextView tv = binding.sampleText;
        //tv.setText(stringFromJNI());
        //onKeyDown()

        gLView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                OnMotionEvent(event, event.getPointerId(0));
                //Log.v("EVENT", "touch");
                return true;
            }
        });

        gLView.setOnKeyListener(new View.OnKeyListener() {
            @Override
            public boolean onKey(View v, int i, KeyEvent event) {
                OnKeyEvent(event);
                //Log.v("EVENT", "key");
                return true;
            }
        });

        gLView.setOnGenericMotionListener(new View.OnGenericMotionListener()
        {
            @Override
            public boolean onGenericMotion(View v, MotionEvent event)
            {
                OnMotionEvent(event, event.getPointerId(0));
                //Log.v("EVENT", "motion");
                return true;
            }
        });

        gLView.setOnHoverListener(new View.OnHoverListener()
        {
            @Override
            public boolean onHover(View v, MotionEvent event)
            {
                OnMotionEvent(event, event.getPointerId(0));
                //Log.v("EVENT", "hover");
                return true;
            }
        });

        gLView.setOnDragListener(new View.OnDragListener()
        {
            @Override
            public boolean onDrag(View v, DragEvent event)
            {
                //OnMotionEvent(event);
                //Log.v("EVENT", "drag");
                return true;
            }
        });

        gLView.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View view) {
                return false;
            }
        });

        sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        sensor = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);

        sensorEventListener = new SensorEventListener()
        {
            @Override
            public void onSensorChanged(SensorEvent event)
            {
                float x = event.values[0];
                float y = event.values[1];
                float z = event.values[2];
                OnGyroEvent(x, y, z);
            }

            @Override
            public void onAccuracyChanged(Sensor sensor, int i) {

            }
        };
        sensorManager.registerListener(sensorEventListener, sensor, SensorManager.SENSOR_DELAY_GAME);
    }



    /**
     * A native method that is implemented by the 'group_3_engine_android' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public static native void EngineInit();
    public static native void EngineDraw();
    public static native void EngineResize(int width, int height);
    public static native void AssetsInit(AssetManager assetManager);
    public static native void WindowInit(Surface surface);

    public static native void OnMotionEvent(MotionEvent event, int pointerID);
    public static native void OnKeyEvent(KeyEvent event);

    public static native void OnGyroEvent(float x, float y, float z);

    //public native void MainLoop();
    //public native void Cleanup();
}