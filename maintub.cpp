#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>

#include "graf.h"
 
using namespace std;
 
// Width & Height of window
#define width 800// 640
#define height 600 //480
#define framerate 50 //Hz min egen variable
#define frametime = 1/framerate + 1000 // Hur länge varje bild ska synas



// Kill program
void endProgram(int code) {SDL_Quit();    exit(code);}
 
// Handle SDL keypresses
void handleKeys(SDL_keysym* keysym, bool state) {
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
				SDL_GetTicks();
				long ttime =  20 + SDL_GetTicks();
        processEvents();
				
				
        graf::Tid(.1);
 
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Clear color and depth buffer
				glLoadIdentity();
        
        graf::Rendera();
 
        SDL_GL_SwapBuffers(); // Update screen
				
				while (SDL_GetTicks() < ttime) {processEvents();}
        // Increase degree of rotation to spin QUAD
 
    }
}
 
// Setup OpenGL perspective
void setupOpengl() {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
//    glEnable(GL_DEPTH_TEST);
    gluPerspective(45, (float)width/height, .1, 100);
    glMatrixMode(GL_MODELVIEW);
		
		
    //Alphablend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
}
 
// Init everything
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetVideoMode(width, height, 24, SDL_OPENGL | SDL_GL_DOUBLEBUFFER);
    setupOpengl();
		
    hant::init();
		
    mainLoop();
		
    hant::avsl();
		
    return 0;
}

