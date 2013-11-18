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
    string current_production;
    bool loaded;

  public:
    int num_iterations;
    float rotation_angle;
    string axiom;
    dictionary<string> production_rules;

    LSystemsModel()
    : current_production()
    , loaded(false)
    , num_iterations(0)
    , rotation_angle(0.0f)
    , axiom()
    , production_rules()
    {
    } 

    LSystemsModel(const char *xmlFilename)
    : current_production()
    , loaded(false)
    , num_iterations(0)
    , rotation_angle(0.0f)
    , axiom()
    , production_rules()
    {
      readConfigurationFile(xmlFilename);
    } 

    bool readConfigurationFile(const char *xmlFilename) {
      TiXmlDocument doc;
      string doc_path;
      dictionary<TiXmlElement *, allocator> ids;
      
      doc_path = xmlFilename;
      doc_path.truncate(doc_path.filename_pos());
      doc.LoadFile(app_utils::get_path(doc_path));

      TiXmlElement *top = doc.RootElement();

      if (!top || strcmp(top->Value(), "lsystems")) {
        printf("warning: not a lsystems file");
        loaded = false;
        return false;
      }
      buildSystem(top);
      loaded = true;
      return true;
    }

    void buildSystem(TiXmlElement *parent) {
      for (TiXmlElement *elem = parent->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
        processElement(elem);
      }
    }

    void processElement(TiXmlElement *elem) {
      const char *elemValue = elem->Value();
      const char *elemText = elem->GetText();

      if (!strcmp(elemValue, "initial-iterations")) {
        this->num_iterations = atoi(elemText);
      } else if (!strcmp(elemValue, "initial-angle")) {
        this->rotation_angle = (float)atof(elemText);
      } else if (!strcmp(elemValue, "axiom")) {
        this->axiom = elemText;
      } else if (!strcmp(elemValue, "rule")) {
        processRule(elem);
      }
    }

    void processRule(TiXmlElement *elem) {
      if (elem->Attribute("predecessor") &&
          elem->Attribute("succesor")) {
        string predecessor(elem->Attribute("predecessor"));
        string succesor(elem->Attribute("succesor"));
        production_rules[predecessor] = succesor;
      }
    }

    bool is_loaded() {
      return loaded;
    }
  };
}
