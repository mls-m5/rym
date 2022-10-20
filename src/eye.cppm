

export module eye;

import vec;
import draw;
import obj;

namespace game {
namespace eye {

static Vec pos;
static double ang;

export void move(Vec v, double a) {
    pos = v;
    ang = a;
    setCam(v, a);
    game::obj::setCenter(v);
}

export void transform() {
    camTransform();
}

} // namespace eye
} // namespace game
