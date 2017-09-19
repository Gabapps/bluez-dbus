#ifndef H_LDBUS_INCLUDE
#define H_LDBUS_INCLUDE

#include "mainloop.h"

#include <dbus/dbus.h>

#define DBUS_OBJ_MANAGER "org.freedesktop.DBus.ObjectManager"
#define DBUS_INSTROSPECT "org.freedesktop.DBus.Introspectable"
#define DBUS_PROPERTIES  "org.freedesktop.DBus.Properties"

/** ldbus.h
  * 
  * This file provides features to use D-Bus in a simple way with an
  * event loop. ldbus_init initialize the link between the libdbus
  * and the mainloop.
*/

/**********************************************************************/

/** Initiate a D-Bus connection to a given sender
  * @param sender The sender we want to communicate with
  * on system bus (ex: org.freedesktop.UPower)
*/
int ldbus_init(const char* sender);

/** Print D-Bus errors */
void ldbus_print_error(void);

/** Get D-Bus connection
  * @return Returns the D-Bus connection structure
*/
DBusConnection* ldbus_get_connection(void);

/** Send a message and retrieve the answer asynchronously
  * @param con The connection structure
  * @param timeout Time in ms before a timeout error
  * is thrown (NOT IMPLEMENTED)
  * @param msg The message to be sent
  * @param handler The handler function called when a reply
  * is received
  * @param data Data User data to pass to the handler function
*/
void ldbus_send_async(DBusConnection* con,
                      DBusMessage* msg,
                      int timeout,
                      DBusPendingCallNotifyFunction handler,
                      void* data);

#endif //H_LDBUS_INCLUDE