module;

export module ship;

import <cmath>;
import <memory>;
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
        auto clampInput = [](double value) {
            return value < -1 ? -1. : value > 1 ? 1. : value;
        };

        const double keyboardSteering =
            (hant::getKey(cn_right) ? 1. : 0.) -
            (hant::getKey(cn_left) ? 1. : 0.);
        const double keyboardThrust =
            (hant::getKey(cn_up) ? 1. : 0.) -
            (hant::getKey(cn_down) ? 1. : 0.);
        const double steering =
            clampInput(keyboardSteering + hant::getAnalogSteering());
        const double thrust =
            clampInput(keyboardThrust + hant::getAnalogThrust());

        rot /= 1.2;
        vel.x *= .9;
        vel.y *= .9;
        rot -= steering * .01;
        if (thrust != 0) {
            vel = Vec(-sin(ang) * .1 * thrust, cos(ang) * .1 * thrust);
        }
        if (skott > 0) {
            skott = skott - t;
        }
        else {
            if (hant::getKey(cn_eld)) {
                add(std::make_unique<Projectile>(
                    pos, vel + Vec(-sin(ang) / 4, cos(ang) / 4)));
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
