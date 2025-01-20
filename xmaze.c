#include <stdio.h>
#include <raylib.h>
#include <pthread.h>
#include "board.h"

board_t board = {0};

static void *
stdin_thread(void *_ign)
{
	wchar_t buffer[1024*10] = {0}, *c;
	int y = 0;
	bool locked = 0;
	
	while ((c = fgetws(buffer, sizeof(buffer)-1, stdin))) {
		switch (*c) {
		case L'|':
			for (c++; *c == L' '; c++) {}
			if (!locked) {
				board_lock(&board);
				locked = true;
			}
			board_set_line(&board, y++, c);
			break;
		case L'.':
			if (locked) {
				board_finish(&board, y);
				locked = false;
				board_unlock(&board);
			}
			y = 0;
			break;
		default:
			fputws(buffer, stdout);
			continue;
		}
	}

	pthread_exit(NULL);
	return NULL;
}

static void *
raylib_thread(void *_ign)
{
	SetTraceLogLevel(LOG_ERROR); 
	InitWindow(board.width, board.height, "XMAZE");
	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		BeginDrawing();
		if (board_trylock(&board)) {
			board_draw(&board);
			board_unlock(&board);
		}
		EndDrawing();
	}
	CloseWindow();
	pthread_exit(NULL);
	return NULL;
}

int
main(int _argc, char *_argv[])
{
	pthread_t threads[2];
	board_init(&board);
	pthread_create(&threads[0], NULL, stdin_thread, NULL);
	pthread_create(&threads[1], NULL, raylib_thread, NULL);
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	board_clean(&board);
	return 0;
}




