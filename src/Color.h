#ifndef _COLOR_H
#define _COLOR_H

#include <memory>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include "Material.h"
#include "Utils.h"

////////////////////////////////////////////////////////////////////////////////
// RGB color type
////////////////////////////////////////////////////////////////////////////////

class Color : public Material
{
    protected:
        float r, g, b;

    public:
        const static std::shared_ptr<Color> BLACK;
        const static std::shared_ptr<Color> WHITE;
        const static std::shared_ptr<Color> RED;
        const static std::shared_ptr<Color> GREEN;
        const static std::shared_ptr<Color> BLUE;

        Color();
        Color(int r, int g, int b);
        Color(float r, float g, float b);
        Color(const Color& other);

        void setR(float r);
        void setR(int r);
        void setG(float g);
        void setG(int g);
        void setB(float b);
        void setB(int b);

        // Red as a float in [0,1]
        float fR() const { return this->r; }
        // Green as a float in [0,1]
        float fG() const { return this->g; }
        // Blue as a float in [0,1]
        float fB() const { return this->b; }
        // Red as an int in [0,255]
        unsigned char iR() const { return (unsigned char)floor(this->r * 255.0f); }
        // Green as an int in [0,255]
        unsigned char iG() const { return (unsigned char)floor(this->g * 255.0f); }
        // Blue as an int in [0,255] 
        unsigned char iB() const { return (unsigned char)floor(this->b * 255.0f); }

        virtual Color colorAt(const P& position, const P& origin);

        Color& operator+=(const Color &c);
        Color& operator-=(const Color &c);

        friend std::ostream& operator<<(std::ostream& s, const Color& color);
        friend bool operator==(const Color& c1, const Color& c2);
        friend bool operator!=(const Color& c1, const Color& c2);
        friend const Color operator+(const Color& c1, const Color& c2);
        friend const Color operator*(const Color& c1, const Color& c2);
        friend const Color operator*(const Color& c, const float scale);
        friend const Color operator/(const Color& c, const float scale);
};

#endif
