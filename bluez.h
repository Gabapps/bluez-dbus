#ifndef H_BLUEZ_INCLUDE
#define H_BLUEZ_INCLUDE

#include "ldbus.h"

#define BLUEZ_INTERFACE "org.bluez"

#define BLUEZ_ADAPTER "org.bluez.Adapter1"
#define BLUEZ_DEVICE  "org.bluez.Device1"
#define BLUEZ_SERVICE "org.bluez.GattService1"
#define BLUEZ_CHAR    "org.bluez.GattCharacteristic1"
#define BLUEZ_DESC    "org.bluez.GattDescriptor1"

#define BLUEZ_SIG_IFACE_ADD     "InterfacesAdded"
#define BLUEZ_SIG_IFACE_RM      "InterfacesRemoved"
#define BLUEZ_SIG_PROP_CHG      "PropertiesChanged"

void bz_init(void);

void bz_update(void);

#endif //H_BLUEZ_INCLUDE