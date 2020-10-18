/*
 * gl_code.h
 *
 *  Created on: 15 mar 2014
 *      Author: mattias
 */

#ifndef GL_CODE_H_
#define GL_CODE_H_

import vec;

void modelTransform(Vec p, double a, double scale = 1);
void resetTransform();
void setCam(Vec p, double a);
void camTransform();
void drawShip(Vec p, double a);
void drawStar(Vec p);
void drawComet(Vec p, double a, double r);
void drawArea(Vec p, double a, double r);
void drawProjectile(Vec p, double a, double scale);
void drawExplosion(Vec pos, double size);
void drawSmoke(Vec p1, Vec p2, double alpha1, double alpha2);
bool initDrawModule(double perspective);
void flushDraw();

#endif /* GL_CODE_H_ */
