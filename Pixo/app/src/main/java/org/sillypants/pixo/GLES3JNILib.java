package org.sillypants.pixo;

public class GLES3JNILib {

    static {
        System.loadLibrary("native-lib");
    }

    public static native void init();
    public static native void resize(int width, int height);
    public static native void mouse(int x, int y);
    public static native void zoom(float x);
    public static native void step();
    public static native void addPattern(String name, String code);
    public static native void randomPattern();
    public static native void setBrightness(float brightness);

}