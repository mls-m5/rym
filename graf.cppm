

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <list>
#include <memory>
#include <vector>

import unit;
import vec;
import common;
import hant;
import roamingbroadphase;
import draw;

#if defined(WIN32) || defined(_WIN32) ||                                       \
    defined(__WIN32) && !defined(__CYGWIN__)
#include "windows.h"
enum controlnum {
    cn_up = VK_UP,
    cn_down = VK_DOWN,
    cn_left = VK_LEFT,
    cn_right = VK_RIGHT,
    cn_eld = VK_SPACE
};
#elif defined(__ANDROID__) || defined(__APPLE__)
enum controlnum {
    cn_up = 1,
    cn_down = 2,
    cn_left = 3,
    cn_right = 4,
    cn_eld = 5
};
#else
#include <SDL2/SDL.h>
enum controlnum {
    cn_up = SDLK_UP,
    cn_down = SDLK_DOWN,
    cn_left = SDLK_LEFT,
    cn_right = SDLK_RIGHT,
    cn_eld = SDLK_SPACE
};
#endif

export module graf;

namespace kont {
int get(controlnum kontn);
}

class Space;

export namespace game {
void Update(double t);
void Render();
void init();
void avsl();

namespace eye {
void move(Vec v, double a);
void transform();
} // namespace eye

namespace obj {
// class Unit;

void add(Unit *e);               // Lägger till objektet i listan
void addSmoke(Vec p1, Vec p2);   // lägger till ett rökobjekt
void flushRem();                 // Tar bort döda objekt
Unit *collision(Vec, Unit *ign); // ign står för det objektet som skall
                                 // ignoreras
Unit *Near(Vec p, double lim, Unit *ign); // Hittar det närmaste objektet

class Star : public Unit // Stjärna
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

class Comet : public Unit // komet
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

class Particle : public Unit // skräp som mest flyger omkring
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

class Explosion : public Unit // En explosion
{
public:
    // Vector pos;
    double stlk;

    Explosion(Vec p, double s) : Unit(p), stlk(s) {
        for (int i = 1; i < 20; i++)
            add(new Particle(pos));
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

class Projectile : public Unit // En projektil
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

        e = obj::collision(pos, this);
        if (e) {
            e->Force(vel * .01);

            e->Damage(.4);

            add(new Explosion(pos, .5));
            _dead = true;
        }
        else if (varand < 0) {
            _dead = true;
        }
        else if ((e = obj::Near(pos, 20, this))) {
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

class LineSmoke // Rök som ser ut som en linje
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

class Ship : public Unit {
    // Vector pos, vel;
    double ang, rot;

    double skott;

public:
    Ship();
    void update(double t) override {
        rot /= 1.2;
        vel.x *= .9;
        vel.y *= .9;
        if (kont::get(cn_left))
            rot += .01;
        if (kont::get(cn_right))
            rot += -.01;
        if (kont::get(cn_up))
            vel = Vec(-sin(ang) * .1, cos(ang) * .1);
        if (skott > 0) {
            skott = skott - t;
        }
        else {
            if (kont::get(cn_eld)) {
                obj::add(new Projectile(
                    pos, vel + Vec(-sin(ang) / 4, cos(ang) / 4)));
                skott = .3;
            }
        }

        pos += vel;
        ang += rot;

        eye::move(pos + Vec(-sin(ang) * 10, cos(ang) * 10), ang);
    }
    void render() override;
};

} // namespace obj
} // namespace game

// Source

using std::unique_ptr;
using std::vector;

namespace game {
namespace obj {

static vector<unique_ptr<Unit>> enh;
static vector<LineSmoke> smoke;
// static int cenh; //Det objektet som behandlas för tillfället
static RoamingBroadphase solids; // enheter som kan kollisionstestas

} // namespace obj
} // namespace game

//__________________kontroller________________________________________

int kont::get(controlnum kontn) {
    return hant::getKey(kontn);
}

//_______________________graf_________________________________________

void game::Update(double t) {
    obj::solids.update(t);

    for (auto &enhet : obj::enh) {
        if (enhet) {
            enhet->update(t);
        }
    }

    for (auto &smoke : obj::smoke) {
        smoke.update(t);
    }

    obj::flushRem();
}

void game::Render() {
    camTransform();

    obj::solids.draw();

    for (auto &it : obj::enh) {
        it->render();
    }
    for (auto &s : obj::smoke) {
        s.render();
    }
    flushDraw();
}

void game::init() {
    for (int i = 1; i < 500; i++) {
        obj::add(new obj::Star);
    }

    for (int i = 0; i < 50; i++) {
        obj::add(new obj::Comet);
    }

    obj::add(new obj::Ship);
}

void game::avsl() {
    obj::flushRem();
}

//________________________oega__________________________________________

namespace game {
namespace eye {

static Vec pos;
static double ang;

} // namespace eye
} // namespace game

void game::eye::move(Vec v, double a) {
    pos = v;
    ang = a;
    setCam(v, a);
    game::obj::solids.setCenter(v);
}

void game::eye::transform() {
    camTransform();
}

//_________________________objekt --- -... .--- . -.- - ________________

void game::obj::add(Unit *e) {
    if (e->isSolid()) {
        solids.add(e);
    }
    else {
        enh.emplace_back(e);
    }
}

void game::obj::flushRem() {
    solids.removeDead();
    enh.erase(remove_if(enh.begin(),
                        enh.end(),
                        [](unique_ptr<Unit> &u) { return u->dead(); }),
              enh.end());

    smoke.erase(remove_if(smoke.begin(),
                          smoke.end(),
                          [](LineSmoke &s) { return s.isDead(); }),
                smoke.end());
}

game::obj::Unit *game::obj::collision(Vec p, Unit *ign) {
    if (UseRoamingBroadphase) {
        return solids.collision(p, ign);
    }

    if (ign) //Är det något objekt som skall ignoreras?
    {
        for (auto it : solids) {
            if (it->collision(p) && it != ign)
                return it;
        }
    }
    else // inget objekt ignoreras
    {
        for (auto it : solids) {
            if (it->collision(p))
                return it;
        }
    }

    return nullptr;
}

game::obj::Unit *game::obj::Near(Vec p, double limit, Unit *ignore) {
    if (UseRoamingBroadphase) {
        return solids.getNearest(p, limit, ignore);
    }
    // Keeping old algorith for reference

    double distance = limit; // Det närmsta objektet som påträffats
    double dm;               // det senast mätta värdet
    Unit *e = nullptr;       // Det närmast mätta objektet

    for (auto it : solids) {
        if (ignore != it) // Om objektet inte skall ignoreras
        {
            dm = it->distance(p);
            if (dm < distance && dm > 0) {
                e = it;
                distance = dm;
            }
        }
    }
    return e; // Objektet returneras
}

//---------skepp-------------------------------------------------------

void game::obj::Ship::render() {
    drawShip(pos, ang);
}

game::obj::Ship::Ship() {
    pos = Vec(0, 0, 0);
    vel = Vec(0, 0, 0);
    ang = 0;
    rot = 0;
    skott = 0;
}

//_______________________star________________________________________

//________________________komet______________________________________

//_______________projekt_______________________________________________

void game::obj::addSmoke(Vec p1, Vec p2) {
    game::obj::smoke.emplace_back(p1, p2);
}
