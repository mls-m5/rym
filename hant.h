//Här skall saker för att hålla reda på diverse grejer finnas
#ifndef hant_h
#define hant_h

#include "graf.h"

namespace hant
{
    void init();
    void avsl();
    
    int getKey(int key);
    void setkey(int key, bool val);
}

#endif
