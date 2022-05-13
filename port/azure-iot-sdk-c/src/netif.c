// Copyright (c) WIZnet. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// CAVEAT: This sample is to demonstrate Azure IoT client concepts only and is not a guide design principles or style
// Checking of return codes and error values shall be omitted for brevity.  Please practice sound engineering practices
// when writing production code.

/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>

#include "netif.h"

#include "iot_socket.h"
#include "Driver_WiFi.h"

#include "sys/socket.h"

/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
/* SNTP */
#define SNTP_SERVER_NAME "pool.ntp.org"
#define TIMEZONE (1 * 60 * 60 * 9) // Korea, UTC+9

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
/* Network */
NetworkContext_t g_network_context;

/* SNTP */
static uint8_t g_sntp_server_ip[4] = {0, };
static uint8_t g_sntp_get_current_timestamp_flag = 0;
static time_t g_curr_sectime = 0;

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* DNS */
int32_t wizchip_get_host_by_name(const char *name, int32_t af, uint8_t *ip, uint32_t *ip_len)
{
    int32_t retval = -1;

    *ip_len = 4U;

    retval = iotSocketGetHostByName(name, af, ip, ip_len);

    if (name[0] == '\0')
    {
        return (-1);
    }
    else
    {
        if (retval == 0)
        {
            printf(" DNS success\n");
            printf(" DNS target domain : %s\n", name);
            printf(" DNS IP of target domain : %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
        }
        else
        {
            printf(" DNS failed\n");

            return (-1);
        }
    }

    return 0;
}

/* SNTP */
void wizchip_sntp_init(void)
{
    int32_t retval = -1;
    uint32_t ip_len = 0;

    /* Resolve SNTP/NTP server IP address */
    retval = wizchip_get_host_by_name(SNTP_SERVER_NAME, IOT_SOCKET_AF_INET, g_sntp_server_ip, &ip_len);

    if (retval != 0)
    {
        printf(" Fail to getting SNTP server IP, set the '216.239.35.0 (time.google.com)'\n");

        // time.google.com
        g_sntp_server_ip[0] = 216;
        g_sntp_server_ip[1] = 239;
        g_sntp_server_ip[2] = 35;
        g_sntp_server_ip[3] = 0;
    }
}

time_t wizchip_sntp_get_current_timestamp(void)
{
    int32_t retval = -1;
    uint8_t buf[48] = {0, };
    uint32_t timeout = 0;

    if (g_sntp_get_current_timestamp_flag == 0)
    {
        /* Compose SNTP request: vers.3, mode = client */
        memset(buf, 0, sizeof(buf));
        buf[0] = 0x1B;

        /* Create UDP socket */
        g_network_context.socket_descriptor = iotSocketCreate(IOT_SOCKET_AF_INET, IOT_SOCKET_SOCK_DGRAM, IOT_SOCKET_IPPROTO_UDP);

        if (g_network_context.socket_descriptor < 0)
        {
            printf(" SNTP socket create failed : %d\n", g_network_context.socket_descriptor);
        }

        /* Set socket receive timeout: 5 seconds */
        timeout = 5000U;

        retval = iotSocketSetOpt(g_network_context.socket_descriptor, IOT_SOCKET_SO_RCVTIMEO, &timeout, sizeof(timeout));

        if (retval < 0)
        {
            iotSocketClose(g_network_context.socket_descriptor);
        }

        /* Send SNTP request (port 123) */
        retval = iotSocketSendTo(g_network_context.socket_descriptor, buf, sizeof(buf), g_sntp_server_ip, sizeof(g_sntp_server_ip), 123U);

        if (retval < 0)
        {
            iotSocketClose(g_network_context.socket_descriptor);
        }

        /* Read SNTP response */
        retval = iotSocketRecv(g_network_context.socket_descriptor, buf, sizeof(buf));

        if (retval < 0)
        {
            iotSocketClose(g_network_context.socket_descriptor);
        }

        /* Extract time */
        g_curr_sectime = ((buf[40] << 24) | (buf[41] << 16) | (buf[42] << 8) | buf[43]) - 2208988800U + TIMEZONE;

        if ((retval >= 0) && (g_curr_sectime > 0))
        {
            g_sntp_get_current_timestamp_flag = 1;
        }

        iotSocketClose(g_network_context.socket_descriptor);

        return g_curr_sectime;
    }
    else
    {
        return g_curr_sectime;
    }
}
