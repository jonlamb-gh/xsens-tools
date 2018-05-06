/**
 * @file parser.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "xsparser/xsparser.h"
#include "xsparser/xsmessage.h"

int main(int argc, char **argv)
{
    xsparser_s parser;

    uint8_t * const rx_buffer = calloc(
            sizeof(*rx_buffer),
            XS_MAXGARBAGE);
    assert(rx_buffer != NULL);

    xsparser_init(
            rx_buffer,
            XS_MAXGARBAGE,
            &parser);

    assert(parser.state == XSPARSER_STATE_PREAMBLE);

    free(rx_buffer);

    return EXIT_SUCCESS;
}
