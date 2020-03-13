// tools_precompiled.h
//

#pragma once

#include "../idlib/precompiled.h"


#if defined(WIN32)
#include <renderer/qgllib/glew.h>
#include <renderer/qgllib/qgllib.h>
#else
#include <GL/glew.h>
#endif

extern "C" {
#if defined(WIN32)
#include <renderer/libjpeg-turbo-master/jpeglib.h>
#else
#include <jpeglib.h>
#endif
}
