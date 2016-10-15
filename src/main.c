#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <stdint.h>
#include <string.h>

/* GPIO 17 */
#define GEIGERZAEHLER_INPUT 0
#define TIMER_TIME 1

volatile uint64_t geigerzaehler_count;

void geigerzaehler(void)
{
	geigerzaehler_count++;
}

void timer_init(struct timeval &tv, time_t sec)
{
	gettimeofday(&tv, NULL);
	tv.tv_sec += sec;
}

int check_timer(struct timeval &tv, time_t sec)
{
	struct timeval cur_tv;
	gettimeofday(&cur_tv, NULL);

	if (cur_tv.tv_sec > tv.tv_sec) {
		timer_init(tv, sec);
		return 1;
	}

	return 0;
}

int main(void)
{
	struct timeval tv;
	geigerzaehler_count = 0;

	if (wiringPiSetup() < 0) {
		fprintf(stderr, "Unable to setup wiringPi: %s\n", strerror(errno));
		return -1;
	}

	if (wiringPiISR(GEIGERZAEHLER_INPUT, INT_EDGE_FALLING, &geigerzaehler) < 0) {
		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));
		return -1;
	}

	timer_init(tv, TIMER_TIME);

	for (;;) {
		if (check_timer(tv, TIMER_TIME)) {
			/* send data */
		}
	}
}
