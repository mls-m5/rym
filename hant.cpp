#include "hant.h"

#define KEYCOUNT 500
namespace hant
{
    int key[KEYCOUNT];
}

void hant::init()
{
	for (int i = 0; i < KEYCOUNT; ++i){
		key[KEYCOUNT] = 0;
	}
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

