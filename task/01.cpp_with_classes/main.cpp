/* Example how C developers can try to improve example with using C++ as C with classes */

#ifdef WIN32
# include <winsock.h>
#else
# include <arpa/inet.h>
#endif
#include <iostream>
#include <stdexcept>
#include <string>
#include <optional>
#include <memory>

extern "C" {
#include <api.h>
}

/* special exception */
class connection_exception: public std::exception {
public:
    connection_exception() = delete;
    explicit connection_exception( const uint8_t dev_id ) {
        exception_text = "cannot open communication to device #"
            + std::to_string( dev_id );
    }

    const char* what() const throw() override {
        return exception_text.c_str();
    }

private:
    std::string exception_text;

};

/* class for communication with devices */
class device {
public:
    device() = delete;
    explicit device( const uint8_t dev_id ):
        m_dev_id( dev_id )
      , m_conn( connection_open( dev_id ) ) {
        if( m_conn == INVALID_CONNECTION )
            throw connection_exception( dev_id );
    }
    ~device() {
        if( m_conn != INVALID_CONNECTION )
            connection_close( m_conn );
    }

    /* set of function to write different data:
     *      write8/16 and read8/16,
     * but still no mapping between memory in the device and size of data there */

    bool write8( const uint8_t upper_addr, const uint8_t lower_addr, const uint8_t value ) {
        uint8_t local_value = value;
        return ( connection_write( m_conn, upper_addr, lower_addr, &local_value, sizeof( uint8_t ) ) >= 0 );
    }

    bool write16( const uint8_t upper_addr, const uint8_t lower_addr, const uint16_t value ) {
        /* better than C, host-to-network translation is executed in one place */
        uint16_t local_value = htons( value );
        return ( connection_write( m_conn, upper_addr, lower_addr, &local_value, sizeof( uint16_t ) ) >= 0 );
    }

    bool read8( const uint8_t upper_addr, uint8_t lower_addr, uint8_t &value ) {
        return ( connection_read( m_conn, upper_addr, lower_addr, &value, sizeof( uint8_t ) ) >= 0 );
    }

    bool read16( const uint8_t upper_addr, uint8_t lower_addr, uint16_t &value ) {
        uint16_t local_value;
        if( connection_read( m_conn, upper_addr, lower_addr, &local_value, sizeof( uint16_t ) ) < 0 )
            return false;

        /* better than C, host-to-network translation is executed in one place */
        value = ntohs( local_value );
        return true;
    }

private:
    uint8_t m_dev_id = 0;
    conn_h  m_conn   = INVALID_CONNECTION;

};


/* some modern C++ */
std::optional< std::string > device1_task() {
    static const uint8_t dev_id = 1;

    std::unique_ptr< device > dev_conn;
    try {
        dev_conn = std::make_unique< device > ( dev_id );
    }
    catch( connection_exception &ex ) {
        return std::string( ex.what() );
    }

    /* in common it is not really better that pure C */
    uint8_t power_on = 0xFD;
    if( !dev_conn->write8( 0x00, 0x00, power_on ) )
        return std::string( "cannot send command to power on" );

    uint8_t ready_value;
    if( !dev_conn->read8( 0xAA, 0xFF, ready_value ) )
        return std::string( "cannot read status of the device" );

    if( ready_value != 42 )
        return std::string( "device doesn't ready" );

    uint16_t hello_value = 0x100;
    if( !dev_conn->write16( 0x10, 0xA0, hello_value ) )
        return std::string( "cannot send hello command" );

    return std::nullopt;
}


std::optional< std::string > device2_task() {
    static const uint8_t dev_id = 2;

    std::unique_ptr< device > dev_conn;
    try {
        dev_conn = std::make_unique< device > ( dev_id );
    }
    catch( connection_exception &ex ) {
        return std::string( ex.what() );
    }

    uint16_t hello_value = 0x100;
    if( !dev_conn->write16( 0x10, 0xA0, hello_value ) )
        return std::string( "cannot send hello command" );

    return std::nullopt;
}


int main() {
    if( auto res = device1_task() )
        std::cout << "Communication with first device failed"
                  << *res
                  << std::endl;

    if( auto res = device2_task() )
        std::cout << "Communication with second device failed"
                  << *res
                  << std::endl;

    return 0;
}
