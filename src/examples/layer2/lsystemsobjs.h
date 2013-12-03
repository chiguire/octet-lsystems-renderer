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

namespace octet {
  class LSystemsModel {

    bool loaded_;
    int num_iterations_;
    float rotation_angle_;
    string axiom_;
    dynarray<string> productions_;
    dictionary<string> production_rules_;

    void buildSystem(TiXmlElement *parent) {
      for (TiXmlElement *elem = parent->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
        processElement(elem);
      }
    }

    void processElement(TiXmlElement *elem) {
      const char *elemValue = elem->Value();
      const char *elemText = elem->GetText();

      if (!strcmp(elemValue, "initial-iterations")) {
        this->num_iterations_ = atoi(elemText);
      } else if (!strcmp(elemValue, "initial-angle")) {
        this->rotation_angle_ = (float)atof(elemText);
        //printf("Rotation angle is: %.2f.\n", rotation_angle_);
      } else if (!strcmp(elemValue, "axiom")) {
        this->axiom_ = elemText;
        this->productions_.push_back(axiom_);
      } else if (!strcmp(elemValue, "rule")) {
        processRule(elem);
      }
    }

    void processRule(TiXmlElement *elem) {
      if (elem->Attribute("predecessor") &&
          elem->Attribute("succesor")) {
        string predecessor(elem->Attribute("predecessor"));
        string succesor(elem->Attribute("succesor"));
        production_rules_[predecessor.c_str()] = succesor;
      }
    }

  public:
    LSystemsModel()
    : loaded_(false)
    , num_iterations_(0)
    , rotation_angle_(0.0f)
    , axiom_()
    , productions_()
    , production_rules_()
    {
    } 

    LSystemsModel(const char *xmlFilename)
    : loaded_(false)
    , num_iterations_(0)
    , rotation_angle_(0.0f)
    , axiom_()
    , productions_()
    , production_rules_()
    {
      readConfigurationFile(xmlFilename);
    } 

    void cleanModel() {
      productions_.reset();
      production_rules_.reset();
      axiom_.truncate(0);
      loaded_ = false;
    }

    bool readConfigurationFile(const char *xmlFilename) {
      TiXmlDocument doc;
      dictionary<TiXmlElement *, allocator> ids;

      cleanModel();
      
      doc.LoadFile(app_utils::get_path(xmlFilename));

      TiXmlElement *top = doc.RootElement();

      if (!top || strcmp(top->Value(), "lsystems")) {
        printf("warning: not a lsystems file");
        loaded_ = false;
        return false;
      }
      buildSystem(top);
      
      for (int i = 0; i != num_iterations_; i++) {
        step();
      }
      
      loaded_ = true;

      return true;
    }

    const string *step() {
      const char *previous_production = getProduction()->c_str();
      int len = strlen(previous_production);
      string result; //get last production
      
      printf("Generating step %d.\n", productions_.size());

      // deterministic context-free
      // string replace for each character, not yet context sensitive
      for (int i = 0; i != len; i++) {
        char comp[] = { previous_production[i], 0 };
        
        //printf("step(): Checking char '%c'.\n", comp[0]);
        if (production_rules_.contains(comp)) {
          //printf("step(): Found rule, adding string '%s'.\n", production_rules_[comp].c_str());
          result += production_rules_[comp];
        } else {
          string comp_str(comp);
          //printf("step(): Found no rule, adding string '%s'.\n", comp_str);
          result += comp_str;
        }
        //printf("step(): Current string '%s'.\n", result.c_str());
      }

      //printf("step(): Final string '%s'.\n", result.c_str());

      productions_.push_back(result);
       
      return &productions_[productions_.size()-1];
    }

    const string *getProduction(int number = -1) {
      int result = number;

      while (result < 0) {
        result += productions_.size();
      }

      // automatically step through tree if getting a production
      // not yet calculated
      if (result + 1 > (int)productions_.size()) {
        int difference = result - productions_.size() + 1;

        //printf("Generating %d productions.\n", difference);

        for (int i = 0; i != difference; i++) {
          step();
        }
      }

      return &productions_[result];
    }

    bool is_loaded() {
      return loaded_;
    }

    float get_rotation_angle() {
      return rotation_angle_;
    }

    int get_initial_iterations() {
      return num_iterations_;
    }

    const char *get_axiom() {
      return axiom_.c_str();
    }

    void dump_productions() {
      for (int i = 0; i != productions_.size(); i++) {
        printf("Step %d: %s.\n", i, productions_[i]);
      }
    }
  };

