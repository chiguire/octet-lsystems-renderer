////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012, 2013
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// animation example: Drawing an jointed figure with animation
//
// Level: 2
//
// Demonstrates:
//   Collada meshes
//   Collada nodes
//   Collada animation
//
// note this app is not in the octet namespace as it is not part of octet
// and so we need to use  on several classes.

#include "lsystemsobjs.h"

namespace octet {
  class lsystems : public app {
    typedef mat4t mat4t;
    typedef vec4 vec4;
    typedef animation animation;
    typedef scene_node scene_node;

    // shaders to draw triangles
    bump_shader object_shader;
    bump_shader skin_shader;

    // helper to rotate camera about scene
    mouse_ball ball;

    // helper for drawing text
    text_overlay overlay;

    // helper for picking objects on the screen
    object_picker picker;

    vec4 color;
    color_shader cshader;
    LSystemsModel model;
    Tree2DRenderer model_renderer;

    mat4t cameraToWorld;

  public:
    // this is called when we construct the class
    lsystems(int argc, char **argv)
    : app(argc, argv)
    , ball()
    , color(1.0f, 0.0f, 0.0f, 1.0f)
    , cshader()
    , model()
    , model_renderer(&cshader, color)
    , cameraToWorld()
    {
    }

    // this is called once OpenGL is initialized
    void app_init() {
      // set up the shaders
      object_shader.init(false);
      skin_shader.init(true);

      const char *filename = 0;

      int selector = 0;
      switch (selector) {
        case 0: filename = "assets/lsystems1.xml"; break;
        case 1: filename = "assets/lsystems1.xml"; break;
        case 2: filename = "assets/lsystems1.xml"; break;
        case 3: filename = "assets/lsystems1.xml"; break;
      }

      model.readConfigurationFile(filename);
      model_renderer.setModel(&model);
      model_renderer.color = color;

      cameraToWorld.loadIdentity();
      cameraToWorld.translate(0, 0, -100.0f);

      /*
      string s0(*model.getProduction(0));
      string s1(*model.getProduction(1));
      string s2(*model.getProduction(2));
      string s3(*model.getProduction(3));
      string s4(*model.getProduction(4));
      string s5(*model.getProduction(5));

      printf("Tree iteration 0: %s\n", s0.c_str());
      printf("Tree iteration 1: %s\n", s1.c_str());
      printf("Tree iteration 2: %s\n", s2.c_str());
      printf("Tree iteration 3: %s\n", s3.c_str());
      printf("Tree iteration 4: %s\n", s4.c_str());
      printf("Tree iteration 5: %s\n", s5.c_str());
      */
      //overlay.init();
      picker.init(this);
    }

    // this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx, vy;
      get_viewport_size(vx, vy);
      // set a viewport - includes whole window area
      glViewport(0, 0, vx, vy);

      // clear the background to black
      glClearColor(0.5f, 0.5f, 0.5f, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      if (model.is_loaded()) {
        int vx = 0, vy = 0;
        get_viewport_size(vx, vy);

        model_renderer.render(cameraToWorld, 5);

      }
    }
  };
}
