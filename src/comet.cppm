module;

#include <cmath>

export module comet;

import unit;
import vec;
import draw;
import obj;

namespace game {
namespace obj {

export class Comet : public Unit // komet
{
public:
    // Vector pos, vel;
    double ang, rot;
    double rad; // kometens radie
    double liv;

    Comet() {
        // static double x = 0;
        // pos = Vec(++x,x);
        pos = Vec(rand() % 1000 / 10 - 50, rand() % 100 - 50);
        vel = Vec(0, 0);
        ang = 0;
        rot = rand() % 1000 / 1000. - .5;
        rad = 1.5;
        liv = rad;
    }

    Comet(Vec p, Vec v, double r) {
        pos = p;
        vel = v;
        ang = 0;
        rot = rand() % 1000 / 1000. - .5;
        rad = r;
        liv = rad;
    }

    ~Comet() override {
        if (rad > .2) {
            for (int i = 0; i < 3; i++) {
                add(new Comet(pos,
                              vel + Vec((rand() % 11 - 5) / 100.,
                                        (rand() % 11 - 5) / 100.),
                              rad / 1.5));
            }
        }
    }

    void update(double t) override {
        pos += vel;
        ang += rot;
    }

    void render() override {
        drawComet(pos, ang, rad);
    }

    bool collision(Vec &p) override {
        auto dx = pos.x - p.x;
        auto dy = pos.y - p.y;
        return ((dx * dx + dy * dy) < rad * rad);
    }

    double distance(Vec &p) override {
        double dx, dy;
        dx = pos.x - p.x;
        dy = pos.y - p.y;

        return sqrt(dx * dx + dy * dy);
    }

    void Force(Vec f) override {
        Vec fo = f * .1;
        vel += fo;
    }

    bool Damage(double d) override {
        if (dead()) {
            return true;
        }
        liv -= d;
        if (liv <= 0) {
            dead(true);
        }
        return dead();
    }

    bool isSolid() override {
        return true;
    }

    double radius() override {
        return rad;
    }
};

} // namespace obj
} // namespace game
