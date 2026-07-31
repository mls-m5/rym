
import graf;
import draw;
import hant;
import graf;
import glapi;

import <GL/gl.h>;
import <SDL2/SDL.h>;
import <iostream>;
import <stdexcept>;
#ifdef __EMSCRIPTEN__
import <emscripten/emscripten.h>;
import <emscripten/html5.h>;
#endif

using namespace std;

// Width & Height of window
const int width = 800;  // 640
const int height = 600; // 480

static SDL_Window *window = nullptr;
static SDL_GLContext context;

#ifdef __EMSCRIPTEN__
EM_BOOL resizeCanvas(int, const EmscriptenUiEvent *, void *) {
    double cssWidth = 0;
    double cssHeight = 0;
    emscripten_get_element_css_size("#canvas", &cssWidth, &cssHeight);

    const double pixelRatio = emscripten_get_device_pixel_ratio();
    const int drawableWidth = static_cast<int>(cssWidth * pixelRatio);
    const int drawableHeight = static_cast<int>(cssHeight * pixelRatio);

    emscripten_set_canvas_element_size(
        "#canvas", drawableWidth, drawableHeight);
    glViewport(0, 0, drawableWidth, drawableHeight);

    if (cssHeight > 0) {
        resizeDrawModule(cssWidth / cssHeight);
    }

    return true;
}
#endif

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

void runFrame() {
#ifndef __EMSCRIPTEN__
    Uint32 ttime = 15 + SDL_GetTicks();
#endif
    processEvents();

#ifdef __EMSCRIPTEN__
    static double previousFrameTime = 0;
    const double currentFrameTime = emscripten_get_now();
    double elapsedMilliseconds =
        previousFrameTime == 0 ? 15 : currentFrameTime - previousFrameTime;
    previousFrameTime = currentFrameTime;

    // Avoid a huge simulation jump after the tab has been suspended.
    if (elapsedMilliseconds > 50) {
        elapsedMilliseconds = 50;
    }

    // The original desktop loop advances 0.1 simulation units per 15 ms.
    game::Update(0.1 * elapsedMilliseconds / 15.0);
#else
    game::Update(.1);
#endif

    gl.glClear(GL_DEPTH_BUFFER_BIT |
               GL_COLOR_BUFFER_BIT); // Clear color and depth buffer

    game::Render();

    SDL_GL_SwapWindow(window); // Update screen

    processEvents();

#ifndef __EMSCRIPTEN__
    if (SDL_GetTicks() < ttime) {
        SDL_Delay(ttime - SDL_GetTicks());
    }
#endif
}

#ifndef __EMSCRIPTEN__
[[noreturn]] void mainLoop() {
    while (true) {
        runFrame();
    }
}
#endif

// Init everything
int main(int /*argc*/, char * /*argv*/[]) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "failed to init video\n";
        return -1;
    }
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif

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

#ifndef __EMSCRIPTEN__
    SDL_GL_SetSwapInterval(1);
#endif

    // setupOpengl();
    initDrawModule(static_cast<double>(width) / height);

    game::init();

#ifdef __EMSCRIPTEN__
    emscripten_set_resize_callback(
        EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, false, resizeCanvas);
    resizeCanvas(0, nullptr, nullptr);
    emscripten_set_main_loop(runFrame, 0, true);
#else
    mainLoop();
#endif

    game::avsl();

    return 0;
}
