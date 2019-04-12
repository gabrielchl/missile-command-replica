#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <ncurses.h>
#include <math.h>
#include <pthread.h>

#define FRAME_WIDTH 124
#define FRAME_HEIGHT 40

#define START_PADDING_HORIZONTAL 30
#define START_PADDING_VERTICAL 12

extern pthread_mutex_t lock;

extern int start_explosion_pos[80][2];
extern char *STAGE_1, *STAGE_2;

enum drawMode {ERASE, DRAW};

extern WINDOW *carousel_thread_screen;
extern int carousel_thread_live;
extern int carousel_thread_start_x, carousel_thread_end_x, carousel_thread_y;
extern char *carousel_thread_text;
extern int carousel_thread_color_pair;

void drawFromFile(WINDOW *screen, int start_x, int start_y, char file[], enum drawMode mode);

void drawFromString(WINDOW *screen, int start_x, int start_y, char *line, enum drawMode mode);

void updateSmallExplosionStage(WINDOW *screen, int from_missile, int to_missile, int color);

void refreshHighScore(WINDOW *screen, int cur_score, int high_score);

void *carouselFromString(void *argument);

#endif