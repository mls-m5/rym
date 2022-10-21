module;

#if defined(__ANDROID__) || defined(__APPLE__)

#else

#include <SDL2/SDL.h>

#endif

export module hant;

import <map>;

#if defined(WIN32) || defined(_WIN32) ||                                       \
    defined(__WIN32) && !defined(__CYGWIN__)
// #include "windows.h"
export enum controlnum {
    cn_up = VK_UP,
    cn_down = VK_DOWN,
    cn_left = VK_LEFT,
    cn_right = VK_RIGHT,
    cn_eld = VK_SPACE
};
#elif defined(__ANDROID__) || defined(__APPLE__)
export enum controlnum {
    cn_up = 1,
    cn_down = 2,
    cn_left = 3,
    cn_right = 4,
    cn_eld = 5
};
#else
export enum controlnum {
    cn_up = SDLK_UP,
    cn_down = SDLK_DOWN,
    cn_left = SDLK_LEFT,
    cn_right = SDLK_RIGHT,
    cn_eld = SDLK_SPACE
};
#endif

namespace hant {

export int getKey(int key);
export void setkey(int key, bool val);

} // namespace hant

using namespace std;

namespace hant {
static map<int, int> key;
}

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
