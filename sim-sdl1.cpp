#include "PatternMixer.h"

#include <unistd.h>
#include <stdlib.h>
#if defined(_MSC_VER)
#include "SDL.h"
#else
#include "SDL/SDL.h"
#endif

SDL_Surface *screen;

#define LEDXSIZE 20
#define LEDYSIZE 40

void render(PatternMixer &pm, uint8_t *buffer)
{   
  //  pm.draw(buffer);
  
  // Lock surface if needed
  if (SDL_MUSTLOCK(screen)) 
    if (SDL_LockSurface(screen) < 0) 
      return;

  // Ask SDL for the time in milliseconds
  //int tick = SDL_GetTicks();

  int yofs = 0;
  for (int y = LedsY - 1; y >= 0; --y) {
    for (int yp = 0; yp < LEDYSIZE; ++yp) {
      int xofs = 0;
      for (int x = 0; x < (int)LedsX; ++x) {
	for (int xp = 0; xp < LEDXSIZE; ++xp) {
	  int off = (y * LedsX + x) * 3;
	  ((uint8_t *)screen->pixels)[yofs + xofs++] = buffer[off++];
	  ((uint8_t *)screen->pixels)[yofs + xofs++] = buffer[off++];
	  ((uint8_t *)screen->pixels)[yofs + xofs++] = buffer[off++];
	  xofs++;
	}
      }
      yofs += screen->pitch;
    }
  }


  // Unlock if needed
  if (SDL_MUSTLOCK(screen)) 
    SDL_UnlockSurface(screen);

  // Tell SDL to update the whole screen
  SDL_UpdateRect(screen, 0, 0, LedsX * LEDXSIZE, LedsY * LEDYSIZE);    
}


// Entry point
int main(int argc, char *argv[])
{
  // Initialize SDL's subsystems - in this case, only video.
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) 
  {
    fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
    exit(1);
  }

  // Register SDL_Quit to be called at exit; makes sure things are
  // cleaned up when we quit.
  atexit(SDL_Quit);
    
  // Attempt to create a 640x480 window with 32bit pixels.
  screen = SDL_SetVideoMode(LedsX * LEDXSIZE, LedsY * LEDYSIZE, 32, SDL_SWSURFACE);
  
  // If we fail, return error.
  if ( screen == NULL ) 
  {
    fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
    exit(1);
  }

  uint8_t *buffer;
  buffer = new uint8_t[LedsX * LedsY * 3];

  PatternMixer pm;

  // Main loop: loop forever.
  while (1)
  {
    // Render stuff
    pm.draw(buffer);
    render(pm, buffer);
    usleep(30000);

    // Poll for events, and handle the ones we care about.
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) 
    {
      switch (event.type) 
      {
      case SDL_KEYDOWN:
        break;
      case SDL_KEYUP:
        // If escape is pressed, return (and thus, quit)
        if (event.key.keysym.sym == SDLK_ESCAPE)
          return 0;
        if (event.key.keysym.sym == SDLK_SPACE)
          pm.next();
        break;
      case SDL_QUIT:
        return(0);
      }
    }
    
  }
  return 0;
}
