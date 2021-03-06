/**
  ******************************************************************************
  * @file    entropy_hardware_poll.c
  *
  ******************************************************************************
  */

#include <string.h>
#include "platform.h"   // __LIT for all compilers
#include "r_s12ad_rx_if.h"
#include "mbedtls/entropy_poll.h"

void get_random_number(uint8_t *data, uint32_t len);

/******************************************************************************
Functions : hardware entropy collector(repeatedly called until enough gathered)
******************************************************************************/
int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
    R_INTERNAL_NOT_USED(data);
    R_INTERNAL_NOT_USED(len);

    uint32_t random_number = 0;

    get_random_number((uint8_t *)&random_number, sizeof(uint32_t));
    *olen = 0;

    memcpy(output, &random_number, sizeof(uint32_t));
    *olen = sizeof(uint32_t);

    return 0;
}

/******************************************************************************
Functions : random number generator(XorShift method)

WARNING: The random number generation solution presented in this application is 
    for demonstration purposes only. It is not recommended to go into production with 
    the logic presented here. The current solution takes entropy from the a
    temperature sensor on the board and from the current system time. For 
    production development, Renesas RX65x customers are recommended to use the 
    TRNG included in the Trusted Secure IP Driver. Please see the following for more information
    on the Trusted Secure IP Driver: https://www.renesas.com/us/en/products/software-tools/software-os-middleware-driver/security-crypto/trusted-secure-ip-driver.html

******************************************************************************/
void get_random_number(uint8_t *data, uint32_t len)
{
    static uint32_t y = 2463534242;
    uint32_t res;
    uint32_t lp;
    uint8_t *bPtr;

    vTaskDelay(10);

    y += xTaskGetTickCount();   /* randomness from system timer */

    res = len / 4;
    for (lp = 0; lp < res; lp++)
    {
        y = y ^ (y << 13);
        y = y ^ (y >> 17);
        y = y ^ (y << 5);
        bPtr = (uint8_t*) & y;
#if __LIT
        *((uint32_t *)data) = (uint32_t)((*(bPtr + 3) << 24) | (*(bPtr + 2) << 16) | (*(bPtr + 1) << 8) | *(bPtr + 0));
#else
        *((uint32_t *)data) = y;
#endif
        data += 4;
    }
    y = y ^ (y << 13);
    y = y ^ (y >> 17);
    y = y ^ (y << 5);
    res = (uint32_t)len % 4;
    bPtr = (uint8_t*) & y;
    switch (res)
    {
        case 3:
#if __LIT
            *data++ = bPtr[3];
            *data++ = bPtr[2];
            *data++ = bPtr[1];
#else
            *data++ = bPtr[0];
            *data++ = bPtr[1];
            *data++ = bPtr[2];
#endif
            break;

        case 2:
#if __LIT
            *data++ = bPtr[3];
            *data++ = bPtr[2];
#else
            *data++ = bPtr[0];
            *data++ = bPtr[1];
#endif
            break;

        case 1:
#if __LIT
            *data++ = bPtr[3];
#else
            *data++ = bPtr[0];
#endif
            break;

        default:
            /* no op */
            break;
    }
}
