// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <thread>
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

#include <storage.hpp>

std::string Pixlib::Shader::ShaderPreamble = "#version 330\n";

#include <grpcpp/grpcpp.h>
#include "pixrpc.grpc.pb.h"

using namespace glm;
using namespace std;


class TrackingClient {
 public:
  TrackingClient(std::shared_ptr<grpc::Channel> channel)
      : stub_(pixrpc::Tracking::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  bool get_locations(const Pixlib::TrackingService& tracking_service, Pixlib::App* app) {
    pixrpc::LocationStreamArgs args;
    pixrpc::Location location;
    grpc::ClientContext context;

    std::unique_ptr<grpc::ClientReader<pixrpc::Location> > reader(stub_->location_stream(&context, args));

    while (reader->Read(&location)) {
      app->set_target_location(tracking_service.tracking_offset + glm::vec3(location.x(), location.y(), location.z()));
    }
    grpc::Status status = reader->Finish();
    return status.ok();
  }

 private:
  std::unique_ptr<pixrpc::Tracking::Stub> stub_;
};

void thread_function(Pixlib::TrackingService tracking_service, Pixlib::App *app) {
  TrackingClient location_client(grpc::CreateChannel(
      tracking_service.address, grpc::InsecureChannelCredentials()));
  bool last_success = true;
  while (true) {
    bool this_success = location_client.get_locations(tracking_service, app);
    if (last_success && !this_success) {
      std::cout << "location_stream rpc failed." << std::endl;
      last_success = this_success;
    }
    std::this_thread::sleep_for(std::chrono::duration<float>(0.1));
  }
}

nanogui::Screen *screen = nullptr;

Pixlib::Timer global_timer = Pixlib::Timer(120);

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

void GLFW_error(int error, const char* description)
{
    fprintf(stderr, "%d: %s\n", error, description);
}


int main( int argc, char** argv )
{  
  int arg_i = 1;
  bool fullscreen = false;
  if(argc < 2) {
    fprintf(stderr, "Usage: %s [--fullscreen] db_filename\n", argv[0]);
    exit(1);
  }

  if (strncmp("--fullscreen", argv[arg_i], sizeof("--fullscreen")) == 0) {
    fullscreen = true;
    arg_i += 1;
  }

  const std::string db_filename(argv[arg_i]);
  ALOGV("loading: %s\n", db_filename.c_str());
    glfwSetErrorCallback(GLFW_error);

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

  GLFWmonitor* monitor = NULL;
  int width = 800, height = 800;

  if (fullscreen) {
    monitor = glfwGetPrimaryMonitor();

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = mode->width;
    height = mode->height;
  }

  window = glfwCreateWindow(width, height, "Pixo", monitor, NULL);
  if( window == NULL ){
      fprintf( stderr, "Failed to open GLFW window (%d, %d).\n", width, height );
      glfwTerminate();
      return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);

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
  Storage storage(db_filename);

  Pixlib::App application = Pixlib::App(storage.sculpture, storage.patterns());
  std::vector<Pixlib::TrackingService> tracking_services = storage.tracking_services();
  std::vector<std::thread> threads;
  for(Pixlib::TrackingService service : tracking_services) {
    threads.push_back(std::thread(&thread_function, service, &application));
  }

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
      Pixlib::App* app = (Pixlib::App*)glfwGetWindowUserPointer(window);

      sprintf(ret, "%2.02fms", app->scene_render_time()*1000);
      return ret;
    },
    false)->setValue("00.00");

  gui->addVariable<string>("led time",
    [&](string value) { value; },
    [&]() -> string {
      char ret[256];
      Pixlib::App* app = (Pixlib::App*)glfwGetWindowUserPointer(window);
      sprintf(ret, "%2.02fms", app->led_render_time()*1000);
      return ret;
    },
    false)->setValue("00.00");

  gui->addVariable<string>("Shader",
    [&](string value) { value; },
    [&]() -> string {
      Pixlib::App* app = (Pixlib::App*)glfwGetWindowUserPointer(window);

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

  nanogui::Slider *brightness_slider = new nanogui::Slider(nanoguiWindow);
  brightness_slider->setValue(application.brightness);
  brightness_slider->setRange(std::pair<float, float>(0.0f, 1.0f));
  brightness_slider->setCallback([](float value) {
      Pixlib::App* app = (Pixlib::App*)glfwGetWindowUserPointer(window);
      app->brightness = value;
  });

  gui->addWidget("Brightness", brightness_slider);


  gui->addVariable<float>("Rotation",
    [&](float value) {
      Pixlib::App* app = (Pixlib::App*)glfwGetWindowUserPointer(window);
      app->rotation = value;
    },
    [&]() -> float {
      Pixlib::App* app = (Pixlib::App*)glfwGetWindowUserPointer(window);

      return app->rotation;
    },
    true);

  glfwSetCursorPosCallback(window,
          [](GLFWwindow *window, double x, double y) {
          if (!screen->cursorPosCallbackEvent(x, y)) {
            int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
            Pixlib::App* app = (Pixlib::App*)glfwGetWindowUserPointer(window);
  
            GLfloat xoffset = x - lastX;
            GLfloat yoffset = y - lastY; 

            lastX = x;
            lastY = y;

            if (state == GLFW_PRESS) {
              app->process_mouse_movement(xoffset, yoffset);
            }
          }
      }
  );

  glfwSetMouseButtonCallback(window,
      [](GLFWwindow *window, int button, int action, int modifiers) {
          if (!screen->mouseButtonCallbackEvent(button, action, modifiers)) {

            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
              glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
              glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
          }
      }
  );

  glfwSetKeyCallback(window,
      [](GLFWwindow *window, int key, int scancode, int action, int mods) {
          if (!screen->keyCallbackEvent(key, scancode, action, mods)) {
            if(action == GLFW_RELEASE) {
              return;
            }

            Pixlib::App* app = (Pixlib::App*)glfwGetWindowUserPointer(window);

            if(key == GLFW_KEY_ESCAPE) {
              glfwSetWindowShouldClose(window, GL_TRUE);
            }

            if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
              app->brightness = (key - GLFW_KEY_0 ) / 9.0f;
            }

            if( key == GLFW_KEY_ENTER) {
              app->set_random_pattern();
            }

            if( key ==  GLFW_KEY_PERIOD) {
              app->next_pattern();
            }

            if( key ==  GLFW_KEY_COMMA) {
              app->prev_pattern();
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
            Pixlib::App* app = (Pixlib::App*)glfwGetWindowUserPointer(window);

            app->process_mouse_scroll(y);
          }
     }
  );

  glfwSetFramebufferSizeCallback(window,
      [](GLFWwindow *window, int width, int height) {
        Pixlib::App* app = (Pixlib::App*)glfwGetWindowUserPointer(window);

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
  sigaction(SIGTERM, &sigIntHandler, NULL);

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

    brightness_slider->setValue(application.brightness);

    if (application.get_pattern().get_time_elapsed() > 10*60 ) {
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

  storage.sculpture.camera_perspective.yaw = application.camera.Yaw;
  storage.sculpture.camera_perspective.pitch = application.camera.Pitch;
  storage.sculpture.camera_perspective.zoom = application.camera.Zoom;
  storage.sculpture.camera_perspective.scope = application.camera.scope;

  storage.sculpture.projection_perspective.yaw = application.viewed_from.Yaw;
  storage.sculpture.projection_perspective.pitch = application.viewed_from.Pitch;
  storage.sculpture.projection_perspective.zoom = application.viewed_from.Zoom;
  storage.sculpture.projection_perspective.scope = application.viewed_from.scope;

  storage.sculpture.active_pattern_name = application.get_pattern().name;
  storage.sculpture.brightness = application.brightness;
  storage.sculpture.rotation = application.rotation;
  storage.save_app_state();

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}


