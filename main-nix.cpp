#include "draw.h"
#include <cstdlib>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>

#include "glfunctions.h"
#include <iostream>
using namespace std;

#include "graf.h"

using namespace std;

// Width & Height of window
const int width = 800;  // 640
const int height = 600; // 480

static SDL_Window *window = nullptr;
static SDL_GLContext context;

// Kill program
[[noreturn]] void endProgram(int code) {
    SDL_Quit();
    exit(code);
}

// Handle SDL keypresses
void handleKeys(SDL_Keysym *keysym, bool state) {
    switch (keysym->sym) {
    case SDLK_ESCAPE:
        endProgram(0);
    default:
        hant::setkey(static_cast<int>(keysym->sym), state);
        break;
    }
}

// Process SDL events
void processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            handleKeys(&event.key.keysym, true);
            break;
        case SDL_KEYUP:
            handleKeys(&event.key.keysym, false);
            break;

        case SDL_QUIT:
            endProgram(0);
        }
    }
}

[[noreturn]] void mainLoop() {
    // long ttime;
    while (true) {
        Uint32 ttime = 15 + SDL_GetTicks();
        processEvents();

        game::Update(.1);

        glClear(GL_DEPTH_BUFFER_BIT |
                GL_COLOR_BUFFER_BIT); // Clear color and depth buffer

        game::Render();

        SDL_GL_SwapWindow(window); // Update screen

        processEvents();

        if (SDL_GetTicks() < ttime) {
            SDL_Delay(ttime - SDL_GetTicks());
        }
    }
}

//// Setup OpenGL perspective
// static void setupOpengl() {
//    glViewport(0, 0, width, height);
//    glMatrixMode(GL_PROJECTION);
////    glEnable(GL_DEPTH_TEST);
//    glMatrixMode(GL_MODELVIEW);
//
//
//    //Alphablend
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
//
//    //Line antialiasing
//    glEnable( GL_LINE_SMOOTH );
//    glEnable( GL_POLYGON_SMOOTH );
//    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
//    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
//}

// Init everything
int main(int /*argc*/, char * /*argv*/ []) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        cerr << "failed to init video";
        return -1;
    }
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
    // SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    window = SDL_CreateWindow("rym",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width,
                              height,
                              SDL_WINDOW_OPENGL);
    if (!window) {
        cerr << "could not create window" << endl;
        return 1;
    }

    context = SDL_GL_CreateContext(window);
    if (!context) {
        throw runtime_error("could not create context");
    }
    glCall(cout << "test" << endl);

    SDL_GL_SetSwapInterval(1);

    // setupOpengl();
    initDrawModule(width / height);

    hant::init();

    mainLoop();

    //    hant::avsl();

    //    return 0;
}
