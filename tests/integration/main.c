/**
 * @file main.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdio.h>
#include "string.h"

#include "TROPIC01_hello_world.h"
#include "TROPIC01_hw_wallet.h"

// Code can be found in examples/hw_wallet/
int main(void)
{
    int x;
    // Non destructive
    x = tropic01_hello_world_example();
    // WARNING destructive
    x = tropic01_hw_wallet_example();

    return x;
}