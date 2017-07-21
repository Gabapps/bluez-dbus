#include <errno.h>
#include "log.h"
#include <stdlib.h>

#include "mainloop.h"

struct mainloop_handler_table
{
    mainloop_event_handler handler;
    void* data;
};

struct mainloop_main
{
    struct pollfd fds[MAINLOOP_MAX_FDS];
    struct mainloop_handler_table hdlr_table[MAINLOOP_MAX_FDS];
    int nb_fds;
};

static struct mainloop_main ml;

int mainloop_reg_fd(int fd, short event_type, mainloop_event_handler handler, void *data) {
    if(ml.nb_fds == MAINLOOP_MAX_FDS)
        return 0;

    ml.fds[ml.nb_fds].fd = fd;
    ml.fds[ml.nb_fds].events = event_type;
    ml.hdlr_table[ml.nb_fds].handler = handler;
    ml.hdlr_table[ml.nb_fds].data = data;
    ml.nb_fds++;
    return 1;
}

void mainloop_unreg_fd(int fd, short event_type) {
    int i, start_compress = 0;
    for(i=0; i<ml.nb_fds-start_compress; i++) {
        if(ml.fds[i].fd == fd && ml.fds[i].events == event_type)
            start_compress = 1;
        if(start_compress && i<ml.nb_fds-start_compress) {
            ml.fds[i].fd = ml.fds[i+1].fd;
            ml.fds[i].events = ml.fds[i+1].events;
            ml.hdlr_table[i].handler = ml.hdlr_table[i+1].handler;
            ml.hdlr_table[i].data = ml.hdlr_table[i+1].data;
        }
    }
    ml.nb_fds--;
}

static void mainloop_dispatch_events() {
    int i;
    for(i=0; i<ml.nb_fds; i++) {
        if(ml.fds[i].revents)
            ml.hdlr_table[i].handler(ml.fds[i].fd, ml.fds[i].revents, ml.hdlr_table[i].data);
    }
}

void mainloop_run(void) {
    for(;;) {
        int err = poll(ml.fds, ml.nb_fds, -1);
        if (err < 0 && errno != EINTR && errno != 0) {
            ERROR("poll error\n");
            abort();
        }

        mainloop_dispatch_events();
    }
}