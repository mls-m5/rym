//#include "hant.h"
//#include "graf.h"

export module hant;
#if defined(__GXX_WEAK__)
#include <bits/gthr-default.h>
#endif

#include <map>
// import graf;

namespace hant {
export void init();
export void avsl();

export int getKey(int key);
export void setkey(int key, bool val);
} // namespace hant

using namespace std;

namespace hant {
static map<int, int> key;
}

// void hant::init() {
//    game::init();
//}

// void hant::avsl() {
//    game::avsl();
//}

int hant::getKey(int keyn) {
    if (key[keyn]) {
        key[keyn] = 1;
    }
    return key[keyn];
}

void hant::setkey(int keyn, bool val) {
    if (val) {
        key[keyn] = 2;
    }
    else {
        key[keyn] = 0;
    }
}
