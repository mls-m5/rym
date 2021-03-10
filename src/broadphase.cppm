/*
 * broadphase.h
 *
 *  Created on: 3 maj 2015
 *      Author: Mattias Larsson Sköld
 */

import unit;
import vec;

export module broadphase;

export class BroadPhase {
public:
    using Unit = game::obj::Unit;
    virtual ~BroadPhase() = default;
    virtual Unit *getNearest(Vec &p, double limit, Unit *ignore) = 0;
    virtual Unit *collision(Vec &p, Unit *ignore) = 0;
    virtual void add(Unit *u) = 0;
    virtual void update(double t) = 0;
    virtual void removeDead() = 0;
    virtual void draw() = 0;
    virtual void setCenter(Vec center) = 0;
};
