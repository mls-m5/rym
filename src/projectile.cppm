module;

#include <cmath>

export module projectile;

import unit;
import draw;
import vec;
import obj;
import explosion;

namespace game {

export class Projectile : public Unit // En projektil
{
public:
    // Vector pos, vel;
    double ang, rot, varand;

    Projectile(Vec p, Vec v) : ang(-atan2(v.x, v.y)), rot(0), varand(60) {
        pos = p;
        vel = v;
    }

    ~Projectile() override = default;

    void update(double t) override {
        if (dead()) {
            return;
        }
        pos += vel;
        ang += rot;
        varand -= t;

        Unit *e;

        addSmoke(pos - vel, pos);

        e = game::collision(pos, this);
        if (e) {
            e->Force(vel * .01);

            e->Damage(.4);

            add(new Explosion(pos, .5));
            _dead = true;
        }
        else if (varand < 0) {
            _dead = true;
        }
        else if ((e = Near(pos, 20, this))) {
            Vec v;
            v = pos - e->pos;
            v = v / -(v * v);
            v = v * .05;
            vel += v;
        }
    }

    void render() override {
        drawProjectile(pos, ang, .1);
    }
};

} // namespace game
