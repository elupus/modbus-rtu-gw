/*
 * Copyright Joakim Plate <joakim.plate@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include <stdio.h>
#include <errno.h>
#include <modbus.h>

int main(int argc, char** argv) {
    modbus_t *          ctx = NULL;
    char*               device;
    modbus_mapping_t *  mapping;
    int                 rc;

    device = argv[1];

    ctx = modbus_new_rtu(device, 9600, 'N', 8, 1);
    modbus_set_slave(ctx, 0x5c); /* expected slave of 0x5c */
    modbus_set_debug(ctx, TRUE);
    modbus_connect(ctx);

    mapping = modbus_mapping_new(MODBUS_MAX_READ_BITS     , 0,
                                 MODBUS_MAX_READ_REGISTERS, 0);

    if (mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    for(;;) {
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

        rc = modbus_receive(ctx, query);
        if (rc > 0) {
            modbus_reply(ctx, query, rc, mapping);
        } else if (rc  == -1) {
            /* Connection closed by the client or error */
            break;
        }
    }

    printf("Quit the loop: %s\n", modbus_strerror(errno));

    modbus_mapping_free(mapping);
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}

