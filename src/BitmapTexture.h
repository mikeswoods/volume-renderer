#ifndef _BITMAP_TEXTURE_H
#define _BITMAP_TEXTURE_H
#include <CImg.h>
#include "Material.h"

////////////////////////////////////////////////////////////////////////////////
// Bitmap texture
////////////////////////////////////////////////////////////////////////////////

using namespace cimg_library;

class BitmapTexture : public Material
{
    protected:
        std::string filePath;
        CImg<unsigned char> texture;
        float width, height;

    public:
        BitmapTexture(std::string fileName);

        virtual Color colorAt(const P& position, const P& origin);
};

#endif
