module;

export module obj;

import <list>;
import <memory>;
import <vector>;
import unit;
import vec;
import roamingbroadphase;
import linesmoke;
import common;

using std::unique_ptr;
using std::vector;

namespace game {

static vector<unique_ptr<Unit>> enh;
static vector<LineSmoke> smoke;
static RoamingBroadphase solids; // enheter som kan kollisionstestas

//_________________________objekt --- -... .--- . -.- - ________________

export void add(std::unique_ptr<Unit> e) { // Lägger till objektet i listan
    if (e->isSolid()) {
        solids.add(std::move(e));
    }
    else {
        enh.push_back(std::move(e));
    }
}

export template <typename T>
void add() {
    add(std::make_unique<T>());
}

// Does not work with modules as of jan 2023
// export template <typename T, typename... Args>
// void add(Args &&...args) {
//     add(std::make_unique<T>(std::forward<Args...>(args...)));
// }

export void addSmoke(Vec p1, Vec p2) { // lägger till ett rökobjekt
    game::smoke.emplace_back(p1, p2);
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

    if (ign) // Är det något objekt som skall ignoreras?
    {
        for (auto &it : solids) {
            if (it->collision(p) && it.get() != ign)
                return it.get();
        }
    }
    else // inget objekt ignoreras
    {
        for (auto &it : solids) {
            if (it->collision(p))
                return it.get();
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

    for (auto &it : solids) {
        if (ignore != it.get()) // Om objektet inte skall ignoreras
        {
            dm = it->distance(p);
            if (dm < distance && dm > 0) {
                e = it.get();
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

    for (auto &smoke : smoke) {
        smoke.update(t);
    }

    flushRem();
}

export void render() {

    solids.draw();

    for (auto &it : enh) {
        it->render();
    }
    for (auto &s : smoke) {
        s.render();
    }
}

export void setCenter(Vec p) {
    solids.setCenter(p);
}

} // namespace game
