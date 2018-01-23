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

using namespace glm;
using namespace std;
using namespace Pixlib;

App     *application = nullptr;
Pattern *pattern = nullptr;
FadeCandy* fc = nullptr;
int width, height;

extern "C" {
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_init(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_resize(JNIEnv* env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_mouse(JNIEnv* env, jobject obj, jint x, jint y);
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_zoom(JNIEnv* env, jobject obj, jfloat x);
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_step(JNIEnv* env, jobject obj);
};

JNIEXPORT void JNICALL
Java_org_sillypants_pixo_GLES3JNILib_init(JNIEnv* env, jobject obj) {
    ALOGV("OpenGL %s, GLSL %s", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    glClearColor(1.0f, 0.2f, 0.3f, 1.0f);

    if (fc == nullptr)
    {
      fc = new FadeCandy("localhost", 16);
    }

    if (application != nullptr) {
      delete application;
    }

    application = new App(glm::vec2(256, 256));
    application->addFadeCandy(fc);


    if(pattern != nullptr) {
      delete pattern;
    }
    std::string fragmentCode;
    std::ifstream fShaderFile;
    // ensures ifstream objects can throw exceptions:
    fShaderFile.exceptions(std::ifstream::badbit);

    std::string fulLFragmentath = std::string(
            "/data/user/0/org.sillypants.pixo/files/patterns/spherical_polyhedra.glsl");
    ALOGV("Loading: %s\n", fulLFragmentath.c_str());

    fShaderFile.open(fulLFragmentath.c_str());
    std::stringstream fShaderStream;
    fShaderStream << fShaderFile.rdbuf();
    fShaderFile.close();

    // Convert stream into string
    fragmentCode = fShaderStream.str();

    std::string name;
    for (unsigned int i = fulLFragmentath.size(); i >= 0; i--) {
      if (fulLFragmentath[i] == '/') {
        break;
      }
      name = fulLFragmentath[i] + name;
    }

    //ALOGV("Compiling:\n%s\n", fragmentCode.c_str());
    pattern = new Pattern(fragmentCode.c_str());

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
  application->ProcessMouseMovement(x/2, y/2);
}

JNIEXPORT void JNICALL
Java_org_sillypants_pixo_GLES3JNILib_zoom(JNIEnv* env, jobject obj, jfloat x) {
  application->ProcessMouseScroll(x/2);
}

JNIEXPORT void JNICALL
Java_org_sillypants_pixo_GLES3JNILib_step(JNIEnv* env, jobject obj) {

  if (pattern == nullptr){
    return;
  }
  //ALOGV("GL Step \n");
  static std::chrono::time_point<std::chrono::high_resolution_clock> lastSecond = std::chrono::high_resolution_clock::now();

  static int frames = 0;
  frames++;
  std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();


  application->tick(pattern, width, height);
  application->move_perspective_to_camera();

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
