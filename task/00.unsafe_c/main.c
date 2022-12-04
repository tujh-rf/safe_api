/* Example how it is often implementing on pure C in Embedded systems */

#ifdef WIN32
# include <winsock.h>
#else
# include <arpa/inet.h>
#endif
#include <stdio.h>

#include <api.h>


#define DEV1_ID (1)
#define DEV2_ID (2)


int device1_task() {
    conn_h conn = connection_open( DEV1_ID );
    if( conn == INVALID_CONNECTION )
        return -1;

    /* first possible issue: value and address information spreads in a code,
     * and it is easy to make mistake, even code review often will not help in this case.
     * second possible issue: there is no information about a real size of value,
     * is it uint8_t or uint16_t? RTFM only */
    uint8_t power_on = 0xFD;
    int res = connection_write( conn, 0x00, 0x00, &power_on, sizeof( uint8_t ) );
    if( res < 0 ) {
        /* in case of error - don't forget to close connection every time */
        connection_close( conn );
        return res;
    }

    uint8_t ready_value;
    res = connection_read( conn, 0xAA, 0xFF, &ready_value, sizeof( uint8_t ) );
    if( res < 0 ) {
        /* in case of error - don't forget to close connection every time */
        connection_close( conn );
        return res;
    }
    if( ready_value != 42 ) {
        /* in case of error - don't forget to close connection every time */
        connection_close( conn );
        return -1;
    }

    /* easy to forget about network order */
    uint16_t hello_value = htons( 0x100 );
    res = connection_write( conn, 0x10, 0xA0, &hello_value, sizeof( uint16_t ) );

    connection_close( conn );
    return res;
}


int device2_task() {
    conn_h conn = connection_open( DEV2_ID );
    if( conn == INVALID_CONNECTION )
        return -1;

    uint16_t hello_value = htons( 0x100 );
    int res = connection_write( conn, 0x10, 0xA0, &hello_value, sizeof( uint16_t ) );

    connection_close( conn );
    return res;
}


int main() {
    if( device1_task() < 0 )
        printf( "Communication with first device failed\n" );

    if( device2_task() < 0 )
        printf( "Communication with second device failed\n" );

    return 0;
}
