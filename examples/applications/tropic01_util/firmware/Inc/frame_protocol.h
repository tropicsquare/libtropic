#ifndef FRAME_PROTOCOL_H
#define FRAME_PROTOCOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "usb_devkit_messages.pb.h"

#define FRAME_MAGIC_BYTE_1 0xAA
#define FRAME_MAGIC_BYTE_2 0x55
#define FRAME_MAGIC_BYTES 2
#define FRAME_DATA_LEN_SIZE 2
#define FRAME_DATA_MAX_SIZE 4093
#define FRAME_CRC_SIZE 2

#if UsbDevkitCmd_size > FRAME_DATA_MAX_SIZE
#error \
    "Maximal size of UsbDevkitCmd is bigger than the maximal allowed size of DATA field in the frame!"
#endif

#if UsbDevkitResp_size > FRAME_DATA_MAX_SIZE
#error \
    "Maximal size of UsbDevkitResp is bigger than the maximal allowed size of DATA field in the frame!"
#endif

#define OUT_FRAME_MAX_SIZE \
    (FRAME_MAGIC_BYTES + FRAME_DATA_LEN_SIZE + UsbDevkitResp_size + FRAME_CRC_SIZE)

/** @brief Used for tracking the state of the USB DevKit main loop. */
typedef enum {
    READ_MAGIC_BYTE_1,
    READ_MAGIC_BYTE_2,
    READ_DATA_LEN,
    READ_DATA,
    READ_VERIFY_CRC,
    PROCESS_DATA,
    WRITE_DATA
} usb_devkit_state_t;

/**
 * @brief USB DevKit main loop.
 *
 */
void usb_devkit_main_loop(void);

/**
 * @brief Decode UsbDevkitCmd from `data`, execute the command, encode UsbDevkitResp and construct
 * frame.
 *
 * @param[in]  data             DATA field from incoming frame.
 * @param[in]  data_len         DATA_LEN field from incoming frame.
 * @param[out] frame_buff       Buffer for the constructed frame.
 * @param[in]  frame_buff_size  Size of `frame_buff`.
 * @param[out] frame_buff_len   Constructed frame length (number of bytes written to `frame_buff`).
 * @retval     true             Success.
 * @retval     false            Fail.
 */
bool process_data(const uint8_t *pb_data_in, size_t pb_data_in_len, uint8_t *pb_data_out,
                  size_t pb_data_out_size, size_t *pb_data_out_len);

/**
 * @brief Encodes UsbDevkitResp and constructs frame.
 *
 * @param[in]  resp             USB DevKit response.
 * @param[out] frame_buff       Buffer for storing the frame.
 * @param[in]  frame_buff_size  Size of `out_buff`.
 * @param[out] frame_buff_len   Constructed frame length (number of bytes written to `frame_buff`).
 * @retval     true             Response encoded successfully.
 * @retval     false            Response could not be encoded.
 */
bool construct_resp(const UsbDevkitResp *resp, uint8_t *frame_buff, size_t frame_buff_size,
                    size_t *frame_buff_len);

/**
 * @brief Construct the frame to send.
 *
 * @param[in]  data             DATA field in the frame.
 * @param[in]  data_len         DATA_LEN field in the frame.
 * @param[out] frame_buff       Buffer for the constructed frame.
 * @param[in]  frame_buff_size  Size of `frame_buff`.
 * @param[out] frame_buff_len   Constructed frame length (number of bytes written to `frame_buff`).
 * @retval     true             Frame constructed successfully.
 * @retval     false            Frame could not be constructed, `frame_buff` is too small.
 */
bool construct_frame(const uint8_t *data, size_t data_len, uint8_t *frame_buff, size_t frame_buff_size,
                     size_t *frame_buff_len);

#endif  // FRAME_PROTOCOL_H