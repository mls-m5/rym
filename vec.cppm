/*
 * Vector math: vec.h
 * author: Mattias Larsson Sköld
 */

export module vec;

#include "common.h"
#include <cmath>

export class Vec {
public:
    double x = 0, y = 0, z = 0;

    Vec(double nx = 0, double ny = 0, double nz = 0) : x(nx), y(ny), z(nz) {
    }
    Vec(const Vec &v) = default;
    Vec(Vec &&) = default;
    Vec &operator=(const Vec &) = default;
    Vec &operator=(Vec &&) = default;
    ~Vec() = default;

    Vec operator+=(Vec v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vec operator-=(Vec v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vec operator*=(double t) {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

    Vec operator/=(double t) {
        x /= t;
        y /= t;
        z /= t;
        return *this;
    }

    Vec operator*(double t) {
        return {x * t, y * t, z * t};
    }
    Vec operator/(double t) {
        return {x / t, y / t, z / t};
    }

    double operator*(Vec v2) {
        return x * v2.x + y * v2.y + z + v2.z;
    }

    Vec operator-(Vec v) {
        return {x - v.x, y - v.y, z - v.z};
    }

    Vec operator+(Vec v) {
        return {x + v.x, y + v.y, z + v.z};
    }

    bool operator==(Vec v) {
        return x == v.x && y == v.y && z == v.z;
    }

    double abs() {
        return sqrt(x * x + y * y + z * z);
    }

    double abs2() {
        return x * x + y * y + z * z;
    }

    Vec normalize() {
        *this /= abs();
        return *this;
    }

    Vec cross(Vec v) {
        return {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
    }

    double angle(double a) {
        double angle = atan2(x, y) + a;

        while (angle < pi) {
            angle += pi2;
        }

        while (angle > pi) {
            angle -= pi2;
        }

        return angle;
    }

    double angle() {
        return atan2(x, y);
    }

    // Squared distance, can be used to save performance
    inline double distance2(Vec &v) {
        auto tx = x - v.x;
        auto ty = y - v.y;
        return tx * tx + ty * ty;
    }

    double distance(Vec &v) {
        return sqrt(distance2(v));
    }

    inline double distance2d2(Vec &v) {
        auto tx = x - v.x;
        auto ty = y - v.y;
        return tx * tx + ty * ty;
    }

    double distance2d(Vec &v) {
        return sqrt(distance2d2(v));
    }
};
