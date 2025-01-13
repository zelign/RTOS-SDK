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
		
			/* Perform basic initialization here... since the boot record is going
			   to be read subsequently and again for volume name requests.	*/
		
			/* Determine if we need to copy a saved format.  */
			if (_fx_ram_driver_copy_default_format)
			{
				destination_buffer =  (UCHAR *) media_ptr -> fx_media_driver_info;
				for (i = 0; i < (128 * 0xD5); i++)
				{
					destination_buffer[i] =  large_file_name_format[i];
				}
			}
									   
			/* Clean the driver write callback. */
			driver_write_callback = FX_NULL;
			driver_fault_tolerant_enable_callback = FX_NULL;
			driver_fault_tolerant_apply_log_callback = FX_NULL;
		
			/* Successful driver request.  */
			media_ptr -> fx_media_driver_status =  FX_SUCCESS;
			break;
		}
		case FX_DRIVER_BOOT_READ:
    	{
	        /* Read the boot record and return to the caller.  */
			flash_read(SPI_1, media_ptr->fx_media_memory_buffer, FX_BOOT_RECORD_LOGIC_SECTOR, FX_LOGICAL_SECTOR_SIZE);

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

        case FX_DRIVER_READ:
        {

            /* Calculate the RAM disk sector offset. Note the RAM disk memory is pointed to by
               the fx_media_driver_info pointer, which is supplied by the application in the 
               call to fx_media_open.  */
            source_buffer =  ((UCHAR *) media_ptr -> fx_media_driver_info) +
                                   ((media_ptr -> fx_media_driver_logical_sector + media_ptr -> fx_media_hidden_sectors) * media_ptr -> fx_media_bytes_per_sector);

            /* Copy the RAM sector into the destination.  */
            _fx_utility_memory_copy(source_buffer, media_ptr -> fx_media_driver_buffer, 
                                            media_ptr -> fx_media_driver_sectors * media_ptr -> fx_media_bytes_per_sector);

            /* Successful driver request.  */
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


