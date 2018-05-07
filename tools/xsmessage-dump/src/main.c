/**
 * @file main.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <assert.h>
#include "xsparser/xsparser.h"
#include "xsparser/xsmessage.h"

static volatile sig_atomic_t g_exit_signaled;

static void sig_handler(
        const int sig)
{
    if(sig == SIGINT)
    {
        g_exit_signaled = 1;
    }
}

static void register_sigint(void)
{
    struct sigaction act;

    memset(&act, 0, sizeof(act));
    act.sa_flags = SA_RESTART;
    act.sa_handler = sig_handler;

    if(sigaction(SIGINT, &act, 0) < 0)
    {
        perror("sigaction(SIGINT)");
        exit(1);
    }
}

static int configure_tty(
        const int fd,
        const int speed)
{
    struct termios tty;

    if(tcgetattr(fd, &tty) < 0)
    {
        perror("tcgetattr");
        return -1;
    }

    cfsetospeed(&tty, (speed_t) speed);
    cfsetispeed(&tty, (speed_t) speed);

    tty.c_cflag |= (CLOCAL | CREAD); /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8; /* 8-bit characters */
    tty.c_cflag &= ~PARENB; /* no parity bit */
    tty.c_cflag &= ~CSTOPB; /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS; /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if(tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        perror("tcsetattr");
        return -1;
    }

    return 0;
}

static uint16_t xsmsg_len(
        const xsmessage_header * const msg)
{
    uint16_t len;

    if(msg->length == XS_EXTLENCODE)
    {
        len = (msg->length_data.ext.length.high << 8) | (msg->length_data.ext.length.low);
    }
    else
    {
        len = msg->length;
    }

    return len;
}

static const uint8_t *xsmsg_data(
        const xsmessage_header * const msg)
{
    const uint8_t *data;

    if(msg->length == XS_EXTLENCODE)
    {
        data = &msg->length_data.ext.data[0];
    }
    else
    {
        data = &msg->length_data.data[0];
    }

    return data;
}

static void print_mtdata2(
        const uint16_t data_size,
        const uint8_t * const data)
{
    const uint8_t * const start = data;
    const uint8_t *ptr = start;

    while(ptr < (start + data_size))
    {
        const uint16_t item_id = (ptr[0] << 8) | (ptr[1]);

        ptr += sizeof(item_id);

        const uint8_t item_size = *ptr;

        ptr += sizeof(item_size);

        printf("  - identifer: 0x%04lX\n", (unsigned long) item_id);
        printf("    size: %u\n", (unsigned int) item_size);

        ptr += item_size;
    }
}

static void dump_msg(
        const xsmessage_header * const msg)
{
    const uint16_t data_size = xsmsg_len(msg);
    const uint8_t * const data = xsmsg_data(msg);

    printf("header\n");
    printf("  preamble: 0x%02X\n", (unsigned int) msg->preamble);
    printf("  bus_id: %u\n", (unsigned int) msg->bus_id);
    printf("  message_id: 0x%02X\n", (unsigned int) msg->msg_id);
    printf("  length: %u\n", (unsigned int) data_size);

    if(msg->msg_id == XS_MID_MTDATA2)
    {
        printf("  mtdata2 packet\n");

        print_mtdata2(data_size, data);
    }

    printf("\n");
    (void) fflush(stdout);
}

int main(int argc, char **argv)
{
    // TODO - CLI opts / remove asserts
    const char device_name[] = "/dev/ttyUSB0";
    xsparser_s parser;
    uint8_t io_in_buffer[64];

    const int device_fd = open(
            device_name,
            O_RDWR | O_NOCTTY | O_SYNC);
    assert(device_fd >= 0);

    const int cfg_err = configure_tty(
            device_fd,
            B115200);
    assert(cfg_err == 0);

    uint8_t * const parser_rx_buffer = calloc(
            XS_MAXGARBAGE,
            sizeof(*parser_rx_buffer));
    assert(parser_rx_buffer != NULL);

    g_exit_signaled = 0;
    register_sigint();

    xsparser_init(
            parser_rx_buffer,
            XS_MAXGARBAGE,
            &parser);

    while(g_exit_signaled == 0)
    {
        const int bytes_read = read(
                device_fd,
                &io_in_buffer[0],
                sizeof(io_in_buffer));

        if(bytes_read < 0)
        {
            perror("device_fd read");
            g_exit_signaled = 1;
        }
        else if(bytes_read > 0)
        {
            uint8_t idx;
            for(idx = 0; idx < (uint8_t) bytes_read; idx += 1)
            {
                const uint8_t parser_status = xsparser_parse_byte(
                        io_in_buffer[idx],
                        &parser);

                if(parser_status != 0)
                {
                    dump_msg((xsmessage_header*) parser_rx_buffer);
                }
            }
        }
    }

    close(device_fd);

    free(parser_rx_buffer);

    return EXIT_SUCCESS;
}
