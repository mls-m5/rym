/*
 * draw-win.cpp
 *
 *  Created on: 15 jan. 2016
 *      Author: mattr
 */

#include "draw.h"
#include "GL/gl.h"
#include <iostream>

constexpr double angularMultiplier = 180. / pi;



using namespace std;

void modelTransform(Vec p, double a, double scale) {

}

void resetTransform() {

}
void setCam(Vec p, double a) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glRotatef(-a * angularMultiplier, 0,0,1);
	glScalef(.05,.05,.05);
	glTranslatef(-p.x, -p.y, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void camTransform() {

}

void drawShip(Vec p, double a) {
	glPushMatrix();
	glTranslatef(p.x, p.y, 0);
	glRotatef(a * angularMultiplier, 0,0,1);
	glBegin(GL_TRIANGLES);
	glColor3f(1,1,1);
	glVertex2f(0.f, 1.f);
	glVertex2f(.5f, -1.f);
	glVertex2f(-.5f, -1.f);
	glEnd();
	glPopMatrix();

}
void drawStar(Vec p) {

}



void drawArea(Vec p, double a, double r) {
	glPushMatrix();
	glTranslatef(p.x, p.y, 0);
	glScalef(r, r, r);
	glRotatef(a, 0,0,1);
	glBegin(GL_LINE_LOOP);
	glColor3f(.8,.8, 1);
	glVertex2f(-1, -1);
	glVertex2f(-1, 1);
	glVertex2f(1, 1);
	glVertex2f(1, -1);
	glEnd();
	glPopMatrix();
}


void drawComet(Vec p, double a, double r) {
	glPushMatrix();
	glTranslatef(p.x, p.y, 0);
	glScalef(r, r, r);
	glRotatef(a, 0,0,1);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(.8,.8, 1);
	glVertex2f(-1, -1);
	glVertex2f(-1, 1);
	glVertex2f(1, 1);
	glVertex2f(1, -1);
	glEnd();
	glPopMatrix();
}
void drawProjectile(Vec p, double a, double scale) {

}
void drawExplosion(Vec p, double size) {
	glPushMatrix();
	glTranslatef(p.x, p.y, 0);
	glScalef(size, size, size);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(.8,.8, 1);
	glVertex2f(-1, -1);
	glVertex2f(-1, 1);
	glVertex2f(1, 1);
	glVertex2f(1, -1);
	glEnd();
	glPopMatrix();

}
void drawSmoke(Vec p1, Vec p2, double alpha1, double alpha2){
	glBegin(GL_LINES);
	glColor4f(1,1,1, alpha2);
	glVertex3dv(&p1.x);
	glVertex3dv(&p2.x);
	glEnd();
}
bool initDrawModule(double perspective){

}
void flushDraw(){

}


