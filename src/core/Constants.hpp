#ifndef CONSTANTS_HPP_INCLUDED
#define CONSTANTS_HPP_INCLUDED

#ifndef KVANTENGINE_RESOURCES_PATH
  #error Error: -DKVANTENGINE_RESOURCES_PATH not set. No resources directory specified!
#else
  #include <string>
  namespace Kvant {
    static const std::string res_path{KVANTENGINE_RESOURCES_PATH};
  }
#endif

#endif // CONSTANTS_HPP_INCLUDED