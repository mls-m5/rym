/*
 * gl_code.h
 *
 *  Created on: 15 mar 2014
 *      Author: mattias
 */

#ifndef GL_CODE_H_
#define GL_CODE_H_

#include "vektorer.h"
void modelTransform(vec p, double a, double scale = 1);
void resetTransform();
void setCam(vec p, double a);
void camTransform();
void drawShip(vec p, double a);
void drawStar(vec p);
void drawComet(vec p, double a, double r);
void drawProjectile(vec p, double a, double scale);
void drawExplosion(vec pos, double size);
void drawSmoke(vec p1, vec p2, double alpha);
bool initDrawModule(double perspective);
void flushDraw();


#endif /* GL_CODE_H_ */
