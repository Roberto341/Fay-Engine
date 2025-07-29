#pragma once

#include <string>
#include <FreeImage.h>
#include <glew.h>
namespace Fay
{
    static BYTE* load_image(const char* filename, GLsizei* width, GLsizei* height)
    {
        FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(filename, 0);
        if (fif == FIF_UNKNOWN)
            fif = FreeImage_GetFIFFromFilename(filename);
        if (fif == FIF_UNKNOWN)
            return nullptr;

        if (!FreeImage_FIFSupportsReading(fif))
            return nullptr;

        FIBITMAP* dib = FreeImage_Load(fif, filename);
        if (!dib)
            return nullptr;

        // Convert to 32-bit with alpha channel
        FIBITMAP* dib32 = FreeImage_ConvertTo32Bits(dib);
        FreeImage_Unload(dib);

        *width = FreeImage_GetWidth(dib32);
        *height = FreeImage_GetHeight(dib32);

        size_t size = (*width) * (*height) * 4;
        BYTE* pixels = new BYTE[size];
        memcpy(pixels, FreeImage_GetBits(dib32), size);

        FreeImage_Unload(dib32);

        return pixels;
    }
}