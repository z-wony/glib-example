#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <glib-unix.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdint.h>
#include <fcntl.h>
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

void register_thread_font()
{
	tids[++last_thread_idx] = GETTID();
}

void register_thread_font_for_child(long pid)
{
	tids[++last_thread_idx] = pid;
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

/*
**         For Debug
*******************************************************************************/


#define log(fmt, arg...)     print_with_color(fmt, ##arg)

typedef struct {
    int pid;
    int rfd;
    int wfd;
} comm_channel_s;


static gboolean _parent_read_cb(GIOChannel *source, GIOCondition condition, gpointer data)
{
    int fd = g_io_channel_unix_get_fd(source);
    if (condition != G_IO_IN) {
        log("Pipe Error");
        return FALSE; // Stop watching this pipe
    }

    char buf[10] = { 0, };
    int r = read(fd, buf, 10);
    if (r > 0) {
        log("<parent> read: %s", buf);
    } else {
        log("<parent> read fail: %d", r);
    }

    return TRUE;
}

static void parent_work(comm_channel_s ch[2])
{
    GIOChannel *ioch = g_io_channel_unix_new(ch[0].rfd);
    g_io_add_watch(ioch, (G_IO_IN | G_IO_HUP | G_IO_ERR), _parent_read_cb, NULL);

    ioch = g_io_channel_unix_new(ch[1].rfd);
    g_io_add_watch(ioch, (G_IO_IN | G_IO_HUP | G_IO_ERR), _parent_read_cb, NULL);
}

static void child_work(comm_channel_s ch, const char *name)
{
    int i;
    for (i = 0; i < 10; i++) {
        char buf[10] = { 0, };
        snprintf(buf, 10, "%d: %s", (i + 1), name);
        int r = write(ch.wfd, buf, 10);
        if (r > 0) {
            log("<child> write success");
        } else {
            log("<child> write fail: %d", r);
        }
        sleep(1);
    }
    close(ch.rfd);
    close(ch.wfd);
}

comm_channel_s create_child(const char *child_name)
{
    int fds[2];
    gboolean gret;
    gret = g_unix_open_pipe(fds, FD_CLOEXEC, NULL);
    if (!gret) {
        log("Error");
    }
    int pid = fork();

    comm_channel_s ch;
    ch.rfd = fds[0];
    ch.wfd = fds[1];
    ch.pid = (int)pid;

    gret = g_unix_set_fd_nonblocking(fds[0], TRUE, NULL);
    if (!gret) {
        log("Error2");
    }

    // child
    if (pid == 0) {
	    register_thread_font();
        child_work(ch, child_name);
        exit(0);
    } else {
        // Just for debug print color setting
	    register_thread_font_for_child(pid);
    }

    return ch;
}

int main()
{
	font_guide();

	register_thread_font();

    comm_channel_s ch1 = create_child("Tom");
    usleep(150000);
    comm_channel_s ch2 = create_child("Jhon");

    comm_channel_s chs[2] = { ch1, ch2 };
    parent_work(chs);

    GMainLoop *loop = g_main_loop_new(g_main_context_default(), FALSE);
    g_main_loop_run(loop);

    g_main_loop_unref(loop);
	timecheck_end();

	return 0;
}
