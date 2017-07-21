#include <string.h>
#include "log.h"

#include "bluez.h"

static DBusError error;

static void interfaces_handler(const char* object_path, const char* iface, DBusMessageIter* iter) {
    char *property_name;
    int remove = (iter == NULL);

    if(remove) {
        DEBUG("Removed %s : %s\n", object_path, iface);
    }
    else {
        DEBUG("Added %s : %s\n", object_path, iface);
    }

    if(!strcmp(iface, BLUEZ_ADAPTER)) {

    } else if(!strcmp(iface, BLUEZ_DEVICE)) {

    } else if(!strcmp(iface, BLUEZ_SERVICE)) {
        
    } else if(!strcmp(iface, BLUEZ_CHAR)) {
        
    } else if(!strcmp(iface, BLUEZ_DESC)) {
        
    }
}

static void interfaces_removed_handler(DBusMessage* message) {
    char *object_path, *iface;
    DBusMessageIter iter, subiter;
    dbus_message_iter_init(message, &iter);
    dbus_message_iter_get_basic(&iter, &object_path);

    dbus_message_iter_next(&iter);
    dbus_message_iter_recurse(&iter, &subiter);

    do {
        dbus_message_iter_get_basic(&subiter, &iface);
        interfaces_handler(object_path, iface, NULL);
    } while(dbus_message_iter_next(&subiter));

}

static void obj_manager_handler(DBusMessageIter *iter, void *user_data) {
    DBusMessageIter subiter;
    char *object_path, *iface;

    // (object_path,
    // [
    //     iface1_name : [properties1_name : variant, ...],
    //     ...
    // ])

    // Get object path
    dbus_message_iter_get_basic(iter, &object_path);
    dbus_message_iter_next(iter);

    dbus_message_iter_recurse(iter, &subiter);

    do {
        DBusMessageIter dictiter, propertiesiter;

        dbus_message_iter_recurse(&subiter, &dictiter);
        // Get interface name
        dbus_message_iter_get_basic(&dictiter, &iface);

        dbus_message_iter_next(&dictiter);
        // Get properties array
        dbus_message_iter_recurse(&dictiter, &propertiesiter);
        interfaces_handler(object_path, iface, &propertiesiter);
    } while(dbus_message_iter_next(&subiter));

}

static void handle_signal(DBusMessage *message) {
    const char* signame;

    signame = dbus_message_get_member(message);

    if(!strcmp(signame, BLUEZ_SIG_IFACE_ADD)) {
        DBusMessageIter iter;
        dbus_message_iter_init(message, &iter);
        obj_manager_handler(&iter, NULL);
    } else if(!strcmp(signame, BLUEZ_SIG_IFACE_RM)) {
        interfaces_removed_handler(message);
    } else if(!strcmp(signame, BLUEZ_SIG_PROP_CHG)) {

    }
}

static DBusHandlerResult filter_handler(DBusConnection *connection, DBusMessage *message, void *user_data) {
    int message_type;

    message_type = dbus_message_get_type(message);

    switch(message_type) {
        case DBUS_MESSAGE_TYPE_SIGNAL:
            handle_signal(message);
            return DBUS_HANDLER_RESULT_HANDLED;

        default:
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
}

void bz_init(void) {
    if(!ldbus_init(BLUEZ_INTERFACE))
        ldbus_print_error();
    
    DBusConnection* con = ldbus_get_connection();

    dbus_connection_add_filter(con, filter_handler, NULL, NULL);

    bz_update();
}

static void reply_get_managed_objects(DBusPendingCall *pending, void *user_data) {
    DBusMessageIter iter, subiter;
    DBusMessage *msg = dbus_pending_call_steal_reply(pending);

    dbus_message_iter_init(msg, &iter);
    dbus_message_iter_recurse(&iter, &subiter);

    do {
        DBusMessageIter dictiter;

        dbus_message_iter_recurse(&subiter, &dictiter);

        obj_manager_handler(&dictiter, user_data);
    } while(dbus_message_iter_next(&subiter));
}

void bz_update(void) {
    DBusMessage *msg;
    DBusConnection* con = ldbus_get_connection();

    msg = dbus_message_new_method_call(BLUEZ_INTERFACE, "/", DBUS_OBJ_MANAGER, "GetManagedObjects");

    ldbus_send_async(con, msg, DBUS_TIMEOUT_INFINITE, reply_get_managed_objects, NULL);
}