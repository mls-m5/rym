/*
 * broadphase.h
 *
 *  Created on: 3 maj 2015
 *      Author: Mattias Larsson Sköld
 */


#pragma once
#include "vec.h"

#include "graf.h"
using game::obj::Unit;

class BroadPhase {
public:
	virtual ~BroadPhase() {}
	virtual class Unit *getNearest(Vec &p, double limit, Unit *ignore) = 0;
	virtual class Unit *collision(Vec &p, Unit *ignore) = 0;
	virtual void add(Unit* u) = 0;
	virtual void update(double t) = 0;
	virtual void remove(Unit *u) = 0;
	virtual void draw() = 0;
	virtual void setCenter(Vec center) = 0;
};

