#include <errno.h>
#include "log.h"
#include <stdlib.h>
#include <signal.h>
#include <sys/signalfd.h>

#include "mainloop.h"

struct mainloop_handler_table
{
    mainloop_event_handler handler;
    void* data;
};

struct mainloop_signal_table
{
    int sig;
    mainloop_signal_handler handler;
    void* data;
};

struct mainloop_main
{
    struct pollfd fds[MAINLOOP_MAX_FDS];
    struct mainloop_handler_table hdlr_table[MAINLOOP_MAX_FDS];
    int nb_fds;
    struct mainloop_signal_table signal_table[MAINLOOP_MAX_SIG];
    int nb_sig;
	sigset_t sigmask;
};

static struct mainloop_main ml = {0};

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

static void mainloop_dispatch_signals(int fd, short event_type, void* data) {
    int i, sig, res;
    struct signalfd_siginfo si;

    res = read (fd, &si, sizeof(si));

    sig = si.ssi_signo;

    for(i=0; i<ml.nb_sig; i++) {
        if(ml.signal_table[i].sig == sig) {
            ml.signal_table[i].handler(ml.signal_table[i].sig, ml.signal_table[i].data);
        }
    }
}

int mainloop_reg_signal(int sig, mainloop_signal_handler handler, void* data) {
    if(ml.nb_fds == MAINLOOP_MAX_SIG)
        return 0;
    
    ml.signal_table[ml.nb_sig].sig = sig;
    ml.signal_table[ml.nb_sig].handler = handler;
    ml.signal_table[ml.nb_sig].data = data;

    ml.nb_sig++;

    return 1;
}

static void mainloop_init_signal() {
    int i, fd;

    sigemptyset(&ml.sigmask);

    for(i=0; i<ml.nb_sig; i++) {
        sigaddset(&ml.sigmask, ml.signal_table[i].sig);
    }

    sigprocmask(SIG_BLOCK, &ml.sigmask, NULL);

    fd = signalfd(-1, &ml.sigmask, NULL);

    mainloop_reg_fd(fd, POLLIN, mainloop_dispatch_signals, NULL);
}

static void mainloop_dispatch_events() {
    int i;
    for(i=0; i<ml.nb_fds; i++) {
        if(ml.fds[i].revents)
            ml.hdlr_table[i].handler(ml.fds[i].fd, ml.fds[i].revents, ml.hdlr_table[i].data);
    }
}

void mainloop_run(void) {

    mainloop_init_signal();

    for(;;) {
        int err = poll(ml.fds, ml.nb_fds, -1);
        if (err < 0 && errno != EINTR && errno != 0) {
            ERROR("poll error\n");
            abort();
        }

        mainloop_dispatch_events();
    }
}