/*
 * gl_code.h
 *
 *  Created on: 15 mar 2014
 *      Author: mattias
 */

#ifndef GL_CODE_H_
#define GL_CODE_H_

#include "vektorer.h"
void modelTransform(vector p, double a, double scale = 1);
void resetTransform();
void setCam(vector p, double a);
void camTransform();
void drawShip(vector p, double a);
void drawStar(vector p);
void drawComet(vector p, double a, double r);
void drawProjectile(vector p, double a, double scale);
void drawExplosion(vector pos, double size);
void drawSmoke(vector p1, vector p2, double alpha);
bool initDrawModule(double perspective);
void flushDraw();


#endif /* GL_CODE_H_ */
