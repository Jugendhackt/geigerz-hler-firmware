#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <stdint.h>
#include <string.h>

/* GPIO 17 */
#define GEIGERZAEHLER_INPUT 0

volatile uint64_t geigerzaehler_count;

void geigerzaehler(void)
{
	geigerzaehler_count++;
}

int main(void)
{
	geigerzaehler_count = 0;

	if (wiringPiSetup() < 0) {
		fprintf(stderr, "Unable to setup wiringPi: %s\n", strerror(errno));
		return -1;
	}

	if (wiringPiISR(GEIGERZAEHLER_INPUT, INT_EDGE_FALLING, &geigerzaehler) < 0) {
		fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));
		return -1;
	}

	for (;;) {

	}
}
