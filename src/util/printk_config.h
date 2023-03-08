//
// Created by ryan on 3/7/23.
//

#pragma once

#include "peripherals/mini_uart.h"
#define _putchar        muart_send

/* Disable floating point printing */
#define PRINTK_DISABLE_SUPPORT_FLOAT
#define PRINTK_DISABLE_SUPPORT_EXPONENTIAL
