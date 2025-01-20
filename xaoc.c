#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

char const XAOC_HELP[] =
    "Usage: xaoc PROGRAM ARGS..."   "\n"
    ""                              "\n"
    "This program helps executing AoC solutions in conjuntion of a visualization" "\n"
    "program. It does the following:"                                             "\n"
    ""                                                                                   "\n"
    "  1. Sets the AOC_DISPLAY environment variable."                                    "\n"
    "  2. Reads the first line from PROGRAM to know which visualization program to use." "\n"
    "  3. The first line must start with \"visualizer: VISUALIZER\"."                    "\n" 
    "  4. Pipes the output to the visualization program."                                "\n"
    ""                                                                                   "\n"
    "Copyright (c) 2025 Harkaitz Agirre - All rights reserved." "\n";

int
main(int _argc, char *_argv[])
{
	int p[2] = {-1, -1};
	int ret = 1;
	int e, status, retcode, b;
	pid_t program = -1, visualizer = -1;
	char visualizer_cmd[256];
	size_t visualizer_cmd_len = 0;
	char *errno_msg = NULL;
	char *error_msg = NULL;

	if (_argc == 1 || !strcmp(_argv[1], "-h") || !strcmp(_argv[1], "--help")) {
		puts(XAOC_HELP);
		return 0;
	}

	setenv("AOC_DISPLAY", "0.1", 1);

	e = pipe(p);
	if (e == -1/*err*/) { errno_msg = "Can't create pipe."; goto cleanup; }

	program = fork();
	if (program == -1/*err*/) { errno_msg = "Can't fork program."; goto cleanup; }
	if (!program) {
		dup2(p[1], 1);
		close(p[0]);
		close(p[1]);
		execvp(_argv[1], _argv + 1);
		fprintf(stderr, "xaoc: error: Failed to execute %s: %s\n", _argv[1], strerror(errno));
		return 1;
	}
	close(p[1]); p[1] = -1;

	for(;;) {
		char c;
		b = read(p[0], &c, 1);
		if (b == -1) {
			errno_msg = "Failed reading from program.";
			goto cleanup;
		} else if (!b) {
			error_msg = "The program didn't respond with a visualizer.";
			goto cleanup;
		} else if (c == '\n') {
			visualizer_cmd[visualizer_cmd_len] = '\0';
			break;
		} else if (visualizer_cmd_len == sizeof(visualizer_cmd)) {
			error_msg = "The visualizer command is too long.";
			goto cleanup;
		} else {
			visualizer_cmd[visualizer_cmd_len++] = c;
		}
	}
	if (strncmp(visualizer_cmd, "visualizer: ", 12)/*err*/) {
		error_msg = "The program didn't respond with a visualizer.";
		goto cleanup;
	}
	visualizer = fork();
	if (visualizer == -1/*err*/) { errno_msg = "Can't fork visualizer program."; goto cleanup; }
	if (!visualizer) {
		dup2(p[0], 0);
		close(p[0]);
		execlp(visualizer_cmd+12, visualizer_cmd+12, NULL);
		fprintf(stderr, "xaoc: error: Failed to execute %s: %s\n", visualizer_cmd+12, strerror(errno));
		return 1;
	}
	close(p[0]); p[0] = -1;

	e = waitpid(program, &status, 0);
	if (e == -1/*err*/) { errno_msg = "Failed waiting the program."; goto cleanup; }
	program = -1;
	if (!WIFEXITED(status)/*err*/) { error_msg = "Unexpected program final."; goto cleanup; }
	retcode = WEXITSTATUS(status);
	if (retcode/*err*/) { error_msg = "The program returned failure."; goto cleanup; }

	e = waitpid(visualizer, &status, 0);
	if (e == -1/*err*/) { errno_msg = "Failed waiting the visualizer program."; goto cleanup; }
	visualizer = -1;
	if (!WIFEXITED(status)/*err*/) { error_msg = "Unexpected visualizer program final."; goto cleanup; }
	retcode = WEXITSTATUS(status);
	if (retcode/*err*/) { error_msg = "The visualizer returned failure."; goto cleanup; }

	ret = 0;
 cleanup:
	if (errno_msg) {
		fprintf(stderr, "xaoc: error: %s: %s\n", errno_msg, strerror(errno));
	}
	if (error_msg) {
		fprintf(stderr, "xaoc: error: %s\n", error_msg);
	}
	if (p[0] != -1) {
		close(p[0]);
	}
	if (p[1] != -1) {
		close(p[1]);
	}
	if (program != -1) {
		kill(program, SIGKILL);
		waitpid(program, NULL, 0);
	}
	if (visualizer != -1) {
		kill(visualizer, SIGKILL);
		waitpid(visualizer, NULL, 0);
	}
	return ret;
}
