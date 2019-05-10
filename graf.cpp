
#include "draw.h"
#include "graf.h"
#include <list>
#include <vector>
#include <stdlib.h>
#include <memory>
#include "roamingbroadphase.h"
#include <algorithm>

using std::list;
using std::vector;
using std::unique_ptr;

namespace game
{
    namespace obj
    {
    	vector<unique_ptr<Unit>> enh;
    	vector<LineSmoke> smoke;
        int cenh; //Det objektet som behandlas för tillfället
        RoamingBroadphase solids; //enheter som kan kollisionstestas
    }
}


//__________________kontroller________________________________________

int kont::get(controlnum kontn)
{
    return hant::getKey(kontn);
}

//_______________________graf_________________________________________

void game::Update(float t)
{
	obj::solids.update(t);

	for (int i = 0; i < obj::enh.size(); ++i) {
		obj::enh[i]->update(t);
	}

	for (int i = 0; i < obj::smoke.size(); ++i) {
		obj::smoke[i].update(t);
	}

	obj::flushRem();
}

void game::Render()
{
    camTransform();

    obj::solids.draw();

    for (auto &it: obj::enh){
    	it->render();
    }
    for (auto &s: obj::smoke) {
    	s.render();
    }
    flushDraw();
}

void game::init()
{
    for(int i= 1; i<500; i++)
    {
        obj::add(new obj::Star);
    }


    for (int i = 0; i<50; i++)
    {
        obj::add(new obj::Comet);
    }
    
    obj::add(new obj::Ship);
}

void game::avsl()
{
    obj::flushRem();
}

//________________________oega__________________________________________

namespace game
{
    namespace eye
    {
        Vec pos;
        float ang;
    }
}

void game::eye::move(Vec v, float a)
{
    pos = v;
    ang = a;
    setCam(v, a);
    game::obj::solids.setCenter(v);
}

void game::eye::transform()
{
	camTransform();
}

//_________________________objekt --- -... .--- . -.- - ________________

void game::obj::add(Unit *e)
{
	if (e->isSolid()){
		solids.add(e);
	}
	else{
		enh.emplace_back(e);
	}
}

void game::obj::flushRem() {
	solids.removeDead();
	enh.erase(remove_if(enh.begin(), enh.end(), [] (unique_ptr<Unit> &u) {
		return u->dead == true;
	}), enh.end());
	smoke.erase(remove_if(smoke.begin(), smoke.end(), [] (LineSmoke &s) {
		return s.isDead() == true;
	}), smoke.end());
}


game::obj::Unit *game::obj::Koll(Vec p, Unit *ign)
{
	if (UseRoamingBroadphase) {
		return solids.collision(p, ign);
	}

	else {
		if (ign)  //Är det något objekt som skall ignoreras?
		{
			for (auto it: solids)
			{
				if (it->Koll(p) && it != ign) return it;
			}
			return 0;
		}
		else //inget objekt ignoreras
		{
			for (auto it: solids)
			{
				if (it->Koll(p)) return it;
			}
			return 0;
		}
	}
}

game::obj::Unit *game::obj::Near(Vec p, float limit, Unit *ignore)
{
	if (UseRoamingBroadphase) {
		return solids.getNearest(p, limit, ignore);

	}
	else {
		//Keeping old algorith for reference

		float distance = limit; //Det närmsta objektet som påträffats
		float dm;  //det senast mätta värdet
		Unit *e = 0;  //Det närmast mätta objektet

		for (auto it: solids)
		{
			if (ignore != it) //Om objektet inte skall ignoreras
			{
				dm = it->Distance(p);
				if (dm < distance && dm > 0)
				{
					e = it;
					distance = dm;
				}
			}
		}
		return e; //Objektet returneras

	}
}

//---------skepp-------------------------------------------------------

void game::obj::Ship::update(float t)
{
    rot /= 1.2;
    vel.x *= .9;
    vel.y *= .9;
    if (kont::get(cn_left)) rot += .01;
    if (kont::get(cn_right)) rot += -.01;
    if (kont::get(cn_up)) vel = Vec(-sin(ang)*.1, cos(ang)*.1);
    if (skott>0)
    {
        skott = skott-t;
    }
    else
    {
        if (kont::get(cn_eld))
        {
            obj::add(new Projectile(pos, vel + Vec(-sin(ang)/4, cos(ang)/4)));
            skott = .3;
        }
    }

    pos += vel;
    ang += rot;
    
    
    eye::move(pos + Vec(-sin(ang)*10, cos(ang)*10) , ang);
}

void game::obj::Ship::render()
{
	drawShip(pos, ang);
}

game::obj::Ship::Ship()
{
    pos = Vec(0,0,0);
    vel = Vec(0,0,0);
    ang = 0; rot = 0;
    skott = 0;
}

//_______________________star________________________________________

void game::obj::Star::update(float t)
{

}

void game::obj::Star::render()
{
	drawStar(pos);
}

game::obj::Star::Star()
{
    pos = Vec(rand() % 1000 -500,rand() % 1000 -500, -rand()%100 / 10.);
}

//________________________komet______________________________________

