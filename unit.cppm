
import vec;

export module unit;

namespace game {
namespace obj {

export class Unit {
public:
    Vec pos, vel;
    Unit(Vec pos = {}, Vec vel = {}) : pos(pos), vel(vel) {
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

    void *space() {
        return _space;
    }

    void space(void *space) {
        _space = space;
    }

    bool dead() const {
        return _dead;
    }

    void dead(bool value) {
        _dead = value;
    }

protected:
    //! Ugly i know, but i dont know how to handle this with modules
    void *_space = nullptr;
    bool _dead = false;
};
} // namespace obj
} // namespace game
