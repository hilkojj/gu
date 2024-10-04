
/**
 * This file makes sure stb_image.h is included at most once, with the same configuration.
 * Including it more than once results in errors. Which happens when using unity build.
 */
#pragma once
#define STB_IMAGE_IMPLEMENTATION

#include "../../../external/stb/stb_image.h"
