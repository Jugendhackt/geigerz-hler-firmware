#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#define GGZ_BAUDRATE B9600
#define GPS_BAUDRATE B115200
#define SEND_TIME 60

void printhelp(char *command)
{
	printf("Usage: %s [geigerzaehler] [gpsdongle]\n", command);
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
	int geigerzaehler, gps, cps, cpm;
	struct termios tp;
	char buffer[100];
	ssize_t length;
	float usvhr, lon, lat;
	struct timeval tv;

	if (argc != 3) {
		printhelp(argv[0]);
		return 1;
	}

	geigerzaehler = open(argv[1], O_RDONLY | O_NOCTTY |O_NDELAY);
	if (geigerzaehler < 0) {
		perror(argv[1]);
	}

	fcntl(geigerzaehler, F_SETFL, 0);
	bzero(&tp, sizeof(tp));

	tp.c_cflag = GGZ_BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	tp.c_iflag = IGNPAR | ICRNL;
	tp.c_lflag = ICANON;

	tcsetattr(geigerzaehler, TCSANOW, &tp);

	gps = open(argv[2], O_RDONLY | O_NOCTTY |O_NDELAY);
	if (gps < 0) {
		perror(argv[2]);
	}

	fcntl(gps, F_SETFL, 0);
	bzero(&tp, sizeof(tp));

	tp.c_cflag = GPS_BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	tp.c_iflag = IGNPAR | ICRNL;
	tp.c_lflag = ICANON;

	tcsetattr(gps, TCSANOW, &tp);

	timer_init(tv, SEND_TIME);

	for (;;) {
		length = read(geigerzaehler, &buffer, sizeof(buffer));

		if (length < 0) {
			fprintf(stderr, "Error opening device: %s\n", argv[1]);
			return -1;
		}
		if (length > 0) {
			/* parse geigerzaehler data */
			buffer[length] = '\0';
			sscanf(buffer, "CPS, %*d, CPM, %*d, uSv/hr, %f,%*s", &usvhr);
			printf("uSv/hr: %f\n", usvhr);
		}
		length = read(gps, &buffer, sizeof(buffer));

		if (length < 0) {
			fprintf(stderr, "Error opening device: %s\n", argv[2]);
                        return -1;
		}
		if (length > 0) {
			/* parse gps data */
			buffer[length] = '\0';
		}
		if (check_timer(tv, SEND_TIME)) {
			/* send a packet */
		}
	}

	return 1;
}
