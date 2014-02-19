#include "hant.h"

namespace hant
{
    int key[256];
}

void hant::init()
{
    graf::init();
}

void hant::avsl()
{
    graf::avsl();
}

int hant::getkey(int keyn)
{
    if (key[keyn]) key[keyn] = 1;
    return key[keyn];
}

void hant::setkey(int keyn, bool val)
{
    if (val)
     key[keyn] = 2;
    else
     key[keyn] = 0;
    return;
}

