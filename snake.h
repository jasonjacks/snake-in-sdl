/*

snake.h

*/

#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"
  
#define X_GRID_LENGTH 20
#define Y_GRID_LENGTH 20
#define BLOCK_SIZE 15
#define X_WINDOW_POS 200
#define Y_WINDOW_POS 50

void initialize_sdl_resources();
void initalize_game_state();
void generate_food();
void render_game_state();
void update_trajectory();
void detect_collisions();
void print_game_state();
struct snakeBlock *initializeHead();
struct snakeBlock *insertHead();
struct snakeBlock *removeTail();
int update_game_state();
int isSnakeBlock();
int poll_user_input();
int isDirection();

struct snakeBlock
{ 
  struct snakeBlock *next;
  short x;
  short y;
  short x_pix;
  short y_pix;
}; 
  
struct gameState 
{ 
  int user_input;
  unsigned char trajectory;
  unsigned char food_collision; 
  unsigned char wall_collision;

  struct foodBlock
  {
    short x;
    short y;
    short x_pix;
    short y_pix;
  }food;

  struct snakeBlock *head;
  struct snakeBlock *tail;
};
  
enum snakeTrajectories { RIGHT, UP, LEFT, DOWN };
