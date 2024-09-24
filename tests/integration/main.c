

#include <stdio.h>


#include "libtropic_common.h"
#include "libtropic_example_1.h"
#include "libtropic_config_1.h"
#include "utils.h"


int main(void)
{
    lt_handle_t handle;

    /************************************************************************************************************/
    LOG_OUT("\r\n");
    LOG_OUT("\t\t======================================================================\r\n");
    LOG_OUT("\t\t=====  libtropic example 1 - DESCRIPTION                           ===\r\n");
    LOG_OUT("\t\t======================================================================\r\n\n");

    /************************************************************************************************************/
    libtropic_example___verify_chip_and_start_secure_session(&handle);

    /************************************************************************************************************/
    libtropic_example___show_config_objects(&handle);

    // set config objects somehow

    // update new keys

    // close session

    // open session with new keys

    // disable h0 keys

    //
    /************************************************************************************************************/
    //libtropic_example___other_commands(&handle);

    return 0;
}