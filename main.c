#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>

#include "bluez.h"
 
int main() {
    bz_init();

    mainloop_run();

    return 0;
}