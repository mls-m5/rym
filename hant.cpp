#include "hant.h"

#include <map>

using namespace std;

#define KEYCOUNT 500
namespace hant
{
//    int key[KEYCOUNT + 1];
	map<int ,int> key;
}

void hant::init()
{
	for (int i = 0; i < KEYCOUNT; ++i){
		key[KEYCOUNT] = 0;
	}
    game::init();
}

void hant::avsl()
{
    game::avsl();
}

int hant::getKey(int keyn)
{
    if (key[keyn]){
    	key[keyn] = 1;
    }
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

