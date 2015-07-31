/*

snake.c

*/

#include "snake.h"
#define UPDATE_READY_VALUE 50000

main()
{
  initialize_sdl_resources();
  initialize_game_state(); 
  render_game_state();
 
  int key = NULL;
  do
    key = poll_user_input(key);
  while(!isDirection(key));
  
  int quit, game_over; 
  quit = game_over = 0;
  unsigned long update_counter = UPDATE_READY_VALUE;
  while(!quit && !game_over)
  {
    if (update_counter == UPDATE_READY_VALUE)
    { 
      update_counter = 0;
      game_over = update_game_state(key);
      if(!game_over)
        render_game_state();
    }
    key = poll_user_input(key);
    if (key == SDLK_RETURN)
      quit = 1;
    update_counter++;
  }
  
  SDL_Quit();
}
