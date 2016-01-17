/*
 * broadphase.h
 *
 *  Created on: 3 maj 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once
#include "broadphase.h"
#include <list>
#include "draw.h"


class Space {
public:
	virtual ~Space() {}

	virtual void add(Unit *u) = 0;
	virtual void remove(Unit *u) = 0;

	virtual Vec &pos() = 0;
};

class PartSpace : public Space {
public:
	PartSpace();
	virtual ~PartSpace() {}

	void add(Unit* u) override;

	void remove(Unit *u) {
		_units.remove(u);
	}

	void calculateCenter();
	void calculateRadius();
	double distanceToPoint(Vec p);

	void removeAll();

	void draw();

	Vec &pos() override {
		return _pos;
	}

	void pos(Vec v) {
		_pos = v;
	}

	bool empty() {
		return _units.empty();
	}

	auto size() {
		return _units.size();
	}

	Unit* getNearest(Vec&, double limit, Unit *ignore, double *returnDistance);
	Unit *collision(Vec &p, Unit *ignore);

protected:
	std::list <Unit*> _units;
	Vec _pos;
	double _radius = 1;
};

class RoamingBroadphase : public BroadPhase {
public:
	RoamingBroadphase();
	virtual ~RoamingBroadphase();
	
	Unit* getNearest(Vec &p, double limit, Unit *ignore) override;
	Unit *collision(Vec &p, Unit *ignore) override;

	PartSpace *getNearestPart(Vec &p);
	PartSpace *getLargestSpace(Vec p);

	void add(Unit* u) override;

	void update(double t) override;
	void remove(Unit *u) override;


	//to get for-each to work
	auto inline begin() { //using c++14 auto-standard
		return _units.begin();
	}
	auto inline end() {
		return _units.end();
	}

	void draw() override;
	void setCenter(Vec center) override;

	std::list <Unit *> _units;
	std::list <PartSpace *> _parts;
	Vec _center;
};
