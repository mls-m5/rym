
#include "graf.h"
#include "draw.h"
#include "roamingbroadphase.h"
#include <algorithm>
#include <cstdlib>
#include <list>
#include <memory>
#include <vector>

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

void game::obj::Ship::update(double t) {
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
            obj::add(
                new Projectile(pos, vel + Vec(-sin(ang) / 4, cos(ang) / 4)));
            skott = .3;
        }
    }

    pos += vel;
    ang += rot;

    eye::move(pos + Vec(-sin(ang) * 10, cos(ang) * 10), ang);
}

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

void game::obj::Star::update(double /*t*/) {
}

void game::obj::Star::render() {
    drawStar(pos);
}

game::obj::Star::Star() {
    pos = Vec(rand() % 1000 - 500, rand() % 1000 - 500, -rand() % 100 / 10.);
}

//________________________komet______________________________________

void game::obj::Comet::update(double /*t*/) {
    pos += vel;
    ang += rot;
}

void game::obj::Comet::render() {
    drawComet(pos, ang, rad);
}

bool game::obj::Comet::collision(Vec &p) {
    auto dx = pos.x - p.x;
    auto dy = pos.y - p.y;
    return ((dx * dx + dy * dy) < rad * rad);
}

double game::obj::Comet::distance(Vec &p) {
    double dx, dy;
    dx = pos.x - p.x;
    dy = pos.y - p.y;

    return sqrt(dx * dx + dy * dy);
}

void game::obj::Comet::Force(Vec f) {
    Vec fo = f * .1;
    vel += fo;
}

bool game::obj::Comet::Damage(double d) {
    if (dead()) {
        return true;
    }
    liv -= d;
    if (liv <= 0) {
        dead(true);
    }
    return dead();
}

game::obj::Comet::Comet() {
    // static double x = 0;
    // pos = Vec(++x,x);
    pos = Vec(rand() % 1000 / 10 - 50, rand() % 100 - 50);
    vel = Vec(0, 0);
    ang = 0;
    rot = rand() % 1000 / 1000. - .5;
    rad = 1.5;
    liv = rad;
}

game::obj::Comet::Comet(Vec p, Vec v, double r) {
    pos = p;
    vel = v;
    ang = 0;
    rot = rand() % 1000 / 1000. - .5;
    rad = r;
    liv = rad;
}

game::obj::Comet::~Comet() {
    if (rad > .2) {
        for (int i = 0; i < 3; i++) {
            add(new Comet(
                pos,
                vel + Vec((rand() % 11 - 5) / 100., (rand() % 11 - 5) / 100.),
                rad / 1.5));
        }
    }
}

//_______________projekt_______________________________________________

void game::obj::Projectile::update(double t) {
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

void game::obj::Projectile::render() {
    drawProjectile(pos, ang, .1);
}

game::obj::Projectile::Projectile(Vec p, Vec v)
    : ang(-atan2(v.x, v.y)), rot(0), varand(60) {
    pos = p;
    vel = v;
}

game::obj::Projectile::~Projectile() = default;

//____________________________Explosion_________________________________________

void game::obj::Explosion::update(double t) {
    stlk /= (1 + t);
    if (stlk < .01) {
        _dead = true;
    }
}

void game::obj::Explosion::render() {
    drawExplosion(pos, stlk);
}

game::obj::Explosion::Explosion(Vec p, double s) {
    pos = p;
    stlk = s;
    for (int i = 1; i < 20; i++)
        add(new Particle(pos));
}

//________________________________Partikel______________________________________

void game::obj::Particle::update(double t) {
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

void game::obj::Particle::render() {
    if (!_dead) {
        drawSmoke(start, pos, alpha1, alpha2);
    }
}

game::obj::Particle::Particle(Vec p) {
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

game::obj::Particle::Particle(Vec p, Vec v)
    : duration(2), maxDuration(duration), alpha1(1), alpha2(1) {
    pos = p;
    vel = v;
}

//___________________Rök på linje_______________________________________________

void game::obj::addSmoke(Vec p1, Vec p2) {
    game::obj::smoke.emplace_back(p1, p2);
}

void game::obj::LineSmoke::update(double t) {
    duration -= t;
}

void game::obj::LineSmoke::render() {
    drawSmoke(pos1, pos2, duration / maxDuration, duration / maxDuration);
}

game::obj::LineSmoke::LineSmoke(Vec p1, Vec p2) {
    pos1 = p1;
    pos2 = p2;
    duration = 12;
    maxDuration = duration;
}
