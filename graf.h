#ifndef graf_h
#define graf_h

#include "vektorer.h"
//#include "windows.h"
#include "hant.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include "math.h"
//#include "bitmap.h"
const float pi = 3.1415926535897932384626433832795;

enum controlnum
{
    cn_up = SDLK_UP /*VK_UP*/,
    cn_down = SDLK_DOWN /*VK_DOWN*/,
    cn_left = SDLK_LEFT /*VK_LEFT*/,
    cn_right = SDLK_RIGHT /*VK_RIGHT*/,
    cn_eld =  SDLK_SPACE /*VK_SPACE*/
};

namespace kont
{
    int get(controlnum kontn);
}

namespace graf
{
    void Tid(float t);
    void Rendera();
    void init();
    void avsl();
    
    namespace oega
    {
        void move(vector v, float a);
        void transform();
    }
    
    namespace obj
    {
        class enhet;
        
        void add(enhet *e);  //Lägger till objektet i listan
        void rem(enhet *e);  //Tar bort objektet ur listan
        void remd(enhet *e); //Tar bort objektet helt
        enhet *Koll(vector, enhet *ign); //ign står för det objektet som skall ignoreras
        enhet *Naer(vector p, float lim, enhet *ign); //Hittar det närmaste objektet
        
        class enhet
        {
        public:
            vector pos, vel;
            
            virtual void Tid(float t) {};
            virtual void Rendera() {};
            virtual bool Koll(vector &p) {return 0;};  //kollisionstestning
            virtual float Dist(vector &p) {return 0;};  //Hitta avståndet
            virtual void Force(vector f) {};
            virtual bool Skada(float d) {}; //Sant om enheten går sönder
            
            //enhet() {};
            //~enhet() {rem(this);};
        };
        
        class skepp: public enhet
        {
        public:
            //vector pos, vel;
            float ang, rot;
            float skott;
            
            skepp();
            void Tid(float t);
            void Rendera();
        };
        
        class star: public enhet  //Stjärna
        {
        public:
            //vector pos;
            
            star();
            void Tid(float t);
            void Rendera();
        };
        
        class komet: public enhet  //komet
        {
        public:
            //vector pos, vel;
            float ang, rot;
            float rad; //kometens radie
            float liv;
            
            komet();
            komet(vector p, vector v, float r);
            ~komet();
            void Tid(float t);
            void Rendera();
            bool Koll(vector &p);
            float Dist(vector &p);
            void Force(vector f);
            bool Skada(float d);
        };
        
        class projekt: public enhet //En projektil
        {
        public:
            //vector pos, vel;
            float ang, rot, varand;
            
            projekt(vector p, vector v);
            ~projekt();
            void Tid(float t);
            void Rendera();
        };
        
        class exp1: public enhet //En explosion
        {
        public:
            //vector pos;
            float stlk;
            
            exp1(vector p, float stlk);
            void Tid(float t);
            void Rendera();
        };
        
        class part: public enhet //skräp som mest flyger omkring
        {
        public:
            //vector pos, vel;
            float varand, varandmax;
            
            part(vector p);
            part(vector p, vector v);
            void Tid(float t);
            void Rendera();
            
        };
        
        class linjrok: public enhet //Rök som ser ut som en linje
        {
        public:
            //Vel används som den andra positionen
            float varand, varandmax;
            
            linjrok(vector p1, vector p1);
            void Tid(float t);
            void Rendera();
        };
        
    }
}


#endif
