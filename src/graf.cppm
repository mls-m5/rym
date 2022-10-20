module;

export module graf;

import vec;
import draw;
import obj;
import star;
import comet;
import ship;
import eye;

class Space;

export namespace game {

void Update(double t) {
    obj::update(t);
}

void Render() {
    camTransform();

    obj::render();

    flushDraw();
}

void init() {
    for (int i = 1; i < 500; i++) {
        obj::add(new obj::Star);
    }

    for (int i = 0; i < 50; i++) {
        obj::add(new obj::Comet);
    }

    obj::add(new obj::Ship);
}

void avsl() {
    obj::flushRem();
}

} // namespace game