  class LSystemsRenderer {
  protected:
    LSystemsModel *model;
    dynarray<mat4t> matrix_stack;

    void pushMatrix() {
      matrix_stack.push_back(topMatrix());
    }

    mat4t popMatrix() {
      mat4t result = topMatrix();
      if (matrix_stack.size() == 1) {
        printf("popMatrix(): trying to pop root node");
        return result;
      }
      matrix_stack.pop_back();
      return result;
    }

    mat4t &topMatrix() {
      return matrix_stack[matrix_stack.size()-1];
    }

    void initStack() {
      while (matrix_stack.size() > 1) {
        matrix_stack.pop_back();
      }
      matrix_stack[0].loadIdentity();
    }

  public:
    vec3 origin;
    float rotation_angle;
    vec3 rotation_vector;

    LSystemsRenderer()
    : model(NULL)
    , matrix_stack()
    , origin()
    , rotation_angle(0.0f)
    , rotation_vector(0.0f, 0.0f, 1.0f)
    { 
      mat4t m_(1.0f);
      matrix_stack.push_back(m_);
    }
    
    LSystemsRenderer(LSystemsModel *m)
    : model(m)
    , matrix_stack()
    , origin()
    , rotation_angle(0.0f)
    , rotation_vector(0.0f, 0.0f, 1.0f)
    { 
      mat4t m_(1.0f);
      matrix_stack.push_back(m_);
      if (m) {
        setModel(m);
      }
    }

    virtual void setModel(LSystemsModel *m) {
      model = m;
      initStack();
    }

    virtual void render(mat4t &cameraToWorld, mat4t &cameraToProjection, int num_iterations) {
      initStack();
      //matrix_stack[0].rotate(rotation_angle, rotation_vector.x(), rotation_vector.y(), rotation_vector.z());
      //matrix_stack[0].translate(origin.x(), origin.y(), origin.z());
      const char *production = model->getProduction(num_iterations)->c_str();
      int production_len = strlen(production);

      for (int i = 0; i != production_len; i++) {
        processChar(cameraToWorld, cameraToProjection, *(production+i));
      }
    }
    
    virtual void processChar(mat4t &cameraToWorld, mat4t &cameraToProjection, char c) = 0;
  };

  class Tree2DRenderer : public LSystemsRenderer {
  public:
    float branch_rotate_angle;
    float branch_length;
    float branch_separation;
    texture_shader *tshader;
    
    GLuint leafTex;
    GLuint woodTex;

    Tree2DRenderer(texture_shader *tshader_ = NULL, LSystemsModel *m = NULL)
    : LSystemsRenderer(m)
    , branch_rotate_angle(0.0f)
    , branch_length(5.0f)
    , branch_separation(branch_length)
    , tshader(tshader_)
    {
    }

    void setModel(LSystemsModel *m) {
      LSystemsRenderer::setModel(m);
      if (m) {
        branch_rotate_angle = m->get_rotation_angle();
      }
    }

    void processChar(mat4t &cameraToWorld, mat4t &cameraToProjection, char c) {
      if (c == '[') {
        pushMatrix();
      } else if (c == ']') {
        popMatrix();
      } else if (c == '+') {
        topMatrix().rotate(branch_rotate_angle, rotation_vector.x(), rotation_vector.y(), rotation_vector.z());
      } else if (c == '-') {
        topMatrix().rotate(-branch_rotate_angle, rotation_vector.x(), rotation_vector.y(), rotation_vector.z());
      } else {
        renderLeaf(cameraToWorld, cameraToProjection, c == 'X');
        vec4 up_branch(0.0f, branch_separation, 0.0f, 1.0f);
        topMatrix().translate(up_branch.x(), up_branch.y(), up_branch.z());
      }
    }

    void renderLeaf(mat4t &cameraToWorld, mat4t &cameraToProjection, bool useLeafTex = true) {

      // model -> world -> camera -> projection
      mat4t modelToProjection = mat4t::build_projection_matrix(topMatrix(), cameraToWorld);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, useLeafTex? leafTex: woodTex);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      // set up the uniforms for the shader
      tshader->render(modelToProjection, 0);

      float branch_texture_v = branch_length/1.0f;

      float vertices[] = {
        -0.25f, 0.0f, 0.0f, 0.0f,
        0.25f, 0.0f, 1.0f, 0.0f,
        0.25f,  branch_length, 1.0f, branch_texture_v,
        -0.25f,  branch_length, 0.0f, branch_texture_v
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
