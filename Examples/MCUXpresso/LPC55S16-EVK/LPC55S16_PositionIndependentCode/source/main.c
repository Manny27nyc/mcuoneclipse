/*
 * Copyright (c) 2021, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC55S16.h"
#include "fsl_debug_console.h"
#include "module.h"
#include "myLib.h"

#if 0
int MyLib_Calc(int x) {
  glob++;
//   0: 4a03        ldr r2, [pc, #12] ; (10 <MyLib_Calc+0x10>)
//   2: 447a        add r2, pc
//   4: 6813        ldr r3, [r2, #0]
//   6: 3301        adds  r3, #1
//   8: 6013        str r3, [r2, #0]
  return x*2;
}
//   a: 0040        lsls  r0, r0, #1
//   c: 4770        bx  lr
//   e: bf00        nop
//  10: 0000000a  .word 0x0000000a
int MyLib_Mul2(int x) {
  return x*2;
}
// 838: 0040        lsls  r0, r0, #1
// 83a: 4770        bx  lr
void MyLib_Init(void) {
}
//  18: 4770        bx  lr
//  1a: bf00        nop

#else
static const int16_t code[] = {
#if 0
  0x4A03,
  0x447a,
  0x6813,
  0x3301,
  0x6013,
  0x0040,
  0x4770,
  0xbf00,
  0x000a, 0x0000,
#else
  0x4b03,
  0xf859, 0x2003,
  0x6813,
  0x3301,
  0x6013,
  0x0040,
  0x4770,
  0x0000, 0x0000,
#endif
  0x0040,
  0x4770,

  0x4770,
  0xbf00
};
#endif

/*! \brief
 * The information needed to perform the binding.
 * The offsets are the code offsets inside (Virtual address) from the beginning.
 * The got_plt index is used to identify the got PLT index.
 */
typedef struct {
  const char *name; /*!< name of function */
  size_t offset;    /*!< offset in loaded .code section */
  int got_plt_idx;  /*!< index in .got_plt table */
} binding_t;

static const binding_t bindings[] =
{
    {"MyLib_Calc", 0x0000, 4},
    {"MyLib_Mul2", 0x0014, 5},
    {"MyLib_Init", 0x0018, 3},
};

/* Force the counter to be placed into memory. */
volatile int i, j = 0 ;
void foobar(void) {}

extern unsigned int _sgot, _sgot_plt; /* symbols provided by the linker */

void BindLibrary(void *relocStart) {
  for(int i=0; i<sizeof(bindings)/sizeof(bindings[0]); i++) {
    ((uint32_t*)&_sgot_plt)[bindings[i].got_plt_idx] = (uint32_t)(relocStart+bindings[i].offset);
  }
}

int main(void) {
  /* load lib into memory */
  /* bind methods of memory */
    BindLibrary((void*)code); /* do the binding to the relocated code */

    MyLib_Init();
    i = MyLib_Calc(30);
    j = MyLib_Mul2(55);
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++;
        j++;
        __asm volatile ("nop");
    }
    return 0 ;
}

