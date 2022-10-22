module;

#include <cmath>

export module particle;

import draw;
import vec;
import unit;
import obj;

namespace game {

export class Particle : public Unit // skräp som mest flyger omkring
{
public:
    // Vector pos, vel;
    Vec start;
    double duration, maxDuration;
    double alpha1, alpha2;

    Particle(Vec p) {
        pos = p;
        start = p;
        double vx = 0;
        double vy = 0;
        double vz = 0;
        alpha1 = alpha2 = 1;
        for (int i = 0; i < 6; i++) {
            vx += rand() % 1000 - 500;
            vy += rand() % 1000 - 500;
            vz += rand() % 1000 - 500;
        }
        vel.x = vx / 12000.;
        vel.y = vy / 12000.;
        vel.z = vz / 12000.;
        duration = 2 + rand() % 20 / 20;
        maxDuration = duration;
    }
    Particle(Vec p, Vec v)
        : duration(2), maxDuration(duration), alpha1(1), alpha2(1) {
        pos = p;
        vel = v;
    }
    void update(double t) override {
        if (_dead) {
            return;
        }
        constexpr double step = 1. / 80;
        alpha1 -= step;
        if (duration < 0) {
            alpha2 -= step;
            if (alpha2 <= 0) {
                _dead = true;
            }
        }
        else {
            duration -= t;
            pos += vel;
        }
    }
    void render() override {
        if (!_dead) {
            drawSmoke(start, pos, alpha1, alpha2);
        }
    }
};

} // namespace game
