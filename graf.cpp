
#include "graf.h"
#include <list>

using std::list;

namespace game
{
    namespace obj
    {
        int cenh; //Det objektet som behandlas för tillfället
        list<Unit *> enh;
        list<Unit *> sol; //enheter som kan kollisionstestas
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
	for (auto it: obj::sol){
		it->Update(t);
	}
	for (auto it: obj::enh){
		it->Update(t);
	}

	obj::flushDel();
	obj::flushRem();
}

void game::Rendera()
{
    glPushMatrix();
    eye::transform();
    
    glBegin(GL_POINTS);
    glVertex2f(0,0);
    glEnd();
    
    for (auto it: obj::sol){
    	it->Render();
    }
    for (auto it: obj::enh){
    	it->Render();
    }
    glPopMatrix();
}

void game::init()
{
    for(int i= 1; i<500; i++)
    {
        obj::add(new obj::star);
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
        vec pos;
        float ang;
    }
}

void game::eye::move(vec v, float a)
{
    pos = v;
    ang = a;
}

void game::eye::transform()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glScalef(.1,.1,.1);
    
    glFrustum(-.1,.1, -.1, .1, 1.f, 50.f);
    glRotatef(-10,1,0,0);
    glRotatef(-ang / pi* 180.,0,0,1);
    glTranslatef(-pos.x, -pos.y, -pos.z-20);
    glMatrixMode(GL_MODELVIEW);
}

//_________________________objekt --- -... .--- . -.- - ________________

game::obj::enhet_list_iterator_t game::obj::add(Unit *e)
{
	if (e->isSolid()){
		sol.push_back(e);
		auto it = --sol.end();
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
		sol.remove(it);
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
		delete it;
	}
	delq.clear();
}

game::obj::Unit *game::obj::Koll(vec p, Unit *ign)
{
    if (ign)  //Är det något objekt som skall ignoreras?
    {
    	for (auto it: sol)
        {
            if (it->Koll(p) && it != ign) return it;
        }
        return 0;
    }
    else //inget objekt ignoreras
    {
    	for (auto it: sol)
        {
            if (it->Koll(p)) return it;
        }
        return 0;
    }
}

game::obj::Unit *game::obj::Near(vec p, float lim, Unit *ign)
{
    float d = lim; //Det närmsta objektet som påträffats
    float dm;  //det senast mätta värdet
    Unit *e = 0;  //Det närmast mätta objektet
    for (auto it: sol)
    {
        if (ign != it) //Om objektet inte skall ignoreras
        {
            dm = it->Distance(p);
            if (dm < d && dm > 0)
            {
                e = it;
                d = dm;
            }
        }
    }
    return e; //Objektet returneras
}

//---------skepp-------------------------------------------------------

void game::obj::Ship::Update(float t)
{
    rot /= 1.2;
    vel.x *= .9;
    vel.y *= .9;
    if (kont::get(cn_left)) rot += .01;
    if (kont::get(cn_right)) rot += -.01;
    if (kont::get(cn_up)) vel = Vector(-sin(ang)*.1, cos(ang)*.1);
    if (skott>0)
    {
        skott = skott-t;
    }
    else
    {
        if (kont::get(cn_eld)) 
        {
            obj::add(new Projectile(pos, vel + Vector(-sin(ang)/4, cos(ang)/4)));
            skott = .3;
        }
    }
    
    pos += vel;
    ang += rot;
    
    
    
    eye::move(pos + Vector(-sin(ang)*10, cos(ang)*10) , ang);
}

void game::obj::Ship::Render()
{
    glPushMatrix();
    glTranslatef(pos.x,pos.y,pos.z);
    glRotatef(ang/pi*180,0,0,1);
    glBegin(GL_TRIANGLES);
    glVertex2f(0,1);
    glVertex2f(.5, -1);
    glVertex2f(-.5, -1);
    glEnd();
    
    glPopMatrix();
}

game::obj::Ship::Ship()
{
    pos = Vector(0,0,0);
    vel = Vector(0,0,0);
    ang = 0; rot = 0;
    skott = 0;
}

//_______________________star________________________________________

void game::obj::star::Update(float t)
{
    
}

void game::obj::star::Render()
{
    glPointSize(2);
    glBegin(GL_POINTS);
    glColor3f(1,1,1);
    glVertex3fv((float*)&pos);
    glEnd();
}

game::obj::star::star()
{
    pos = Vector(rand() % 1000 -500,rand() % 1000 -500, -rand()%100 / 10.);
}

//________________________komet______________________________________

void game::obj::Comet::Update(float t)
{
    pos += vel;
    ang += rot;
}

