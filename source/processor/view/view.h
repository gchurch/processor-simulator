//=================================
// include guard
#ifndef VIEW_H
#define VIEW_H

//=================================
//forward declared dependencies
class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
class SDL_Texture;


//=================================
// included dependencies
#include <string>
#include <SDL2/SDL_ttf.h>

#include "lTexture.h"

//=================================
// interface

class View {
 
	//Screen dimension constants
    const int SCREEN_WIDTH;
    const int SCREEN_HEIGHT;

    //The window we'll be rendering to
    SDL_Window* gWindow;

    //The window renderer
    SDL_Renderer* gRenderer;

    //Globally used font
    TTF_Font *gFont;

    //Rendered texture
    LTexture gTextTexture;

    //Main loop flag
	bool quit;

    //processor information
    int numOfRegisters;
    int memorySize;

  public:

    //Constructor
    View();

    //Starts up SDL and creates window
    bool init();

    //Loads media
    bool loadMedia();

    //Frees media and shuts down SDL
    void close();

    void renderText(int x, int y, std::string text);

    //render the current frame
    void frame();

    //handle events
    void eventHandler();

    void drawRegisterTable();

    void drawRegisterFile(int* registerValues);

    void drawMemory(int* memoryValues);

    void clearScreen();

    void updateScreen();

    //============================================
    // getter functions

    int hasQuit();

    //============================================
    // setter functions

    void setNumOfRegisters(int n);

    void setMemorySize(int n);
};

#endif