#ifndef graf_h
#define graf_h

//#include "vektorer.h"
#include "vec.h"
#include "hant.h"
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <cmath>
#include <list>
#include "common.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include "windows.h"
enum controlnum
{
    cn_up = VK_UP,
    cn_down = VK_DOWN,
    cn_left = VK_LEFT,
    cn_right = VK_RIGHT,
    cn_eld =  VK_SPACE
};
#elif defined(__ANDROID__) || defined(__APPLE__)
enum controlnum
{
    cn_up = 1,
    cn_down = 2,
    cn_left = 3,
    cn_right = 4,
    cn_eld =  5
};
#else
#include <SDL/SDL.h>
enum controlnum
{
    cn_up = SDLK_UP,
    cn_down = SDLK_DOWN,
    cn_left = SDLK_LEFT,
    cn_right = SDLK_RIGHT,
    cn_eld =  SDLK_SPACE
};
#endif

constexpr bool UseRoamingBroadphase = true;

namespace kont
{
    int get(controlnum kontn);
}

class Space;

namespace game
{
    void Update(float t);
    void Render();
    void init();
    void avsl();
    
    namespace eye
    {
        void move(Vec v, float a);
        void transform();
    }

    namespace obj
    {
        class Unit;
    	typedef std::list<Unit *>::iterator enhet_list_iterator_t;
        
        enhet_list_iterator_t add(Unit *e);  //Lägger till objektet i listan
        void rem(Unit *e);  //Tar bort objektet ur listan
        void rem(enhet_list_iterator_t it); //Tar bort iterator ur listan (snabbare)
        void remd(Unit *e); //Tar bort objektet helt
        void remd(enhet_list_iterator_t it); //Tar bort iteratorns objekt helt (snabbare)
        void flushRem(); //Tar bort objekt som köats
        void flushDel(); //anropar del på köade objekt
        Unit *Koll(Vec, Unit *ign); //ign står för det objektet som skall ignoreras
        Unit *Near(Vec p, float lim, Unit *ign); //Hittar det närmaste objektet
        
        class Unit
        {
        public:
            Vec pos, vel;
            Unit() {};
            virtual ~Unit() {};
            
            virtual void update(float t) {};
            virtual void render() {};
            virtual bool Koll(Vec &p) {return 0;};  //kollisionstestning
            virtual float Distance(Vec &p) {return 0;};  //Hitta avståndet
            virtual void Force(Vec f) {};
            virtual bool Damage(float d) {return 0;}; //Sant om enheten går sönder
            virtual bool isSolid() {return 0;};

            virtual float radius() {return 0;}
            void setIterator(enhet_list_iterator_t);

            class Space *space() {
            	return _space;
            }

            void space (class Space *space) {
            	_space = space;
            }

        protected:
            bool hasIterator = false;
            enhet_list_iterator_t iterator; //En iterator för att snabbare kunna ta bort röken
            class Space *_space = nullptr;
        };
        
        class Ship: public Unit
        {
        public:
            //Vector pos, vel;
            float ang, rot;
            float skott;
            
            Ship();
            void update(float t) override;
            void render() override;
        };
        
        class Star: public Unit  //Stj�rna
        {
        public:
            //Vector pos;

            Star();
            void update(float t) override;
            void render() override;
        };
        
        class Comet: public Unit  //komet
        {
        public:
            //Vector pos, vel;
            float ang, rot;
            float rad; //kometens radie
            float liv;
            
            Comet();
            Comet(Vec p, Vec v, float r);
            ~Comet();
            void update(float t) override;
            void render() override;
            bool Koll(Vec &p) override;
            float Distance(Vec &p) override;
            void Force(Vec f) override;
            bool Damage(float d) override;
            bool isSolid() override { return true; };
            float radius() override { return rad; };
        };
        
        class Projectile: public Unit //En projektil
        {
        public:
            //Vector pos, vel;
            float ang, rot, varand;
            
            Projectile(Vec p, Vec v);
            ~Projectile();
            void update(float t) override;
            void render() override;
        };
        
        class Explosion: public Unit //En explosion
        {
        public:
            //Vector pos;
            float stlk;
            
            Explosion(Vec p, float stlk);
            void update(float t) override;
            void render() override;
        };
        
        class Particle: public Unit //skräp som mest flyger omkring
        {
        public:
            //Vector pos, vel;
        	Vec start;
            float duration, maxDuration;
            float alpha1, alpha2;
            
            Particle(Vec p);
            Particle(Vec p, Vec v);
            void update(float t) override;
            void render() override;
            
        };
        
        class LineSmoke: public Unit //Rök som ser ut som en linje
        {
        public:
            //Vel används som den andra positionen
            float duration, maxDuration;
            
            LineSmoke(Vec p1, Vec p2);
            void update(float t) override;
            void render() override;
        };

    }
}


#endif
