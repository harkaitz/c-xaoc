#include "board.h"
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#if 1
#  include <raylib.h>
#endif

static skin_t default_skins[] = {
	{ L'#', COLOR_NONE,  COLOR_DARKGRAY },
	{ L'S', COLOR_GREEN, COLOR_NONE     },
	{ L'E', COLOR_RED,   COLOR_NONE     },
	{ L'O', COLOR_BLUE,  COLOR_NONE     },
	{ L'\0'}
};

void
board_init(board_t *_board)
{
	memset(_board, 0, sizeof(board_t));
	pthread_mutex_init(&_board->lock, NULL);
	_board->width = 1000;
	_board->height =1000;
	for (int i=0; default_skins[i].chr; i++) {
		_board->skins[_board->skinsz++] = default_skins[i];
	}
	_board->pad_px = 0;
}

void
board_clean(board_t *_board)
{
}

void
board_lock(board_t *_board)
{
	pthread_mutex_lock(&_board->lock);
}

bool
board_trylock(board_t *_board)
{
	return (pthread_mutex_trylock(&_board->lock)!=EBUSY);
}

void
board_unlock(board_t *_board)
{
	pthread_mutex_unlock(&_board->lock);
}

int
raylib_x(const board_t *_board, int _x)
{
	return (_x * _board->width) / _board->max_x;
}

int
raylib_y(const board_t *_board, int _y)
{
	return (_y * _board->height) / _board->max_y;
}

const skin_t *
board_get_skin(const board_t *_board, wchar_t _chr)
{
	for (int i=0; i<_board->skinsz; i++) {
		if (_board->skins[i].chr == _chr) {
			return _board->skins+i;
		}
	}
	return NULL;
}

err_t *
board_set_line(board_t *_board, int _y, wchar_t *_line)
{
	int           x;
	wchar_t      *chr;
	skin_t const *skin;
	size_t        sz;

	for (x = 0, chr = _line, sz = _board->pointsz;
	     *chr && (*chr != L'\n') && (*chr != L'\r');
	     x++, chr++
	) {

		if (x > _board->max_x) {
			_board->max_x = x;
		}

		skin = board_get_skin(_board, *chr);
		if (!skin) {
			continue;
		}

		if (sz == BOARD_MAX_POINTS) {
			return "Maximun points reached.";
		}
		
		_board->points[sz].x = x;
		_board->points[sz].y = _y;
		_board->points[sz].skin = skin;
		sz++;

	}

	_board->pointsz = sz;

	return NULL;
}

void
board_finish(board_t *_board, int _y)
{
	_board->max_y = _y;
	_board->has_changes = true;
}

err_t *
board_set_config1(board_t *_board, wchar_t _line[])
{
	wchar_t *key, *key_r, *val;
	err_t *err;
	skin_t *skin = NULL;

	for (
	    key = wcstok(_line, L" \r\n", &key_r);
	    key;
	    key = wcstok(NULL, L" \r\n", &key_r)
	) {
		if (!(val = wcschr(key, L'='))) {
			if (!wcscmp(key, L"@")) {
				skin = (skin_t*)board_get_skin(_board, val[0]);
			} else if (_board->skinsz == BOARD_MAX_SKINS) {
				return "Maximun skins reached.";
			} else {
				skin = &_board->skins[_board->skinsz++];
				skin->chr = val[0];
			}
			continue;
		}
		*val = '\0';
		val++;
		err = board_set_config2(_board, skin, key, val);
		if (!err/*err*/) { return err; }
	}

	return NULL;
}

err_t *
board_set_config2(board_t *_board, skin_t *_skin, wchar_t _key[], wchar_t _val[])
{
	return NULL;
}

static Color
raylib_color(color_t _c)
{
	static const Color colors[] = {
		RAYWHITE,
		LIGHTGRAY, GRAY,   DARKGRAY,  YELLOW,
		GOLD,      ORANGE, PINK,      RED,
		MAROON,    GREEN,  LIME,      DARKGREEN,
		SKYBLUE,   BLUE,   DARKBLUE,  PURPLE,
		VIOLET,    DARKPURPLE, BEIGE, BROWN,
		DARKBROWN, WHITE,  BLACK,     BLANK,
		MAGENTA,   RAYWHITE
	};
	return colors[_c];
}


void
board_draw(board_t *_board)
{
	if (_board->has_changes) {
		ClearBackground(raylib_color(_board->bg_color));
		for (size_t i = 0; i<_board->pointsz; i++) {
			point_t *p = _board->points+i;
			if (p->skin->bg_color != COLOR_NONE) {
				DrawRectangle(
				    raylib_x(_board, p->x)+_board->pad_px,
				    raylib_y(_board, p->y)+_board->pad_px,
				    raylib_x(_board, 1)-(_board->pad_px*2)+1,
				    raylib_y(_board, 1)-(_board->pad_px*2)+1,
				    raylib_color(p->skin->bg_color)
				);
			} else if (p->skin->fg_color != COLOR_NONE) {
				DrawRectangle(
				    raylib_x(_board, p->x)+_board->pad_px,
				    raylib_y(_board, p->y)+_board->pad_px,
				    raylib_x(_board, 1)-(_board->pad_px*2)+1,
				    raylib_y(_board, 1)-(_board->pad_px*2)+1,
				    raylib_color(p->skin->fg_color)
				);
			}
		}
		_board->pointsz = 0;
		_board->max_x = 0;
		_board->max_y = 0;
		_board->has_changes = false;
	}
}
