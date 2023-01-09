#include "structs/VulkanCommonStruct.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

bool
VulkanScreenshot::saveScreenshotToFile(std::string const &filepath) const
{
    if (!width || !height) {
        return (false);
    }

    return (stbi_write_png(filepath.c_str(),
                           width,
                           height,
                           static_cast<int32_t>(nbChannel),
                           data.get(),
                           static_cast<int32_t>(width * nbChannel)));
}