void game::obj::Comet::update(float t)
{
    pos += vel;
    ang += rot;
}

void game::obj::Comet::render()
{
	drawComet(pos, ang, rad);
}

bool game::obj::Comet::Koll(Vec &p)
{
    float dx, dy;
    dx = pos.x - p.x;
    dy = pos.y - p.y;
    if ((dx * dx + dy * dy) < rad * rad)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

float game::obj::Comet::Distance(Vec &p)
{
    float dx, dy;
    dx = pos.x -p.x;
    dy = pos.y -p.y;

    return sqrt(dx*dx+dy*dy);
}

void game::obj::Comet::Force(Vec f)
{
    Vec fo = f * .1;
    vel += fo;
}

bool game::obj::Comet::Damage(float d)
{
	if (dead) {
		return 1;
	}
    liv -= d;
    if (liv <= 0)
    {
    	dead = true;
        return 1;
    }
    else
    {
        return 0;
    }
}

game::obj::Comet::Comet()
{
    //static float x = 0;
    //pos = Vec(++x,x);
    pos = Vec(rand() % 1000 / 10 -50,rand() % 100 -50);
    vel = Vec(0,0);
    ang = 0;
    rot = rand()%1000 / 1000. - .5;
    rad = 1.5;
    liv = rad;
}

game::obj::Comet::Comet(Vec p, Vec v, float r)
{
    pos = p;
    vel = v;
    ang = 0;
    rot = rand()%1000 / 1000. - .5;
    rad = r;
    liv = rad;
}

game::obj::Comet::~Comet()
{
    if (rad > .2)
    {
        for (int i=0; i<3; i++)
        {
            add(new Comet(pos,vel+Vec((rand()%11-5)/100.,(rand()%11-5)/100.),rad / 1.5));
        }
    }
}

//_______________projekt_______________________________________________

void game::obj::Projectile::update(float t)
{
	if (dead) {
		return;
	}
    pos += vel;
    ang += rot;
    varand -= t;
    
    Unit * e;
    
    addSmoke(pos-vel, pos);


    e = obj::Koll(pos, this);
    if (e)
    {
        e->Force(vel*.01);
        
        e->Damage(.4);
        
        add(new Explosion(pos,.5));
        dead = true;
    }
    else if (varand <0)
    {
    	dead = true;
    }
    else if ((e=obj::Near(pos,20,this)))
    {
        Vec v;
        v = pos - e->pos;
        v = v / -(v*v);
        v = v *.05;
        vel += v;

    }
}

void game::obj::Projectile::render()
{
	drawProjectile(pos, ang, .1);
}

game::obj::Projectile::Projectile(Vec p, Vec v):
		ang(-atan2(v.x, v.y)),
		rot(0),
		varand(60)
{
	pos = p;
	vel = v;
}

game::obj::Projectile::~Projectile()
{
}

//____________________________Explosion_________________________________________

void game::obj::Explosion::update(float t)
{
    stlk /= (1+t);
    if (stlk < .01)
    {
    	dead = true;
    }

}

void game::obj::Explosion::render()
{
	drawExplosion(pos, stlk);
}

game::obj::Explosion::Explosion(Vec p, float s)
{
    pos = p;
    stlk = s;
    for (int i=1; i<20; i++)  add(new Particle(pos));
}

//________________________________Partikel______________________________________


void game::obj::Particle::update(float t)
{
	if (dead) {
		return;
	}
	constexpr float step = 1. / 80;
	alpha1 -= step;
    if (duration < 0) 
    {
        alpha2 -= step;
        if (alpha2 <= 0) {
        	dead = true;
        }
    }
    else {
    	duration -= t;
    	pos += vel;
    }
}

void game::obj::Particle::render()
{
	if (!dead) {
		drawSmoke(start, pos, alpha1, alpha2);
	}
}

game::obj::Particle::Particle(Vec p)
{
    pos = p;
    start = p;
    float vx = 0;
    float vy = 0;
    float vz = 0;
    alpha1 = alpha2 = 1;
    for (int i=0; i<6;i++)
    {
        vx += rand() % 1000 - 500;
        vy += rand() % 1000 - 500;
        vz += rand() % 1000 - 500;
    }
    vel.x = vx / 12000.;
    vel.y = vy / 12000.;
    vel.z = vz / 12000.;
    duration = 2+rand()%20/20;
    maxDuration = duration;
}

game::obj::Particle::Particle(Vec p, Vec v):
		duration(2),
		maxDuration(duration),
		alpha1(1),
		alpha2(1)
{
	pos = p;
	vel = v;
}

//___________________Rök på linje_______________________________________________

void game::obj::addSmoke(Vec p1, Vec p2) {
	game::obj::smoke.emplace_back(p1, p2);
}

void game::obj::LineSmoke::update(float t)
{
    duration -= t;
}

void game::obj::LineSmoke::render()
{
    drawSmoke(pos1, pos2, duration/maxDuration, duration/maxDuration);
}

game::obj::LineSmoke::LineSmoke(Vec p1, Vec p2)
{
    pos1 = p1;
    pos2 = p2;
    duration = 12;
    maxDuration = duration;
}

