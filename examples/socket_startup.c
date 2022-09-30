/* -----------------------------------------------------------------------------
 * Copyright (c) 2020 Arm Limited (or its affiliates). All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * -------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "Driver_WiFi.h"

/* Wi-Fi configuration */
#define SSID "ssid"
#define PASSWORD "password"
#define SECURITY_TYPE ARM_WIFI_SECURITY_WPA2

#define AP_SSID "wizfi360pico"
#define AP_PASSWORD "1234567890"
#define AP_SECURITY_TYPE ARM_WIFI_SECURITY_WPA2

extern ARM_DRIVER_WIFI Driver_WiFi1;

extern char* serial_read_string(void);

int32_t socket_startup(void)
{
    ARM_WIFI_CONFIG_t config;
    int32_t retval = ARM_DRIVER_ERROR;

    printf(" Connecting to WiFi ...\n");

    retval = Driver_WiFi1.Initialize(NULL);
    printf(" Driver_WiFix.Initialize(NULL) : %d\n", retval);

    retval = Driver_WiFi1.PowerControl(ARM_POWER_FULL);
    printf(" Driver_WiFix.PowerControl(ARM_POWER_FULL) : %d\n", retval);

    memset((void *)&config, 0, sizeof(config));

    config.ssid = SSID;
    config.pass = PASSWORD;
    config.security = SECURITY_TYPE;
    config.ch = 0U;

    retval = Driver_WiFi1.Activate(0U, &config);
    printf(" Driver_WiFix.Activate(0U, &config) : %d\n", retval);

    retval = Driver_WiFi1.IsConnected();
    printf(" Driver_WiFix.IsConnected() : %d\n", retval);

    if (retval == 0U)
    {
        printf(" Wi-Fi network connection failed!\n");

        return -1;
    }
    else // returns '1' if the Wi-Fi connection is successful.
    {
        printf(" Wi-Fi network connection succeeded!\n");
    }

    return 0;
}

int32_t socket_startup_serial(void)
{
    ARM_WIFI_CONFIG_t config;
    int32_t retval = ARM_DRIVER_ERROR;

    printf(" Connecting to WiFi ...\n");

    retval = Driver_WiFi1.Initialize(NULL);
    printf(" Driver_WiFix.Initialize(NULL) : %d\n", retval);

    retval = Driver_WiFi1.PowerControl(ARM_POWER_FULL);
    printf(" Driver_WiFix.PowerControl(ARM_POWER_FULL) : %d\n", retval);

    memset((void *)&config, 0, sizeof(config));

    getchar();  // FIXME: non-breaking space
    printf("input ssid: ");
    char *ssid = serial_read_string();
    printf("input pass: ");
    char *pass = serial_read_string();
    printf("ssid: %s pass: %s\n", ssid, pass);

    config.ssid = ssid;
    config.pass = pass;
    config.security = SECURITY_TYPE;
    config.ch = 0U;

    retval = Driver_WiFi1.Activate(0U, &config);
    printf(" Driver_WiFix.Activate(0U, &config) : %d\n", retval);

    retval = Driver_WiFi1.IsConnected();
    printf(" Driver_WiFix.IsConnected() : %d\n", retval);

    if (retval == 0U)
    {
        printf(" Wi-Fi network connection failed!\n");

        return -1;
    }
    else // returns '1' if the Wi-Fi connection is successful.
    {
        printf(" Wi-Fi network connection succeeded!\n");
    }

    return 0;
}

int32_t socket_startup_ap(void)
{
    ARM_WIFI_CONFIG_t config;
    int32_t retval = ARM_DRIVER_ERROR;

    printf(" AP mode, Connecting to Device ...\n");

    retval = Driver_WiFi1.Initialize(NULL);
    printf(" Driver_WiFix.Initialize(NULL) : %d\n", retval);

    retval = Driver_WiFi1.PowerControl(ARM_POWER_FULL);
    printf(" Driver_WiFix.PowerControl(ARM_POWER_FULL) : %d\n", retval);

    memset((void *)&config, 0, sizeof(config));

    config.ssid = AP_SSID;
    config.pass = AP_PASSWORD;
    config.security = AP_SECURITY_TYPE;
    config.ch = 1U;

    retval = Driver_WiFi1.Activate(1U, &config);
    printf(" Driver_WiFix.Activate(1U, &config) : %d\n", retval);

    if (retval == 0U) {
        printf("WiFi Activated = %d\r\n", retval);
    } else {
        printf("WiFi Activate FAIL = %d\r\n", retval);
    }

    /* Driver Options block */
    uint8_t ap_ip[4] = {192, 168, 36, 1};       /* Server IP address */
    // uint8_t ap_ip[4] = {
    //     0,
    // };
    uint8_t ap_gateway[4] = {192U, 168U, 36U, 1U};
    uint8_t ap_netmask[4] = {255U, 255U, 255U, 0U};
    uint8_t ap_dhcp_pool_start[4] = {192U, 168U, 36U, 100U};
    uint8_t ap_dhcp_pool_end[4] = {192U, 168U, 36U, 200U};
    uint32_t ap_dhcp_lease = 2000;
    // uint8_t dns[4] = {8U, 8U, 8U, 8U};
    // uint8_t ap_mac[6] = {0x02, 0x08, 0xdc, 0x36, 0x07, 0x6d};

    /* Driver Settings block */
    retval += Driver_WiFi1.SetOption(1U, ARM_WIFI_IP, ap_ip, 4U); // 0U:station, 1U:Ap
    // retval = Driver_WiFi1.SetOption (1U, ARM_WIFI_MAC, ap_mac, 6U);
    retval += Driver_WiFi1.SetOption(1U, ARM_WIFI_IP_GATEWAY, ap_gateway, 4U);
    retval += Driver_WiFi1.SetOption(1U, ARM_WIFI_IP_SUBNET_MASK, ap_netmask, 4U);
    retval += Driver_WiFi1.SetOption(1U, ARM_WIFI_IP_DHCP_POOL_BEGIN, ap_dhcp_pool_start, 4U);
    retval += Driver_WiFi1.SetOption(1U, ARM_WIFI_IP_DHCP_POOL_END, ap_dhcp_pool_end, 4U);
    retval += Driver_WiFi1.SetOption(1U, ARM_WIFI_IP_DHCP_LEASE_TIME, ap_dhcp_lease, 4U);
    // retval = Driver_WiFi1.SetOption (1U, ARM_WIFI_IP_DNS1, dns, 4U);

    if (retval == 0U) {
        printf("WiFi SetOption succeeded!\r\n");
    } else {
        printf("WiFi SetOption error failed!\r\n");
    }

    // retval = Driver_WiFi1.IsConnected();
    // printf(" Driver_WiFix.IsConnected() : %d\n", retval);

    // if (retval == 0U)
    // {
    //     printf(" Wi-Fi network connection failed!\n");

    //     return -1;
    // }
    // else // returns '1' if the Wi-Fi connection is successful.
    // {
    //     printf(" Wi-Fi network connection succeeded!\n");
    // }

    return 0;
}
