
#include "draw.h"
#include "graf.h"
#include <list>
#include <stdlib.h>

using std::list;

namespace graf
{
    namespace obj
    {
        int cenh; //Det objektet som behandlas för tillfället
        list<enhet *> enh;
        list<enhet *> sol; //enheter som kan kollisionstestas
        list<enhet *> remq;
        list<enhet *> delq;
        list<enhet_list_iterator_t> remqit_nonsol; //En speciell lista med iteratorer för att snabba på processen
    }
}


//__________________kontroller________________________________________

int kont::get(controlnum kontn)
{
    return hant::getkey(kontn);
}

//_______________________graf_________________________________________

void graf::Tid(float t)
{
	for (auto it: obj::sol){
		it->Tid(t);
	}
	for (auto it: obj::enh){
		it->Tid(t);
	}

	obj::flushDel();
	obj::flushRem();
}

void graf::Rendera()
{

    camTransform();
    for (auto it: obj::sol){
    	it->Rendera();
    }
    for (auto it: obj::enh){
    	it->Rendera();
    }
    flushDraw();
}

void graf::init()
{
    for(int i= 1; i<500; i++)
    {
        obj::add(new obj::star);
    }


    for (int i = 0; i<50; i++)
    {
        obj::add(new obj::komet);
    }

    obj::add(new obj::skepp);
}

void graf::avsl()
{
    obj::delq = obj::remq = obj::enh;
    obj::flushDel();
    obj::flushRem();
}

//________________________oega__________________________________________

namespace graf
{
    namespace oega
    {
        vector pos;
        float ang;
    }
}

void graf::oega::move(vector v, float a)
{
    pos = v;
    ang = a;
    setCam(v, a);
}

void graf::oega::transform()
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

graf::obj::enhet_list_iterator_t graf::obj::add(enhet *e)
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

void graf::obj::rem(enhet *e)
{
	for (auto it: remq){
		if (it == e){
			return; //Objektet redan i kö
		}
	}
	remq.push_back(e);
}

void graf::obj::rem(enhet_list_iterator_t it)
{
	for (auto tit: remqit_nonsol){
		if (tit == it){
			return; //enheten är redan i kö
		}
	}
	remqit_nonsol.push_back(it);
}

void graf::obj::remd(enhet *e)
{
	for (auto it: delq){
		if (it == e){
			return; //Objektet redan i kö
		}
	}
    rem(e);
    delq.push_back(e);
}

void graf::obj::remd(enhet_list_iterator_t it){
	for (auto tit: remqit_nonsol){
		if (tit == it){
			return; //Objektet redan i kö
		}
	}
	rem(it);
	delq.push_back(*it);
}

