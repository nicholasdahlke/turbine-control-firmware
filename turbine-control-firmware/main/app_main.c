#define LOCAL_LOG_LEVEL ESP_LOG_VERBOSE
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_vfs_fat.h>
#include <driver/sdmmc_host.h>
#include <sdmmc_cmd.h>
#include <driver/pulse_cnt.h>
#include "controller.h"

#define MOUNT_POINT "/sdcard"
#define MAX_CHAR_SIZE 64

static const char *TAG = "Turbine Control Firmware";

void initialize()
{

    init_controller();
    //set_fan_state(true);
    
    // SD Card
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
            .format_if_mount_failed = false,
            .max_files = 5,
            .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card;

    const char mount_point[] = MOUNT_POINT;
    printf("Initializing SD Card\n");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = 4;
    //slot_config.gpio_wp = 16;
    //slot_config.gpio_cd = 5;
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ESP_LOGI(TAG, "Mounting filesystem");
    esp_err_t ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                          "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                          "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
    }
    ESP_LOGI(TAG, "Filesystem mounted");
    sdmmc_card_print_info(stdout, card);
    const char *file_hello = MOUNT_POINT"/hello.txt";
    char data[MAX_CHAR_SIZE];
    snprintf(data, MAX_CHAR_SIZE, "%s %s!\n", "Hello", card->cid.name);
    ESP_LOGI(TAG, "Opening file %s", file_hello);
    FILE *f = fopen(file_hello, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
    }
    fprintf(f, data);
    fclose(f);
    ESP_LOGI(TAG, "File written");
    //const char *path = MOUNT_POINT"/test.txt";
    //FILE *f = fopen(path, "r");
    //if (f == NULL) {
    //    ESP_LOGE(TAG, "Failed to open file for reading");
    //}
    //char line[MAX_CHAR_SIZE];
    //fgets(line, sizeof(line), f);
    //fclose(f);
    //ESP_LOGI(TAG, "Read from file: '%s'", line);

    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");

    // Test fans
    set_fan_state(true);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    set_fan_state(false);
    set_gpio(DO3_GPIO, true);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    set_gpio(DO3_GPIO, false);
    ESP_LOGI(TAG, "Boot sequence complete");




}

void app_main(void)
{
    initialize();
    //set_divider_state(1);
    //set_load_current(1.5);
    //set_gpio(DO2_GPIO, true);
    //bool value = false;
    //for(;;)
    //{
       // ESP_LOGI(TAG, "Current enc speed of %f", get_angular_speed());
       // ESP_LOGI(TAG, "Current wind speed of %f", get_wind_speed());
       // ESP_LOGI(TAG, "Current load current of %f A", get_current_adc(current_sense_calib));
        //ESP_LOGI(TAG, "Value of DI1 is %d", get_gpio(DI1_GPIO));
        //ESP_LOGI(TAG, "Value of DI2 is %d", get_gpio(DI2_GPIO));
        //ESP_LOGI(TAG, "Value of DI3 is %d", get_gpio(DI3_GPIO));
        //ESP_ERROR_CHECK(get_gpio(DI1_GPIO));
//	set_fan_state(value);
//	value = !value;
//	vTaskDelay(5000 / portTICK_PERIOD_MS);
  //  }
}
