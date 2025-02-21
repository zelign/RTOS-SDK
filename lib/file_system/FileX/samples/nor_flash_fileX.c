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
    memset(fx_user_media_ptr, 0, sizeof(FX_MEDIA));

    fx_media_memory_ptr = (UCHAR *)malloc(CONFIG_SECTOR_SIZE);
    if (!fx_media_memory_ptr) {
        printf("Allocate memory for media memory is FAILED!\n");
        goto fail;
    }
    memset(fx_media_memory_ptr, 0, CONFIG_SECTOR_SIZE);

    if (!is_flash_init()) {
        printf("Please init flash first!\n");
        goto fail;
    }

    fx_flash_disk_info_ptr = get_current_flash();

    printf("Open media ...\n");

    fx_result = fx_media_open(fx_user_media_ptr, "Nor Flash", fx_nor_flash_media_driver, NULL, fx_media_memory_ptr, CONFIG_SECTOR_SIZE);
	if (fx_result != FX_SUCCESS) {
        /* Format the NOR Flash disk - the memory for the NOR Flash disk was setup in
       tx_application_define above.  */
        printf("Open media failed, please first format media ...\n");
        fx_result = fx_media_format(fx_user_media_ptr,
                        fx_nor_flash_media_driver,               // Driver entry
                        NULL,                                   // Nor flash disk memory pointer
                        fx_media_memory_ptr,                    // Media buffer pointer
                        CONFIG_SECTOR_SIZE,                     // Media buffer size
                        "NOR FLASH",                            // Volume Name
                        1,                                      // Number of FATs
                        512,                                     // Number of Root Directory Entries
                        0,                                      // Hidden sectors
                        (fx_flash_disk_info_ptr->chip_size*1024/(fx_flash_disk_info_ptr->sector_size * 1024 / fx_flash_disk_info_ptr->page_num)),   // Total sectors
                        ((fx_flash_disk_info_ptr->sector_size * 1024)/fx_flash_disk_info_ptr->page_num),                            // Sector size
                        fx_flash_disk_info_ptr->page_num,       // Sectors per cluster
                        0,                                      // Heads
                        0);                                     // Sectors per track
        if (fx_result != FX_SUCCESS) {
            free(fx_user_media_ptr);
            printf("Failed! %d\n", fx_result);
            goto fail;
        }
        printf("Success!\n");

        printf("Open media ...\n");
        fx_result = fx_media_open(fx_user_media_ptr, "Nor Flash", fx_nor_flash_media_driver, NULL, fx_media_memory_ptr, CONFIG_SECTOR_SIZE);
        if (fx_result != FX_SUCCESS) {
            printf("Failed! %d\n", fx_result);
            goto fail;
        }
        printf("Success!\n");
    } else
        printf("Success!\n");
#ifdef FX_READ_WRITE_TEST
    printf("Create file /FileX_test ...\n");

    FX_FILE *fx_file_ptr = (FX_FILE *)malloc(sizeof(FX_FILE));
    if (!fx_file_ptr) {
        printf("Allocate memory for file is FAILED!\n");
        goto fail;
    }
    memset(fx_file_ptr, 0, sizeof(FX_FILE));

    fx_result = fx_file_create(fx_user_media_ptr, "/FileX_test");
    if (fx_result != FX_SUCCESS) {
        printf("Failed! %d\n", fx_result);
        goto fail;
    }
    printf("Success!\n");

    printf("Open file /FileX_test ...!\n");
    fx_result = fx_file_open(fx_user_media_ptr, fx_file_ptr, "/FileX_test", FX_OPEN_FOR_WRITE);
    if (fx_result != FX_SUCCESS) {
        printf("Failed! %d\n", fx_result);
        goto fail;
    }
    printf("Success!\n");
    printf("Write file /FileX_test ...!\n");
    char write_test[] = "Hello FileX!\n";
    fx_result = fx_file_write(fx_file_ptr, &write_test[0], strlen(write_test) + 1);
    if (fx_result != FX_SUCCESS) {
        printf("Failed! %d\n", fx_result);
        goto fail;
    }
    fx_result = fx_file_seek(fx_file_ptr, FX_SEEK_BEGIN);
    if (fx_result != FX_SUCCESS) {
        printf("Failed Seek! %d\n", fx_result);
        goto fail;
    }
    printf("Success!\n");
    printf("Read file /FileX_test ...!\n");

    char *read_test = (char *)malloc(32);
    memset(read_test, 0, 32);
    ULONG actual_size = 0;
    fx_result = fx_file_read(fx_file_ptr, read_test, strlen(write_test), &actual_size);
    if (fx_result != FX_SUCCESS) {
        printf("Failed! %d\n", fx_result);
        goto fail;
    }
    printf("Success! [%s]\n", read_test);
#endif //FX_READ_WRITE_TEST
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

