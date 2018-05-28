// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

#include <iostream>

#include <GL/glew.h>
#include <gtk/gtk.h>

#include <pixlib.hpp>

#define NELEM(array)  (sizeof(array) / sizeof(*(array)))


std::string Pixlib::Shader::ShaderPreamble = "#version 330\n";

std::shared_ptr<Pixlib::App> pixlib_app;

// Hold init data for GTK signals:
struct signal {
  const gchar *signal;
  GCallback  handler;
  GdkEventMask   mask;
};

// Get number of elements in an array:
#define NELEM(array)  (sizeof(array) / sizeof(*(array)))

// Loop over an array of given size:
#define FOREACH_NELEM(array, nelem, iter) \
  for (__typeof__(*(array)) *iter = (array); \
    iter < (array) + (nelem); \
    iter++)

// Loop over an array of known size:
#define FOREACH(array, iter) \
  FOREACH_NELEM(array, NELEM(array), iter)

static void
connect_signals (GtkWidget *widget, struct signal *signals, size_t members)
{
  FOREACH_NELEM (signals, members, s) {
    gtk_widget_add_events(widget, s->mask);
    g_signal_connect(widget, s->signal, s->handler, NULL);
  }
}

static void
activate(GtkApplication *app,
    gpointer user_data) {
    GtkWidget *window;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Hello GNOME");
    gtk_widget_show_all(window);
}

void do_offscreens(GtkGLArea *glarea) {
  gtk_gl_area_make_current(glarea);

  pixlib_app->tick();
  pixlib_app->move_perspective_to_camera();

}

static void
on_realize (GtkGLArea *glarea)
{
  // Make current:
  gtk_gl_area_make_current(glarea);

  // Init GLEW:
  glewExperimental = GL_TRUE;
  glewInit();


  // Print version info:
  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  // Enable depth buffer:
  gtk_gl_area_set_has_depth_buffer(glarea, TRUE);

  // Get frame clock:
  GdkGLContext *glcontext = gtk_gl_area_get_context(glarea);
  GdkWindow *glwindow = gdk_gl_context_get_window(glcontext);
  GdkFrameClock *frame_clock = gdk_window_get_frame_clock(glwindow);

  // Connect update signal:
  g_signal_connect_swapped
    ( frame_clock
    , "before-paint"
    , G_CALLBACK(do_offscreens)
    , glarea
    ) ;
  // Connect update signal:
  g_signal_connect_swapped
    ( frame_clock
    , "update"
    , G_CALLBACK(gtk_gl_area_queue_render)
    , glarea
    ) ;


  // Start updating:
  gdk_frame_clock_begin_updating(frame_clock);

  pixlib_app = std::make_shared<Pixlib::App>(Pixlib::Storage("db.sqlite"));
  pixlib_app->set_random_pattern();
}

