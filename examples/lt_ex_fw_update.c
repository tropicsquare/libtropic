/**
 * @file TROPIC01_hw_wallet.c
 * @brief Example usage of TROPIC01 chip in a generic *hardware wallet* project.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "string.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_examples.h"

/**
 * @name Hello World
 * @note We recommend reading TROPIC01's datasheet before diving into this example!
 * @par
 */

#if defined(_MSC_VER) || (defined(__INTEL_COMPILER) && defined(_WIN32))
   #if defined(_M_X64)
      #define BITNESS 64
      #define LONG_SIZE 4
   #else
      #define BITNESS 32
      #define LONG_SIZE 4
   #endif
#elif defined(__clang__) || defined(__INTEL_COMPILER) || defined(__GNUC__)
   #if defined(__x86_64)
      #define BITNESS 64
   #else
      #define BITNESS 32
   #endif
   #if __LONG_MAX__ == 2147483647L
      #define LONG_SIZE 4
   #else
      #define LONG_SIZE 8
   #endif
#endif
#include "fw_file_0_1_2.h"

void lt_ex_fw_update(void)
{
   LOG_OUT("\r\n\r\nFW update\r\n");

   lt_handle_t h = {0};

   lt_init(&h);

   uint8_t chip_id[LT_L2_GET_INFO_CHIP_ID_SIZE] = {0};
   LOG_OUT("lt_get_info_chip_id()                     %s\r\n",  lt_ret_verbose(lt_get_info_chip_id(&h, chip_id, LT_L2_GET_INFO_CHIP_ID_SIZE)));
   LOG_OUT("chip_id: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",chip_id[7], chip_id[6], chip_id[5], chip_id[4],
                                                        chip_id[3], chip_id[2], chip_id[1], chip_id[0]);

   uint8_t riscv_fw_ver[LT_L2_GET_INFO_RISCV_FW_SIZE] = {0};
   LOG_OUT("lt_get_info_riscv_fw_ver()                %s\r\n", lt_ret_verbose(lt_get_info_riscv_fw_ver(&h, riscv_fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE)));
   LOG_OUT("riscv_fw_ver: %d.%d.%d (+ unused %d)\r\n", riscv_fw_ver[3],riscv_fw_ver[2],riscv_fw_ver[1],riscv_fw_ver[0]);

   uint8_t spect_fw_ver[LT_L2_GET_INFO_SPECT_FW_SIZE] = {0};
   LOG_OUT("lt_get_info_spect_fw_ver()                %s\r\n", lt_ret_verbose(lt_get_info_spect_fw_ver(&h, spect_fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE)));
   LOG_OUT("spect_fw_ver: %d.%d.%d (+ unused %d)\r\n", spect_fw_ver[3],spect_fw_ver[2],spect_fw_ver[1],spect_fw_ver[0]);

   uint8_t X509_cert[LT_L2_GET_INFO_REQ_CERT_SIZE] = {0};
   LOG_OUT("lt_get_info_cert()                        %s\r\n", lt_ret_verbose(lt_get_info_cert(&h, X509_cert, LT_L2_GET_INFO_REQ_CERT_SIZE)));

   uint8_t stpub[32] = {0};
   LOG_OUT("lt_cert_verify_and_parse()                %s\r\n", lt_ret_verbose(lt_cert_verify_and_parse(X509_cert, LT_L2_GET_INFO_REQ_CERT_SIZE, stpub)));

   LOG_OUT("lt_session_start()                        %s\r\n", lt_ret_verbose(lt_session_start(&h, stpub, pkey_index_0, sh0priv, sh0pub)));

   uint8_t in[100];
   uint8_t out[100];
   LOG_OUT("lt_ping()       %s\r\n", lt_ret_verbose(lt_ping(&h, out, in, 100)));

   uint8_t chip_status;
   LOG_OUT("lt_reboot() reboot into maintenance       %s\r\n", lt_ret_verbose(lt_reboot(&h, LT_L2_STARTUP_ID_MAINTENANCE)));
   LOG_OUT("lt_get_chip_status()                      %s\r\n", lt_ret_verbose(lt_get_chip_status(&h, &chip_status)));
   LOG_OUT("chip_status: READY %d, ALARM %d, STARTUP %d\r\n", (chip_status & 1), (chip_status & 2)>>1, (chip_status & 4)>>2);
   LOG_OUT("lt_session_start()                        %s\r\n", lt_ret_verbose(lt_session_start(&h, stpub, pkey_index_0, sh0priv, sh0pub)));
   LOG_OUT("lt_ping()                                 %s\r\n", lt_ret_verbose(lt_ping(&h, out, in, 100)));

   LOG_OUT("lt_reboot() reboot                        %s\r\n", lt_ret_verbose(lt_reboot(&h, LT_L2_STARTUP_ID_REBOOT)));
   LOG_OUT("lt_get_chip_status()                      %s\r\n", lt_ret_verbose(lt_get_chip_status(&h, &chip_status)));
   LOG_OUT("chip_status: READY %d, ALARM %d, STARTUP %d\r\n", (chip_status & 1), (chip_status & 2)>>1, (chip_status & 4)>>2);
   LOG_OUT("lt_session_start()                        %s\r\n", lt_ret_verbose(lt_session_start(&h, stpub, pkey_index_0, sh0priv, sh0pub)));
   LOG_OUT("lt_ping()                                 %s\r\n", lt_ret_verbose(lt_ping(&h, out, in, 100)));

   LOG_OUT("lt_reboot() reboot into maintenance       %s\r\n", lt_ret_verbose(lt_reboot(&h, LT_L2_STARTUP_ID_MAINTENANCE)));
   LOG_OUT("lt_get_chip_status()                      %s\r\n", lt_ret_verbose(lt_get_chip_status(&h, &chip_status)));
   LOG_OUT("chip_status: READY %d, ALARM %d, STARTUP %d\r\n", (chip_status & 1), (chip_status & 2)>>1, (chip_status & 4)>>2);
   LOG_OUT("lt_session_start()                        %s\r\n", lt_ret_verbose(lt_session_start(&h, stpub, pkey_index_0, sh0priv, sh0pub)));
   LOG_OUT("lt_ping()                                 %s\r\n", lt_ret_verbose(lt_ping(&h, out, in, 100)));

   LOG_OUT("lt_reboot() reboot                        %s\r\n", lt_ret_verbose(lt_reboot(&h, LT_L2_STARTUP_ID_REBOOT)));
   LOG_OUT("lt_get_chip_status()                      %s\r\n", lt_ret_verbose(lt_get_chip_status(&h, &chip_status)));
   LOG_OUT("chip_status: READY %d, ALARM %d, STARTUP %d\r\n", (chip_status & 1), (chip_status & 2)>>1, (chip_status & 4)>>2);
   LOG_OUT("lt_session_start()                        %s\r\n", lt_ret_verbose(lt_session_start(&h, stpub, pkey_index_0, sh0priv, sh0pub)));
   LOG_OUT("lt_ping()                                 %s\r\n", lt_ret_verbose(lt_ping(&h, out, in, 100)));

   return;
}

