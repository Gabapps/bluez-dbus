#include "log.h"
#include <signal.h>

#include "ldbus.h"

struct ldbus
{
    DBusConnection *con;
};

static DBusError error;
static struct ldbus db;

static void fd_event(int fd, short event_type, void *data) {
    int flags = 0;
    DBusWatch* watch = data;

    if(event_type | EVENT_TYPE_READ)
        flags |= DBUS_WATCH_READABLE;
    if(event_type | EVENT_TYPE_WRITE)
        flags |= DBUS_WATCH_WRITABLE;

    dbus_watch_handle(watch, flags);
}

static dbus_bool_t add_watch(DBusWatch *watch, void *data) {
    short event_type = 0;
    int flags, fd, enabled;
    fd = dbus_watch_get_unix_fd(watch);
    flags = dbus_watch_get_flags(watch);
    enabled = dbus_watch_get_enabled(watch);
    DEBUG("New watch : %d %d %d\n", fd, flags, enabled);

    if(!enabled) return TRUE;

    if(flags & DBUS_WATCH_READABLE)
        event_type |= EVENT_TYPE_READ;
    if(flags & DBUS_WATCH_WRITABLE)
        event_type |= EVENT_TYPE_WRITE;

    return mainloop_reg_fd(fd, event_type, fd_event, (void*)watch);
}

static void rm_watch(DBusWatch *watch, void *data) {
    DEBUG("Rm watch : %d %d %d\n", dbus_watch_get_unix_fd(watch),
                                   dbus_watch_get_flags(watch),
                                   dbus_watch_get_enabled(watch));

    int flags, fd, enabled;
    short event_type = 0;
    fd = dbus_watch_get_unix_fd(watch);
    flags = dbus_watch_get_flags(watch);
    enabled = dbus_watch_get_enabled(watch);

    if(enabled) return;

    if(flags & DBUS_WATCH_READABLE)
        event_type |= EVENT_TYPE_READ;
    if(flags & DBUS_WATCH_WRITABLE)
        event_type |= EVENT_TYPE_WRITE;

    mainloop_unreg_fd(fd, event_type);
}

static void toggle_watch(DBusWatch *watch, void *data) {
    DEBUG("Toggle watch : %d %d %d\n", dbus_watch_get_unix_fd(watch),
                                       dbus_watch_get_flags(watch),
                                       dbus_watch_get_enabled(watch));
    int enabled = dbus_watch_get_enabled(watch);
    if(enabled) add_watch(watch, NULL);
    else        rm_watch(watch, NULL);
}

static dbus_bool_t add_timeout(DBusTimeout *timeout, void *data) {
    DEBUG("Add timeout %d\n", dbus_timeout_get_interval(timeout));
    return TRUE;
}

static void toggle_timeout(DBusTimeout *timeout, void *data) {
    DEBUG("Toggle timeout %d\n", dbus_timeout_get_interval(timeout));
}

static void rm_timeout(DBusTimeout *timeout, void *data) {
    DEBUG("Rm timeout %d\n", dbus_timeout_get_interval(timeout));
}

static void dispatch_all(void) {
    dbus_connection_ref(db.con);
    while (dbus_connection_get_dispatch_status(db.con) == DBUS_DISPATCH_DATA_REMAINS)
        dbus_connection_dispatch(db.con);
    dbus_connection_unref(db.con);
}

static void wakeup_handler(int signum) {
    if(signum != SIGPOLL)
        return;

    if (dbus_connection_get_dispatch_status(db.con) != DBUS_DISPATCH_DATA_REMAINS)
        return;

    DEBUG("Dispatch message queue\n");
    dispatch_all();
}

static void wakeup_main(void* data) {
    signal(SIGPOLL, wakeup_handler);

    raise(SIGPOLL);
}

static void dispatch(DBusConnection *connection, DBusDispatchStatus new_status, void *data) {
    DEBUG("New status %d\n", new_status);
}

bool ldbus_init(const char* sender) {
    char matchfmt[64];
    DBusMessage* msg;
    DBusPendingCall* pc;
    dbus_error_init(&error);
    db.con = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
    if(dbus_error_is_set(&error)) return FALSE;

    if(sender) {
        sprintf(matchfmt, "sender='%s'", sender);
        dbus_bus_add_match(db.con, matchfmt, &error);
        if(dbus_error_is_set(&error)) return FALSE;
    }

    DEBUG("Unique name : %s\n", dbus_bus_get_unique_name(db.con));

    dbus_connection_set_watch_functions(db.con, add_watch, rm_watch, toggle_watch, NULL, NULL);

    dbus_connection_set_wakeup_main_function(db.con, wakeup_main, NULL, NULL);

    dbus_connection_set_dispatch_status_function(db.con, dispatch, NULL, NULL);

    dbus_connection_set_timeout_functions(db.con, add_timeout, toggle_timeout, rm_timeout, NULL, NULL);
}

void ldbus_print_error(void) {
    if (!dbus_error_is_set(&error)) return;
    ERROR("%s\n", error.message);
}

DBusConnection* ldbus_get_connection(void) {
    return db.con;
}

void ldbus_send_async(DBusConnection* con,
                      DBusMessage* msg,
                      int timeout,
                      DBusPendingCallNotifyFunction handler,
                      void* data) {
    DBusPendingCall* pc;
    dbus_connection_send_with_reply(db.con, msg, &pc, timeout);

    dbus_message_unref(msg);
    dbus_pending_call_set_notify(pc, handler, data, NULL);
}