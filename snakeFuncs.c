/*

snakeFuncs.c

*/

#include "snake.h"

SDL_Window *w;
SDL_Renderer *r;
SDL_Rect block;
SDL_Event e;
struct gameState g;
int key_right = SDLK_d;
int key_up    = SDLK_w;
int key_left  = SDLK_a;
int key_down  = SDLK_s;

void initialize_sdl_resources()
{ 
  short x_pixels = X_GRID_LENGTH*BLOCK_SIZE;
  short y_pixels = Y_GRID_LENGTH*BLOCK_SIZE;
  //create resources
  SDL_Init(SDL_INIT_VIDEO);
  w = SDL_CreateWindow("Snake", X_WINDOW_POS, Y_WINDOW_POS, x_pixels, y_pixels, 0);
  r = SDL_CreateRenderer(w, -1, 0);
  //set black background
  SDL_SetRenderDrawColor(r, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(r, NULL);
  //fill borders 
  SDL_SetRenderDrawColor(r, 255, 0, 0, SDL_ALPHA_OPAQUE);
  //--top
  block.x = 0;
  block.y = 0;
  block.w = x_pixels;
  block.h = BLOCK_SIZE;
  SDL_RenderFillRect(r, &block);
  //--bottom
  block.y = y_pixels - BLOCK_SIZE;
  SDL_RenderFillRect(r, &block);
  //--left
  block.y = 0;
  block.w = BLOCK_SIZE;
  block.h = y_pixels;
  SDL_RenderFillRect(r, &block);
  //--right
  block.x = x_pixels - BLOCK_SIZE;
  SDL_RenderFillRect(r, &block); 
  //default block size
  block.w = BLOCK_SIZE;
  block.h = BLOCK_SIZE;
}

void initialize_game_state()
{
  g.head = initializeHead();
  g.tail = g.head;
  g.trajectory = 99;
  g.food_collision = 0;
  g.wall_collision = 0;
  generate_food();
}

struct snakeBlock *initializeHead()
{
  struct snakeBlock *ptr = (struct snakeBlock *)malloc(sizeof(struct snakeBlock));
  ptr->next = NULL;
  ptr->x = X_GRID_LENGTH/2;
  ptr->y = Y_GRID_LENGTH/2;
  ptr->x_pix = ptr->x*BLOCK_SIZE;
  ptr->y_pix = ptr->y*BLOCK_SIZE;
  return ptr;
}

void generate_food()
{
  //assumption: square grid
  unsigned int maxVal = X_GRID_LENGTH - 3; 
  unsigned int bucketSize = RAND_MAX/maxVal;
 
  int rx;
  int ry; 
  do
  {
    rx = rand();
    ry = rand();
    rx = rx/bucketSize;
    ry = ry/bucketSize;
    rx++;   
    ry++;
  }while(isSnakeBlock(rx, ry, g.tail));
 
  g.food.x = rx;
  g.food.y = ry;
  g.food.x_pix = g.food.x*BLOCK_SIZE;
  g.food.y_pix = g.food.y*BLOCK_SIZE;
}

int isSnakeBlock(int nx, int ny, struct snakeBlock *t)
{
  do
    if (t->x == nx && t->y == ny)
      return 1;
  while((t = t->next) != NULL);

  return 0;
}

void render_game_state()
{  
  //render food block
  SDL_SetRenderDrawColor(r, 255, 165, 0, SDL_ALPHA_OPAQUE);
  block.x = g.food.x_pix;
  block.y = g.food.y_pix;
  SDL_RenderFillRect(r, &block);

  //render snake blocks
  SDL_SetRenderDrawColor(r, 0, 255, 0, SDL_ALPHA_OPAQUE);
  struct snakeBlock *iter;
  iter = g.tail;
  while(iter != NULL)
  {
    block.x = iter->x_pix;
    block.y = iter->y_pix;
    SDL_RenderFillRect(r, &block);
    iter = iter->next;
  }

  //update view
  SDL_RenderPresent(r);
}

int poll_user_input(int key)
{
  if(SDL_PollEvent(&e))
    if(e.type == SDL_KEYDOWN)
      return e.key.keysym.sym;

  return key;
}

int isDirection(int key)
{
  return ((key == key_right) || (key == key_up) || (key == key_left) || (key == key_down));
}

struct snakeBlock *insertHead(struct snakeBlock *h)
{
  struct snakeBlock *ptr = (struct snakeBlock *)malloc(sizeof(struct snakeBlock));
  ptr->next = NULL;
  ptr->x = h->x;
  ptr->y = h->y;
  if(g.trajectory == UP)
    ptr->y--;
  if(g.trajectory == DOWN)
    ptr->y++;
  if(g.trajectory == RIGHT)
    ptr->x++;
  if(g.trajectory == LEFT)
    ptr->x--;

  ptr->x_pix = ptr->x*BLOCK_SIZE;
  ptr->y_pix = ptr->y*BLOCK_SIZE;
  h->next = ptr;
  return ptr;
}

struct snakeBlock *removeTail(struct snakeBlock *p)
{ 
  block.x = p->x_pix;
  block.y = p->y_pix;
  SDL_SetRenderDrawColor(r, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(r, &block);
  struct snakeBlock *t = p->next;
  free(p);
  return t;
}

int update_game_state(int key)
{
  update_trajectory(key);
  detect_collisions();
  if(g.wall_collision)
    return 1; 
  g.head = insertHead(g.head);
  if(g.food_collision)
  {
    g.food_collision = 0;
    generate_food();
  }
  else
    g.tail = removeTail(g.tail);

  return 0;
}

void update_trajectory (int key)
{
  if (key == key_right && g.trajectory != LEFT)
      g.trajectory = RIGHT;
  if (key == key_left && g.trajectory != RIGHT)
      g.trajectory = LEFT;
  if (key == key_up && g.trajectory != DOWN)
      g.trajectory = UP;
  if (key == key_down && g.trajectory != UP)
      g.trajectory = DOWN;
}

void detect_collisions()
{ 
  //wall collision
  if (g.head->x > X_GRID_LENGTH-2 || g.head->x < 1 || g.head->y > Y_GRID_LENGTH-2 || g.head->y < 1)
    g.wall_collision = 1;

  //self collision -- using same flag
  if (g.trajectory == RIGHT)
    g.wall_collision = isSnakeBlock(g.head->x+1, g.head->y, g.tail);
  else if (g.trajectory == UP)
    g.wall_collision = isSnakeBlock(g.head->x, g.head->y-1, g.tail);
  else if (g.trajectory == LEFT)
    g.wall_collision = isSnakeBlock(g.head->x-1, g.head->y, g.tail);
  else if (g.trajectory == DOWN)
    g.wall_collision = isSnakeBlock(g.head->x, g.head->y+1, g.tail);
  
  //food collision
  if (g.head->x == g.food.x && g.head->y == g.food.y)
    g.food_collision = 1;
}

void print_game_state()
{
  printf("trajectory: %d\n",                  g.trajectory);
  printf("food_collision: %d\n",              g.food_collision);
  printf("wall_collision: %d\n",              g.wall_collision);
  printf("food x,y: %d,%d\n",                 g.food.x, g.food.y);
  printf("food x_pix, y_pix: %d,%d\n",        g.food.x_pix, g.food.y_pix);
  printf("snake tail x,y: %d, %d\n",          g.tail->x, g.tail->y);
  printf("snake tail x_pix, y_pix: %d, %d\n", g.tail->x_pix, g.tail->y_pix);
  printf("snake head x,y: %d, %d\n",          g.head->x, g.head->y);
  printf("snake head x_pix, y_pix: %d, %d\n", g.head->x_pix, g.head->y_pix);
  printf("g.head->next == NULL: %d\n",        g.head->next == NULL);  
  printf("g.tail->next == NULL: %d\n",        g.tail->next == NULL);  
}
