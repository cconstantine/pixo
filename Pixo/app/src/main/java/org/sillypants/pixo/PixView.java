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
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.GestureDetector;
import android.view.View;
import android.opengl.GLSurfaceView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
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
    }
    private float mPreviousX;
    private float mPreviousY;

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        mScaleDetector.onTouchEvent(e);
        mScrollDetector.onTouchEvent(e);

        return true;
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

        private void copyAsset(String assetSet) {
            AssetManager assetManager = context.getAssets();
            String pathToInternalDir = context.getFilesDir().getAbsolutePath();

            new File(pathToInternalDir + "/" + assetSet).mkdirs();
            try {
                String files[] = assetManager.list(assetSet);
                for(int i =0;i < files.length;i++) {
                    Log.v("PixView", "Asset: " + pathToInternalDir  + "/" + assetSet + "/" + files[i]);
                    File outfile = new File(pathToInternalDir  + "/" + assetSet + "/" + files[i]);
                    outfile.createNewFile();
                    OutputStream output = new FileOutputStream(outfile);
                    InputStream input = assetManager.open(assetSet + "/" + files[i]);
                    try {
                        byte[] buffer = new byte[4 * 1024];
                        int read;
                        while ((read = input.read(buffer)) != -1) {
                            output.write(buffer, 0, read);
                        }
                        output.flush();
                    } finally {
                        output.close();
                    }
                }
            } catch(IOException e) {
                Log.v("PixView", "IOException", e);
            }

        }

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            copyAsset("patterns");
            GLES3JNILib.init();
        }
    }
}
