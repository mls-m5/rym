#ifndef graf_h
#define graf_h

#include "vektorer.h"
#include "hant.h"
//#include <GL/gl.h>
//#include <GL/glu.h>
#include "math.h"
#include <list>
const float pi = 3.1415926535897932384626433832795;

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

namespace kont
{
    int get(controlnum kontn);
}

namespace game
{
    void Update(float t);
    void Rendera();
    void init();
    void avsl();
    
    namespace eye
    {
        void move(vec v, float a);
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
        Unit *Koll(vec, Unit *ign); //ign står för det objektet som skall ignoreras
        Unit *Near(vec p, float lim, Unit *ign); //Hittar det närmaste objektet
        
        class Unit
        {
        public:
            vec pos, vel;
            Unit(): hasIterator(false) {};
            virtual ~Unit() {};
            
            virtual void Update(float t) {};
            virtual void Render() {};
            virtual bool Koll(vec &p) {return 0;};  //kollisionstestning
            virtual float Distance(vec &p) {return 0;};  //Hitta avståndet
            virtual void Force(vec f) {};
            virtual bool Damage(float d) {return 0;}; //Sant om enheten går sönder
            virtual bool isSolid() {return 0;};
            void setIterator(enhet_list_iterator_t);

        protected:
            bool hasIterator;
            enhet_list_iterator_t iterator; //En iterator för att snabbare kunna ta bort röken
        };
        
        class Ship: public Unit
        {
        public:
            //vector pos, vel;
            float ang, rot;
            float skott;
            
            Ship();
            void Update(float t);
            void Render();
        };
        
        class star: public Unit  //Stj�rna
        {
        public:
            //vector pos;

            star();
            void Update(float t);
            void Render();
        };
        
        class Comet: public Unit  //komet
        {
        public:
            //vector pos, vel;
            float ang, rot;
            float rad; //kometens radie
            float liv;
            
            Comet();
            Comet(vec p, vec v, float r);
            ~Comet();
            void Update(float t);
            void Render();
            bool Koll(vec &p);
            float Distance(vec &p);
            void Force(vec f);
            bool Damage(float d);
            bool isSolid() { return true; };
        };
        
        class Projectile: public Unit //En projektil
        {
        public:
            //vector pos, vel;
            float ang, rot, varand;
            
            Projectile(vec p, vec v);
            ~Projectile();
            void Update(float t);
            void Render();
        };
        
        class Explosion: public Unit //En explosion
        {
        public:
            //vector pos;
            float stlk;
            
            Explosion(vec p, float stlk);
            void Update(float t);
            void Render();
        };
        
        class Particle: public Unit //skräp som mest flyger omkring
        {
        public:
            //vector pos, vel;
            float duration, maxDuration;
            
            Particle(vec p);
            Particle(vec p, vec v);
            void Update(float t);
            void Render();
            
        };
        
        class LineSmoke: public Unit //Rök som ser ut som en linje
        {
        public:
            //Vel används som den andra positionen
            float varand, varandmax;
            
            LineSmoke(vec p1, vec p2);
            void Update(float t);
            void Render();
        };

    }
}


#endif
