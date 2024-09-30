/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "spi.h"
#include "initcall.h"
#include "../../output/config.h"

/**
 * @brief set trans direction
 * 
 * @param sd 
 * @param tdt 
 * @param flag 0: full duplex, 1: only receive (double line single direction)
 *             0: forbidden input, 1: permit input (single line double direction)
 * 
 */
static inline void trans_direct(enum spi_dev sd, enum trans_direct_type tdt, bool flag)
{
    if (tdt) {
        /* select double line single direction */
        REG32((sd + SPI_CR1)) |= (1 << 15);
        /* configurate input */
        REG32((sd + SPI_CR1)) &= ~(1 << 10);
        REG32((sd + SPI_CR1)) |= (flag << 10);
    } else {
        /* select single line double direction */
        REG32((sd + SPI_CR1)) &= ~(1 << 15);
        /* configurate input */
        REG32((sd + SPI_CR1)) &= ~(1 << 14);
        REG32((sd + SPI_CR1)) |= (flag << 14);
    }
}

static inline void hardware_crc_caculation_enable(enum spi_dev sd, bool b)
{
    REG32((sd + SPI_CR1)) &= ~(1 << 13);
    REG32((sd + SPI_CR1)) |= (b << 13);
}

static inline void data_frame_format(enum spi_dev sd, enum data_frame_format_e dff)
{
    REG32((sd + SPI_CR1)) &= ~(1 << 11);
    REG32((sd + SPI_CR1)) |= (dff << 11);
}

static inline void software_slave_management(enum spi_dev sd, enum software_slave_management_e ssm)
{
    REG32((sd + SPI_CR1)) &= ~(1 << 9);
    REG32((sd + SPI_CR1)) |= (ssm << 9);
}

static inline void frame_format(enum spi_dev sd, enum frame_format_e ff)
{
    REG32((sd + SPI_CR1)) &= ~(1 << 7);
    REG32((sd + SPI_CR1)) |= (ff << 7);
}

void spi_enable(enum spi_dev sd, bool b)
{
    REG32((sd + SPI_CR1)) &= ~(1 << 6);
    REG32((sd + SPI_CR1)) |= (b << 6);
}

static inline void baud_rate_control(enum spi_dev sd, enum baud_rate_control_e brc)
{
    REG32((sd + SPI_CR1)) &= ~(7 << 3);
    REG32((sd + SPI_CR1)) |= (brc << 3);
}

static inline void master_selection(enum spi_dev sd, enum master_selection_e ms)
{
    REG32((sd + SPI_CR1)) &= ~(1 << 2);
    REG32((sd + SPI_CR1)) |= (ms << 2);
}

static inline void clock_polarity_phase(enum spi_dev sd, enum clock_polarity_phase_e cpp)
{
    REG32((sd + SPI_CR1)) &= ~(3 << 0);
    REG32((sd + SPI_CR1)) |= (cpp << 0);
}

void spi_init(enum spi_dev sd, enum trans_direct_type tdt, enum data_frame_format_e dff,
              enum software_slave_management_e ssm, enum frame_format_e ff, enum baud_rate_control_e brc,
              enum master_selection_e ms, enum clock_polarity_phase_e cpp)
{
    trans_direct(sd, tdt, DISABLE);
    data_frame_format(sd, dff);
    software_slave_management(sd, ssm);
    frame_format(sd, ff);
    baud_rate_control(sd, brc);
    master_selection(sd, ms);
    clock_polarity_phase(sd, cpp);
    spi_enable(sd, ENABLE);
}
#ifdef CONFIG_SPI
BOOT_INIT_3(spi_init);
#endif