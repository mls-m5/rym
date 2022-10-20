module;

export module draw;

import vec;

export void drawArea(Vec p, double a, double r);

export void drawSmoke(Vec p1, Vec p2, double alpha1, double alpha2);

export void drawStar(Vec p);

export void drawComet(Vec p, double a, double r);

export void drawExplosion(Vec pos, double size);

export void drawProjectile(Vec p, double a, double scale);

export void setCam(Vec p, double a);

export void camTransform();

export void drawShip(Vec p, double a);

export bool initDrawModule(double perspective);

export void flushDraw();
