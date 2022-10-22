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
    update(t);
}

void Render() {
    camTransform();

    render();

    flushDraw();
}

void init() {
    for (int i = 1; i < 500; i++) {
        add(new Star);
    }

    for (int i = 0; i < 50; i++) {
        add(new Comet);
    }

    add(new Ship);
}

void avsl() {
    flushRem();
}

} // namespace game
