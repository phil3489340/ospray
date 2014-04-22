// ospray stuff
#include "geometry.h"
// embree stuff
#include "common/sys/library.h"
// stl stuff
#include <map>
// std c stuff
#include <dlfcn.h>
// ISPC-side
#include "geometry_ispc.h"

namespace ospray {
  typedef Geometry *(*creatorFct)();

  std::map<std::string, creatorFct> geometryRegistry;

    //! set given geometry's material. 
    /*! all material assignations should go through this function; the
        'material' field itself is private). This allows the
        respective geometry's derived instance to always properly set
        the material field of the ISCP-equivalent whenever the
        c++-side's material gets changed */
  void Geometry::setMaterial(Material *mat)
  {
    material = mat;
    if (!getIE()) 
      std::cout << "#osp: warning - geometry does not have an ispc equivalent!" << std::endl;
    else {
      ispc::Geometry_setMaterial(this->getIE(),mat?mat->getIE():NULL);
    }
  }


  /*! \brief creates an abstract material class of given type 
    
    The respective material type must be a registered material type
    in either ospray proper or any already loaded module. For
    material types specified in special modules, make sure to call
    ospLoadModule first. */
  Geometry *Geometry::createGeometry(const char *type)
  {
    std::map<std::string, Geometry *(*)()>::iterator it = geometryRegistry.find(type);
    if (it != geometryRegistry.end())
      return it->second ? (it->second)() : NULL;
    
    if (ospray::logLevel >= 2) 
      std::cout << "#ospray: trying to look up geometry type '" 
                << type << "' for the first time" << std::endl;

    std::string creatorName = "ospray_create_geometry__"+std::string(type);
    creatorFct creator = (creatorFct)dlsym(RTLD_DEFAULT,creatorName.c_str());
    geometryRegistry[type] = creator;
    if (creator == NULL) {
      if (ospray::logLevel >= 1) 
        std::cout << "#ospray: could not find geometry type '" << type << "'" << std::endl;
      return NULL;
    }
    return (*creator)();
  }
};

