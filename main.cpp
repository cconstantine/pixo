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
using namespace glm;



#include <shader.hpp>
#include <camera.hpp>
#include <model.hpp>
#include <led_cluster.hpp>
#include <renderer.hpp>
#include <scene.hpp>
#include <nanogui/nanogui.h>

using namespace nanogui;
Screen *screen = nullptr;
Scene *scene = nullptr;
Shader *pattern = nullptr;

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
    patterns.push_back(Shader("../shaders/pattern.frag", argv[i]));
  }

  pattern = &patterns[rand() % patterns.size()];
  const int leds_per_side = atoi(argv[1]);
  const int canvasSize = sqrt(leds_per_side*leds_per_side*leds_per_side)*2;

  int rows = pow(2, ceil(log(sqrt(pow(leds_per_side,3)))/log(2)));
  int cols = rows;
  fprintf(stderr, "Leds per side:     %3d  (total: %.0f )\n", leds_per_side, pow(leds_per_side, 3));
  fprintf(stderr, "Led canvas: %4d x %4d (total: %d)\n", cols, rows, rows*cols);
  fprintf(stderr, "pattern canvas: %d x %d\n", canvasSize, canvasSize);

  Texture texture = Texture(canvasSize, canvasSize);
  Texture fb_texture = Texture(cols, rows);

  PatternRender pattern_render(texture);
  FrameBufferRender fb_screen(fb_texture);
  ScreenRender screen_renderer(window);

  scene = new Scene(&screen_renderer, &fb_screen);


  LedCluster domeLeds(leds_per_side, texture, fb_texture);
  fb_screen.models.push_back(&domeLeds.leds_for_calc);
  screen_renderer.models.push_back(&domeLeds.leds_for_display);
  
  // Load models
  //Model display("../models/screen.obj", texture);
  //display.addInstance(glm::vec3(), glm::vec2(1.0, 1.0), glm::vec3());
  //screen_renderer.models.push_back(&display);

  // Model panel("../models/panel.obj", fb_texture);
  // panel.addInstance(glm::vec3(), glm::vec2(0.0, 0.0), glm::vec3());
  // screen_renderer.models.push_back(&panel);



  // Create a nanogui screen and pass the glfw pointer to initialize
  screen = new Screen();
  screen->initialize(window, true);

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

  gui->addVariable<string>("Shader",
    [&](string value) { value; },
    [&]() -> string {
      return &pattern->fragmentPath.c_str()[12];
    },
    false)->setValue("                 ");

  ImageView *imageWidget = new ImageView(nanoguiWindow, texture.id);
  imageWidget->setFixedSize(Eigen::Vector2i(160, 160));
  imageWidget->setFixedScale(true);
  gui->addWidget("", imageWidget);
  screen->setVisible(true);
  screen->performLayout();

  imageWidget->fit();



  glfwSetCursorPosCallback(window,
          [](GLFWwindow *window, double x, double y) {
          if (!screen->cursorPosCallbackEvent(x, y)) {
            scene->mouse_callback(window, x, y);
          }
      }
  );

  glfwSetMouseButtonCallback(window,
      [](GLFWwindow *window, int button, int action, int modifiers) {
          if (!screen->mouseButtonCallbackEvent(button, action, modifiers)) {
            scene->mouse_button_callback(window, button, action, modifiers);
          }
      }
  );

  glfwSetKeyCallback(window,
      [](GLFWwindow *window, int key, int scancode, int action, int mods) {
          if (!screen->keyCallbackEvent(key, scancode, action, mods)) {
            scene->key_callback(window, key, scancode, action, mods);
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
            scene->mouse_scroll_callback(window, x, y);

          }
     }
  );

  glfwSetFramebufferSizeCallback(window,
      [](GLFWwindow *window, int width, int height) {
          screen->resizeCallbackEvent(width, height);
      }
  );
  while(!glfwWindowShouldClose(window)) {

    glfwPollEvents();

    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    screen->performLayout();


    // Render the pattern
    pattern_render.render(*pattern);


    // Render the scene
    scene->render();


    //domeLeds.setGamma(scene->getGamma());
    bool next = scene->nextPattern();
    if (next) {
      pattern = &patterns[rand() % patterns.size()];
    }
    //domeLeds.update(frameBuffer);

    gui->refresh();
    // Draw nanogui
    screen->drawContents();
    screen->drawWidgets();

		glfwSwapBuffers(window);

	}

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


