#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

#ifdef DEBUG
#include <alloca.h>
#include <sys/uio.h>

void fdprintv(int fd, const char **args) {
	int len = 0;
	for (; args[len] != NULL; len++) {}
	struct iovec *vec = alloca(sizeof(struct iovec) * len);
	for (int i = 0; i < len; i++) {
		vec[i] = (struct iovec) {(void *) args[i], strlen(args[i])};
	}
	writev(fd, vec, len);
}
#define logln(...) fdprintv(2, (const char *[]){__VA_ARGS__, "\n", NULL})
#else
#define logln(...)
#endif

char* itoa(int val, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) return "";
	static char result[32] = {0};
	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_val;

	do {
		tmp_val = val;
		val /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_val - val * base)];
	} while ( val );

	// Apply negative sign
	if (tmp_val < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

#define cargs(...) ((char *[]){__VA_ARGS__, NULL})

int main(int argc, char **argv) {
	chdir(getenv("HOME"));
	logln("Read arguments and environment...");
	char *audiofile = getenv("BG_THEME");
	if (audiofile == NULL)
		audiofile = audiofile_default;
	char *pidfile = getenv("PIDFILE");
	if (pidfile == NULL)
		pidfile = pidfile_default;
	char *switchfile = getenv("SWITCH");
	if (switchfile == NULL)
		switchfile = switchfile_default;
	logln("Audio: ", audiofile);
	logln("PIDfile: ", pidfile);
	logln("Read switchfile ", switchfile, "...");
	char buffer;
	int fd = open(switchfile, 0);
	int rl = read(fd, &buffer, 1);
	if (rl == 0)
		buffer = '1';
	close(fd);
	switch (buffer) {
		case '0':
			logln("Read '0' from switchfile. Proceeding to kill mpv...");
			fd = open(switchfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			write(fd, "1", 1);
			close(fd);
			fd = open(pidfile, 0);
			if (fd < 0)
				break;
			char pid_from_file[21];
			rl = read(fd, pid_from_file, 21);
			close(fd);
			if (rl == 0)
				break;
			pid_from_file[rl] = 0;
			kill(atoi(pid_from_file), SIGTERM);
			unlink(pidfile);
			break;
		case '1':
		default:
			logln("Read '1' from switchfile. Proceeding to start mpv...");
			fd = open(switchfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			write(fd, "0", 1);
			close(fd);
			// TODO pidfile
			pid_t child = fork();
			if (child == 0) {
				fd = open(pidfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				char *pid_string = itoa(getpid(), 10);
				write(fd, pid_string, strlen(pid_string));
				close(fd);
				logln("New PID: ", pid_string);
				execvp("mpv", cargs("mpv", "--no-terminal", "--loop", audiofile, NULL));
				// Should never happen
				logln("Failed to start mpv.");
				_exit(192);
			} else if (child < 0) {
				logln("Failed to start mpv.");
				return 192;
			}
			break;
	}
}
