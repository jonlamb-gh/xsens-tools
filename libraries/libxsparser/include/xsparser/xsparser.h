/**
 * @file xsparser.h
 * @brief TODO.
 *
 */

#ifndef XSPARSER_H
#define XSPARSER_H

#include <stdint.h>
#include "xsmessage.h"

typedef enum
{
    XSPARSER_STATE_PREAMBLE = 0,
    XSPARSER_STATE_BUS_ID,
    XSPARSER_STATE_MSG_ID,
    XSPARSER_STATE_LEN,
    XSPARSER_STATE_EXT_LEN_MSB,
    XSPARSER_STATE_EXT_LEN_LSB,
    XSPARSER_STATE_PAYLOAD,
    XSPARSER_STATE_CHECKSUM
} xsparser_state_kind;

typedef struct
{
    xsparser_state_kind state; /*!< Parser state. */
    uint32_t bytes_read; /*!< Number of bytes received for the current message. */
    uint16_t total_size; /*!< Size of the current message (header, payload, and checksum) in bytes. */
    uint16_t payload_size; /*!< Size of the current message payload in bytes. */
    uint8_t checksum; /*!< Calculated checksum for the current message. */
    uint32_t valid_count; /*!< Number of valid messages parsed. */
    uint32_t invalid_count; /*!< Number of invalid messages parsed/aborted. */
    uint8_t *rx_buffer; /*!< Pointer to user buffer where received message data is stored. */
    uint16_t rx_buffer_size; /*!< Size of user receive buffer in bytes. */
} xsparser;

void xsparser_init(
        uint8_t * const rx_buffer,
        const uint16_t rx_buffer_size,
        xsparser * const parser);

uint8_t xsparser_parse_byte(
        const uint8_t byte,
        xsparser * const parser);

#endif /* XSPARSER_H */
