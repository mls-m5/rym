/*
 * broadphase.cpp
 *
 *  Created on: 3 maj 2015
 *      Author: Mattias Larsson Sköld
 */

#import <algorithm>
#import <cmath>
#import <cstdlib>
#import <limits>
#import <list>

import broadphase;
import vec;
import unit;
import draw;
import common;

export module roamingbroadphase;

using namespace game::obj;

class Space {
public:
    virtual ~Space() = default;

    virtual void add(Unit *u) = 0;
    virtual void remove(Unit *u) = 0;

    virtual Vec &pos() = 0;
};

class PartSpace : public Space {
public:
    PartSpace();
    ~PartSpace() override;

    void add(Unit *u) override;

    void remove(Unit *u) override {
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

    Unit *getNearest(Vec &, double limit, Unit *ignore, double *returnDistance);
    Unit *collision(Vec &p, Unit *ignore);

protected:
    std::list<Unit *> _units;
    Vec _pos;
    double _radius = 1;
};

export class RoamingBroadphase : public BroadPhase {
public:
    RoamingBroadphase();
    ~RoamingBroadphase() override;

    Unit *getNearest(Vec &p, double limit, Unit *ignore) override;
    Unit *collision(Vec &p, Unit *ignore) override;

    PartSpace *getNearestPart(Vec &p);
    PartSpace *getLargestSpace(Vec p);

    void add(Unit *u) override;

    void update(double t) override;
    void removeDead() override;

    // to get for-each to work
    auto inline begin() { // using c++14 auto-standard
        return _units.begin();
    }
    auto inline end() {
        return _units.end();
    }

    void draw() override;
    void setCenter(Vec center) override;

private:
    std::list<Unit *> _units;
    std::list<PartSpace *> _parts;
    Vec _center;
};

// Source
constexpr bool debugView = false;

using game::obj::Unit;

RoamingBroadphase::RoamingBroadphase() {
    for (int i = 0; i < 20; ++i) {
        _parts.push_back(new PartSpace);
    }
}

RoamingBroadphase::~RoamingBroadphase() {
}

PartSpace::~PartSpace() {
}

void PartSpace::calculateCenter() {
    Vec center;
    if (_units.empty()) {
        return;
    }
    int count = 0;
    for (auto unit : _units) {
        center += unit->pos;
        ++count;
    }

    center /= count;

    _pos = _pos * .9 + center * .1; // Move slowly to the new center
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
                objectRadius = r; // We add this later to avoid sqrt-ing it
            }
        }
    }
    _radius = sqrt(max) + objectRadius;
}

Unit *RoamingBroadphase::getNearest(Vec &p, double limit, Unit *ignore) {
    auto nearestDistance = limit * limit;
    Unit *unit = nullptr;

    for (auto it : _parts) {
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

Unit *PartSpace::getNearest(Vec &p,
                            double limit,
                            Unit *ignore,
                            double *returnDistance) {
    auto nearestDistance = limit * limit;

    auto distanceToCenter = _pos.distance2d2(p);
    auto maxDistance = limit + _radius;

    // Check if any unit could possibly be near enough
    if (distanceToCenter > maxDistance * maxDistance) {
        *returnDistance = nearestDistance;
        return nullptr;
    }

    Unit *unit = nullptr;
    for (auto it : _units) {
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

PartSpace *RoamingBroadphase::getNearestPart(Vec &p) {
    PartSpace *nearest = nullptr;
    double distance2 = std::numeric_limits<double>::max();

    for (auto it : _parts) {
        auto t = it->pos().distance2d2(p);
        if (t < distance2) {
            nearest = it;
            distance2 = t;
        }
    }

    return nearest;
}

void RoamingBroadphase::removeDead() {
    _units.erase(std::remove_if(_units.begin(),
                                _units.end(),
                                [](Unit *u) -> bool {
                                    if (u->dead()) {
                                        if (auto space = static_cast<Space *>(
                                                u->space())) {
                                            //                                            auto s =
                                            //                                                static_cast<Space *>(space);
                                            space->remove(u);
                                        }
                                        delete u;
                                    }
                                    return u->dead();
                                }),
                 _units.end());
}

void RoamingBroadphase::draw() {
    for (auto it : *this) {
        it->render();
    }

    for (auto it : _parts) {
        it->draw();
    }
}

void RoamingBroadphase::update(double t) {
    for (auto it : _parts) {
        if (it->empty()) {
            // Move to a place with more objects
            if (auto largestSpace = getLargestSpace(it->pos())) {
                auto newPos = largestSpace->pos();
                newPos.x += rand() % 2 - 1;
                newPos.y += rand() % 2 - 1;
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

    for (auto it : *this) {
        it->update(t);

        if (rand() % 100 > 90) { // Limit this expensive calculation
            auto part = getNearestPart(it->pos);
            if (it->space() != part) {
                auto space = static_cast<Space *>(it->space());
                space->remove(it);
                part->add(it);
            }
        }
    }
}

PartSpace::PartSpace() : _pos(rand() % 100 - 50, rand() % 100 - 50) {
}

void RoamingBroadphase::add(Unit *u) {
    _units.push_back(u);

    auto part = getNearestPart(u->pos);
    part->add(u);
}

void PartSpace::add(Unit *u) {
    _units.push_back(u);
    u->space(this);
}

Unit *RoamingBroadphase::collision(Vec &p, Unit *ignore) {
    for (auto it : _parts) {
        auto tu = it->collision(p, ignore);
        if (tu) {
            return tu;
        }
    }

    return nullptr;
}

Unit *PartSpace::collision(Vec &p, Unit *ignore) {
    auto distanceToCenter = _pos.distance2d2(p);
    if (distanceToCenter <= _radius * _radius) {
        for (auto it : _units) {
            if (it == ignore) {
                continue;
            }
            if (it->collision(p)) {
                return it;
            }
        }
    }
    return nullptr;
}

void RoamingBroadphase::setCenter(Vec center) {
    _center = center;
}

PartSpace *RoamingBroadphase::getLargestSpace(Vec /*p*/) {
    size_t max = 0;
    PartSpace *largestSpace = nullptr;

    for (auto it : _parts) {
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
    for (auto it : _units) {
        it->dead(true);
    }
}
