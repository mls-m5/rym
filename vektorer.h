#pragma once

struct vec
{
	vec (float x, float y, float z): x(x), y(y), z(z) {}
	vec (float x, float y) : x(x), y(y), z(0) {}
	vec (): x(0), y(0), z(0) {};

    float x,y,z;
};

vec Vector(float x, float y , float z);
vec Vector(float x, float y);
vec operator+(vec &v1, vec &v2);
vec operator+(vec &v1, vec v2);
vec operator-(vec &v1, vec &v2);
//vector operator-(vector &v1, vector v2);
vec operator*(vec &v, float s);
float operator*(vec &v1, vec &v2);
//float operator*(vector &v1, vector v2);
vec operator/(vec &v, float s);
vec &operator+=(vec &v1, vec &v2);
vec &operator*=(vec &v1, float s);
