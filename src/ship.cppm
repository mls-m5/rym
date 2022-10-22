module;

#include <cmath>

export module ship;

import vec;
import unit;
import draw;
import hant;
import projectile;
import obj;
import eye;

namespace game {

export class Ship : public Unit {
    // Vector pos, vel;
    double ang, rot;

    double skott;

public:
    Ship() {
        pos = Vec(0, 0, 0);
        vel = Vec(0, 0, 0);
        ang = 0;
        rot = 0;
        skott = 0;
    }
    void update(double t) override {
        rot /= 1.2;
        vel.x *= .9;
        vel.y *= .9;
        if (hant::getKey(cn_left))
            rot += .01;
        if (hant::getKey(cn_right))
            rot += -.01;
        if (hant::getKey(cn_up))
            vel = Vec(-sin(ang) * .1, cos(ang) * .1);
        if (skott > 0) {
            skott = skott - t;
        }
        else {
            if (hant::getKey(cn_eld)) {
                add(new Projectile(pos,
                                   vel + Vec(-sin(ang) / 4, cos(ang) / 4)));
                skott = .3;
            }
        }

        pos += vel;
        ang += rot;

        eye::move(pos + Vec(-sin(ang) * 10, cos(ang) * 10), ang);
    }
    void render() override {
        drawShip(pos, ang);
    }
};

} // namespace game
