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

std::string Pixlib::Shader::ShaderPreamble = "#version 330\n";


#include <pixpq/pixpq.hpp>

using namespace glm;
using namespace std;

class PixoListener
 : public pixpq::listener<pixpq::sculpture::settings>,
   public pixpq::listener<pixpq::sculpture::pattern>,
   public pixpq::listener<pixpq::tracking::location> {
public:
  PixoListener(Pixlib::App *app) : app(app) {}

  virtual void update(const pixpq::sculpture::settings& s) {
    app->set_pattern(s.active_pattern);
    app->brightness = s.brightness;
    app->gamma = s.gamma;
  }

  virtual void update(const pixpq::tracking::location& loc) {
    app->set_target_location(glm::vec3(loc.x, loc.y, loc.z));
  }

  virtual void update(const pixpq::sculpture::pattern& pattern) {
    if (pattern.enabled) {
      app->register_pattern(make_shared<Pixlib::Pattern>(pattern.name, pattern.glsl_code, pattern.overscan));
    } else {
      app->disable_pattern(pattern.name);
    }
  }

 private:
  Pixlib::App *app;
};


class PixoSettingsManager : public pixpq::manager {
public:
  PixoSettingsManager(const std::string& sculpture_name, Pixlib::App* app) : pixpq::manager(""), app(app), sculpture_name(sculpture_name) { }

