#include <iostream>
#include "Color.h"

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
    r(Utils::unitClamp(_r)), 
    g(Utils::unitClamp(_g)), 
    b(Utils::unitClamp(_b))
{ }

Color::Color(const Color& other)
{
    this->r = other.r;
    this->g = other.g;
    this->b = other.b;
}

// Predefined colors:
const Color Color::BLACK = Color(0.0f,0.0f,0.0f);
const Color Color::WHITE = Color(0.0f,0.0f,0.0f);
const Color Color::RED   = Color(1.0f,0.0f,0.0f);
const Color Color::GREEN = Color(0.0f,1.0f,0.0f);
const Color Color::BLUE  = Color(0.0f,0.0f,1.0f);

////////////////////////////////////////////////////////////////////////////////
// Setters
////////////////////////////////////////////////////////////////////////////////
///
void Color::setR(float _r) 
{
    this->r = Utils::unitClamp(_r);
}

void Color::setG(float _g) 
{
    this->g = Utils::unitClamp(_g); 
}

void Color::setB(float _b) 
{
    this->b = Utils::unitClamp(_b);
}

void Color::setR(int _r) 
{
    this->r = Utils::unitClamp((float)_r / 255.0f);
}

void Color::setG(int _g) 
{   
    this->g = Utils::unitClamp((float)_g / 255.0f);
}

void Color::setB(int _b) 
{
    this->b = Utils::unitClamp((float)_b / 255.0f);
}

// Simply ignore both position and origin: the same color is always returned
Color Color::colorAt(const P& position, const P& origin)
{
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
// Operations
////////////////////////////////////////////////////////////////////////////////

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
    this->r = Utils::unitClamp(this->r + c.r);
    this->b = Utils::unitClamp(this->b + c.b);
    this->g = Utils::unitClamp(this->g + c.g);
    return *this;
}

Color& Color::operator-=(const Color &c)
{
    this->r = Utils::unitClamp(this->r - c.r);
    this->b = Utils::unitClamp(this->b - c.b);
    this->g = Utils::unitClamp(this->g - c.g);
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
// To string
////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& s, const Color& color)
{
    return s << "Color(" << color.fR() << "," 
                         << color.fG() << "," 
                         << color.fB() << ")";
}

