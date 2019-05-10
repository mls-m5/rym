/*
 * broadphase.cpp
 *
 *  Created on: 3 maj 2015
 *      Author: Mattias Larsson Sköld
 */

#include "roamingbroadphase.h"
#include "draw.h"
//#include "unit.h"
#include "graf.h"
#include <limits>
#include <cstdlib>
#include <algorithm>

constexpr bool debugView = false;

using namespace game::obj;

RoamingBroadphase::RoamingBroadphase() {
	for (int i = 0; i < 20; ++i) {
		_parts.push_back(new PartSpace);
	}
}

RoamingBroadphase::~RoamingBroadphase() {
}

void PartSpace::calculateCenter() {
	Vec center;
	if (_units.empty()) {
		return;
	}
	int count = 0;
	for (auto unit: _units) {
		center += unit->pos;
		++count;
	}

	center /= count;

	_pos = _pos * .9 + center * .1; //Move slowly to the new center
//	_pos = center;
}

void PartSpace::calculateRadius() {
	auto max = -.1;
	double objectRadius = 0;
	for (auto it : _units) {
		auto d = it->pos.distance2d2(_pos);
		if (d > max) {
			max = d;
			auto r = it->radius();
			if (r > objectRadius) {
				objectRadius = r; //We add this later to avoid sqrt-ing it
			}
		}
	}
	_radius = sqrt(max) + objectRadius;
}

Unit* RoamingBroadphase::getNearest(Vec &p, double limit, Unit *ignore) {
	auto nearestDistance = limit * limit;
	Unit *unit = nullptr;

	for (auto it: _parts) {
		double t;
		auto tu = it->getNearest(p, limit, ignore, &t);
		if (t < nearestDistance) {
			nearestDistance = t;
			unit = tu;
		}
	}

	return unit;
}

void PartSpace::draw() {
	if (debugView and UseRoamingBroadphase) {
		drawArea(_pos, 0, _radius);
	}
}

Unit* PartSpace::getNearest(Vec &p, double limit, Unit *ignore, double *returnDistance) {
	auto nearestDistance = limit * limit;

	auto distanceToCenter = _pos.distance2d2(p);
	auto maxDistance = limit + _radius;

	//Check if any unit could possibly be near enough
	if (distanceToCenter > maxDistance * maxDistance) {
		*returnDistance = nearestDistance;
		return nullptr;
	}

	Unit *unit = nullptr;
	for (auto it: _units) {
		if (it == ignore) {
			continue;
		}
		auto t = it->pos.distance2d2(p);
		if (t < nearestDistance) {
			nearestDistance = t;
			unit = it;
		}
	}
	*returnDistance = nearestDistance;

	return unit;
}

PartSpace* RoamingBroadphase::getNearestPart(Vec& p) {
	PartSpace *nearest = nullptr;
	double distance2 = std::numeric_limits<double>::max();

	for (auto it: _parts) {
		auto t = it->pos().distance2d2(p);
		if (t < distance2) {
			nearest = it;
			distance2 = t;
		}
	}

	return nearest;
}


void RoamingBroadphase::removeDead() {
	_units.erase(std::remove_if(_units.begin(), _units.end(), [](Unit * u) -> bool {
		if (u->dead) {
			if (auto space = u->space()) {
				space->remove(u);
			}
			delete u;
		}
		return u->dead == true;
	}), _units.end());
}

void RoamingBroadphase::draw() {
	for (auto it: *this) {
		it->render();
	}

	for (auto it: _parts) {
		it->draw();
	}
}

void RoamingBroadphase::update(double t) {
	for (auto it: _parts) {
		if (it->empty()) {
			//Move to a place with more objects
			if (auto largestSpace = getLargestSpace(it->pos())) {
				auto newPos = largestSpace->pos();
				newPos.x += rand() %2 - 1;
				newPos.y += rand() %2 - 1;
				it->pos(newPos);
			}
		}
		else {
			it->calculateCenter();
		}

		if (it->distanceToPoint(_center) > 100) {
			it->removeAll();
		}

		it->calculateRadius();
	}

	for (auto it: *this) {
		it->update(t);

		if (rand() % 100 > 90) { //Limit this expensive calculation
			auto part = getNearestPart(it->pos);
			if (it->space() != part) {
				it->space()->remove(it);
				part->add(it);
			}
		}
	}
}

PartSpace::PartSpace():
	_pos(rand() % 100 - 50, rand() % 100 - 50){

}

void RoamingBroadphase::add(Unit* u) {
	_units.push_back(u);

	auto part = getNearestPart(u->pos);
	part->add(u);
}

void PartSpace::add(Unit* u) {
	_units.push_back(u);
	u->space(this);
}

Unit* RoamingBroadphase::collision(Vec& p, Unit* ignore) {
	for (auto it: _parts) {
		auto tu = it->collision(p, ignore);
		if (tu) {
			return tu;
		}
	}

	return nullptr;
}

Unit* PartSpace::collision(Vec& p, Unit* ignore) {
	auto distanceToCenter = _pos.distance2d2(p);
	if (distanceToCenter <= _radius * _radius) {
		for (auto it: _units) {
			if (it == ignore) {
				continue;
			}
			if (it->Koll(p)) {
				return it;
			}
		}
	}
	return nullptr;
}

void RoamingBroadphase::setCenter(Vec center) {
	_center = center;
}

PartSpace* RoamingBroadphase::getLargestSpace(Vec p) {
	int max = 0;
	PartSpace *largestSpace = nullptr;

	for (auto it: _parts) {
		auto s = it->size();
		if (s > max) {
			max = s;
			largestSpace = it;
		}
	}

	return largestSpace;
}

double PartSpace::distanceToPoint(Vec p) {
	auto distance = _pos.distance2d(p);
	return distance - _radius;
}

void PartSpace::removeAll() {
	for (auto it: _units) {
		it->dead = true;
	}
}
