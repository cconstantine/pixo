package org.sillypants.pixo;

import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;
import android.text.TextPaint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.GestureDetector;
import android.view.View;
import android.opengl.GLSurfaceView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

/**
 * TODO: document your custom view class.
 */
public class PixView extends GLSurfaceView {

    private static final String TAG = "GLES3JNI";
    private static final boolean DEBUG = true;
    private int lastX = 0;
    private int lastY = 0;
    Renderer renderer;
    private ScaleGestureDetector mScaleDetector;
    private GestureDetector mScrollDetector;

    public PixView(Context context) {
        super(context);
        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
        //setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        //setEGLContextClientVersion(2);
        setEGLContextClientVersion(2);

        renderer = new Renderer(context);
        setRenderer(renderer);

        setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        mScaleDetector = new ScaleGestureDetector(context, new ScaleListener());
        mScrollDetector = new GestureDetector(context, new ScrollListener());
        setFocusable(true);
        setFocusableInTouchMode(true);


    }
    private float mPreviousX;
    private float mPreviousY;

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        super.onTouchEvent(e);
        mScaleDetector.onTouchEvent(e);
        mScrollDetector.onTouchEvent(e);

        return true;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        Log.v("PixView", "onKeyDown");

        switch (keyCode) {
            case KeyEvent.KEYCODE_LEFT_BRACKET:
                Log.v("PixView", "[");
                GLES3JNILib.zoom(-(int)100);
                return true;
            case KeyEvent.KEYCODE_RIGHT_BRACKET:
                Log.v("PixView", "]");
                GLES3JNILib.zoom((int)100);
                return true;
            case KeyEvent.KEYCODE_DPAD_UP:
                GLES3JNILib.mouse(0, -100);
                return true;
            case KeyEvent.KEYCODE_DPAD_DOWN:
                GLES3JNILib.mouse(0, 100);
                return true;
            case KeyEvent.KEYCODE_DPAD_RIGHT:
                GLES3JNILib.mouse(-100, 0);
                return true;
            case KeyEvent.KEYCODE_DPAD_LEFT:
                GLES3JNILib.mouse(100, 0);
                return true;
            case KeyEvent.KEYCODE_ENTER:
                GLES3JNILib.randomPattern();
                return true;
            case KeyEvent.KEYCODE_0:
                GLES3JNILib.setBrightness(0.0f);
                return true;
            case KeyEvent.KEYCODE_9:
                GLES3JNILib.setBrightness(1.0f);
                return true;
            case KeyEvent.KEYCODE_BACK:
                return true;

            default:
                return super.onKeyUp(keyCode, event);
        }
    }

    private class ScaleListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            GLES3JNILib.zoom((-100*(1 - detector.getScaleFactor())));
            return true;
        }

    }

    private class ScrollListener extends GestureDetector.SimpleOnGestureListener {

        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2,
                                float distanceX, float distanceY) {
            GLES3JNILib.mouse(-(int)distanceX, -(int)distanceY);
            return true;
        }
    }
    private static class Renderer implements GLSurfaceView.Renderer {
        private Context context;
        public Renderer(Context context) {
            super();
            this.context = context;
        }

        public void onDrawFrame(GL10 gl) {
            GLES3JNILib.step();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            GLES3JNILib.resize(width, height);
            String extensions = gl.glGetString(GL10.GL_EXTENSIONS);
            Log.v("PixView", "extensions: " + extensions);
        }
        private String read_stream(InputStream input) {
            final int bufferSize = 1024;
            final char[] buffer = new char[bufferSize];
            final StringBuilder out = new StringBuilder();
            try {
                InputStreamReader in = new InputStreamReader(input, "UTF-8");
                for (; ; ) {
                    int rsz = in.read(buffer, 0, buffer.length);
                    if (rsz < 0)
                        break;
                    out.append(buffer, 0, rsz);
                }
            } catch(IOException e) {
                Log.v("PixView", "IOException", e);
            } finally {
                return out.toString();
            }

        }

        private void loadPatterns() {
            String assetSet = "patterns";
            AssetManager assetManager = context.getAssets();
            String pathToInternalDir = context.getFilesDir().getAbsolutePath();

            new File(pathToInternalDir + "/" + assetSet).mkdirs();
            try {
                String files[] = {
                        "basic.glsl",
                        "blades.glsl",
                        "clouds.glsl",
                        "cnoise.glsl",
                        "corrente_continua.glsl",
                        "fire.glsl",
                        "fireball.glsl",
                        "flower.glsl",
                        "hypno_rings.glsl",
                        "lightning.glsl",
                        "paint.glsl",
                        "plasam.glsl",
                        "pulsing_circles.glsl",
                        "rings.glsl",
                        "soccer.glsl",
                        "spherical_polyhedra.glsl",
                        "spiral.glsl",
                        "star.glsl",
                        "swirl.glsl",
                        "tiedy.glsl",
                        "warping_grid.glsl"
                };
                for(int i =0;i < files.length;i++) {
                    Log.v("PixView", "Asset: " + pathToInternalDir  + "/" + assetSet + "/" + files[i]);
                    InputStream input = assetManager.open(assetSet + "/" + files[i]);
                    String result = read_stream(input);
                    GLES3JNILib.addPattern(files[i], result);
                }
            } catch(IOException e) {
                Log.v("PixView", "IOException", e);
            }

        }

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            //copyAsset("patterns");
            GLES3JNILib.init();
            loadPatterns();
        }
    }
}
