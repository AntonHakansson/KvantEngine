#ifndef CHECKS_HPP_INCLUDED
#define CHECKS_HPP_INCLUDED
#include "utils/Logger.hpp"

#define GL_CHECK(x)                                                            \
  x;                                                                           \
  {                                                                            \
    GLenum glError = glGetError();                                             \
    if (glError != GL_NO_ERROR) {                                              \
      LOG_ERROR << "glGetError() = " << glError << " at line " << __LINE__;    \
      exit(1);                                                                 \
    }                                                                          \
  }
#endif // CHECKS_HPP_INCLUDED