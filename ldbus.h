#ifndef H_LDBUS_INCLUDE
#define H_LDBUS_INCLUDE

#include "mainloop.h"

#include <dbus/dbus.h>

#define DBUS_OBJ_MANAGER "org.freedesktop.DBus.ObjectManager"
#define DBUS_INSTROSPECT "org.freedesktop.DBus.Introspectable"
#define DBUS_PROPERTIES  "org.freedesktop.DBus.Properties"

typedef int bool;

bool ldbus_init(const char* sender);

void ldbus_print_error(void);

DBusConnection* ldbus_get_connection(void);

void ldbus_send_async(DBusConnection* con,
                      DBusMessage* msg,
                      int timeout,
                      DBusPendingCallNotifyFunction handler,
                      void* data);

#endif //H_LDBUS_INCLUDE