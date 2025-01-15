#include "fx_api.h"
#ifdef FX_ENABLE_FAULT_TOLERANT
#include "fx_fault_tolerant.h"
#endif /* FX_ENABLE_FAULT_TOLERANT */
#include "system.h"
#include "storage.h"
#include "FreeRTOS.h"
#include "spi.h"
#include "flash.h"

static void print_memory(unsigned int start_addr, unsigned int length) {
    unsigned char *ptr = (unsigned char *)start_addr;
    unsigned int i;

    for (i = 0; i < length; i++) {
        if (i % 16 == 0) {
            if (i != 0) {
                printf("  ");
                for (int j = i - 16; j < i; j++) {
                    unsigned char c = ptr[j];
                    if (c >= 32 && c <= 126)
                        putc_usart1(c);
                    else
                        printf(".");
                }
            }
            printf("\n");
            printf("0x%x: ", (unsigned int)(ptr + i));
        }
        printf("%X ", ptr[i]);
    }

    if (i % 16 != 0) {
        int padding = 16 - (i % 16);
        for (int j = 0; j < padding; j++) {
            printf("   ");
        }

        printf("  ");
        for (int j = i - (i % 16); j < i; j++) {
            unsigned char c = ptr[j];
            if (c >= 32 && c <= 126)
                putc_usart1(c);
            else
                printf(".");
        }
        printf("\n");
    }
}

VOID fx_nor_flash_media_driver(FX_MEDIA * media_ptr)
{
	INT FX_ret = 0;
	if (!media_ptr) {
		printf("Please pass a valid media pointer!\n");
		media_ptr -> fx_media_driver_status =  FX_IO_ERROR;
		return;
	}

	switch (media_ptr->fx_media_driver_request) {
		case FX_DRIVER_INIT:
		{
			/* FLASH drivers are responsible for setting several fields in the 
			   media structure, as follows:
		
					media_ptr -> fx_media_driver_free_sector_update
					media_ptr -> fx_media_driver_write_protect
		
			   The fx_media_driver_free_sector_update flag is used to instruct
			   FileX to inform the driver whenever sectors are not being used.
			   This is especially useful for FLASH managers so they don't have 
			   maintain mapping for sectors no longer in use.
		
			   The fx_media_driver_write_protect flag can be set anytime by the
			   driver to indicate the media is not writable.  Write attempts made
			   when this flag is set are returned as errors.  */
		
			/* Successful driver request.  */
			flash_chip_erase();
			media_ptr -> fx_media_driver_status =  FX_SUCCESS;
			break;
		}
		case FX_DRIVER_BOOT_READ:
    	{
	        /* Read the boot record and return to the caller.  */
			flash_storage_read(FX_BOOT_RECORD_LOGIC_SECTOR, (char *)(media_ptr->fx_media_memory_buffer), CONFIG_SECTOR_SIZE);

	        /* Determine if the boot record is valid.  */
	        if (((media_ptr->fx_media_memory_buffer[0] != (UCHAR)0xEB) ||
				(media_ptr->fx_media_memory_buffer[2] != (UCHAR)0x90)) &&
				((media_ptr->fx_media_memory_buffer[0] != (UCHAR)0xE9)))
	        {

	            /* Invalid boot record, return an error!  */
	            media_ptr -> fx_media_driver_status =  FX_MEDIA_INVALID;
	            return;
	        }

	        /* Successful driver request.  */
	        media_ptr -> fx_media_driver_status =  FX_SUCCESS;
	        break;
    	}
		case FX_DRIVER_BOOT_WRITE:
		{
			print_memory(media_ptr->fx_media_driver_buffer, CONFIG_SECTOR_SIZE);
			FX_ret = by25q64as_read_write(SPI_1, (unsigned int)FX_BOOT_RECORD_LOGIC_SECTOR, media_ptr->fx_media_driver_buffer, CONFIG_SECTOR_SIZE);
			if (FX_ret != FX_SUCCESS) {
				printf("Write to boot sector failed!\n");
				media_ptr -> fx_media_driver_status =  FX_BOOT_ERROR;
			}
			printf("\nWrite to boot sector success!\n");
			print_memory(FX_BOOT_RECORD_LOGIC_SECTOR, CONFIG_SECTOR_SIZE);
			media_ptr -> fx_media_driver_status =  FX_SUCCESS;
			break;
		}
        case FX_DRIVER_READ:
        {
            media_ptr -> fx_media_driver_status =  FX_SUCCESS;
            break;
        }
		case FX_DRIVER_WRITE:
		{
			media_ptr -> fx_media_driver_status =  FX_SUCCESS;
            break;
		}
		case FX_DRIVER_UNINIT:
		{
			media_ptr -> fx_media_driver_status =  FX_SUCCESS;
            break;
		}
		default:
		{
	        /* Invalid driver request.  */
	        media_ptr -> fx_media_driver_status =  FX_IO_ERROR;
	        break;
    	}
	}
	

}


