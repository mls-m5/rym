
#include <list>
#include <memory>
#include <vector>

import unit;
import vec;
import roamingbroadphase;
import linesmoke;
import common;

export module obj;

using std::unique_ptr;
using std::vector;

namespace game {
namespace obj {

static vector<unique_ptr<Unit>> enh;
static vector<LineSmoke> smoke;
static RoamingBroadphase solids; // enheter som kan kollisionstestas

//_________________________objekt --- -... .--- . -.- - ________________

export void add(Unit *e) { // Lägger till objektet i listan
    if (e->isSolid()) {
        solids.add(e);
    }
    else {
        enh.emplace_back(e);
    }
}

export void addSmoke(Vec p1, Vec p2) { // lägger till ett rökobjekt
    game::obj::smoke.emplace_back(p1, p2);
}

export void flushRem() { // Tar bort döda objekt

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

export Unit *collision(Vec p,
                       Unit *ign) { // ign står för det objektet som skall
                                    // ignoreras

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

export Unit *Near(Vec p,
                  double limit,
                  Unit *ignore) { // Hittar det närmaste objektet
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

export void update(double t) {
    solids.update(t);

    for (auto &enhet : enh) {
        if (enhet) {
            enhet->update(t);
        }
    }

    for (auto &smoke : obj::smoke) {
        smoke.update(t);
    }

    obj::flushRem();
}

export void render() {

    obj::solids.draw();

    for (auto &it : obj::enh) {
        it->render();
    }
    for (auto &s : obj::smoke) {
        s.render();
    }
}

export void setCenter(Vec p) {
    solids.setCenter(p);
}

} // namespace obj
} // namespace game
