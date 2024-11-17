/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "spi.h"
#include "initcall.h"

/**
 * @brief set trans direction
 * 
 * @param sd 
 * @param tdt 
 * @param rm 0: full duplex, 1: only receive (double line single direction)
 *             0: forbidden input, 1: permit input (single line double direction)
 * 
 */
static inline void trans_direct(enum spi_dev sd, enum trans_direct_type tdt, enum rev_mode rm)
{
    if (tdt) {
        /* select double line single direction */
        REG32((sd + SPI_CR1)) |= (1 << 15);
        /* configurate input */
        REG32((sd + SPI_CR1)) &= ~(1 << 10);
        REG32((sd + SPI_CR1)) |= (rm << 10);
    } else {
        /* select single line double direction */
        REG32((sd + SPI_CR1)) &= ~(1 << 15);
        /* configurate input */
        REG32((sd + SPI_CR1)) &= ~(1 << 14);
        REG32((sd + SPI_CR1)) |= (rm << 14);
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

static inline void lsb_first_select(enum spi_dev sd, enum lsb_first_e lf)
{
    REG32((sd + SPI_CR1)) &= ~(1 << 7);
    REG32((sd + SPI_CR1)) |= (lf << 7);
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
    REG32((sd + SPI_CR1)) &= ~(0x3 << 0);
    REG32((sd + SPI_CR1)) |= (cpp << 0);
}

static inline void crc_polynomial(enum spi_dev sd, unsigned short pol)
{
    REG32((sd + SPI_CRCPR)) = (unsigned short)pol;
}

static inline void frame_format(enum spi_dev sd, enum frame_format_e ff)
{
    REG32((sd + SPI_CR2)) &= ~(1 << 4);
    REG32((sd + SPI_CR2)) |= (ff << 4);
}

static inline void inter_slave_select(enum spi_dev sd, bool flag)
{
    REG32((sd + SPI_CR1)) &= ~(0x1 << 8);
    if (flag)
        REG32((sd + SPI_CR1)) |= (0x1 << 8);
}

void spi_init(enum spi_dev sd, //select a spi base address
                    enum trans_direct_type tdt, //the spi trans direction
                    enum data_frame_format_e dff, //the spi send data size
                    enum software_slave_management_e ssm, //whether use software or hardware
                    enum lsb_first_e lf, //the first bit need to be send
                    enum baud_rate_control_e brc, //baud rate
                    enum master_selection_e ms, //master or slave
                    enum frame_format_e ff, //frame format
                    enum clock_polarity_phase_e cpp, //polarity and phase
                    bool flag) 
{
    trans_direct(sd, tdt, DISABLE);
    data_frame_format(sd, dff);
    software_slave_management(sd, ssm);
    lsb_first_select(sd, lf);
    baud_rate_control(sd, brc);
    master_selection(sd, ms);
    frame_format(sd, ff);
    clock_polarity_phase(sd, cpp);
    crc_polynomial(sd, SPI_CRCPR_Default);
    inter_slave_select(sd, flag);
    REG32(sd + SPI_I2SCFG) &= ~(0x1 << 11);
    spi_enable(sd, ENABLE);
}

/**
 * @brief 
 * 
 * @param sd 
 * @return true: not empty
 * @return false: empty
 */
bool spi_rev_buf_is_null(enum spi_dev sd)
{
    return ((REG32(sd + SPI_SR) & 0x1)? TRUE: FALSE);
}

/**
 * @brief 
 * 
 * @param sd 
 * @return true null
 * @return false 
 */
bool spi_sed_buf_is_null(enum spi_dev sd)
{
    return ((REG32(sd + SPI_SR) & 0x2)? TRUE: FALSE);
}

/**
 * @brief 
 * 
 * @param sd 
 * @return true 
 * @return false 
 */
bool spi_chk_buy(enum spi_dev sd)
{
    return ((REG32(sd + SPI_SR) & 0x80)? TRUE: FALSE);
}

/**
 * @brief 
 * 
 * @param sd 
 * @param write 
 * @param read 
 */
unsigned char spi_trans(enum spi_dev sd, void *write)
{
    if (REG32(sd + SPI_CR1) && SPI_DATA_FRAME_FORMAT_CHECK_MASK) {
        /* The data frame format is 16 bit write/read */
        while(!spi_sed_buf_is_null(sd));
        REG32(sd + SPI_DR) = *(unsigned short *)write;
        while(!spi_rev_buf_is_null(sd));
        return (REG32((sd + SPI_DR)) & 0xffff);
    } else {
        /* The data frame format is 8 bit write/read */
        while(!spi_sed_buf_is_null(sd));
        REG32(sd + SPI_DR) = *(unsigned char *)write;
        while(!spi_rev_buf_is_null(sd));
        return (REG32((sd + SPI_DR)) & 0xff);
        }
}