  Pixlib::App *app;
  std::string sculpture_name;
};

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
    fprintf(stderr, "Usage: %s [--fullscreen] sculpture_name\n", argv[0]);
    exit(1);
  }

  if (strncmp("--fullscreen", argv[arg_i], sizeof("--fullscreen")) == 0) {
    fullscreen = true;
    arg_i += 1;
  }

  const std::string sculpture_name(argv[arg_i]);
  ALOGV("loading: %s\n", sculpture_name.c_str());
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
  Pixlib::App application = Pixlib::App();

  PixoSettingsManager manager(sculpture_name, &application);
  manager.ensure_schema();

  pixpq::sculpture::settings settings = manager.get<pixpq::sculpture::settings>(pixpq::sculpture::settings::by_id(sculpture_name));

  std::vector<pixpq::sculpture::fadecandy> fadecandies = manager.get_all<pixpq::sculpture::fadecandy>(pixpq::sculpture::fadecandy::get(settings));

  for(pixpq::sculpture::fadecandy fc : fadecandies) {
    std::list<glm::vec3> leds;
    for(pixpq::sculpture::led led : manager.get_all<pixpq::sculpture::led>(pixpq::sculpture::led::find(fc))) {
      leds.push_back(glm::vec3(led.x, led.y, led.z));
    }
    application.add_fadecandy(fc.address, leds);
  }
  
  std::shared_ptr<PixoListener> updates_listener = std::make_shared<PixoListener>(&application);
  manager.set_listener<pixpq::sculpture::settings>(updates_listener);
  manager.set_listener<pixpq::sculpture::pattern>(updates_listener);
  manager.set_listener<pixpq::tracking::location>(updates_listener);

  for(pixpq::sculpture::pattern pattern : manager.get_all<pixpq::sculpture::pattern>(pixpq::sculpture::pattern::is_enabled())) {
    application.register_pattern(make_shared<Pixlib::Pattern>(pattern.name, pattern.glsl_code, pattern.overscan));
  }

  try {
    pixpq::tracking::location loc = manager.get<pixpq::tracking::location>(pixpq::tracking::location::by_name(sculpture_name));
    updates_listener->update(loc);
  } catch( const pqxx::unexpected_rows& e) {
    pixpq::tracking::location loc(sculpture_name, application.camera.Position.x, application.camera.Position.y, application.camera.Position.z);
    manager.get<pixpq::tracking::location>(pixpq::tracking::location::upsert(loc));
  }

  updates_listener->update(settings);


  glfwSetWindowUserPointer(window, &manager);
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
      PixoSettingsManager* manager = (PixoSettingsManager*)glfwGetWindowUserPointer(window);

      sprintf(ret, "%2.02fms", manager->app->scene_render_time()*1000);
      return ret;
    },
    false)->setValue("00.00");

  gui->addVariable<string>("led time",
    [&](string value) { value; },
    [&]() -> string {
      char ret[256];
      PixoSettingsManager* manager = (PixoSettingsManager*)glfwGetWindowUserPointer(window);
      sprintf(ret, "%2.02fms", manager->app->led_render_time()*1000);
      return ret;
    },
    false)->setValue("00.00");

  gui->addVariable<string>("Shader",
    [&](string value) { value; },
    [&]() -> string {
      PixoSettingsManager* manager = (PixoSettingsManager*)glfwGetWindowUserPointer(window);

      return manager->app->get_pattern()->name.c_str();
    },
    false)->setValue("                 ");

  gui->addVariable<bool>("Overscan",
    [&](bool value) {
      PixoSettingsManager* manager = (PixoSettingsManager*)glfwGetWindowUserPointer(window);
      std::string pattern_name     = manager->app->get_pattern()->name;
      pixpq::sculpture::pattern p  = manager->get<pixpq::sculpture::pattern>(pixpq::sculpture::pattern::by_id(pattern_name));
      p.overscan = value;
      manager->get<pixpq::sculpture::pattern>(pixpq::sculpture::pattern::upsert(p));
    },
    [&]() -> bool {
      PixoSettingsManager* manager = (PixoSettingsManager*)glfwGetWindowUserPointer(window);
      std::string pattern_name = manager->app->get_pattern()->name;
      pixpq::sculpture::pattern p = manager->get<pixpq::sculpture::pattern>(pixpq::sculpture::pattern::by_id(pattern_name));
      return p.overscan;
    },
    true);
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
      PixoSettingsManager* manager = (PixoSettingsManager*)glfwGetWindowUserPointer(window);
      pixpq::sculpture::settings settings = manager->get<pixpq::sculpture::settings>(pixpq::sculpture::settings::by_id(manager->sculpture_name));

      settings.brightness = value;
      manager->get<pixpq::sculpture::settings>(pixpq::sculpture::settings::update(settings));
  });

  gui->addWidget("Brightness", brightness_slider);

  nanogui::Slider *gamma_slider = new nanogui::Slider(nanoguiWindow);
  gamma_slider->setValue(application.gamma);
  gamma_slider->setRange(std::pair<float, float>(1.0f, 3.0f));
  gamma_slider->setCallback([](float value) {
      PixoSettingsManager* manager = (PixoSettingsManager*)glfwGetWindowUserPointer(window);
      pixpq::sculpture::settings settings = manager->get<pixpq::sculpture::settings>(pixpq::sculpture::settings::by_id(manager->sculpture_name));

      settings.gamma = value;
      manager->get<pixpq::sculpture::settings>(pixpq::sculpture::settings::update(settings));
  });

  gui->addWidget("Gamma", gamma_slider);

  // nanogui::ComboBox *tracker = new nanogui::ComboBox(nanoguiWindow);

  gui->addVariable<bool>("Track Camera",
    [&](bool value) {
      PixoSettingsManager* manager = (PixoSettingsManager*)glfwGetWindowUserPointer(window);
      manager->app->tracking_camera = value;
    },
    [&]() -> bool {
      PixoSettingsManager* manager = (PixoSettingsManager*)glfwGetWindowUserPointer(window);

      return manager->app->tracking_camera;
    },
    true);

  glfwSetCursorPosCallback(window,
          [](GLFWwindow *window, double x, double y) {
          if (!screen->cursorPosCallbackEvent(x, y)) {
            int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
            PixoSettingsManager* manager = (PixoSettingsManager*)glfwGetWindowUserPointer(window);
  
            GLfloat xoffset = x - lastX;
            GLfloat yoffset = y - lastY; 

            lastX = x;
            lastY = y;

            if (state == GLFW_PRESS) {
              manager->app->process_mouse_movement(xoffset, yoffset);
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

            PixoSettingsManager* manager = (PixoSettingsManager*)glfwGetWindowUserPointer(window);
            pixpq::sculpture::settings settings = manager->get<pixpq::sculpture::settings>(pixpq::sculpture::settings::by_id(manager->sculpture_name));

            if(key == GLFW_KEY_ESCAPE) {
              glfwSetWindowShouldClose(window, GL_TRUE);
              return;
            }

            if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
              settings.brightness = (key - GLFW_KEY_0 ) / 9.0f;
            }

            if( key == GLFW_KEY_ENTER) {
              settings.active_pattern = manager->app->random_pattern();
            }

            if( key ==  GLFW_KEY_PERIOD) {
              settings.active_pattern = manager->app->next_pattern();
            }

            if( key ==  GLFW_KEY_COMMA) {
              settings.active_pattern = manager->app->prev_pattern();
            }

            if (key == GLFW_KEY_P) {
              manager->app->paused = !manager->app->paused;
            }

            if (key == GLFW_KEY_O) {
              std::string pattern_name     = manager->app->get_pattern()->name;
              pixpq::sculpture::pattern p  = manager->get<pixpq::sculpture::pattern>(pixpq::sculpture::pattern::by_id(pattern_name));
              p.overscan = !p.overscan;
              manager->get<pixpq::sculpture::pattern>(pixpq::sculpture::pattern::upsert(p));
            }
            manager->get<pixpq::sculpture::settings>(pixpq::sculpture::settings::update(settings));
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
            PixoSettingsManager* manager = (PixoSettingsManager*)glfwGetWindowUserPointer(window);

            manager->app->process_mouse_scroll(y);
          }
     }
  );

  glfwSetFramebufferSizeCallback(window,
      [](GLFWwindow *window, int width, int height) {
        PixoSettingsManager* manager = (PixoSettingsManager*)glfwGetWindowUserPointer(window);

        manager->app->set_screen_size(width, height);
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

    manager.process_updates();
    if (application.tracking_camera) {
      pixpq::tracking::location loc(
        sculpture_name,
        application.camera.Position.x,
        application.camera.Position.y,
        application.camera.Position.z);
      manager.get<pixpq::tracking::location>(pixpq::tracking::location::upsert(loc));
    }

    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    application.render_leds();
    application.render_scene();

    imageWidget->bindImage(application.get_pattern()->get_texture().id);
    gui->refresh();
    screen->performLayout();
    screen->drawContents();
    screen->drawWidgets();

    brightness_slider->setValue(application.brightness);
    gamma_slider->setValue(application.gamma);

    if (application.get_pattern()->get_time_elapsed() > 10*60 ) {
      pixpq::sculpture::settings settings = manager.get<pixpq::sculpture::settings>(pixpq::sculpture::settings::by_id(sculpture_name));
      settings.active_pattern = application.random_pattern();
      manager.get<pixpq::sculpture::settings>(pixpq::sculpture::settings::update(settings));
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

  // storage.sculpture.camera_perspective.yaw = application.camera.Yaw;
  // storage.sculpture.camera_perspective.pitch = application.camera.Pitch;
  // storage.sculpture.camera_perspective.zoom = application.camera.Zoom;
  // storage.sculpture.camera_perspective.scope = application.camera.scope;

  // storage.sculpture.projection_perspective.yaw = application.viewed_from.Yaw;
  // storage.sculpture.projection_perspective.pitch = application.viewed_from.Pitch;
  // storage.sculpture.projection_perspective.zoom = application.viewed_from.Zoom;
  // storage.sculpture.projection_perspective.scope = application.viewed_from.scope;

  // storage.sculpture.active_pattern_name = application.get_pattern().name;
  // storage.sculpture.brightness = application.brightness;
  // storage.sculpture.gamma = application.gamma;
  // storage.sculpture.rotation = application.rotation;
  // storage.save_app_state();

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}


