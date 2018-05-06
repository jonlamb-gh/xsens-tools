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

int main(int argc, char **argv)
{
    xsparser_s parser;

    uint8_t * const rx_buffer = calloc(
            sizeof(*rx_buffer),
            XS_MAXGARBAGE);
    assert(rx_buffer != NULL);

    g_exit_signaled = 0;
    register_sigint();

    xsparser_init(
            rx_buffer,
            XS_MAXGARBAGE,
            &parser);

    while(g_exit_signaled == 0)
    {
        // TODO
        (void) sleep(1);
    }

    free(rx_buffer);

    return EXIT_SUCCESS;
}
