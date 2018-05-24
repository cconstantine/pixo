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
#include <glm/gtx/string_cast.hpp>
#include <pixlib.hpp>


#include <nanogui/nanogui.h>
#include <mongoose.h>
#include <thread>

std::string Pixlib::Shader::ShaderPreamble = "#version 330\n";

using namespace glm;
using namespace std;
using namespace Pixlib;
nanogui::Screen *screen = nullptr;

Timer global_timer = Timer(120);

static GLfloat lastX = 400, lastY = 300;
static bool firstMouse = true;

bool keys[1024];

void sig_int_handler(int s){
  static unsigned int hits = 1;
  if (hits++ >= 3) {
    exit(1);
  }
  glfwSetWindowShouldClose(window, GL_TRUE);
}


static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;
static int has_prefix(const struct mg_str *uri, const struct mg_str *prefix) {
  return uri->len > prefix->len && memcmp(uri->p, prefix->p, prefix->len) == 0;
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  static const struct mg_str api_prefix = MG_MK_STR("/api/v1");
  struct http_message *hm = (struct http_message *) ev_data;
  struct mg_str key;

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      if (has_prefix(&hm->uri, &api_prefix)) {

        mg_printf(nc, "%s",
                  "HTTP/1.0 501 Not Implemented\r\n"
                  "Content-Length: 0\r\n\r\n");
      } else {
        mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */
      }
      break;
    default:
      break;
  }
}

bool running = true;

void web_listen() {

  struct mg_mgr mgr;
  struct mg_connection *nc;
  int i;

  /* Open listening socket */
  mg_mgr_init(&mgr, NULL);
  nc = mg_bind(&mgr, s_http_port, ev_handler);
  mg_set_protocol_http_websocket(nc);

  while(running) {
    mg_mgr_poll(&mgr, 100);

  }
  mg_mgr_free(&mgr);
}

std::thread webserver (web_listen); 
int main( int argc, char** argv )
{  
  if(argc < 2) {
    fprintf(stderr, "Usage: %s db_filename\n", argv[0]);
    exit(1);
  }

  const std::string db_filename(argv[1]);
  ALOGV("loading: %s\n", db_filename.c_str());

  unsigned int arg_i = 2;


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
      fprintf( stderr, "Failed to open GLFW window.\n" );
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

  App application = App(Storage(db_filename));

  glfwSetWindowUserPointer(window, &application);
  // Create a nanogui screen and pass the glfw pointer to initialize
  screen = new nanogui::Screen();
  screen->initialize(window, true);
  GLenum glErr = glGetError();
  while (glErr != GL_NO_ERROR)
  {
      glErr = glGetError();
  }

  // Create nanogui gui
  nanogui::FormHelper *gui = new nanogui::FormHelper(screen);
  nanogui::ref<nanogui::Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Status");

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
      App* app = (App*)glfwGetWindowUserPointer(window);

      sprintf(ret, "%2.02fms", app->scene_render_time()*1000);
      return ret;
    },
    false)->setValue("00.00");

  gui->addVariable<string>("led time",
    [&](string value) { value; },
    [&]() -> string {
      char ret[256];
      App* app = (App*)glfwGetWindowUserPointer(window);
      sprintf(ret, "%2.02fms", app->led_render_time()*1000);
      return ret;
    },
    false)->setValue("00.00");

  gui->addVariable<string>("Shader",
    [&](string value) { value; },
    [&]() -> string {
      App* app = (App*)glfwGetWindowUserPointer(window);

      return app->get_pattern().name.c_str();
    },
    false)->setValue("                 ");

  nanogui::ImageView *imageWidget = new nanogui::ImageView(nanoguiWindow, application.get_pattern_texture().id);
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
            App* app = (App*)glfwGetWindowUserPointer(window);

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

            app->process_mouse_movement(xoffset, yoffset);
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
            App* app = (App*)glfwGetWindowUserPointer(window);

            app->process_mouse_scroll(y);
          }
     }
  );

  glfwSetFramebufferSizeCallback(window,
      [](GLFWwindow *window, int width, int height) {
        App* app = (App*)glfwGetWindowUserPointer(window);

        app->set_screen_size(width, height);
        screen->resizeCallbackEvent(width, height);
      }
  );


  glErr = glGetError();
  while (glErr != GL_NO_ERROR)
  {
      ALOGV("Preloop %04x\n", glErr);
      glErr = glGetError();
  }


  std::chrono::time_point<std::chrono::high_resolution_clock> last_pattern_change = std::chrono::high_resolution_clock::now();
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = sig_int_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  application.set_screen_size(width, height);

  while(!glfwWindowShouldClose(window)) {
    global_timer.start();

    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    application.render_leds();
    application.render_scene();
    application.move_perspective_to_camera();

    imageWidget->bindImage(application.get_pattern().get_texture().id);
    gui->refresh();
    screen->performLayout();
    screen->drawContents();
    screen->drawWidgets();

    if(keys[NEXT_PATTERN]) {
      keys[NEXT_PATTERN] = false;

      application.set_random_pattern();
    } else if (application.get_pattern().get_time_elapsed() > 10*60 ) {
      application.set_random_pattern();
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
  
  running = false;
  webserver.join();

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}


