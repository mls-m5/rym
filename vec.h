/*
 * Vector math: vec.h
 * author: Mattias Larsson Sköld
 */

#pragma once

#include <math.h>
#include "common.h"

class Vec
{
	public:
		double x, y, z;

		Vec (): x(0), y(0), z(0) {};
		Vec (double nx, double ny, double nz):x(nx), y(ny), z(nz) {}
		Vec (double nx, double ny): x(nx), y(ny), z(0) {}
		Vec (const Vec& v): x(v.x), y(v.y), z(v.z) {}


		Vec operator+=(Vec v){
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}

		Vec operator-=(Vec v){
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}


		Vec operator*=(double t){
			x *= t;
			y *= t;
			z *= t;
			return *this;
		}

		Vec operator/=(double t){
			x /= t;
			y /= t;
			z /= t;
			return *this;
		}

		Vec operator*(double t){
			return Vec(x *t, y*t, z*t);
		}
		Vec operator/(double t){
			return Vec(x /t, y/t, z/t);
		}

		double operator *(Vec v2){
			return x * v2.x +
				y * v2.y +
				z + v2.z;
		}

		Vec operator -(Vec v){
			return Vec(
				x - v.x,
				y - v.y,
				z - v.z);
		}

		Vec operator +(Vec v){
			return Vec(
				x + v.x,
				y + v.y,
				z + v.z);
		}

		bool operator== (Vec v){
			return x == v.x && y == v.y && z == v.z;
		}

		double abs(){
			return sqrt(x*x + y*y + z*z);
		}

		double abs2(){
			return x*x + y*y + z*z;
		}

		Vec normalize(){
			* this /= abs();
			return *this;
		}

		Vec cross(Vec v){
			return Vec(
				y * v.z - z * v.y,
				z * v.x - x * v.z,
				x * v.y - y * v.x
			);
		}

		double angle(double a){
			double angle = atan2(x,y) + a;

			redo1:
			if (angle < pi){
				angle += pi * 2.;
				goto redo1;
			}

			redo2:
			if (angle > pi){
				angle -= pi * 2.;
				goto redo2;
			}
			return angle;
		}

		double angle(){
			return atan2(x,y);
		}


		//Squared distance, can be used to save performance
		inline double distance2(Vec &v) {
			auto tx = x - v.x;
			auto ty = y - v.y;
			auto tz = z - v.z;
			return tx * tx + ty * ty;
		}

		double distance(Vec &v) {
			return sqrt(distance2(v));
		}

		inline double distance2d2(Vec &v) {
			auto tx = x - v.x;
			auto ty = y - v.y;
			auto tz = z - v.z;
			return tx * tx + ty * ty;
		}

		double distance2d(Vec &v) {
			return sqrt(distance2d2(v));
		}
};

