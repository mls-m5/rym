#include <map>

export module hant;

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
