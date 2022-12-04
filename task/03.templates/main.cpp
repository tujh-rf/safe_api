/* Example of compile-time validation of data with templates */

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

/* address now contains information about data size as well */
template< uint8_t Upper, uint8_t Lower, typename DataType >
struct address {
    typedef DataType type;

    /* anonymous enum will provide address information in compile time */
    enum {
        UPPER = Upper,
        LOWER = Lower
    };

    address( DataType _value = 0 ):
        value( _value ) {
    }

    DataType value;
};


namespace addresses {

/* very good, all information about memory cell in one place */
static const address< 0x00, 0x00, uint8_t >  power_on( 0xFD );
static const address< 0x10, 0xA0, uint16_t > hello;
static const address< 0xAA, 0xFF, uint8_t >  ready;

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

    /* let ask compiler implement read/write functions for us */

    template< typename Data >
    bool write( const Data &data );

    template< typename Data >
    std::optional< typename Data::type > read( const Data &data );

private:
    uint8_t m_dev_id = 0;
    conn_h  m_conn   = INVALID_CONNECTION;

};

/* common template without implementation - will raise compile time error for unknow type */
template< typename A >
struct data_write;

/* template specialization for uint8_t */
template<>
struct data_write< uint8_t > {
    static bool write( const conn_h conn, const uint8_t upper_addr, const uint8_t lower_addr, const uint8_t value ) {
        /* C'ish way of calling */
        uint8_t local_value = value;
        return ( connection_write( conn, upper_addr, lower_addr, &local_value, sizeof( uint8_t ) ) >= 0 );
    }
};

/* template specialization for uint8_t */
template<>
struct data_write< uint16_t > {
    static bool write( const conn_h conn, const uint8_t upper_addr, const uint8_t lower_addr, const uint16_t value ) {
        uint16_t local_value = htons( value );
        return ( connection_write( conn, upper_addr, lower_addr, &local_value, sizeof( uint16_t ) ) >= 0 );
    }
};

/* repeat the same for reading operation */
template< typename A >
struct data_read;

template<>
struct data_read< uint8_t > {
    static std::optional< uint8_t > read( const conn_h conn, const uint8_t upper_addr, const uint8_t lower_addr ) {
        uint8_t local_value;

        if( connection_read( conn, upper_addr, lower_addr, &local_value, sizeof( uint8_t ) ) >= 0 )
            return local_value;
        return std::nullopt;
    }
};

template<>
struct data_read< uint16_t > {
    static std::optional< uint16_t > read( const conn_h conn, const uint8_t upper_addr, const uint8_t lower_addr ) {
        uint16_t local_value;

        if( connection_read( conn, upper_addr, lower_addr, &local_value, sizeof( uint16_t ) ) >= 0 )
            return ntohs( local_value );
        return std::nullopt;
    }
};

/* there are implementation of read/write operations for device class,
 * compiler will generate function for each type from upper templates */
template< typename Data >
inline bool device::write( const Data &data ) {
    return data_write< typename Data::type >::write( this->m_conn, data.UPPER, data.LOWER, data.value );
}

template< typename Data >
inline std::optional< typename Data::type > device::read( const Data &data ) {
    return data_read< typename Data::type >::read( this->m_conn, data.UPPER, data.LOWER );
}


std::optional< std::string > device1_task() {
    static const uint8_t dev_id = 1;

    std::unique_ptr< device > dev_conn;
    try {
        dev_conn = std::make_unique< device > ( dev_id );
    }
    catch( connection_exception &ex ) {
        return std::string( ex.what() );
    }

    /* because power_on already contains value it is possible to use it directly */
    if( !dev_conn->write( addresses::power_on ) )
        return std::string( "cannot send command to power on" );

    /* no information about data size is needed here, everything is in addresses map */
    uint8_t ready_value;
    if( auto res = dev_conn->read( addresses::ready ) ) {
        /* compiler can check and produce warning for incompatible data types */
        ready_value = *res;
    }
    else
        return std::string( "cannot read status of the device" );

    if( ready_value != 42 )
        return std::string( "device doesn't ready" );

    /* if the value for command is not specified in the map,
     * it is always possible to specify it in place of usage */
    auto hello = addresses::hello;
    hello.value = 0x100;
    if( !dev_conn->write( hello ) )
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

    auto hello = addresses::hello;
    hello.value = 0x100;
    if( !dev_conn->write( hello ) )
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
