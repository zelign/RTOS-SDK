#include "fx_api.h"
#ifdef FX_ENABLE_FAULT_TOLERANT
#include "fx_fault_tolerant.h"
#endif /* FX_ENABLE_FAULT_TOLERANT */
#include "system.h"
#include "FreeRTOS.h"
#include "initcall.h"
#include "fileX.h"
#include "autoconfig.h"
#include "flash.h"
#include "task.h"

static UCHAR * fx_media_memory_ptr = NULL;
static FX_MEDIA * fx_user_media_ptr = NULL;

extern VOID fx_nor_flash_media_driver(FX_MEDIA * media_ptr);

void nor_flash_fileX_handler ( void *pvParameters )
{
    (void) pvParameters;
    fx_system_initialize();
	UINT fx_result = 0;
    struct flash_info *fx_flash_disk_info_ptr = NULL;

    fx_user_media_ptr = (FX_MEDIA *)malloc(sizeof(FX_MEDIA));
    if (!fx_user_media_ptr) {
        printf("Allocate memory for media ptr is FAILED!\n");
        goto fail;
    }
    fx_media_memory_ptr = (UCHAR *)malloc(CONFIG_SECTOR_SIZE);

    if (!is_flash_init()) {
        printf("Please init flash first!\n");
        goto fail;
    }
    fx_flash_disk_info_ptr = (struct flash_info *)malloc(sizeof(struct flash_info));

    fx_flash_disk_info_ptr = get_current_flash();

    /* Format the NOR Flash disk - the memory for the NOR Flash disk was setup in
       tx_application_define above.  */
    fx_result = fx_media_format(fx_user_media_ptr,
                    fx_nor_flash_media_driver,               // Driver entry
                    NULL,                                   // Nor flash disk memory pointer
                    fx_media_memory_ptr,                    // Media buffer pointer
                    CONFIG_SECTOR_SIZE,                     // Media buffer size
                    "NOR FLASH",                            // Volume Name
                    1,                                      // Number of FATs
                    32,                                     // Number of Root Directory Entries
                    0,                                      // Hidden sectors
                    (fx_flash_disk_info_ptr->chip_size/fx_flash_disk_info_ptr->sector_size*fx_flash_disk_info_ptr->page_num),   // Total sectors
                    ((fx_flash_disk_info_ptr->sector_size * 1024)/fx_flash_disk_info_ptr->page_num),                            // Sector size
                    fx_flash_disk_info_ptr->page_num,       // Sectors per cluster
                    0,                                      // Heads
                    0);                                     // Sectors per track
    if (fx_result != FX_SUCCESS) {
        free(fx_user_media_ptr);
		printf("Format media failed! %d\n", fx_result);
		goto fail;
	}
    printf("Format media Success\n");
#if 0
    fx_result = fx_media_open(&fx_nor_flash_media, "Nor Flash", fx_nor_flash_media_driver, NULL, &media_memory[0]);
	if (fx_result != FX_SUCCESS) {
		printf("Open media failed!\n");
		goto fail;
	}
#endif
fail:
	vTaskDelete(NULL);
}

void start_fileX(void)
{
    xTaskCreate(nor_flash_fileX_handler,
        "nor_flash_fileX",
        configMINIMAL_STACK_SIZE * 8,
        NULL,
        configMAX_PRIORITIES - 3,
        NULL);
}

#ifdef CONFIG_FILEX
APP_INIT_1(start_fileX);
#endif

