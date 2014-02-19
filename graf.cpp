
#include "graf.h"
#include <list>

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
    glPushMatrix();
    oega::transform();
    
    glBegin(GL_POINTS);
    glVertex2f(0,0);
    glEnd();
    
    for (auto it: obj::sol){
    	it->Rendera();
    }
    for (auto it: obj::enh){
    	it->Rendera();
    }
    glPopMatrix();
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
}

void graf::oega::transform()
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

void graf::obj::add(enhet *e)
{
	if (e->isSolid()){
		sol.push_back(e);
	}
	else{
		enh.push_back(e);
	}
}

void graf::obj::rem(enhet *e)
{
	remq.push_back(e);
	remq.sort();
	remq.unique();
}

void graf::obj::remd(enhet *e)
{
    rem(e);
    delq.push_back(e);
    delq.sort();
    delq.unique();
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

void graf::obj::flushRem() {
	for (auto it: remq){
		sol.remove(it);
		enh.remove(it);
	}
	remq.clear();
}

void graf::obj::flushDel() {
	for (auto it: delq){
		delete it;
	}
	delq.clear();
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
    glPointSize(2);
    glBegin(GL_POINTS);
    glColor3f(1,1,1);
    glVertex3fv((float*)&pos);
    glEnd();
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
    
    add(new linjrok(pos-vel, pos));
    
    e = obj::Koll(pos, this);
    if (e) 
    {
        e->Force(vel*.01);
        //vel = Vector(1,0);
        //rem(e);
        //delete e;
        
        e->Skada(.4);
        
        add(new exp1(pos,.5));
        remd(this);
    }
    else if (varand <0)
    {
        remd(this);
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
        remd(this);
    }
    
}

void graf::obj::exp1::Rendera()
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
        remd(this);
    }
}

void graf::obj::part::Rendera()
{
    glPointSize(2.2);
    glBegin(GL_POINTS);
    glColor3f(varand / varandmax,varand / varandmax,varand / varandmax);
    glVertex3fv((float*)&pos);
    glEnd();
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
        remd(this);
    }
}

void graf::obj::linjrok::Rendera()
{
    glBegin(GL_LINES);
    glColor4f(1,1,1,varand / varandmax);
    glVertex3fv((float*)&pos);
    glVertex3fv((float*)&vel);
    glEnd();
}

graf::obj::linjrok::linjrok(vector p1, vector p2)
{
    pos = p1;
    vel = p2;
    varand = 12;
    varandmax = varand;
}
