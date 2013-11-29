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

    texture_shader tshader;

    LSystemsModel model;
    Tree2DRenderer model_renderer;

    mat4t cameraToWorld;
    mat4t cameraToProjection;

    vec4 camera_position;
    int current_iterations;

    bool just_pressed;
    bool display_help;

    GLuint leafTex;
    GLuint woodTex;
    GLuint helpTex;
  public:
    // this is called when we construct the class
    lsystems(int argc, char **argv)
    : app(argc, argv)
    , model()
    , model_renderer(NULL)
    , cameraToWorld()
    , camera_position(0.0f, 0.0f, 0.0f, 1.0f)
    , just_pressed(false)
    , display_help(true)
    {
    }

    // this is called once OpenGL is initialized
    void app_init() {
      // set up the shaders
      tshader.init();

      const char *filename = 0;

      int selector = 0;
      switch (selector) {
        case 0: filename = "assets/lsystems1.xml"; break;
        case 1: filename = "assets/lsystems2.xml"; break;
        case 2: filename = "assets/lsystems3.xml"; break;
        case 3: filename = "assets/lsystems4.xml"; break;
      }

      current_iterations = 0;
      model_renderer.tshader = &tshader;

      helpTex = resources::get_texture_handle(GL_RGBA, "assets/help.gif");
      leafTex = resources::get_texture_handle(GL_RGBA, "assets/leaf.gif");
      woodTex = resources::get_texture_handle(GL_RGBA, "assets/wood.gif");
      model_renderer.leafTex = leafTex;
      model_renderer.woodTex = woodTex;

      loadModel(filename);
      //printf("Displaying step %d: \"%s\"\n", current_iterations, model.getProduction(current_iterations)->c_str());

      cameraToWorld.loadIdentity();
      camera_position[2] = 50.0f;
      cameraToWorld.translate(0, 0, camera_position[2]);
    }

    void loadModel(const char *filename) {
      model.readConfigurationFile(filename);
      model.dump_productions();
      model_renderer.setModel(&model);
      current_iterations = model.get_initial_iterations();
      just_pressed = true;
    }

    // this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx, vy;
      get_viewport_size(vx, vy);
      // set a viewport - includes whole window area
      glViewport(0, 0, vx, vy);

      // clear the background to black
      glClearColor(0.75f, 0.75f, 0.75f, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      if (model.is_loaded()) {
        int vx = 0, vy = 0;
        get_viewport_size(vx, vy);

        cameraToWorld.loadIdentity();
        cameraToWorld.translate(camera_position.x(), camera_position.y(), camera_position.z());

        model_renderer.render(cameraToWorld, cameraToProjection, current_iterations);

      }

      if (is_key_down('1') && !just_pressed) {
        loadModel("assets/lsystems1.xml");
      } else if (is_key_down('2') && !just_pressed) {
        loadModel("assets/lsystems2.xml");
      } else if (is_key_down('3') && !just_pressed) {
        loadModel("assets/lsystems3.xml");
      } else if (is_key_down('4') && !just_pressed) {
        loadModel("assets/lsystems4.xml");
      } else if (is_key_down('5') && !just_pressed) {
        loadModel("assets/lsystems5.xml");
      } else if (is_key_down('6') && !just_pressed) {
        loadModel("assets/lsystems6.xml");
      } else if (is_key_down('7') && !just_pressed) {
        loadModel("assets/lsystems7.xml");
      } else if (is_key_down('8') && !just_pressed) {
        loadModel("assets/lsystems8.xml");
      }else if (is_key_down('N') && !just_pressed) {
        current_iterations--;
        if (current_iterations < 0) current_iterations = 0;
        //printf("Displaying step %d: \"%s\"\n", current_iterations, model.getProduction(current_iterations)->c_str());
        just_pressed = true;
      } else if (is_key_down('M') && !just_pressed) {
        current_iterations++;
        //printf("Displaying step %d: \"%s\"\n", current_iterations, model.getProduction(current_iterations)->c_str());
        just_pressed = true;
      } else if (just_pressed &&
        !(is_key_down('1') || is_key_down('2') ||
          is_key_down('3') || is_key_down('4') ||
          is_key_down('5') || is_key_down('6') ||
          is_key_down('7') || is_key_down('8') ||
          is_key_down('N') || is_key_down('M')
         )) {
        just_pressed = false;
      }

      if (is_key_down('Z')) {
        camera_position[0] = 0.0f;
        camera_position[1] = 0.0f;
        camera_position[2] = 50.0f;
      }

      if (is_key_down('X')) {
        model_renderer.branch_length = 0.5f;
        model_renderer.branch_separation = 0.5;
        model_renderer.branch_rotate_angle = model.get_rotation_angle();
      }

      if (is_key_down('R')) {
        model_renderer.branch_separation -= 1.0f;
        if (model_renderer.branch_separation < 1.0f) model_renderer.branch_separation = 1.0f;
      } else if (is_key_down('T')) {
        model_renderer.branch_separation += 1.0f;
      }

      if (is_key_down('Y')) {
        model_renderer.branch_length -= 1.0f;
        if (model_renderer.branch_length < 1.0f) model_renderer.branch_length = 1.0f;
      } else if (is_key_down('U')) {
        model_renderer.branch_length += 1.0f;
      }

      if (is_key_down('H')) {
        model_renderer.branch_rotate_angle -= 0.5f;
      } else if (is_key_down('J')) {
        model_renderer.branch_rotate_angle += 0.5f;
      }
      
      if (is_key_down('W')) {
        camera_position[1] += 0.25f * (camera_position[2]/5.0f);
      } else if (is_key_down('S')) {
        camera_position[1] -= 0.25f * (camera_position[2]/5.0f);
      }

      if (is_key_down('A')) {
        camera_position[0] -= 0.25f * (camera_position[2]/5.0f);
      } else if (is_key_down('D')) {
        camera_position[0] += 0.25f * (camera_position[2]/5.0f);
      }

      if (is_key_down('Q')) {
        camera_position[2] -= 0.5f;
        if (camera_position[2] < 10.0f) camera_position[2] = 10.0f;
      } else if (is_key_down('E')) {
        camera_position[2] += 0.5f;
        if (camera_position[2] > 200.0f) camera_position[2] = 200.0f;
      }

      /*if (is_key_down('W') || is_key_down('S') || is_key_down('A') || is_key_down('D')) {
        printf("Camera position: (%.2f, %.2f, %.2f)\n", camera_position.x(), camera_position.y(), camera_position.z());
      }*/
      render_help();
    }

    void render_help() {
      
      if (!display_help) return;

      mat4t help_text_matrix;
      help_text_matrix.loadIdentity();
      help_text_matrix.translate(-29.0f, -29.0f, 0.0f);

      mat4t help_cam_matrix;
      help_cam_matrix.loadIdentity();
      help_cam_matrix.translate(0, 0, 30);

      // model -> world -> camera -> projection
      mat4t modelToProjection = mat4t::build_projection_matrix(help_text_matrix, help_cam_matrix);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, helpTex);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

      // set up the uniforms for the shader
      tshader.render(modelToProjection, 0);

      int zoomFactor = 10;

      float vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
        3.4615f*zoomFactor, 0.0f, 1.0f, 0.0f,
        3.4615f*zoomFactor, 1.0f*zoomFactor, 1.0f, 1.0f,
        0.0f, 1.0f*zoomFactor, 0.0f, 1.0f
      };

      glVertexAttribPointer(attribute_pos, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)vertices );
      glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(vertices + 2) );
      glEnableVertexAttribArray(attribute_pos);
      glEnableVertexAttribArray(attribute_uv);

      // finally, draw the box (4 vertices)
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
  };
}
