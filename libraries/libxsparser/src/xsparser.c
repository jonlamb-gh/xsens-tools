/**
 * @file xsparser.c
 * @brief TODO.
 *
 */

#include <stdint.h>
#include "xsparser/xsmessage.h"
#include "xsparser/xsparser.h"

static __attribute__((always_inline)) inline void update_byte(
        const uint8_t byte,
        xsparser_s * const parser)
{
    // TODO
    //assert(parser->bytes_read < parser->rx_buffer_size);

    parser->rx_buffer[parser->bytes_read] = byte;
    parser->bytes_read += 1;
    parser->checksum += byte;
}

void xsparser_init(
        uint8_t * const rx_buffer,
        const uint16_t rx_buffer_size,
        xsparser_s * const parser)
{
    parser->state = XSPARSER_STATE_PREAMBLE;
    parser->bytes_read = 0;
    parser->total_size = 0;
    parser->payload_size = 0;
    parser->checksum = 0;
    parser->valid_count = 0;
    parser->invalid_count = 0;
    parser->rx_buffer = rx_buffer;
    parser->rx_buffer_size = rx_buffer_size;
}

uint8_t xsparser_parse_byte(
        const uint8_t byte,
        xsparser_s * const parser)
{
    uint8_t msg_ready = 0;

    if(parser->state == XSPARSER_STATE_PREAMBLE)
    {
        parser->bytes_read = 0;
        parser->total_size = 0;
        parser->payload_size = 0;

        if(byte == XS_PREAMBLE)
        {
            update_byte(byte, parser);
            parser->checksum = 0;

            parser->state = XSPARSER_STATE_BUS_ID;
        }
    }
    else if(parser->state == XSPARSER_STATE_BUS_ID)
    {
        update_byte(byte, parser);
        parser->state = XSPARSER_STATE_MSG_ID;

        if((byte != XS_BID_MASTER) && (byte != XS_BID_SELF))
        {
            parser->state = XSPARSER_STATE_PREAMBLE;
        }
    }
    else if(parser->state == XSPARSER_STATE_MSG_ID)
    {
        update_byte(byte, parser);
        parser->state = XSPARSER_STATE_LEN;
    }
    else if(parser->state == XSPARSER_STATE_LEN)
    {
        update_byte(byte, parser);

        if(byte == XS_NO_PAYLOAD)
        {
            // message with no payload
            parser->total_size = XS_LEN_MSGHEADERCS;
            parser->state = XSPARSER_STATE_CHECKSUM;
        }
        else if(byte < XS_EXTLENCODE)
        {
            // message with standard payload
            parser->payload_size = (uint16_t) byte;
            parser->total_size = (XS_LEN_MSGHEADERCS + parser->payload_size);
            parser->state = XSPARSER_STATE_PAYLOAD;

            if(parser->payload_size > XS_MAXSHORTDATALEN)
            {
                parser->state = XSPARSER_STATE_PREAMBLE;
            }
        }
        else
        {
            // message with extended payload
            parser->state = XSPARSER_STATE_EXT_LEN_MSB;
        }
    }
    else if(parser->state == XSPARSER_STATE_EXT_LEN_MSB)
    {
        update_byte(byte, parser);
        parser->payload_size = (((uint16_t) byte) << 8);
        parser->state = XSPARSER_STATE_EXT_LEN_LSB;
    }
    else if(parser->state == XSPARSER_STATE_EXT_LEN_LSB)
    {
        update_byte(byte, parser);
        parser->payload_size |= ((uint16_t) byte);
        parser->total_size = (XS_LEN_MSGEXTHEADERCS + parser->payload_size);
        parser->state = XSPARSER_STATE_PAYLOAD;

        if(parser->payload_size > XS_MAXDATALEN)
        {
            parser->state = XSPARSER_STATE_PREAMBLE;
        }
    }
    else if(parser->state == XSPARSER_STATE_PAYLOAD)
    {
        if(parser->total_size > XS_MAXMSGLEN)
        {
            parser->state = XSPARSER_STATE_PREAMBLE;
        }

        update_byte(byte, parser);

        if((parser->bytes_read + 1) == (uint32_t) parser->total_size)
        {
            parser->state = XSPARSER_STATE_CHECKSUM;
        }
    }
    else if(parser->state == XSPARSER_STATE_CHECKSUM)
    {
        update_byte(byte, parser);

        if(parser->checksum == 0)
        {
            parser->valid_count += 1;
            msg_ready = 1;
        }
        else
        {
            parser->invalid_count += 1;
        }

        parser->state = XSPARSER_STATE_PREAMBLE;
    }

    return msg_ready;
}
