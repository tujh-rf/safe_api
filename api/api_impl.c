
#include <stdio.h>

#include "api.h"


/* needed for HEX translation uint64_t -> 16 symbols */
#define BUFFER_LEN 16
static char symbols[] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F'
};


conn_h connection_open( uint8_t dev_id ) {
    // valid range is 1..4
    if( ( !dev_id ) || ( dev_id > 5 ) )
        return INVALID_CONNECTION;

    printf( "DEV%i: connection ON\n", dev_id );

    return dev_id;
}


void connection_close( conn_h handle ) {
    printf( "DEV%i: connection OFF\n", handle );
}


int connection_write( conn_h handle,
                      uint8_t upper_addr, uint8_t lower_addr,
                      void *data_ptr, size_t data_len ) {
    if( ( !data_len ) || ( data_len > 8 ) )
        return -1;

    /* translation of data to HEX stream */
    char buffer_ptr[BUFFER_LEN + 1] = { 0 };
    size_t pos = 0;
    uint8_t *value_ptr = (uint8_t*)data_ptr;
    for( size_t i = 0; i < data_len; ++i ) {
        uint8_t value_byte = value_ptr[i];

        // upper half byte
        buffer_ptr[pos] = symbols[ ( value_byte & 0xf0 ) >> 4 ];
        ++pos;

        // lower half byte
        buffer_ptr[pos] = symbols[ value_byte & 0xf ];
        ++pos;
    }
    buffer_ptr[pos] = '\x0';

    printf( "DEV%i: [%02X:%02X] written %s\n",
            handle,
            upper_addr, lower_addr,
            buffer_ptr );

    return (int)data_len;
}


int connection_read( conn_h handle,
                     uint8_t upper_addr, uint8_t lower_addr,
                     void *data_ptr, size_t data_len ) {
    if( ( !data_len ) || ( data_len > 8 ) )
        return -1;

    uint8_t *value_ptr = (uint8_t*)data_ptr;
    ( *value_ptr ) = 42; // universal answer

    printf( "DEV%i: [%02X:%02X] read 2A\n", handle, upper_addr, lower_addr );

    return (int)data_len;
}
