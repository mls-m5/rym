

export module linesmoke;

import vec;
import draw;

namespace game {
namespace obj {

export class LineSmoke // Rök som ser ut som en linje
{
public:
    Vec pos1, pos2;
    double duration, maxDuration;

    LineSmoke(Vec p1, Vec p2) {
        pos1 = p1;
        pos2 = p2;
        duration = 12;
        maxDuration = duration;
    }
    void update(double t) {
        duration -= t;
    }
    void render() {
        drawSmoke(pos1, pos2, duration / maxDuration, duration / maxDuration);
    }
    bool isDead() {
        return duration < 0;
    }
};

} // namespace obj
} // namespace game
