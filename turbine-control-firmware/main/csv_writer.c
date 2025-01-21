//
// Created by Nicholas Dahlke on 20.01.2025.
//
#include "csv_writer.h"
static const char *TAG = "CSV Writer";

void init_csv_writer()
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    slot_config.width = 4;
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ESP_LOGI(TAG, "Mounting filesystem");
    ESP_ERROR_CHECK(esp_vfs_fat_sdmmc_mount(
        mount_point,
        &host,
        &slot_config,
        &mount_config,
        &card
    ));
    ESP_LOGI(TAG, "Filesystem mounted");
    sdmmc_card_print_info(stdout, card);

    const char *log_file = MOUNT_POINT"/log.csv";
    csv_file = fopen(log_file, "w");

}

void close_csv_writer()
{
    ESP_LOGI(TAG, "Unmounting card");
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");
}

void csv_write_data(*float data, int size_data)
{
    char line[MAX_CHAR_SIZE];
    strcat(1,2);
}



// End of file csv_writer.h