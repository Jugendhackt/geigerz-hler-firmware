#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#define BAUDRATE B9600
#define SEND_TIME 60

void printhelp(char *command)
{
	printf("Usage: %s [uartdevice]\n", command);
}

void timer_init(struct timeval tv, time_t sec)
{
	gettimeofday(&tv, NULL);
	tv.tv_sec += sec;
}

int check_timer(struct timeval tv, time_t sec)
{
	struct timeval ct;
	gettimeofday(&ct, NULL);

	if (ct.tv_sec > tv.tv_sec) {
		timer_init(tv, sec);
		return 1;
	}

	return -1;
}

int main(int argc, char *argv[])
{
	int fd, cps, cpm;
	struct termios tp;
	char buffer[100];
	ssize_t length;
	float usvhr;
	struct timeval tv;

	if (argc != 2) {
		printhelp(argv[0]);
		return 1;
	}

	fd = open(argv[1], O_RDONLY | O_NOCTTY |O_NDELAY);
	if (fd < 0) {
		perror(argv[1]);
	}

	fcntl(fd, F_SETFL, 0);
	bzero(&tp, sizeof(tp));

	tp.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	tp.c_iflag = IGNPAR | ICRNL;
	tp.c_lflag = ICANON;

	tcsetattr(fd, TCSANOW, &tp);

	timer_init(tv, SEND_TIME);

	for (;;) {
		length = read(fd, &buffer, sizeof(buffer));

		if (length < 0) {
			fprintf(stderr, "Error opening device: %s\n", argv[1]);
			return -1;
		}
		if (length > 0) {
			buffer[length] = '\0';
			sscanf(buffer, "CPS, %d, CPM, %d, uSv/hr, %f", &cps, &cpm, &usvhr);
			printf("cps: %d\ncpm: %d\nuSv/hr: %f\n", cps, cpm, usvhr);
		}
		if (check_timer(tv, SEND_TIME)) {
			/* send a packet */
		}
	}

	return 1;
}
