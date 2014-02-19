#include "vektorer.h"

vector Vector(float x, float y , float z)
{
    vector v = {x,y,z};
    return v;
}

vector Vector(float x, float y)
{
    vector v = {x,y,0};
    return v;
}

vector operator+(vector &v1, vector &v2)
{
    vector v = {v1.x + v2.x, v1.y + v2.y , v1.z + v2.z};
    return v;
}

vector operator+(vector &v1, vector v2)
{
    vector v = {v1.x + v2.x, v1.y + v2.y , v1.z + v2.z};
    return v;
}


vector operator-(vector &v1, vector &v2)
{
    vector v = {v1.x - v2.x, v1.y - v2.y , v1.z - v2.z};
    return v;
}

vector operator-(vector &v1, vector v2)
{
    vector v = {v1.x - v2.x, v1.y - v2.y , v1.z - v2.z};
    return v;
}
vector operator*(vector &v1, float s)
{
    vector v = {v1.x *s , v1.y * s , v1.z * s};
    return v;
}

//Skalärprodukt
float operator*(vector &v1, vector &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float operator*(vector &v1, vector v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
vector operator/(vector &v1, float s)
{
    vector v = {v1.x /s , v1.y / s , v1.z / s};
    return v;
}

vector &operator+=(vector &v1, vector &v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    return v1;
}

vector &operator+=(vector &v1, vector v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    return v1;
}

vector &operator*=(vector &v1, float s)
{
    v1.x *= s;
    v1.y *= s;
    v1.z *= s;
    return v1;
}
