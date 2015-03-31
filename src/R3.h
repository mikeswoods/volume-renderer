#ifndef _R3_H
#define _R3_H

#include <iostream>
#include <glm/glm.hpp>

////////////////////////////////////////////////////////////////////////////////
// Vector type in R3 space
////////////////////////////////////////////////////////////////////////////////

typedef glm::vec3 V;

float x(const V& v);
float y(const V& v);
float z(const V& v);

std::ostream& operator<<(std::ostream& s, const V& v);

V operator*(const V& v, float s);
V operator/(const V& v, float s);

////////////////////////////////////////////////////////////////////////////////
// Point type in R3 space
////////////////////////////////////////////////////////////////////////////////

class P
{
    public:
        const static P AT_INFINITY;

        glm::vec3 p;

        P();
        P(float x, float y, float z);
        P(const P& other);

        float x() { return this->p.x; } 
        float y() { return this->p.y; } 
        float z() { return this->p.z; } 

        friend std::ostream& operator<<(std::ostream& s, const P& p);

        P& operator+=(const V& v);
        P& operator-=(const V& v);
};

float x(const P& p);
float y(const P& p);
float z(const P& p);

float dist(const P& p1, const P& p2);
P relative(const P& p, const P& center);
int traverse(float stepSize, float offset, const P& start, const P& end, P& X, V& N);

P operator+(const P& p, const V& v);
P operator+(const P& p, float mu);
P operator*(const P& p, float mu);
P operator+(const V& v, const P& p);
V operator-(const P& p1, const P& p2);
P operator-(const P& p, float mu);


#endif
