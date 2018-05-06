/**
 * @file xsmessage.h
 * @brief TODO.
 *
 * @note Most of these definitions were taken from the Xsens SDK.
 *
 */

#ifndef XSMESSAGE_H
#define XSMESSAGE_H

#include <stdint.h>

#define XS_PREAMBLE             (0xFA)
#define XS_EXTLENCODE           (0xFF)
#define XS_NO_PAYLOAD           (0x00)

#define XS_LEN_MSGHEADER        (4)
#define XS_LEN_MSGEXTHEADER     (6)
#define XS_LEN_MSGHEADERCS      (5)
#define XS_LEN_MSGEXTHEADERCS   (7)
#define XS_LEN_CHECKSUM         (1)
#define XS_LEN_UNSIGSHORT       (2)
#define XS_LEN_UNSIGINT         (4)
#define XS_LEN_FLOAT            (4)

#define XS_MAXDATALEN           (8192-XS_LEN_MSGEXTHEADERCS)
#define XS_MAXSHORTDATALEN      (254)
#define XS_MAXMSGLEN            (XS_MAXDATALEN+XS_LEN_MSGEXTHEADERCS)
#define XS_MAXSHORTMSGLEN       (XS_MAXSHORTDATALEN+XS_LEN_MSGHEADERCS)
#define XS_MAXGARBAGE           (XS_MAXMSGLEN+1)

typedef struct __attribute__((packed))
{
    uint8_t preamble;
    uint8_t busid;
    uint8_t msgid;
    uint8_t length;
    union length_data
    {
        struct extended_length
        {
            struct ext_parts
            {
                uint8_t high;
                uint8_t low;
            } length;
            uint8_t data[1];
        } ext;
        uint8_t data[1];
    } length_data;
} xsmessage_header;

#endif /* XSMESSAGE_H */
