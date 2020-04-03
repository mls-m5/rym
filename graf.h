#ifndef graf_h
#define graf_h

//#include "vektorer.h"
#include "common.h"
#include "hant.h"
#include "vec.h"
#include <cmath>
#include <list>

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

constexpr bool UseRoamingBroadphase = true;

namespace kont {
int get(controlnum kontn);
}

class Space;

namespace game {
void Update(double t);
void Render();
void init();
void avsl();

namespace eye {
void move(Vec v, double a);
void transform();
} // namespace eye

namespace obj {
class Unit;

void add(Unit *e);               // Lägger till objektet i listan
void addSmoke(Vec p1, Vec p2);   // lägger till ett rökobjekt
void flushRem();                 // Tar bort döda objekt
Unit *collision(Vec, Unit *ign); // ign står för det objektet som skall
                                 // ignoreras
Unit *Near(Vec p, double lim, Unit *ign); // Hittar det närmaste objektet

class Unit {
public:
    Vec pos, vel;
    Unit() {
    }
    virtual ~Unit() = default;

    virtual void update(double /*t*/) {
    }
    virtual void render() {
    }
    virtual bool collision(Vec & /*p*/) {
        return 0;
    } // kollisionstestning
    virtual double distance(Vec & /*p*/) {
        return 0;
    } // Hitta avståndet
    virtual void Force(Vec /*f*/) {
    }
    virtual bool Damage(double /*d*/) {
        return 0;
    } // Sant om enheten går sönder
    virtual bool isSolid() {
        return 0;
    }

    virtual double radius() {
        return 0;
    }

    class Space *space() {
        return _space;
    }

    void space(class Space *space) {
        _space = space;
    }

    bool dead() const {
        return _dead;
    }

    void dead(bool value) {
        _dead = value;
    }

protected:
    class Space *_space = nullptr;
    bool _dead = false;
};

class Ship : public Unit {
    // Vector pos, vel;
    double ang, rot;

    double skott;

public:
    Ship();
    void update(double t) override;
    void render() override;
};

class Star : public Unit // Stjärna
{
public:
    // Vector pos;

    Star();
    void update(double t) override;
    void render() override;
};

class Comet : public Unit // komet
{
public:
    // Vector pos, vel;
    double ang, rot;
    double rad; // kometens radie
    double liv;

    Comet();
    Comet(Vec p, Vec v, double r);
    ~Comet() override;
    void update(double t) override;
    void render() override;
    bool collision(Vec &p) override;
    double distance(Vec &p) override;
    void Force(Vec f) override;
    bool Damage(double d) override;
    bool isSolid() override {
        return true;
    }
    double radius() override {
        return rad;
    }
};

class Projectile : public Unit // En projektil
{
public:
    // Vector pos, vel;
    double ang, rot, varand;

    Projectile(Vec p, Vec v);
    ~Projectile() override;
    void update(double t) override;
    void render() override;
};

class Explosion : public Unit // En explosion
{
public:
    // Vector pos;
    double stlk;

    Explosion(Vec p, double stlk);
    void update(double t) override;
    void render() override;
};

class Particle : public Unit // skräp som mest flyger omkring
{
public:
    // Vector pos, vel;
    Vec start;
    double duration, maxDuration;
    double alpha1, alpha2;

    Particle(Vec p);
    Particle(Vec p, Vec v);
    void update(double t) override;
    void render() override;
};

class LineSmoke // Rök som ser ut som en linje
{
public:
    Vec pos1, pos2;
    double duration, maxDuration;

    LineSmoke(Vec p1, Vec p2);
    void update(double t);
    void render();
    bool isDead() {
        return duration < 0;
    }
};

} // namespace obj
} // namespace game

#endif
