#ifndef __SPI_H__
#define __SPI_H__

#include "system.h"
#include "board_init.h"

void spi_init(enum spi_dev sd, enum trans_direct_type tdt, enum data_frame_format_e dff,
              enum software_slave_management_e ssm, enum frame_format_e ff, enum baud_rate_control_e brc,
              enum master_selection_e ms, enum clock_polarity_phase_e cpp);

void spi_enable(enum spi_dev sd, bool b);

#endif
