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

    bool readConfigurationFile(const char *xmlFilename) {
      TiXmlDocument doc;
      dictionary<TiXmlElement *, allocator> ids;
      
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
        char comp[2] = { previous_production[i], 0 };
        
        if (production_rules_[comp]) {
          result += production_rules_[comp];
        } else {
          string comp_str(comp);
          result += comp_str;
        }
      }

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
      if (result >= productions_.size()) {
        int difference = result - productions_.size();

        printf("Generating %d productions.\n", difference);

        for (int i = 0; i != difference; i++) {
          step();
        }
      }

      return &productions_[result];
    }

    bool is_loaded() {
      return loaded_;
    }
  };
}
