#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <iostream>
#include <glad/glad.h>
#include "../stb_image/stb_image.h"

namespace TextureLoader
{
    unsigned int loadTexture(const char *path);
}

#endif