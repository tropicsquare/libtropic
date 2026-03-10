#include <stdio.h>

#include "stm32u5xx_ll_utils.h"
#include "tusb.h"

#define USB_VID 0x0483
#define USB_PID 0x5740
#define USB_BCD 0x0200

#define CONFIG_TOTAL_LEN 75

#define EPNUM_CDC_NOTIF 0x82
#define EPNUM_CDC_OUT 0x01
#define EPNUM_CDC_IN 0x81

enum { ITF_NUM_CDC = 0, ITF_NUM_CDC_DATA, ITF_NUM_TOTAL };

//--------------------------------------------------------------------
// Device Descriptors
//--------------------------------------------------------------------
static tusb_desc_device_t const desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = USB_BCD,
    .bDeviceClass = TUSB_CLASS_CDC,
    .bDeviceSubClass = CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL,
    .bDeviceProtocol = CDC_COMM_PROTOCOL_NONE,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = 0x0200,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01};

uint8_t const *tud_descriptor_device_cb(void) { return (uint8_t const *)&desc_device; }

//--------------------------------------------------------------------
// Configuration Descriptor
//--------------------------------------------------------------------
// full speed configuration
static uint8_t const desc_fs_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0xC0, 0),
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 4, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT, EPNUM_CDC_IN,
                       CFG_TUD_CDC_EP_BUFSIZE),
};

uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
    (void)index;
    return desc_fs_configuration;
}

static char *usb_serial_number(void)
{
    static char buffer[16];
    uint32_t sn0 = LL_GetUID_Word0();
    uint32_t sn1 = LL_GetUID_Word1();
    uint32_t sn2 = LL_GetUID_Word2();

    sn0 += sn2;

    snprintf(buffer, sizeof(buffer), "%04lx%08lx", ((sn1 >> 16U) ^ sn1) & 0xFFFFU, sn0);
    return buffer;
}

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;

    static uint16_t descriptor[32];
    const char *strings[] = {
        [1] = "TropicSquare",
        [2] = "TROPIC01 USB DevKit",
        [3] = NULL,
        [4] = "TROPIC01 CDC",
    };

    uint8_t length;

    if (index == 0) {
        descriptor[1] = 0x0409;
        length = 1;
    }
    else {
        const char *selected = strings[index];
        if (index == 3) {
            selected = usb_serial_number();
        }

        if (selected == NULL) {
            return NULL;
        }

        length = (uint8_t)strlen(selected);
        if (length > 31) {
            length = 31;
        }

        for (uint8_t i = 0; i < length; i++) {
            descriptor[1 + i] = selected[i];
        }
    }

    descriptor[0] = (TUSB_DESC_STRING << 8) | (2U * length + 2U);

    return descriptor;
}
