#include "vektorer.h"

vec Vector(float x, float y , float z)
{
    vec v = {x,y,z};
    return v;
}

vec Vector(float x, float y)
{
    vec v = {x,y,0};
    return v;
}

vec operator+(vec &v1, vec &v2)
{
    vec v = {v1.x + v2.x, v1.y + v2.y , v1.z + v2.z};
    return v;
}

vec operator+(vec &v1, vec v2)
{
    vec v = {v1.x + v2.x, v1.y + v2.y , v1.z + v2.z};
    return v;
}


vec operator-(vec &v1, vec &v2)
{
    vec v = {v1.x - v2.x, v1.y - v2.y , v1.z - v2.z};
    return v;
}

vec operator-(vec &v1, vec v2)
{
    vec v = {v1.x - v2.x, v1.y - v2.y , v1.z - v2.z};
    return v;
}
vec operator*(vec &v1, float s)
{
    vec v = {v1.x *s , v1.y * s , v1.z * s};
    return v;
}

//Skalärprodukt
float operator*(vec &v1, vec &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float operator*(vec &v1, vec v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
vec operator/(vec &v1, float s)
{
    vec v = {v1.x /s , v1.y / s , v1.z / s};
    return v;
}

vec &operator+=(vec &v1, vec &v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    return v1;
}

vec &operator+=(vec &v1, vec v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    return v1;
}

vec &operator*=(vec &v1, float s)
{
    v1.x *= s;
    v1.y *= s;
    v1.z *= s;
    return v1;
}
