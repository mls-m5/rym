
#import <cmath>

import draw;
import vec;
import unit;

export module star;

namespace game {
namespace obj {

export class Star : public Unit // Stjärna
{
public:
    // Vector pos;

    Star() {
        pos =
            Vec(rand() % 1000 - 500, rand() % 1000 - 500, -rand() % 100 / 10.);
    }

    void update(double t) override {
    }

    void render() override {
        drawStar(pos);
    }
};

} // namespace obj
} // namespace game
