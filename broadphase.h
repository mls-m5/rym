/*
 * broadphase.h
 *
 *  Created on: 3 maj 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include "graf.h"
#include "vec.h"

using game::obj::Unit;

class BroadPhase {
public:
    virtual ~BroadPhase() = default;
    virtual class Unit *getNearest(Vec &p, double limit, Unit *ignore) = 0;
    virtual class Unit *collision(Vec &p, Unit *ignore) = 0;
    virtual void add(Unit *u) = 0;
    virtual void update(double t) = 0;
    virtual void removeDead() = 0;
    virtual void draw() = 0;
    virtual void setCenter(Vec center) = 0;
};
