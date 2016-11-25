// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <boost/shared_ptr.hpp>


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
using namespace glm;



#include <shader.hpp>
#include <camera.hpp>
#include <cube.hpp>
#include <led_cluster.hpp>
#include <renderer.hpp>
#include <scene.hpp>
#include <nanogui/nanogui.h>
#include <fade_candy.hpp>

using namespace nanogui;
Screen *screen = nullptr;
Scene *scene = nullptr;
Shader *pattern = nullptr;
std::chrono::duration<float> time_duration;

IsoCamera viewed_from;
IsoCamera camera;

static GLfloat lastX = 400, lastY = 300;
static bool firstMouse = true;

bool keys[1024];
std::string Shader::root = std::string("../");

int main( int argc, char** argv )
{
  
  if(argc < 3) {
    fprintf(stderr, "Usage: %s LEDS_PER_SIDE [pattern file]*\n", argv[0]);
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
  // Options
  //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
  glPointSize(1);
  
  vector<Shader> patterns;
  for(int i = 2;i < argc;i++) {
    patterns.push_back(Shader("shaders/pattern.frag", &argv[i][2]));
  }

  pattern = &patterns[rand() % patterns.size()];
  const int leds_per_side = atoi(argv[1]);
  FadeCandy fc = FadeCandy("localhost", leds_per_side);


  LedCluster domeLeds(&fc);
  scene = new Scene(&domeLeds);

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
  bool enabled = true;
  FormHelper *gui = new FormHelper(screen);
  nanogui::ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Status");

  gui->addVariable<string>("fps",
    [&](string value) { value; },
    [&]() -> string {
      char ret[256];
      float fps = scene->getFps();
      sprintf(ret, "%2.02f", fps);
      return ret;
    },
    false)->setValue("00.00");

  gui->addVariable<string>("render time",
    [&](string value) { value; },
    [&]() -> string {
      char ret[256];
      sprintf(ret, "%2.02f", time_duration.count());
      return ret;
    },
    false)->setValue("00.00");

  gui->addVariable<string>("Shader",
    [&](string value) { value; },
    [&]() -> string {
      return pattern->fragmentPath.c_str();
    },
    false)->setValue("                 ");

  ImageView *imageWidget = new ImageView(nanoguiWindow, domeLeds.getPatternTexture().id);
  imageWidget->setFixedSize(Eigen::Vector2i(160, 160));
  imageWidget->setFixedScale(true);
  gui->addWidget("", imageWidget);
  screen->setVisible(true);
  screen->performLayout();

  imageWidget->fit();



  glfwSetCursorPosCallback(window,
          [](GLFWwindow *window, double x, double y) {
          if (!screen->cursorPosCallbackEvent(x, y)) {
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

            camera.ProcessMouseMovement(xoffset, yoffset);
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

            // Camera controls
            if(key == GLFW_KEY_W) {
              keys[FORWARD] = (action == GLFW_PRESS);  
            }
            if(key == GLFW_KEY_S) {
              keys[BACKWARD] = (action == GLFW_PRESS);  
            }
            if(key == GLFW_KEY_A) {
              keys[LEFT] = (action == GLFW_PRESS); 
            }
            if(key == GLFW_KEY_D) {
              keys[RIGHT] = (action == GLFW_PRESS); 
            }
            if(key == GLFW_KEY_LEFT_SHIFT) {
              keys[MATCH_VIEW] = (action == GLFW_PRESS); 
            }
            if(key == GLFW_KEY_LEFT_CONTROL) {
              keys[TOWARDS_VIEW] = (action == GLFW_PRESS); 
            }
            if(key == GLFW_KEY_ENTER) {
              keys[NEXT_PATTERN] = (action == GLFW_PRESS); 
            }
            if(key == GLFW_KEY_RIGHT ) {
              keys[ORB_RIGHT] = (action == GLFW_PRESS); 
            }
            if(key == GLFW_KEY_LEFT) {
              keys[ORB_LEFT] = (action == GLFW_PRESS); 
            }
            if(key == GLFW_KEY_UP ) {
              keys[ORB_UP] = (action == GLFW_PRESS); 
            }
            if(key == GLFW_KEY_DOWN) {
              keys[ORB_DOWN] = (action == GLFW_PRESS); 
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
            camera.ProcessMouseScroll(y);

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
  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // Render the scene
    std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

    domeLeds.render(viewed_from, *pattern);
    scene->render(camera, width, height);

    std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
    time_duration = end - start;

    bool next = keys[NEXT_PATTERN];
    if(next) {
      keys[NEXT_PATTERN] = false;
      pattern = &patterns[rand() % patterns.size()];
    }

    // if(keys[MATCH_VIEW ]) {
    //   scene->matchViewToPerspective();
    // }


    gui->refresh();
    // Draw nanogui
    screen->performLayout();
    screen->drawContents();
    screen->drawWidgets();

		glfwSwapBuffers(window);


    GLenum glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        ALOGV("glError %04x\n", glErr);
        glErr = glGetError();
    }

	}

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


