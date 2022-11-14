#pragma once

#define glCall(call)                                                           \
    call;                                                                      \
    checkGlError(#call)
