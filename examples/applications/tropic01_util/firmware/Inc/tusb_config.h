#ifndef TUSB_CONFIG_H
#define TUSB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define CFG_TUSB_MCU OPT_MCU_STM32U5
#define CFG_TUSB_RHPORT0_MODE OPT_MODE_DEVICE
#define CFG_TUSB_OS OPT_OS_NONE

#define CFG_TUD_ENABLED 1

#define CFG_TUD_ENDPOINT0_SIZE 64

#ifdef USB_DEVKIT_DEBUG_CDC_ITF
#define CFG_TUD_CDC 2
#else
#define CFG_TUD_CDC 1
#endif
#define CFG_TUD_CDC_RX_BUFSIZE 512
#define CFG_TUD_CDC_TX_BUFSIZE 512
#define CFG_TUD_CDC_EP_BUFSIZE 64

#ifdef __cplusplus
}
#endif

#endif  // TUSB_CONFIG_H