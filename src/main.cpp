// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <boost/shared_ptr.hpp>


#include <iostream>

// Include GLEW
//#include <GL/glew.h>

// Include GLFW
#include <opengl.h>
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
#include <timer.hpp>

#include <nanogui/nanogui.h>
#include <fade_candy.hpp>

using namespace nanogui;
Scene *scene = nullptr;
Shader *pattern = nullptr;
Timer global_timer = Timer(120);
LedCluster *domeLeds;

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
        return -1;
    }  

 /* 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DEPTH_BITS, 24 );
*/
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 800, 800, "Pixo", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
printf("VENDOR:  %s\n", glGetString(GL_VENDOR));
printf("RENDER:  %s\n", glGetString(GL_RENDERER));
printf("VERSION: %s\n", glGetString(GL_VERSION));

exit(1);
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


  domeLeds = new LedCluster(&fc);
  scene = new Scene(domeLeds);

  // Create a nanogui screen and pass the glfw pointer to initialize

  glfwSetCursorPosCallback(window,
          [](GLFWwindow *window, double x, double y) {
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
  );

  glfwSetMouseButtonCallback(window,
      [](GLFWwindow *window, int button, int action, int modifiers) {

            if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
              glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
              glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
          }
  );

  glfwSetKeyCallback(window,
      [](GLFWwindow *window, int key, int scancode, int action, int mods) {
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
  );

  glfwSetScrollCallback(window,
      [](GLFWwindow *window, double x, double y) {
            camera.ProcessMouseScroll(y);
     }
  );

  GLenum glErr = glGetError();
  while (glErr != GL_NO_ERROR)
  {
      ALOGV("Preloop %04x\n", glErr);
      glErr = glGetError();
  }
  int frames = 0;
  while(!glfwWindowShouldClose(window)) {
    if (frames++ > 60) {
      fprintf(stderr, "fps: %3.2f\n", scene->getFps());
      frames = 0;
    }
    global_timer.start();

    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    viewed_from.moveTowards(camera, scene->getTimeDelta()*0.8);

    domeLeds->render(viewed_from, *pattern);
    scene->render(camera, width, height);

    bool next = keys[NEXT_PATTERN];
    if(next) {
      keys[NEXT_PATTERN] = false;
      pattern = &patterns[rand() % patterns.size()];
    }


    global_timer.end();

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


