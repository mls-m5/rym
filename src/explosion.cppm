
export module explosion;

import draw;
import unit;
import vec;
import particle;
import obj;
import <memory>;

namespace game {

export class Explosion : public Unit // En explosion
{
public:
    // Vector pos;
    double stlk;

    Explosion(Vec p, double s) : Unit(p), stlk(s) {
        for (int i = 1; i < 20; i++)
            add(std::make_unique<Particle>(pos));
    }
    void update(double t) override {
        stlk /= (1 + t);
        if (stlk < .01) {
            _dead = true;
        }
    }
    void render() override {
        drawExplosion(pos, stlk);
    }
};

} // namespace game