static gboolean
on_render (GtkGLArea *glarea, GdkGLContext *context)
{ // Clear canvas:
  glClearColor(0.10f, 0.10f, 0.10f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GLenum glErr = glGetError();
  while (glErr != GL_NO_ERROR)
  {
      ALOGV("glError %04x\n", glErr);
      glErr = glGetError();
  }
  glViewport (0, 0, pixlib_app->camera.width, pixlib_app->camera.height);

  pixlib_app->scene.render(pixlib_app->camera);


  return TRUE;
}

static void
on_resize (GtkGLArea *area, gint width, gint height)
{
  ALOGV("RESIZE: %d x %d\n", width, height);
  //view_set_window(width, height);
  //background_set_window(width, height);
  pixlib_app->set_screen_size(width, height);

}

static void
connect_window_signals (GtkWidget *window)
{
  struct signal signals[] = {
    { "destroy",      G_CALLBACK(gtk_main_quit),  GDK_ALL_EVENTS_MASK     },
  };

  connect_signals(window, signals, NELEM(signals));
}

static void
connect_glarea_signals (GtkWidget *glarea)
{
  struct signal signals[] = {
    { "realize",      G_CALLBACK(on_realize),   GDK_ALL_EVENTS_MASK     },
    { "render",     G_CALLBACK(on_render),    GDK_ALL_EVENTS_MASK     },
    { "resize",     G_CALLBACK(on_resize),    GDK_ALL_EVENTS_MASK     },
    // { "scroll-event",    G_CALLBACK(on_scroll),    GDK_SCROLL_MASK   },
    // { "button-press-event",    G_CALLBACK(on_button_press),  GDK_BUTTON_PRESS_MASK },
    // { "button-release-event",  G_CALLBACK(on_button_release),  GDK_BUTTON_RELEASE_MASK },
    // { "motion-notify-event", G_CALLBACK(on_motion_notify), GDK_BUTTON1_MOTION_MASK },
  };

  connect_signals(glarea, signals, NELEM(signals));
}


using namespace Pixlib;
int main( int argc, char** argv )
{  
  // if(argc < 2) {
  //   fprintf(stderr, "Usage: %s db_filename\n", argv[0]);
  //   exit(1);
  // }

  // const std::string db_filename(argv[1]);
  // ALOGV("loading: %s\n", db_filename.c_str());

  // unsigned int arg_i = 2;

 // Initialize GTK:
  if (!gtk_init_check(&argc, &argv)) {
    ALOGV("Could not initialize GTK");
    return false;
  }


  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  GtkWidget *glarea = gtk_gl_area_new();
  gtk_container_add(GTK_CONTAINER(window), glarea);

  connect_window_signals(window);
  connect_glarea_signals(glarea);

  gtk_widget_show_all(window);

  // Enter GTK event loop:
  gtk_main();

  return 0;

  
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_DEPTH_BITS, 24 );

  // // Open a window and create its OpenGL context
  // window = glfwCreateWindow( 800, 800, "Pixo", NULL, NULL);
  // if( window == NULL ){
  //     fprintf( stderr, "Failed to open GLFW window.\n" );
  //     getchar();
  //     glfwTerminate();
  //     return -1;
  // }
  // glfwMakeContextCurrent(window);


  // // Ensure we can capture the escape key being pressed below
  // glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // // Initialize GLEW
  // glewExperimental = GL_TRUE; // Needed for core profile
  // if (glewInit() != GLEW_OK) {
  //   fprintf(stderr, "Failed to initialize GLEW\n");
  //   getchar();
  //   glfwTerminate();
  //   return -1;
  // }
  // // Setup some OpenGL options
  // // Enable depth test
  // glEnable(GL_DEPTH_TEST);

  // App application = App(Storage(db_filename));

  // glfwSetWindowUserPointer(window, &application);
  // // Create a nanogui screen and pass the glfw pointer to initialize
  // screen = new nanogui::Screen();
  // screen->initialize(window, true);
  // GLenum glErr = glGetError();
  // while (glErr != GL_NO_ERROR)
  // {
  //     glErr = glGetError();
  // }

  // // Create nanogui gui
  // nanogui::FormHelper *gui = new nanogui::FormHelper(screen);
  // nanogui::ref<nanogui::Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Status");

  // gui->addVariable<string>("fps",
  //   [&](string value) { value; },
  //   [&]() -> string {
  //     char ret[256];
  //     float fps = application.scene_fps();
  //     sprintf(ret, "%2.02f", fps);
  //     return ret;
  //   },
  //   false)->setValue("00.00");

  // gui->addVariable<string>("render time",
  //   [&](string value) { value; },
  //   [&]() -> string {
  //     char ret[256];
  //     sprintf(ret, "%2.02fms", global_timer.duration()*1000);
  //     return ret;
  //   },
  //   false)->setValue("00.00");

  // gui->addVariable<string>("scene time",
  //   [&](string value) { value; },
  //   [&]() -> string {

  //     char ret[256];
  //     App* app = (App*)glfwGetWindowUserPointer(window);

  //     sprintf(ret, "%2.02fms", app->scene_render_time()*1000);
  //     return ret;
  //   },
  //   false)->setValue("00.00");

  // gui->addVariable<string>("led time",
  //   [&](string value) { value; },
  //   [&]() -> string {
  //     char ret[256];
  //     App* app = (App*)glfwGetWindowUserPointer(window);
  //     sprintf(ret, "%2.02fms", app->led_render_time()*1000);
  //     return ret;
  //   },
  //   false)->setValue("00.00");

  // gui->addVariable<string>("Shader",
  //   [&](string value) { value; },
  //   [&]() -> string {
  //     App* app = (App*)glfwGetWindowUserPointer(window);

  //     return app->get_pattern().c_str();
  //   },
  //   false)->setValue("                 ");

  // nanogui::ImageView *imageWidget = new nanogui::ImageView(nanoguiWindow, application.get_pattern_texture().id);
  // imageWidget->setFixedSize(Eigen::Vector2i(160, 160));
  // imageWidget->setFixedScale(true);
  // gui->addWidget("", imageWidget);
  // screen->setVisible(true);
  // screen->performLayout();

  // imageWidget->fit();

  // glfwSetCursorPosCallback(window,
  //         [](GLFWwindow *window, double x, double y) {
  //         if (!screen->cursorPosCallbackEvent(x, y)) {
  //           int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
  //           App* app = (App*)glfwGetWindowUserPointer(window);

  //           if (state != GLFW_PRESS) {
  //             firstMouse = true;
  //           }
  //           if(firstMouse)
  //           {
  //             lastX = x;
  //             lastY = y;
  //             firstMouse = false;
  //           }

  //           GLfloat xoffset = x - lastX;
  //           GLfloat yoffset = y - lastY; 

  //           lastX = x;
  //           lastY = y;

  //           app->process_mouse_movement(xoffset, yoffset);
  //         }
  //     }
  // );

  // glfwSetMouseButtonCallback(window,
  //     [](GLFWwindow *window, int button, int action, int modifiers) {
  //         if (!screen->mouseButtonCallbackEvent(button, action, modifiers)) {

  //           if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
  //             glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  //           } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
  //             glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  //           }
  //         }
  //     }
  // );

  // glfwSetKeyCallback(window,
  //     [](GLFWwindow *window, int key, int scancode, int action, int mods) {
  //         if (!screen->keyCallbackEvent(key, scancode, action, mods)) {
  //           if (action == GLFW_REPEAT) {
  //             return;
  //           }
  //           if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  //             glfwSetWindowShouldClose(window, GL_TRUE);

  //           // Camera controls
  //           if(key == GLFW_KEY_W) {
  //             keys[FORWARD] = (action == GLFW_PRESS);  
  //           }
  //           if(key == GLFW_KEY_S) {
  //             keys[BACKWARD] = (action == GLFW_PRESS);  
  //           }
  //           if(key == GLFW_KEY_A) {
  //             keys[LEFT] = (action == GLFW_PRESS); 
  //           }
  //           if(key == GLFW_KEY_D) {
  //             keys[RIGHT] = (action == GLFW_PRESS); 
  //           }
  //           if(key == GLFW_KEY_LEFT_SHIFT) {
  //             keys[MATCH_VIEW] = (action == GLFW_PRESS); 
  //           }
  //           if(key == GLFW_KEY_LEFT_CONTROL) {
  //             keys[TOWARDS_VIEW] = (action == GLFW_PRESS); 
  //           }
  //           if(key == GLFW_KEY_ENTER) {
  //             keys[NEXT_PATTERN] = (action == GLFW_PRESS); 
  //           }
  //           if(key == GLFW_KEY_RIGHT ) {
  //             keys[ORB_RIGHT] = (action == GLFW_PRESS); 
  //           }
  //           if(key == GLFW_KEY_LEFT) {
  //             keys[ORB_LEFT] = (action == GLFW_PRESS); 
  //           }
  //           if(key == GLFW_KEY_UP ) {
  //             keys[ORB_UP] = (action == GLFW_PRESS); 
  //           }
  //           if(key == GLFW_KEY_DOWN) {
  //             keys[ORB_DOWN] = (action == GLFW_PRESS); 
  //           }
  //         }
  //     }
  // );

  // glfwSetCharCallback(window,
  //     [](GLFWwindow *window, unsigned int codepoint) {
  //         screen->charCallbackEvent(codepoint);
  //     }
  // );

  // glfwSetDropCallback(window,
  //     [](GLFWwindow *window, int count, const char **filenames) {
  //         screen->dropCallbackEvent(count, filenames);
  //     }
  // );

  // glfwSetScrollCallback(window,
  //     [](GLFWwindow *window, double x, double y) {
  //         if (!screen->scrollCallbackEvent(x, y)) {
  //           App* app = (App*)glfwGetWindowUserPointer(window);

  //           app->process_mouse_scroll(y);
  //         }
  //    }
  // );

  // glfwSetFramebufferSizeCallback(window,
  //     [](GLFWwindow *window, int width, int height) {
  //       App* app = (App*)glfwGetWindowUserPointer(window);

  //       app->set_screen_size(width, height);
  //       screen->resizeCallbackEvent(width, height);
  //     }
  // );


  // glErr = glGetError();
  // while (glErr != GL_NO_ERROR)
  // {
  //     ALOGV("Preloop %04x\n", glErr);
  //     glErr = glGetError();
  // }


  // std::chrono::time_point<std::chrono::high_resolution_clock> last_pattern_change = std::chrono::high_resolution_clock::now();
  // struct sigaction sigIntHandler;
  // sigIntHandler.sa_handler = sig_int_handler;
  // sigemptyset(&sigIntHandler.sa_mask);
  // sigIntHandler.sa_flags = 0;
  // sigaction(SIGINT, &sigIntHandler, NULL);

  // int width, height;
  // glfwGetFramebufferSize(window, &width, &height);
  // application.set_screen_size(width, height);

  // while(!glfwWindowShouldClose(window)) {
  //   global_timer.start();

  //   glfwPollEvents();
  //   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //   application.tick();
  //   application.move_perspective_to_camera();

  //   gui->refresh();
  //   screen->performLayout();
  //   screen->drawContents();
  //   screen->drawWidgets();

  //   if(keys[NEXT_PATTERN]) {
  //     keys[NEXT_PATTERN] = false;

  //     application.set_random_pattern();
  //   } else if (application.pattern_get_time_elapsed() > 10*60 ) {
  //     application.set_random_pattern();
  //   }

  //   global_timer.end();
    
  //   glfwSwapBuffers(window);

  //   GLenum glErr = glGetError();
  //   while (glErr != GL_NO_ERROR)
  //   {
  //       ALOGV("glError %04x\n", glErr);
  //       glErr = glGetError();
  //   }

  // }
  
  // running = false;
  // webserver.join();

  // // Close OpenGL window and terminate GLFW
  // glfwTerminate();

  // return 0;
}


