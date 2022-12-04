/* Abstract API of communication protocol */

#include <stdlib.h>
#include <stdint.h>


#ifndef _DEVICE_API_H_
#define _DEVICE_API_H_


//! connection handle
typedef uint8_t conn_h;


//! invalid handle value
#define INVALID_CONNECTION ( 0xff )


/*! \param[in] dev_id ID of device on the bus 
 *  \return valid connection handle if connection is possible or INVALID_CONNECTION
 *
 * open connection to a device */
conn_h connection_open( uint8_t dev_id );


/*! \param[in] handle connection handle to close
 *
 * close connection to a device */
void connection_close( conn_h handle );


/*! \param[in] handle connection handle
 *  \param[in] upper_addr upper part of memory cell on the device
 *  \param[in] lower_addr lower part of memory cell on the device
 *  \param[in] data_ptr pointer to data to be written to the device
 *  \param[in] data_len size_of data
 *  \return length of written data or negative value in case of communication error
 * 
 * write block of data to the specific cell on the device */
int connection_write( conn_h handle,
                      uint8_t upper_addr, uint8_t lower_addr,
                      void *data_ptr, size_t data_len );


/*! \param[in] handle connection handle
 *  \param[in] upper_addr upper part of memory cell on the device
 *  \param[in] lower_addr lower part of memory cell on the device
 *  \param[in] data_ptr pointer to data to store result
 *  \param[in] data_len size of expected data
 *  \result length of read data or negative value in case of communication error
 * 
 * read block of data from the specific cell on the device */
int connection_read( conn_h handle,
                     uint8_t upper_addr, uint8_t lower_addr,
                     void *data_ptr, size_t data_len );


#endif /* _DEVICE_API_H_ */
