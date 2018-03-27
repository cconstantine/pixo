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
float brightness = 1.0f;
std::string Pixlib::Shader::ShaderPreamble = R"(
#version 310 es
#ifdef GL_ES
precision highp float;
#endif
)";

extern "C" {
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_init(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_resize(JNIEnv* env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_mouse(JNIEnv* env, jobject obj, jint x, jint y);
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_zoom(JNIEnv* env, jobject obj, jfloat x);
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_step(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_addPattern(JNIEnv * env, jobject jobj, jstring name, jstring code);
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_randomPattern(JNIEnv * env, jobject jobj);
JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_setBrightness(JNIEnv * env, jobject jobj, jfloat x);


};

JNIEXPORT void JNICALL
Java_org_sillypants_pixo_GLES3JNILib_init(JNIEnv* env, jobject obj) {
    ALOGV("OpenGL %s, GLSL %s", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    int cube_size = 16;
    glClearColor(1.0f, 0.2f, 0.3f, 1.0f);

    if (fc == nullptr)
    {
      fc = new FadeCandy("localhost", cube_size);
    }

    if (application != nullptr) {
      delete application;
    }

    application = new App(glm::vec2(cube_size*cube_size, cube_size*cube_size));
    application->addFadeCandy(fc);


    if(pattern != nullptr) {
      delete pattern;
    }
}

JNIEXPORT void JNICALL Java_org_sillypants_pixo_GLES3JNILib_addPattern(JNIEnv * env, jobject jobj, jstring jname, jstring jcode)
{
  const char * n, *c;
  n = env->GetStringUTFChars( jname , NULL );
  c = env->GetStringUTFChars( jcode , NULL );

  std::string name(n);
  std::string code(c);

  application->patterns[name] = make_shared<Pattern>(code);
  if (pattern == nullptr) {

    ALOGV("name: %s", n);
    pattern = application->patterns[name].get();
  }

  env->ReleaseStringUTFChars(jname, n);
  env->ReleaseStringUTFChars(jcode, c);
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
Java_org_sillypants_pixo_GLES3JNILib_randomPattern(JNIEnv * env, jobject jobj) {
  auto it = application->patterns.begin();
  std::advance(it, rand() % application->patterns.size());
  pattern = it->second.get();
  pattern->resetStart();

  std::string name = it->first;
  ALOGV("Using pattern: %s\n", name.c_str());
}

JNIEXPORT void JNICALL
Java_org_sillypants_pixo_GLES3JNILib_setBrightness(JNIEnv * env, jobject jobj, jfloat x) {
  brightness = x;
}



JNIEXPORT void JNICALL
Java_org_sillypants_pixo_GLES3JNILib_step(JNIEnv* env, jobject obj) {

  if (pattern == nullptr){
    return;
  }
  if(pattern->getTimeElapsed() > 10*60) {
    Java_org_sillypants_pixo_GLES3JNILib_randomPattern(env, obj);
  }
  //ALOGV("GL Step \n");
  static std::chrono::time_point<std::chrono::high_resolution_clock> lastSecond = std::chrono::high_resolution_clock::now();

  static int frames = 0;
  frames++;
  std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();


  application->tick(pattern, brightness, width, height);
  application->move_perspective_to_camera();

  std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> time_duration = end - start;
  std::chrono::duration<float> second_seeking = end - lastSecond;

  if (second_seeking.count() > 1.0f) {
      ALOGV("GL Step: %2d fps, %3.1fms \n", frames, 1000.0f*time_duration.count());
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
