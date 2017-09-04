// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <pattern.hpp>
#include <camera.hpp>
#include <cube.hpp>
#include <led_cluster.hpp>
#include <renderer.hpp>
#include <scene.hpp>
#include <timer.hpp>

#include <nanogui/nanogui.h>
#include <fade_candy.hpp>

#include <app.hpp>


using namespace nanogui;
using namespace glm;
using namespace std;

Timer global_timer = Timer(120);

static GLfloat lastX = 400, lastY = 300;
static bool firstMouse = true;

bool keys[1024];
Screen *screen;

void sig_int_handler(int s){
  static unsigned int hits = 1;
  if (hits++ >= 3) {
    exit(1);
  }
  glfwSetWindowShouldClose(window, GL_TRUE);
}

int main( int argc, char** argv )
{  
  if(argc < 3) {
    fprintf(stderr, "Usage: %s LEDS_PER_SIDE hostname [pattern file]*\n", argv[0]);
    exit(1);
  }
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }  

  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DEPTH_BITS, 24 );

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 800, 800, "Pixo", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // Initialize GLEW
  glewExperimental = GL_TRUE; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }
  // Setup some OpenGL options
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  
  const int leds_per_side = atoi(argv[1]);
  FadeCandy fc = FadeCandy(argv[2], leds_per_side);

  App application(&fc);
  glfwSetWindowUserPointer(window, &application);

  for(int i = 3;i < argc;i++) {
    std::string fragmentCode;
    std::ifstream fShaderFile;
    // ensures ifstream objects can throw exceptions:
    fShaderFile.exceptions (std::ifstream::badbit);

    std::string fulLFragmentath = std::string(argv[i]);
    ALOGV("Loading: %s\n", fulLFragmentath.c_str());

    fShaderFile.open(fulLFragmentath.c_str());
    std::stringstream fShaderStream;
    fShaderStream << fShaderFile.rdbuf();
    fShaderFile.close();
    
    // Convert stream into string
    fragmentCode = fShaderStream.str();

    std::string name;
    for(unsigned int i = fulLFragmentath.size();i >= 0;i--) {
      if (fulLFragmentath[i] == '/') {
        break;
      }
      name = fulLFragmentath[i] + name;
    }
    application.add_pattern(new Pattern(name, fragmentCode.c_str()));
  }

  // Create a nanogui screen and pass the glfw pointer to initialize
  screen = new Screen();
  screen->initialize(window, true);
  GLenum glErr = glGetError();
  while (glErr != GL_NO_ERROR)
  {
      ALOGV("Screen %04x\n", glErr);
      glErr = glGetError();
  }
  // Create nanogui gui
  FormHelper *gui = new FormHelper(screen);
  nanogui::ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Status");

  gui->addVariable<string>("fps",
    [&](string value) { value; },
    [&]() -> string {
      char ret[256];
      float fps = application.scene_fps();
      sprintf(ret, "%2.02f", fps);
      return ret;
    },
    false)->setValue("00.00");

  gui->addVariable<string>("render time",
    [&](string value) { value; },
    [&]() -> string {
      char ret[256];
      sprintf(ret, "%2.02fms", global_timer.duration()*1000);
      return ret;
    },
    false)->setValue("00.00");

  gui->addVariable<string>("scene time",
    [&](string value) { value; },
    [&]() -> string {
      char ret[256];
      sprintf(ret, "%2.02fms", application.scene_render_time()*1000);
      return ret;
    },
    false)->setValue("00.00");

  gui->addVariable<string>("led time",
    [&](string value) { value; },
    [&]() -> string {
      char ret[256];
      sprintf(ret, "%2.02fms", application.led_render_time()*1000);
      return ret;
    },
    false)->setValue("00.00");

  gui->addVariable<string>("Shader",
    [&](string value) { value; },
    [&]() -> string {
      return application.current_pattern()->getName();
    },
    false)->setValue("                 ");

  ImageView *imageWidget = new ImageView(nanoguiWindow, application.getPatternTexture().id);
  imageWidget->setFixedSize(Eigen::Vector2i(160, 160));
  imageWidget->setFixedScale(true);
  gui->addWidget("", imageWidget);
  screen->setVisible(true);
  screen->performLayout();

  imageWidget->fit();


  glfwSetCursorPosCallback(window,
          [] (GLFWwindow *window, double x, double y) {
          if (!screen->cursorPosCallbackEvent(x, y)) {
            App* app = (App*)glfwGetWindowUserPointer(window);
            int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

            if (state != GLFW_PRESS) {
              firstMouse = true;
            }
            if(firstMouse)
            {
              lastX = x;
              lastY = y;
              firstMouse = false;
            }

            GLfloat xoffset = x - lastX;
            GLfloat yoffset = y - lastY; 

            lastX = x;
            lastY = y;

            app->ProcessMouseMovement(xoffset, yoffset);
          }
      }
  );

  glfwSetMouseButtonCallback(window,
      [](GLFWwindow *window, int button, int action, int modifiers) {
          if (!screen->mouseButtonCallbackEvent(button, action, modifiers)) {
            if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
              glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
              glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
          }
      }
  );

  glfwSetKeyCallback(window,
      [](GLFWwindow *window, int key, int scancode, int action, int mods) {
          if (!screen->keyCallbackEvent(key, scancode, action, mods)) {
            if (action == GLFW_REPEAT) {
              return;
            }
            if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
              glfwSetWindowShouldClose(window, GL_TRUE);
 
            if(key == GLFW_KEY_ENTER) {
              keys[NEXT_PATTERN] = (action == GLFW_PRESS); 
            }

          }
      }
  );

  glfwSetCharCallback(window,
      [](GLFWwindow *window, unsigned int codepoint) {
          screen->charCallbackEvent(codepoint);
      }
  );

  glfwSetDropCallback(window,
      [](GLFWwindow *window, int count, const char **filenames) {
          screen->dropCallbackEvent(count, filenames);
      }
  );

  glfwSetScrollCallback(window,
      [](GLFWwindow *window, double x, double y) {
          if (!screen->scrollCallbackEvent(x, y)) {
            App* app = (App*)glfwGetWindowUserPointer(window);

            app->ProcessMouseScroll(y);

          }
     }
  );

  glfwSetFramebufferSizeCallback(window,
      [](GLFWwindow *window, int width, int height) {
          screen->resizeCallbackEvent(width, height);
      }
  );

  glErr = glGetError();
  while (glErr != GL_NO_ERROR)
  {
      ALOGV("Preloop %04x\n", glErr);
      glErr = glGetError();
  }

  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = sig_int_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  while(!glfwWindowShouldClose(window)) {
    global_timer.start();

    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    application.tick(width, height);
    application.move_perspective_to_camera();

    if(keys[NEXT_PATTERN]) {
      keys[NEXT_PATTERN] = false;
      application.change_pattern();
    }


    gui->refresh();
    // Draw nanogui
    screen->performLayout();
    screen->drawContents();
    screen->drawWidgets();

    global_timer.end();
    
    glfwSwapBuffers(window);

    GLenum glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        ALOGV("glError %04x\n", glErr);
        glErr = glGetError();
    }

  }

  fc.clear();
  

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  ALOGV("Exiting\n");

  return 0;
}


