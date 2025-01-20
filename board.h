#ifndef H_BOARD_H
#define H_BOARD_H

#include <pthread.h>
#include <stdbool.h>
#include <wchar.h>

#ifndef BOARD_MAX_X
#  define BOARD_MAX_X 2000
#endif
#ifndef BOARD_MAX_Y
#  define BOARD_MAX_Y 2000
#endif
#ifndef BOARD_MAX_POINTS
#  define BOARD_MAX_POINTS (BOARD_MAX_X*BOARD_MAX_Y)
#endif
#ifndef BOARD_MAX_SKINS
#  define BOARD_MAX_SKINS 32
#endif


typedef struct skin_s  skin_t;
typedef struct board_s board_t;
typedef struct point_s point_t;
typedef char const err_t;

typedef enum color_e {
     COLOR_NONE = 0x00,
     COLOR_LIGHTGRAY, COLOR_GRAY,   COLOR_DARKGRAY,  COLOR_YELLOW,
     COLOR_GOLD,      COLOR_ORANGE, COLOR_PINK,      COLOR_RED,
     COLOR_MAROON,    COLOR_GREEN,  COLOR_LIME,      COLOR_DARKGREEN,
     COLOR_SKYBLUE,   COLOR_BLUE,   COLOR_DARKBLUE,  COLOR_PURPLE,
     COLOR_VIOLET,    COLOR_DARKPURPLE, COLOR_BEIGE, COLOR_BROWN,
     COLOR_DARKBROWN, COLOR_WHITE,  COLOR_BLACK,     COLOR_TRANSPARENT,
     COLOR_MAGENTA,   COLOR_RAYWHITE
} color_t;

struct skin_s {
     wchar_t chr;
     color_t fg_color;
     color_t bg_color;
};

struct point_s {
     int           x,y;
     const skin_t *skin;
};

struct board_s {
     pthread_mutex_t lock;
     point_t         points[BOARD_MAX_POINTS];
     size_t          pointsz;
     skin_t          skins[BOARD_MAX_SKINS];
     size_t          skinsz;
     int             max_x, max_y;
     int             width, height;
     color_t         bg_color;
     int             pad_px;
     bool            has_changes;
};

extern void board_init(board_t *_board);
extern void board_clean(board_t *_board);
extern void board_lock(board_t *_board);
extern bool board_trylock(board_t *_board);
extern void board_unlock(board_t *_board);

extern int raylib_x(const board_t *_board, int _x);
extern int raylib_y(const board_t *_board, int _y);

extern const skin_t *board_get_skin(const board_t *_board, wchar_t _chr);

extern err_t *board_set_line(board_t *_board, int _y, wchar_t _line[]);
extern void   board_finish(board_t *_board, int _y);

extern err_t *board_set_config1(board_t *_board, wchar_t _line[]);
extern err_t *board_set_config2(board_t *_board, skin_t *_skin, wchar_t _key[], wchar_t _val[]);





extern void board_draw(board_t *_board);

#endif
