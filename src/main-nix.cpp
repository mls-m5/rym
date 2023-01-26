
import graf;
import draw;
import hant;
import graf;
import glapi;

import <GL/gl.h>;
import <SDL2/SDL.h>;
import <iostream>;
import <stdexcept>;

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

        gl.glClear(GL_DEPTH_BUFFER_BIT |
                   GL_COLOR_BUFFER_BIT); // Clear color and depth buffer

        game::Render();

        SDL_GL_SwapWindow(window); // Update screen

        processEvents();

        if (SDL_GetTicks() < ttime) {
            SDL_Delay(ttime - SDL_GetTicks());
        }
    }
}

// Init everything
int main(int /*argc*/, char * /*argv*/[]) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "failed to init video\n";
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
        std::cerr << "could not create window\n";
        return 1;
    }

    context = SDL_GL_CreateContext(window);
    if (!context) {
        throw runtime_error("could not create context");
    }

    SDL_GL_SetSwapInterval(1);

    // setupOpengl();
    initDrawModule(width / height);

    game::init();

    mainLoop();

    game::avsl();

    return 0;
}
