#include <stdio.h>

#define TAPS 16
#define TSTEP1 32
#define TSTEP2 48

#include "expected.inc"

// CLOBBER is a compiler barrier
static void clobber() {
    asm volatile ("" : : : "memory");
}

int main(int argc, char* argv[]) {
    int n, m;
    volatile short *coef = (short *)0x60004000;
    volatile short *input = (short *)0x60002000;
    volatile short *output = (short *)0x60001000;
    volatile short *output_buffer = (short *)0x60006000;
    short error, total_error;

    // Error check for both FIR filter operations
    total_error = 0;
    // for (n = 0; n < (TSTEP1 + TSTEP2); n++) {
    //     error = expected[n] - output[n]; // Error for this time-step
    //     total_error += (error < 0) ? (-error) : error; // Absolute value
    // }

    //printf("cpu main FIR total error: %d\n", total_error);

    // The rest of this file contains tests to illustrate
    // transactions to and from the DMA and Accelerator.

    volatile long long *dma_st = (volatile long long *)0x70000000;
    volatile long long **dma_sr = (volatile long long **)0x70000010;
    volatile long long **dma_dr = (volatile long long **)0x70000018;
    volatile long long *dma_len = (volatile long long *)0x70000020;
    volatile long long *accel_st = (volatile long long *)0x70010000;
    volatile long long *accel_ctrl = (volatile long long *)0x70010008;
    volatile long long *accel_w = (volatile long long *)0x70010010;
    volatile long long *accel_x = (volatile long long *)0x70010030;
    volatile long long *accel_z = (volatile long long *)0x70010050;

    *dma_sr = (volatile long long *)((long)input & 0x1fffffff);
    *dma_dr = (volatile long long *)((long)accel_x & 0x1fffffff);
    *dma_len = 32; // starts transfer
    clobber();

    *dma_sr = (volatile long long *)((long)input + 32 & 0x1fffffff);
    *dma_dr = (volatile long long *)((long)accel_x & 0x1fffffff);
    *dma_len = 32; // starts transfer
    clobber();

    *dma_sr = (volatile long long *)((long)input + 64 & 0x1fffffff);
    *dma_dr = (volatile long long *)((long)accel_x & 0x1fffffff);
    *dma_len = 64; // starts transfer
    clobber();

    *dma_sr = (volatile long long *)((long)input + 128 & 0x1fffffff);
    *dma_dr = (volatile long long *)((long)accel_x & 0x1fffffff);
    *dma_len = 32; // starts transfer
    clobber();

    total_error = 0;
    for (n = 0; n < TAPS; n++) {
        error = input[n] - output[n]; // Error for this time-step
        total_error += (error < 0) ? (-error) : error; // Absolute value
    }
    //printf("cpu main DMA transfer 1 total error: %d\n", total_error);

    *dma_sr = (volatile long long *)((long)coef & 0x1fffffff);
    *dma_dr = (volatile long long *)((long)accel_w & 0x1fffffff);
    *dma_len = 64; // starts transfer
    clobber();

    *dma_sr = (volatile long long *)((long)accel_z & 0x1fffffff);
    *dma_dr = (volatile long long *)((long)output & 0x1fffffff);
    *dma_len = 64; // starts transfer
    clobber();

    total_error = 0;
    for (n = 0; n < TAPS; n++) {
        error = coef[n] - output[n]; // Error for this time-step
        total_error += (error < 0) ? (-error) : error; // Absolute value
    }

    *accel_ctrl = 2;
    //printf("cpu main accel_ctrl write/read long long test error: ");
    //printf("%d\n", 2 - *accel_st);

    clobber();
    //printf("cpu main DMA transfer 2 total error: %d\n", total_error);

    *dma_sr = (volatile long long *)((long)accel_z & 0x1fffffff);
    *dma_dr = (volatile long long *)((long)output & 0x1fffffff);
    *dma_len = 64;

    while (*accel_st != 3) {
        // Wait for it to be done
    }

    *accel_ctrl = 9;

    *dma_sr = (volatile long long *)((long)accel_z & 0x1fffffff);
    *dma_dr = (volatile long long *)((long)output + 64 & 0x1fffffff);
    *dma_len = 64; // starts transfer
    clobber();

    *dma_sr = (volatile long long *)((long)accel_z & 0x1fffffff);
    *dma_dr = (volatile long long *)((long)output + 128 & 0x1fffffff);
    *dma_len = 32; // starts transfer
    clobber();

    total_error = 0;
    // for (n = 0; n < (TSTEP1 + TSTEP2); n++) {
    //     error = expected[n] - output[n]; // Error for this time-step
    //     total_error += (error < 0) ? (-error) : error; // Absolute value
    //     //printf("cpu main k: %d output: %d expected %d\n", n, output[n], expected[n]);
    // }

    //  printf("cpu main FIR total error: %d\n", total_error);

    *accel_ctrl = (volatile long long)0x0f; // Exit

    return 0;
}


