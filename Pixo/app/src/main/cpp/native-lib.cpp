#include <jni.h>
#include <string>
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include <android/log.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <chrono>
#include <EGL/egl.h>

#include <pixlib.hpp>

Scene *scene = nullptr;
Shader *pattern = nullptr;
FadeCandy* fc = nullptr;
LedCluster* domeLeds;
ScreenRender* screen_renderer;
int width, height;

std::string Shader::root;


IsoCamera viewed_from;
IsoCamera camera;

class Textures {
public:
    Textures() : texture(400, 400), fb_texture(256,256) { }

    Texture texture;
    Texture fb_texture;
};
Textures* texs;

extern "C" {
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_init(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_resize(JNIEnv* env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_mouse(JNIEnv* env, jobject obj, jint x, jint y);
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_step(JNIEnv* env, jobject obj);
};

JNIEXPORT void JNICALL
Java_org_sillypants_pixo_GLES3JNILib_init(JNIEnv* env, jobject obj) {
    ALOGV("OpenGL %s, GLSL %s", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    glClearColor(1.0f, 0.2f, 0.3f, 1.0f);

    DIR *dp;
    struct dirent *ep;
    Shader::root = std::string("/data/user/0/org.sillypants.pixo/files/");
    dp = opendir ("/data/user/0/org.sillypants.pixo/files/shaders");
    if (dp != NULL)
    {
        while (ep = readdir (dp))
            ALOGV("flie: %s\n", ep->d_name);
        (void) closedir (dp);
    }
    if (scene != nullptr)
    {
      delete pattern;
      delete texs;
      delete domeLeds;
      delete screen_renderer;
      delete scene;
    }
    if (fc == nullptr)
    {
      fc = new FadeCandy("192.168.42.66", 16);
    }
    pattern = new Shader("shaders/pattern.frag", "patterns/neon_ring.glsl");
    texs = new Textures();
    domeLeds = new LedCluster(fc, texs->texture, texs->fb_texture);

    screen_renderer = new ScreenRender();

    scene = new Scene(screen_renderer, domeLeds);
}

JNIEXPORT void JNICALL
Java_org_sillypants_pixo_GLES3JNILib_resize(JNIEnv* env, jobject obj, jint jwidth, jint jheight) {

//    glViewport(0, 0, width, height);
    width = jwidth;
    height = jheight;
    glViewport(0, 0, width, height);
}

JNIEXPORT void JNICALL
Java_org_sillypants_pixo_GLES3JNILib_mouse(JNIEnv* env, jobject obj, jint x, jint y) {

  camera.ProcessMouseMovement(x/2, y/2);
}

JNIEXPORT void JNICALL
Java_org_sillypants_pixo_GLES3JNILib_step(JNIEnv* env, jobject obj) {
  //ALOGV("GL Step \n");
  static std::chrono::time_point<std::chrono::high_resolution_clock> lastSecond = std::chrono::high_resolution_clock::now();

  static int frames = 0;
  frames++;
  std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

  domeLeds->render(viewed_from, *pattern);
  scene->render(camera, width, height);

  std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> time_duration = end - start;
  std::chrono::duration<float> second_seeking = end - lastSecond;

  if (second_seeking.count() > 1.0f) {
      ALOGV("GL Step: frames: %d \n", frames);
      ALOGV("GL Step: time_duration: %fms \n", 1000*time_duration.count());
      frames = 0;
      lastSecond = std::chrono::high_resolution_clock::now();
  }


  GLenum glErr = glGetError();
  while (glErr != GL_NO_ERROR)
  {
      ALOGV("glError %04x\n", glErr);
      glErr = glGetError();
  }
}
