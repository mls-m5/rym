#pragma once

// Really old vector implementation. Not used and only saved for nostalgia

struct vec {
    double x = 0, y = 0, z = 0;
};

vec operator+(vec &v1, vec &v2);
vec operator+(vec &v1, vec v2);
vec operator-(vec &v1, vec &v2);
vec operator*(vec &v, double s);
double operator*(vec &v1, vec &v2);
vec operator/(vec &v, double s);
vec &operator+=(vec &v1, vec &v2);
vec &operator*=(vec &v1, double s);

vec operator+(vec &v1, vec &v2) {
    return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vec operator+(vec &v1, vec v2) {
    return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vec operator-(vec &v1, vec &v2) {
    return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

vec operator-(vec &v1, vec v2) {
    return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}
vec operator*(vec &v1, double s) {
    return {v1.x * s, v1.y * s, v1.z * s};
}

// Skalärprodukt
double operator*(vec &v1, vec &v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

double operator*(vec &v1, vec v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
vec operator/(vec &v1, double s) {
    return {v1.x / s, v1.y / s, v1.z / s};
}

vec &operator+=(vec &v1, vec &v2) {
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    return v1;
}

vec &operator+=(vec &v1, vec v2) {
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    return v1;
}

vec &operator*=(vec &v1, double s) {
    v1.x *= s;
    v1.y *= s;
    v1.z *= s;
    return v1;
}