void game::obj::Comet::Render()
{
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(ang, 0,0,1);
    glScalef(rad,rad,rad);
    
    glBegin(GL_QUADS);
    glColor3f(6.,.6,1);
    glVertex2f(-1,1);
    glVertex2f(1,1);
    glVertex2f(1,-1);
    glVertex2f(-1,-1);
    glEnd();
    
    glPopMatrix();
}

bool game::obj::Comet::Koll(vec &p)
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

float game::obj::Comet::Distance(vec &p)
{
    float dx, dy;
    dx = pos.x -p.x;
    dy = pos.y -p.y;
    
    return sqrt(dx*dx+dy*dy);
}

void game::obj::Comet::Force(vec f)
{
    vec fo = f * .1;
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
    //pos = Vector(++x,x);
    pos = Vector(rand() % 1000 / 10 -50,rand() % 100 -50);
    vel = Vector(0,0);
    ang = 0;
    rot = rand()%1000 / 1000. - .5;
    rad = 1.5;
    liv = rad;
}

game::obj::Comet::Comet(vec p, vec v, float r)
{
    pos = p;
    vel = v;
    ang = 0;
    rot = 0;
    rad = r;
    liv = rad;
}

game::obj::Comet::~Comet()
{
    if (rad > .2)
    {
        for (int i=0; i<3; i++)
        {
            add(new Comet(pos,vel+Vector((rand()%11-5)/100.,(rand()%11-5)/100.),rad / 1.5));
        }
    }
}

//_______________projekt_______________________________________________

void game::obj::Projectile::Update(float t)
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
        //vel = Vector(1,0);
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
        vec v;
        v = pos - e->pos;
        //vel *= .99;
        v = v / -(v*v);
        v = v *.05;
        vel += v;
        
    }
}

void game::obj::Projectile::Render()
{
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(ang/pi*180, 0,0,1);
    glScalef(.1,.1,.4);
    
    glBegin(GL_QUADS);
    glColor3f(1,.3,.3);
    glVertex2f(0,1);
    glVertex2f(1,0);
    glVertex2f(0,-2);
    glVertex2f(-1,0);
    glEnd();
    
    glPopMatrix();
}

game::obj::Projectile::Projectile(vec p, vec v):
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

void game::obj::Explosion::Update(float t)
{
    stlk /= (1+t);
    if (stlk < .01)
    {
        remd(iterator);
    }
    
}

void game::obj::Explosion::Render()
{
    glPushMatrix();
    glTranslatef(pos.x,pos.y,pos.z);
    glScalef(stlk, stlk, stlk);
    glBegin(GL_QUADS);
    glColor3f(6.,.6,1);
    glVertex2f(-1,1);
    glVertex2f(1,1);
    glVertex2f(1,-1);
    glVertex2f(-1,-1);
    glEnd();
    glPopMatrix();
}

game::obj::Explosion::Explosion(vec p, float s)
{
    pos = p;
    stlk = s;
    for (int i=1; i<20; i++)  add(new Particle(pos));
}

//________________________________Partikel______________________________________


void game::obj::Particle::Update(float t)
{
    pos += vel;
    add(new LineSmoke(pos-vel, pos));
    duration -= t;
    if (duration < 0) 
    {
        remd(iterator);
    }
}

void game::obj::Particle::Render()
{
    glPointSize(2.2);
    glBegin(GL_POINTS);
    glColor3f(duration / maxDuration,duration / maxDuration,duration / maxDuration);
    glVertex3fv((float*)&pos);
    glEnd();
}

game::obj::Particle::Particle(vec p)
{
    pos = p;
    float vx = 0;
    float vy = 0;
    float vz = 0;
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

game::obj::Particle::Particle(vec p, vec v):
		duration(2),
		maxDuration(duration)
{
	pos = p;
	vel = v;
}

//___________________Rök på linje_______________________________________________

void game::obj::LineSmoke::Update(float t)
{
    varand -= t;
    if (varand < 0) 
    {
    	remd(iterator);
    }
}

void game::obj::LineSmoke::Render()
{
    glBegin(GL_LINES);
    glColor4f(1,1,1,varand / varandmax);
    glVertex3fv((float*)&pos);
    glVertex3fv((float*)&vel);
    glEnd();
}

game::obj::LineSmoke::LineSmoke(vec p1, vec p2)
{
    pos = p1;
    vel = p2;
    varand = 12;
    varandmax = varand;
}

void game::obj::Unit::setIterator(enhet_list_iterator_t it) {
	iterator = it;
	hasIterator = true;
}
