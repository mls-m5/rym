#pragma once
struct vector
{
	vector (float x, float y, float z): x(x), y(y), z(z) {}
	vector (float x, float y) : x(x), y(y), z(0) {}
	vector (): x(0), y(0), z(0) {};
    float x,y,z;
};

vector Vector(float x, float y , float z);
vector Vector(float x, float y);
vector operator+(vector &v1, vector &v2);
vector operator+(vector &v1, vector v2);
vector operator-(vector &v1, vector &v2);
//vector operator-(vector &v1, vector v2);
vector operator*(vector &v, float s);
float operator*(vector &v1, vector &v2);
//float operator*(vector &v1, vector v2);
vector operator/(vector &v, float s);
vector &operator+=(vector &v1, vector &v2);
vector &operator*=(vector &v1, float s);