void graf::obj::flushRem() {
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

void graf::obj::flushDel() {
	for (auto it: delq){
		delete it;
	}
	delq.clear();
}

graf::obj::enhet *graf::obj::Koll(vector p, enhet *ign)
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

graf::obj::enhet *graf::obj::Naer(vector p, float lim, enhet *ign)
{
    float d = lim; //Det närmsta objektet som påträffats
    float dm;  //det senast mätta värdet
    enhet *e = 0;  //Det närmast mätta objektet
    for (auto it: sol)
    {
        if (ign != it) //Om objektet inte skall ignoreras
        {
            dm = it->Dist(p);
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

void graf::obj::skepp::Tid(float t)
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
            obj::add(new projekt(pos, vel + Vector(-sin(ang)/4, cos(ang)/4)));
            skott = .3;
        }
    }

    pos += vel;
    ang += rot;

    oega::move(pos + Vector(-sin(ang)*10, cos(ang)*10) , ang);
}

void graf::obj::skepp::Rendera()
{
	drawShip(pos, ang);
}

graf::obj::skepp::skepp()
{
    pos = Vector(0,0,0);
    vel = Vector(0,0,0);
    ang = 0; rot = 0;
    skott = 0;
}

//_______________________star________________________________________

void graf::obj::star::Tid(float t)
{

}

void graf::obj::star::Rendera()
{
	drawStar(pos);
//    glPointSize(2);
//    glBegin(GL_POINTS);
//    glColor3f(1,1,1);
//    glVertex3fv((float*)&pos);
//    glEnd();
}

graf::obj::star::star()
{
    pos = Vector(rand() % 1000 -500,rand() % 1000 -500, -rand()%100 / 10.);
}

//________________________komet______________________________________

void graf::obj::komet::Tid(float t)
{
    pos += vel;
    ang += rot;
}

void graf::obj::komet::Rendera()
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

bool graf::obj::komet::Koll(vector &p)
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

float graf::obj::komet::Dist(vector &p)
{
    float dx, dy;
    dx = pos.x -p.x;
    dy = pos.y -p.y;

    return sqrt(dx*dx+dy*dy);
}

void graf::obj::komet::Force(vector f)
{
    vector fo = f * .1;
    vel += fo;
}

bool graf::obj::komet::Skada(float d)
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

graf::obj::komet::komet()
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

graf::obj::komet::komet(vector p, vector v, float r)
{
    pos = p;
    vel = v;
    ang = 0;
    rot = 0;
    rad = r;
    liv = rad;
}

graf::obj::komet::~komet()
{
    if (rad > .2)
    {
        for (int i=0; i<3; i++)
        {
            add(new komet(pos,vel+Vector((rand()%11-5)/100.,(rand()%11-5)/100.),rad / 1.5));
        }
    }
}

//_______________projekt_______________________________________________

void graf::obj::projekt::Tid(float t)
{
    pos += vel;
    ang += rot;
    varand -= t;

    enhet * e;

    {
    	auto linj = new linjrok(pos-vel, pos);
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

        e->Skada(.4);

        add(new exp1(pos,.5));
        remd(iterator);
    }
    else if (varand <0)
    {
        remd(iterator);
    }
    else if ((e=obj::Naer(pos,20,this)))
    {
        vector v;
        v = pos - e->pos;
        //vel *= .99;
        v = v / -(v*v);
        v = v *.05;
        vel += v;

    }
}

void graf::obj::projekt::Rendera()
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

graf::obj::projekt::projekt(vector p, vector v):
		rot(0),
		ang(-atan2(v.x, v.y)),
		varand(60)
{
	pos = p;
	vel = v;
}

graf::obj::projekt::~projekt()
{
}

//____________________________Explosion_________________________________________

void graf::obj::exp1::Tid(float t)
{
    stlk /= (1+t);
    if (stlk < .01)
    {
        remd(iterator);
    }

}

void graf::obj::exp1::Rendera()
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

graf::obj::exp1::exp1(vector p, float s)
{
    pos = p;
    stlk = s;
    for (int i=1; i<20; i++)  add(new part(pos));
}

//________________________________Partikel______________________________________


void graf::obj::part::Tid(float t)
{
    pos += vel;
    add(new linjrok(pos-vel, pos));
    varand -= t;
    if (varand < 0)
    {
        remd(iterator);
    }
}

void graf::obj::part::Rendera()
{
	//Is visible through the smoke
//    glPointSize(2.2);
//    glBegin(GL_POINTS);
//    glColor3f(varand / varandmax,varand / varandmax,varand / varandmax);
//    glVertex3fv((float*)&pos);
//    glEnd();
}

graf::obj::part::part(vector p)
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
    varand = 2+rand()%20/20;
    varandmax = varand;
}

graf::obj::part::part(vector p, vector v):
		varand(2),
		varandmax(varand)
{
	pos = p;
	vel = v;
}

//___________________Rök på linje_______________________________________________

void graf::obj::linjrok::Tid(float t)
{
    varand -= t;
    if (varand < 0)
    {
    	remd(iterator);
    }
}

void graf::obj::linjrok::Rendera()
{
//    glBegin(GL_LINES);
//    glColor4f(1,1,1,varand / varandmax);
//    glVertex3fv((float*)&pos);
//    glVertex3fv((float*)&vel);
//    glEnd();
    drawSmoke(pos, vel, varand/varandmax);
}

graf::obj::linjrok::linjrok(vector p1, vector p2)
{
    pos = p1;
    vel = p2;
    varand = 12;
    varandmax = varand;
}

void graf::obj::enhet::setIterator(enhet_list_iterator_t it) {
	iterator = it;
	hasIterator = true;
}
