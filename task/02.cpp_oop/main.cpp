/* Example of C++ with more or less correct OOP,
 * of course it can be even better */

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

/* not only device has its own class but at least address has it as well */
struct address {
    address() = delete;
    explicit address( const uint8_t upper_addr, const uint8_t lower_addr ):
        upper_part( upper_addr )
      , lower_part( lower_addr ) {
    }

    uint8_t upper_part;
    uint8_t lower_part;
};

/* map of known memory cells */
namespace addresses {

/* good, all addresses easy to check on code review,
 * but still no checks for data size */
static const address power_on( 0x00, 0x00 );
static const address hello( 0x10, 0xA0 );
static const address ready( 0xAA, 0xFF );

}


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

    bool write8( const address &addr, const uint8_t value ) {
        uint8_t local_value = value;
        return ( connection_write( m_conn, addr.upper_part, addr.lower_part, &local_value, sizeof( uint8_t ) ) >= 0 );
    }

    bool write16( const address addr, const uint16_t value ) {
        uint16_t local_value = htons( value );
        return ( connection_write( m_conn, addr.upper_part, addr.lower_part, &local_value, sizeof( uint16_t ) ) >= 0 );
    }

    /* small improvement of read functions, better than before */
    std::optional< uint8_t > read8( const address addr ) {
        uint16_t local_value;
        if( connection_read( m_conn, addr.upper_part, addr.lower_part, &local_value, sizeof( uint8_t ) ) >= 0 )
            return local_value;
        return std::nullopt;
    }

    std::optional< uint16_t > read16( const address addr ) {
        uint16_t local_value;
        if( connection_read( m_conn, addr.upper_part, addr.lower_part, &local_value, sizeof( uint16_t ) ) >= 0 )
            return ntohs( local_value );
        return std::nullopt;
    }

private:
    uint8_t m_dev_id = 0;
    conn_h  m_conn   = INVALID_CONNECTION;

};


std::optional< std::string > device1_task() {
    static const uint8_t dev_id = 1;

    std::unique_ptr< device > dev_conn;
    try {
        dev_conn = std::make_unique< device > ( dev_id );
    }
    catch( connection_exception &ex ) {
        return std::string( ex.what() );
    }

    uint8_t power_on = 0xFD;
    if( !dev_conn->write8( addresses::power_on, power_on ) )
        return std::string( "cannot send command to power on" );

    uint8_t ready_value;
    if( auto res = dev_conn->read8( addresses::ready ) ) {
        ready_value = *res;
    }
    else
        return std::string( "cannot read status of the device" );

    if( ready_value != 42 )
        return std::string( "device doesn't ready" );

    uint16_t hello_value = 0x100;
    if( !dev_conn->write16( addresses::hello, hello_value ) )
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
    if( !dev_conn->write16( addresses::hello, hello_value ) )
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
