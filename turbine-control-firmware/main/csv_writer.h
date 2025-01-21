//
// Created by Nicholas Dahlke on 20.01.2025.
//

#pragma once
#include <esp_vfs_fat.h>
#include <driver/sdmmc_host.h>
#include <sdmmc_cmd.h>

#define MOUNT_POINT "/sdcard"
#define MAX_CHAR_SIZE 1024

static sdmmc_card_t *card;
static const char mount_point[] = MOUNT_POINT;
static FILE *csv_file;

void init_csv_writer();
void close_csv_writer();
void csv_write_data(*float data, int size_data);

// End of file csv_writer.h