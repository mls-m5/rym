
#include "draw.h"
#include "graf.h"
#include <list>
#include <stdlib.h>
#include "roamingbroadphase.h"


using std::list;

namespace game
{
    namespace obj
    {
        int cenh; //Det objektet som behandlas för tillfället
        list<Unit *> enh;
        RoamingBroadphase solids; //enheter som kan kollisionstestas
        list<Unit *> remq;
        list<Unit *> delq;
        list<enhet_list_iterator_t> remqit_nonsol; //En speciell lista med iteratorer för att snabba på processen
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
//	for (auto it: obj::solids){
//		it->update(t);
//	}
	obj::solids.update(t);


	for (auto it: obj::enh){
		it->update(t);
	}

	obj::flushDel();
	obj::flushRem();
}

void game::Render()
{
    camTransform();

//    for (auto it: obj::solids){
//    	it->render();
//    }
    obj::solids.draw();

    for (auto it: obj::enh){
    	it->render();
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
    obj::delq = obj::remq = obj::enh;
    obj::flushDel();
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
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glScalef(.1,.1,.1);
//
//    glFrustum(-.1,.1, -.1, .1, 1.f, 50.f);
//    glRotatef(-10,1,0,0);
//    glRotatef(-ang / pi* 180.,0,0,1);
//    glTranslatef(-pos.x, -pos.y, -pos.z-20);
//    glMatrixMode(GL_MODELVIEW);
	camTransform();
}

//_________________________objekt --- -... .--- . -.- - ________________

game::obj::enhet_list_iterator_t game::obj::add(Unit *e)
{
	if (e->isSolid()){
		solids.add(e);
		auto it = --solids.end();
		e->setIterator(it);
		return it;
	}
	else{
		enh.push_back(e);
		auto it = --enh.end();
		e->setIterator(it);
		return it;
	}
}

void game::obj::rem(Unit *e)
{
	for (auto it: remq){
		if (it == e){
			return; //Objektet redan i kö
		}
	}
	remq.push_back(e);
}

void game::obj::rem(enhet_list_iterator_t it)
{
	for (auto tit: remqit_nonsol){
		if (tit == it){
			return; //enheten är redan i kö
		}
	}
	remqit_nonsol.push_back(it);
}

void game::obj::remd(Unit *e)
{
	for (auto it: delq){
		if (it == e){
			return; //Objektet redan i kö
		}
	}
    rem(e);
    delq.push_back(e);
}

void game::obj::remd(enhet_list_iterator_t it){
	for (auto tit: remqit_nonsol){
		if (tit == it){
			return; //Objektet redan i kö
		}
	}
	rem(it);
	delq.push_back(*it);
}

void game::obj::flushRem() {
	for (auto it: remq){
//		solids.remove(it);
		enh.remove(it);
	}
	remq.clear();
	for (auto it: remqit_nonsol){
		enh.erase(it);
	}
	remqit_nonsol.clear();
}

void game::obj::flushDel() {
	for (auto it: delq){
		if (it->isSolid()) {
			solids.remove(it);
		}
		delete it;
	}
	delq.clear();
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
//    glPointSize(2);
//    glBegin(GL_POINTS);
//    glColor3f(1,1,1);
//    glVertex3fv((float*)&pos);
//    glEnd();
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
//    glPushMatrix();
//    glTranslatef(pos.x, pos.y, pos.z);
//    glRotatef(ang, 0,0,1);
//    glScalef(rad,rad,rad);
//
//    glBegin(GL_QUADS);
//    glColor3f(6.,.6,1);
//    glVertex2f(-1,1);
//    glVertex2f(1,1);
//    glVertex2f(1,-1);
//    glVertex2f(-1,-1);
//    glEnd();
//
//    glPopMatrix();
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
    liv -= d;
    if (liv <= 0)
    {
        remd(this);
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
    pos += vel;
    ang += rot;
    varand -= t;
    
    Unit * e;
    
    {
    	auto linj = new LineSmoke(pos-vel, pos);
    	auto it = add(linj);
    	linj->setIterator(it);
    }


    e = obj::Koll(pos, this);
    if (e)
    {
        e->Force(vel*.01);
        //vel = Vec(1,0);
        //rem(e);
        //delete e;
        
        e->Damage(.4);
        
        add(new Explosion(pos,.5));
        remd(iterator);
    }
    else if (varand <0)
    {
        remd(iterator);
    }
    else if ((e=obj::Near(pos,20,this)))
    {
        Vec v;
        v = pos - e->pos;
        //vel *= .99;
        v = v / -(v*v);
        v = v *.05;
        vel += v;

    }
}

void game::obj::Projectile::render()
{
//    glPushMatrix();
//    glTranslatef(pos.x, pos.y, pos.z);
//    glRotatef(ang/pi*180, 0,0,1);
//    glScalef(.1,.1,.4);
//
//    glBegin(GL_QUADS);
//    glColor3f(1,.3,.3);
//    glVertex2f(0,1);
//    glVertex2f(1,0);
//    glVertex2f(0,-2);
//    glVertex2f(-1,0);
//    glEnd();
//
//    glPopMatrix();
	drawProjectile(pos, ang, .1);
}

game::obj::Projectile::Projectile(Vec p, Vec v):
		rot(0),
		ang(-atan2(v.x, v.y)),
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
        remd(iterator);
    }

}

void game::obj::Explosion::render()
{
//    glPushMatrix();
//    glTranslatef(pos.x,pos.y,pos.z);
//    glScalef(stlk, stlk, stlk);
//    glBegin(GL_QUADS);
//    glColor3f(6.,.6,1);
//    glVertex2f(-1,1);
//    glVertex2f(1,1);
//    glVertex2f(1,-1);
//    glVertex2f(-1,-1);
//    glEnd();
//    glPopMatrix();
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
	constexpr float step = 1. / 80;
//    add(new LineSmoke(pos-vel, pos));
	alpha1 -= step;
    if (duration < 0) 
    {
        alpha2 -= step;
        if (alpha2 <= 0) {
        	remd(iterator);
        }
    }
    else {
    	duration -= t;
    	pos += vel;
    }
}

void game::obj::Particle::render()
{
	//Is visible through the smoke
//    glPointSize(2.2);
//    glBegin(GL_POINTS);
//    glColor3f(varand / varandmax,varand / varandmax,varand / varandmax);
//    glVertex3fv((float*)&pos);
//    glEnd();
    drawSmoke(start, pos, alpha1, alpha2);
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
		maxDuration(duration)
{
	pos = p;
	vel = v;
}

//___________________Rök på linje_______________________________________________

void game::obj::LineSmoke::update(float t)
{
    duration -= t;
    if (duration < 0)
    {
    	remd(iterator);
    }
}

void game::obj::LineSmoke::render()
{
//    glBegin(GL_LINES);
//    glColor4f(1,1,1,varand / varandmax);
//    glVertex3fv((float*)&pos);
//    glVertex3fv((float*)&vel);
//    glEnd();
    drawSmoke(pos, vel, duration/maxDuration, duration/maxDuration);
}

game::obj::LineSmoke::LineSmoke(Vec p1, Vec p2)
{
    pos = p1;
    vel = p2;
    duration = 12;
    maxDuration = duration;
}

void game::obj::Unit::setIterator(enhet_list_iterator_t it) {
	iterator = it;
	hasIterator = true;
}
