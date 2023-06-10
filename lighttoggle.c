#include <fcntl.h>
#include <getopt.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <hidapi/hidapi.h>

// USER CONFIG
const char *set_effect_colors = "FFFFFF"; // Set the color. RRGGBB
const unsigned int set_effect_brightness = 1; // Set the brightness of the color. Value from 0-4
// END CONFIG


// Model O Mouse
static const uint16_t VID_GLORIOUS_MODEL_O = 0x258a;
static const uint16_t PID_GLORIOUS_MODEL_O = 0x36;

typedef struct {
    uint8_t r, g, b;
} RGB8;

typedef struct {
    uint8_t r, b, g;
} RBG8;

static RBG8 int_to_rbg(unsigned int value) {
    RBG8 rgb;
    rgb.r = value >> 16U;
    rgb.g = value >> 8U;
    rgb.b = value;
    return rgb;
}

struct config {
    uint8_t report_id;
    uint8_t command_id;
    uint8_t unk1;
    uint8_t config_write;
    uint8_t unk2[6];
    uint8_t config1;
    uint8_t dpi_count:4;
    uint8_t active_dpi:4;
    uint8_t dpi_enabled;
    uint8_t dpi[16];
    RGB8 dpi_color[8];
    uint8_t rgb_effect;
    uint8_t glorious_mode;
    uint8_t glorious_direction;
    uint8_t single_mode;
    RBG8 single_color;
    uint8_t breathing7_mode;
    uint8_t breathing7_colorcount;
    RBG8 breathing7_colors[7];
    uint8_t tail_mode;
    uint8_t unk4[33];
    uint8_t rave_mode;
    RBG8 rave_colors[2];
    uint8_t wave_mode;
    uint8_t breathing1_mode;
    RBG8 breathing1_color;
    uint8_t unk5;
    uint8_t lift_off_distance;
};

int main() {
    hid_init();
    struct hid_device_info *devices = hid_enumerate(VID_GLORIOUS_MODEL_O, PID_GLORIOUS_MODEL_O);
    struct hid_device_info *device = devices;
    char *dev_path = NULL;
    while (device) {
        if (device->interface_number == 1) {
            dev_path = strdup(device->path);
            break;
        }
        device = device->next;
    }

    hid_free_enumeration(devices);
    hid_exit();
    hid_device *dev = NULL;

    if (!dev_path) {
        fprintf(stderr, "No supported device found.\n");
        return 0;
    }

    dev = hid_open_path(dev_path);
    free(dev_path);
    dev_path = NULL;

    if (!dev) {
        fprintf(stderr, "Failed to open HID device. Try sudo.\n");
        return 1;
    }

    uint8_t cmd[6] = { 0x5, 0x11 };
    hid_send_feature_report(dev, cmd, sizeof(cmd));

    struct config *cfg = calloc(1, 520);
    cfg->report_id = 0x4;

    hid_get_feature_report(dev, (uint8_t *)cfg, 520);

    cfg->rgb_effect = 0x02;
    unsigned int rgb_colors[7] = { 0 };
    sscanf(set_effect_colors, "%x,%x,%x,%x,%x,%x,%x", &rgb_colors[0], &rgb_colors[1], &rgb_colors[2], &rgb_colors[3], &rgb_colors[4], &rgb_colors[5], &rgb_colors[6]);

    // Flip-flop set_effect_brightness between 0 and its selected value
    cfg->single_mode = (cfg->single_mode == set_effect_brightness << 4U) ? 0 : set_effect_brightness << 4U;

    cfg->single_color = int_to_rbg(rgb_colors[0]);

    cfg->config_write = 131 - 8;

    hid_send_feature_report(dev, (uint8_t *)cfg, 520);

    hid_close(dev);
    hid_exit();

    return 0;
}
