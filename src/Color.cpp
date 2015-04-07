#include <iostream>
#include "Color.h"

/******************************************************************************/

using namespace std;
using namespace Utils;

/******************************************************************************/

const std::shared_ptr<Color> Color::BLACK = make_shared<Color>(0.0f, 0.0f, 0.0f);
const std::shared_ptr<Color> Color::WHITE = make_shared<Color>(0.0f, 0.0f, 0.0f);
const std::shared_ptr<Color> Color::RED   = make_shared<Color>(1.0f, 0.0f, 0.0f);
const std::shared_ptr<Color> Color::GREEN = make_shared<Color>(0.0f, 1.0f, 0.0f);
const std::shared_ptr<Color> Color::BLUE  = make_shared<Color>(0.0f, 0.0f, 1.0f);

/******************************************************************************/

Color::Color()
{
    this->r = this->g = this->b = 0.0f;
}

Color::Color(int r, int g, int b)
{ 
    this->setR(r);
    this->setG(g);
    this->setB(b);
}

Color::Color(float _r, float _g, float _b) : 
    r(unitClamp(_r)), 
    g(unitClamp(_g)), 
    b(unitClamp(_b))
{ }

Color::Color(const Color& other)
{
    this->r = other.r;
    this->g = other.g;
    this->b = other.b;
}

void Color::setR(float _r) 
{
    this->r = unitClamp(_r);
}

void Color::setG(float _g) 
{
    this->g = unitClamp(_g); 
}

void Color::setB(float _b) 
{
    this->b = unitClamp(_b);
}

void Color::setR(int _r) 
{
    this->r = unitClamp((float)_r / 255.0f);
}

void Color::setG(int _g) 
{   
    this->g = unitClamp((float)_g / 255.0f);
}

void Color::setB(int _b) 
{
    this->b = unitClamp((float)_b / 255.0f);
}

// Simply ignore both position and origin: the same color is always returned
Color Color::colorAt(const P& position, const P& origin)
{
    return *this;
}

bool operator==(const Color& c1, const Color& c2)
{
    return (c1.r == c2.r) && (c1.g == c2.g) && (c1.b == c2.b);
}

bool operator!=(const Color& c1, const Color& c2)
{
    return (c1.r != c2.r) && (c1.g != c2.g) && (c1.b != c2.b);
}

const Color operator+(const Color& c1, const Color& c2)
{
    return Color(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b);
}

const Color operator*(const Color& c1, const Color& c2)
{
    return Color(c1.r * c2.r, c1.g * c2.g, c1.b * c2.b);
}

const Color operator*(const Color& c, const float scale)
{
    return Color(c.r * scale, c.g * scale, c.b * scale);
}

const Color operator/(const Color& c, const float scale)
{
    return Color(c.r / scale, c.g / scale, c.b / scale);
}

Color& Color::operator+=(const Color &c)
{
    this->r = unitClamp(this->r + c.r);
    this->b = unitClamp(this->b + c.b);
    this->g = unitClamp(this->g + c.g);
    return *this;
}

Color& Color::operator-=(const Color &c)
{
    this->r = unitClamp(this->r - c.r);
    this->b = unitClamp(this->b - c.b);
    this->g = unitClamp(this->g - c.g);
    return *this;
}

std::ostream& operator<<(std::ostream& s, const Color& color)
{
    return s << "Color(" << color.fR() << "," 
                         << color.fG() << "," 
                         << color.fB() << ")";
}

