#ifndef H_MAINLOOP_INCLUDE
#define H_MAINLOOP_INCLUDE

#include <sys/poll.h>

#ifndef MAINLOOP_MAX_FDS
#define MAINLOOP_MAX_FDS 16
#endif

#define EVENT_TYPE_READ POLLIN
#define EVENT_TYPE_WRITE POLLOUT

typedef void (*mainloop_event_handler)(int fd, short event_type, void* data);

int mainloop_reg_fd(int fd, short event_type, mainloop_event_handler handler, void* data);

void mainloop_unreg_fd(int fd, short event_type);

void mainloop_run(void);

#endif //H_MAINLOOP_INCLUDE