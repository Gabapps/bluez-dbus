#ifndef H_MAINLOOP_INCLUDE
#define H_MAINLOOP_INCLUDE

#include <sys/poll.h>

// Maximum number of file descriptor events watchable
#ifndef MAINLOOP_MAX_FDS
#define MAINLOOP_MAX_FDS 16
#endif

// Maximum number of signals watchable
#ifndef MAINLOOP_MAX_SIG
#define MAINLOOP_MAX_SIG 2
#endif

#define EVENT_TYPE_READ POLLIN
#define EVENT_TYPE_WRITE POLLOUT

/** mainloop.h
  * 
  * This file provides features to run a simple event loop.
  * File descriptors and signals can be watched by the event loop
  * using mainloop_reg_fd and mainloop_reg_signal functions.
  * The event loop is run using the mainloop_run function.
*/

/**********************************************************************/

/** Handler function called on file descriptor events
  * @param fd The file descriptor which received the event
  * @param event_type The event received
  * @param data The user data passed in mainloop_reg_fd function
*/
typedef void (*mainloop_event_handler)(int fd, short event_type, void* data);

/** Handler function called on signal events
  * @param fd The number of the received signal
  * @param data NOT IMPLEMENTED
*/
typedef void (*mainloop_signal_handler)(int sig, void* data);

/** Register a file descriptor to be watched by the mainloop
  * @param fd File descriptor to watch
  * @param event_type Type of event to watch (POLLIN, POLLOUT...)
  * @param handler Handler function called when the file descriptor is
  * ready for the given event_type
  * @param data Data User data to pass to the handler function
  *
  * @return Returns 1 if it succeeds, 0 if it reaches the max number of
  * files descriptors allowed : MAINLOOP_MAX_FDS
*/
int mainloop_reg_fd(int fd, short event_type, mainloop_event_handler handler, void* data);


/** Unregister a file descriptor from the mainloop
  * @param fd File descriptor to stop watching
  * @param event_type Type of event to watch (POLLIN, POLLOUT...)
*/
void mainloop_unreg_fd(int fd, short event_type);


/** Register a signal to be watched by the mainloop
  * Note : Signals can't be registered after mainloop_run is called
  * @param fd Signal to watch
  * @param handler Handler function called when the signal is
  * received
  * @param data NOT IMPLEMENTED
  *
  * @return Returns 1 if it succeeds, 0 if it reaches the max number of
  * signals allowed : MAINLOOP_MAX_SIG
*/
int mainloop_reg_signal(int sig, mainloop_signal_handler handler, void* data);

/** Run the mainloop and poll events
  * Note : The mainloop will return on unhandled signals (SIGINT, SIGTERM...)
*/
void mainloop_run(void);

#endif //H_MAINLOOP_INCLUDE