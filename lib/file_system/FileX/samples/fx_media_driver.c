#include "fx_api.h"
#ifdef FX_ENABLE_FAULT_TOLERANT
#include "fx_fault_tolerant.h"
#endif /* FX_ENABLE_FAULT_TOLERANT */
#include "system.h"
#include "storage.h"
#include "FreeRTOS.h"
#include "spi.h"
#include "flash.h"

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
			FX_ret = flash_storage_write((unsigned int)FX_BOOT_RECORD_LOGIC_SECTOR, media_ptr->fx_media_driver_buffer, CONFIG_SECTOR_SIZE);
			if (FX_ret != FX_SUCCESS) {
				printf("Write to boot sector failed!\n");
				media_ptr -> fx_media_driver_status =  FX_BOOT_ERROR;
			}
			media_ptr -> fx_media_driver_status =  FX_SUCCESS;
			break;
		}
        case FX_DRIVER_READ:
        {
			flash_storage_read((unsigned int)(media_ptr -> fx_media_driver_logical_sector * media_ptr->fx_media_bytes_per_sector), /* calculate the write address */
					(char *)media_ptr->fx_media_driver_buffer, CONFIG_SECTOR_SIZE);
            media_ptr -> fx_media_driver_status =  FX_SUCCESS;
            break;
        }
		case FX_DRIVER_WRITE:
		{
			FX_ret = flash_storage_write((unsigned int)(media_ptr -> fx_media_driver_logical_sector * media_ptr->fx_media_bytes_per_sector), /* calculate the write address */
					media_ptr->fx_media_driver_buffer, CONFIG_SECTOR_SIZE);
			if (FX_ret != FX_SUCCESS) {
				printf("Write to sector failed!\n");
				media_ptr -> fx_media_driver_status =  FX_BOOT_ERROR;
			}

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


