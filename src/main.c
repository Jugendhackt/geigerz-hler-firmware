#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <curl/curl.h>

#define GGZ_BAUDRATE B9600
#define GPS_BAUDRATE B9600
#define SEND_TIME 10

void send_data(char *data[])
{
	CURL *curl;
	CURLcode res;

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/json");

	curl_global_init(CURL_GLOBAL_ALL);

	curl= curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://tmp.pajowu.de/api/data/?format=json");

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER,headers);

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();	
}

void printhelp(char *command)
{
	printf("Usage: %s [geigerzaehler] [gpsdongle]\n", command);
}

int main(int argc, char *argv[])
{
	int geigerzaehler, gps, cps, cpm;
	struct termios tp;
	char buffer[100];
	ssize_t length;
	float usvhr, lon, lat;
	struct timeval tv, ct;

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

	gettimeofday(&tv, NULL);
	tv.tv_sec += SEND_TIME;

	for (;;) {
		length = read(geigerzaehler, &buffer, sizeof(buffer));

		if (length < 0) {
			fprintf(stderr, "Error opening device: %s\n", argv[1]);
			return -1;
		}
		if (length > 0 && buffer[0] != '\n') {
			/* parse geigerzaehler data */
			buffer[length] = '\0';
			sscanf(buffer, "CPS, %*d, CPM, %*d, uSv/hr, %f%*s", &usvhr);
			printf("uSv/hr: %f\n", usvhr);
		}
		length = 0;

		length = read(gps, &buffer, sizeof(buffer));

		if (length < 0) {
			fprintf(stderr, "Error opening device: %s\n", argv[2]);
                        return -1;
		}
		if (length > 0) {
			/* parse gps data */
			buffer[length] = '\0';
		}

		gettimeofday(&ct, NULL);

		if (ct.tv_sec > tv.tv_sec) {
			/* send a packet */
			gettimeofday(&tv, NULL);
			tv.tv_sec += SEND_TIME;
		}
	}

	return 1;
}
