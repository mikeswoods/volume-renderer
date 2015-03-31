#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <limits>
#include "R3.h"

////////////////////////////////////////////////////////////////////////////////
// Vector type in R3 space
////////////////////////////////////////////////////////////////////////////////

const P P::AT_INFINITY = P(std::numeric_limits<float>::infinity()
                          ,std::numeric_limits<float>::infinity()
                          ,std::numeric_limits<float>::infinity());

float x(const V& v) 
{
    return v.x;
}

float y(const V& v) 
{
    return v.y;
}

float z(const V& v) 
{
    return v.z;
}

// Get a string representation of a vector:
std::ostream& operator<<(std::ostream &s, const V &v)
{
    return s << "<" << v.x << "," << v.y << "," << v.z << ">";
}

// Multiply a vector by a scalar yields a vector:
V operator*(const V &v, float s)
{
    return V(x(v) * s, y(v) * s, z(v) * s);
}

V operator/(const V &v, float s)
{
    return V(x(v) / s, y(v) / s, z(v) / s);
}

////////////////////////////////////////////////////////////////////////////////
// Point type in R3 space
////////////////////////////////////////////////////////////////////////////////

P::P()
{
    this->p = glm::vec3();
}

P::P(float x, float y, float z)
{
    this->p = glm::vec3(x,y,z);
}

// P copy constructor
P::P(const P &other)
{
    this->p = other.p;
}

float x(const P& p)
{
    return p.p.x;
}

float y(const P& p)
{
    return p.p.y;
}

float z(const P& p)
{
    return p.p.z;
}

// Get a string representation of a point:
std::ostream& operator<<(std::ostream &s, const P &p)
{
    return s << "[" << x(p) << "," << y(p) << "," << z(p) << "]";
}

// Compute the distance between this point and another
float dist(const P &p1, const P &p2)
{
    return glm::distance(p1.p,p2.p);
}

// Computes the location of a point relative to a given origin position
P relative(const P& p, const P& origin)
{
    return P(0.0f,0.0f,0.0f) + (origin - p);
}

// Step along calculation, returning the number of steps as well as
// setting the initial position point X and step vector N
int traverse(float stepSize, float offset, const P& start, const P& end, P& X, V& N)
{
    V D = glm::normalize(end - start);
    N = D * stepSize;
    X = start + (D * offset);
    return (int)ceil(dist(start, end) / stepSize);
}

// Add a point and a vector yields a point:
P operator+(const P &p, const V &v)
{
    return P(x(p) + x(v), y(p) + y(v), z(p) + z(v));
}

P operator+(const P& p, float mu)
{
    return P(x(p) + mu, y(p) + mu, z(p) + mu);   
}

P operator*(const P& p, float mu)
{
    return P(x(p) * mu, y(p) * mu, z(p) * mu);   
}

P& P::operator+=(const V &v)
{
    this->p += v;
    return *this;
}

P& P::operator-=(const V &v)
{
    this->p -= v;
    return *this;
}

// Subtracting a point and a point yields a vector:
V operator-(const P &p1, const P &p2)
{
    return V(x(p1) - x(p2), y(p1) - y(p2), z(p1) - z(p2));
}

P operator-(const P& p, float mu)
{
    return P(x(p) - mu, y(p) - mu, z(p) - mu);   
}
