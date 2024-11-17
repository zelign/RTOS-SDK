#ifndef __SPI_H__
#define __SPI_H__

#include "system.h"
#include "board_init.h"
#include "../../output/config.h"

#define SPI_CRCPR_Default                           0x7
#define SPI1_CLK_ENABLE                             ((RCC_APB2ENR) |= (0x1 << 12))
#define SPI1_RESET                                  \
                                                    ((RCC_APB2RSTR) |= (0x1 << 12)); \
                                                    ((RCC_APB2RSTR) &= ~(0x1 << 12))
#define SPI_DATA_FRAME_FORMAT_CHECK_MASK            (0x0800)

void spi_enable(enum spi_dev sd, bool b);

/**
 * @brief 
 * 
 * @param sd 
 * @param tdt 
 * @param dff 
 * @param ssm 
 * @param lf 
 * @param brc 
 * @param ms 
 * @param ff 
 * @param cpp 
 */
void spi_init(enum spi_dev sd, //select a spi base address
                    enum trans_direct_type tdt, //the spi trans direction
                    enum data_frame_format_e dff, //the spi send data size
                    enum software_slave_management_e ssm, //whether use software or hardware
                    enum lsb_first_e lf, //the first bit need to be send
                    enum baud_rate_control_e brc, //baud rate
                    enum master_selection_e ms, //master or slave
                    enum frame_format_e ff, //frame format
                    enum clock_polarity_phase_e cpp, //polarity and phase
                    bool flag);
/**
 * @brief 
 * 
 * @param sd 
 * @param write 
 * @param read 
 */
unsigned char spi_trans(enum spi_dev sd, void *write);

/**
 * @brief 
 * 
 * @param sd 
 * @return true: not empty
 * @return false: empty
 */
bool spi_rev_buf_is_null(enum spi_dev sd);

/**
 * @brief 
 * 
 * @param sd 
 * @return true 
 * @return false 
 */
bool spi_chk_buy(enum spi_dev sd);

#endif
