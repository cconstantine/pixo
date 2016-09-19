// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

// Include GLEW
// #include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;


const int canvasSize = 400;

#include <shader.hpp>
#include <camera.hpp>
#include <model.hpp>
#include <led_cluster.hpp>
#include <renderer.hpp>

int main( int argc, char** argv )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 800, 800, "Dome Lights", NULL, NULL);
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
  // glewExperimental = GL_TRUE; // Needed for core profile
  // if (glewInit() != GLEW_OK) {
  //   fprintf(stderr, "Failed to initialize GLEW\n");
  //   getchar();
  //   glfwTerminate();
  //   return -1;
  // }

  // Setup some OpenGL options
  glEnable(GL_DEPTH_TEST);

  std::vector<uint8_t> frameBuffer;
  int frameBytes =1000*10 * 3;
  frameBuffer.resize(frameBytes);

  //FrameBufferRender fb_screen(3, domeLeds.balls.numInstances());
  FrameBufferRender fb_screen(1000, 10, &frameBuffer[0]);
  ScreenRender screen_renderer(window);
  Scene scene(&screen_renderer, &fb_screen);
  
  fprintf(stderr, "argc: %d\n", argc);
  vector<Shader> patterns;
  for(int i = 1;i < argc;i++) {
    patterns.push_back(Shader("../shaders/pattern.frag", argv[i]));
  }

  Shader pattern = patterns[rand() % patterns.size()];


  PatternRender pattern_render(canvasSize, canvasSize);
  Texture texture = pattern_render.getTexture();

  LedCluster domeLeds(texture);
  screen_renderer.models.push_back(&domeLeds.balls);
  fb_screen.models.push_back(&domeLeds.plane);
  //screen_renderer.models.push_back(&domeLeds.plane);


  Texture fb_texture = fb_screen.getTexture();

  // Load models
  Model screen("../models/screen.obj", texture);
  screen.addInstance(glm::vec3(), glm::vec2(1.0, 1.0), glm::vec3());
  screen_renderer.models.push_back(&screen);

  //Model panel("../models/panel.obj", fb_texture);
  //panel.addInstance(glm::vec3(), glm::vec2(0.0, 0.0), glm::vec3());
  //screen_renderer.models.push_back(&panel);



  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Render the pattern
    pattern_render.render(pattern);

    // Render the scene
    scene.render();

    domeLeds.setGamma(scene.getGamma());
    bool next = scene.nextPattern();
    if (next) {
      pattern = patterns[rand() % patterns.size()];
    }
    domeLeds.update(frameBuffer);

		// Swap buffers
		glfwSwapBuffers(window);
	}

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


