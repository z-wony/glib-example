#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdint.h>
#include <sys/time.h>

/*******************************************************************************
**         For Debug
*/

#define GETTID()	((long)syscall(SYS_gettid))

static char *font_color[6] = {
	"\033[0m", /* reset */
	"\033[31m", /* red */
	"\033[32m", /* green */
	"\033[34m", /* blue */
	"\033[35m", /* magenta */
	"\033[36m" /* cyan */
};
static long tids[6] = {0L, };
static int last_thread_idx = 0;

static long main_tid = 0;
static long sub_tid = 0;

static long st_sec = 0, st_usec = 0;
static long ed_sec = 0, ed_usec = 0;

void timecheck_start()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	st_sec = tv.tv_sec;
	st_usec = tv.tv_usec;
}

void timecheck_end()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	ed_sec = tv.tv_sec;
	ed_usec = tv.tv_usec;

	long sec, usec;
	sec = ed_sec - st_sec;
	usec = ed_usec - st_usec;
	if (usec < 0) {
		sec--;
		usec += (long)1000000;
	}
	print_with_color("spended : %ld.%ld sec", sec, usec);
}

void register_thread_font()
{
	tids[++last_thread_idx] = GETTID();
}

void reset_thread_font()
{
	last_thread_idx = 0;
}

void print_with_color(const char *format, ...)
{
	long _tid = GETTID();
	int i, j;
	for (i = 1; i <= last_thread_idx; i++) {
		if (_tid == tids[i]) {
			for (j = 1; j < i; j++)
				printf("\t");
			printf("[%s%ld%s]", font_color[i], _tid, font_color[0]);
			va_list args;
			va_start(args, format);
			vprintf(format, args);
			va_end(args);
			printf("\n");
		}
	}
}

void font_guide()
{
	printf("=== font color guide ===\n");
	int i = 1;
	printf("%s%s%s\n", font_color[i++], "1st thread", font_color[0]);
	printf("%s%s%s\n", font_color[i++], "2nd thread", font_color[0]);
	printf("%s%s%s\n", font_color[i++], "3rd thread", font_color[0]);
	printf("%s%s%s\n", font_color[i++], "4th thread", font_color[0]);
	printf("%s%s%s\n", font_color[i++], "5th thread", font_color[0]);
	printf("========================\n\n");
}

/*
**         For Debug
*******************************************************************************/

#define MAX_COUNT	1000
#define TEST_CASE	1000
GList *my_queue = NULL;

void *test_runnable(void *data)
{
	register_thread_font();
//	print_with_color("thread start >>>");

	int base = (int)(intptr_t)data;

	int i;
	for (i = 0; i < MAX_COUNT; i++) {
//		print_with_color("push data :%d", (base + i));
		my_queue = g_list_append(my_queue, (void *)(intptr_t)(base + i));
	//	if (!(i % 4))
	//		usleep(1);
	}

//	print_with_color("<<< thread end");
	return NULL;
}

int test_run() {

	my_queue = NULL;

	pthread_t th1;
	pthread_create(&th1, NULL, test_runnable, (void *)(intptr_t)(10 * MAX_COUNT));

	pthread_t th2;
	pthread_create(&th2, NULL, test_runnable, (void *)(intptr_t)(20 * MAX_COUNT));

	pthread_t th3;
	pthread_create(&th3, NULL, test_runnable, (void *)(intptr_t)(30 * MAX_COUNT));

	pthread_join(th1, NULL);
	pthread_join(th2, NULL);
	pthread_join(th3, NULL);

	int len = g_list_length(my_queue);
	print_with_color("result length : %d", len);
	int i, k;
/*
	for (i = 0; i < len; i++) {
		k = (int)(intptr_t)g_queue_peek_nth(my_queue, i);
		print_with_color("idx [%2d] : %d", i, k);
	}
*/
	g_list_free(my_queue);
	my_queue = NULL;

	return len;
}

int main()
{
	font_guide();
	int i, ret, pass, fail;
	pass = fail = 0;
	timecheck_start();
	for (i = 1; i <= TEST_CASE; i++) {
		register_thread_font();
		print_with_color("== test %dth", i);
		ret = test_run();
		reset_thread_font();

		if (ret == (3 * MAX_COUNT)) pass++;
		else 	fail++;
	}
	
	register_thread_font();
	print_with_color("pass : %d / fail : %d", pass, fail);
	timecheck_end();

	return 0;
}
