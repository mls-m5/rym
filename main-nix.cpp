#include <stdlib.h>
#include "draw.h"
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>

#include <iostream>
#include "glfunctions.h"
using namespace std;

#include "graf.h"
 
using namespace std;
 
// Width & Height of window
#define width 800// 640
#define height 600 //480
#define framerate 50 //Hz min egen variable
#define frametime = 1/framerate + 1000 // Hur länge varje bild ska synas

SDL_Window *window = 0;
SDL_GLContext context;


// Kill program
void endProgram(int code) {SDL_Quit();    exit(code);}
 
// Handle SDL keypresses
void handleKeys(SDL_Keysym* keysym, bool state) {
    switch(keysym->sym) {
        case SDLK_ESCAPE:    endProgram(0); break;
				default: hant::setkey((int)keysym->sym, state); break;
    }
}
 
// Process SDL events
void processEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_KEYDOWN:    handleKeys(&event.key.keysym, true ); break;
            case SDL_KEYUP  : handleKeys(&event.key.keysym, false);    break;
						
            case SDL_QUIT   : endProgram(0); break;
        }
    }
}
 
void mainLoop() {
		//long ttime;
    while(true) {
				
    	//ttime = SDL_GetTicks() + frametime;
    	long ttime =  15 + SDL_GetTicks();
        processEvents();
				
				
        game::Update(.1);
 
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Clear color and depth buffer
        
        game::Render();
 
        SDL_GL_SwapWindow(window); // Update screen
				
        processEvents();

        if (SDL_GetTicks() < ttime) {
        	SDL_Delay(ttime - SDL_GetTicks());
        }
    }
}
 
//// Setup OpenGL perspective
//static void setupOpengl() {
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
int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		cerr << "failed to init video";
		return -1;
	}
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);



    window = SDL_CreateWindow("rym", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    if (!window) {
    	cerr << "could not create window" << endl;
    	return 1;
    }


    context = SDL_GL_CreateContext(window);
    if (!context) {
    	throw runtime_error("could not create context");
    	return 1;
    }
    glCall(cout << "test" << endl);

    SDL_GL_SetSwapInterval(1);

    //setupOpengl();
	initDrawModule(width / height);
		
    hant::init();
		
    mainLoop();
		
    hant::avsl();
		
    return 0;
}

