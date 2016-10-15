#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define TIMER_TIME 1

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

	timer_init(tv, TIMER_TIME);

	for (;;) {
		if (check_timer(tv, TIMER_TIME)) {
			/* send data */
		}
	}
}
