//#include "string"
#include "string.h"
#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "math.h"

#include <stdbool.h>

unsigned char header[54] = {
    0x42,        // identity : B
    0x4d,        // identity : M
    0, 0, 0, 0,  // file size
    0, 0,        // reserved1
    0, 0,        // reserved2
    54, 0, 0, 0, // RGB data offset
    40, 0, 0, 0, // struct BITMAPINFOHEADER size
    0, 0, 0, 0,  // bmp width
    0, 0, 0, 0,  // bmp height
    1, 0,        // planes
    24, 0,       // bit per pixel
    0, 0, 0, 0,  // compression
    0, 0, 0, 0,  // data size
    0, 0, 0, 0,  // h resolution
    0, 0, 0, 0,  // v resolution
    0, 0, 0, 0,  // used colors
    0, 0, 0, 0   // important colors
};

union word
{
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

unsigned int input_rgb_raw_data_offset;
const unsigned int output_rgb_raw_data_offset = 54;
int width;
int height;
unsigned int width_bytes;
unsigned char bits_per_pixel;
unsigned short bytes_per_pixel;
unsigned char *source_bitmap;
unsigned char *target_bitmap;
const int WHITE = 255;
const int BLACK = 0;
const int THRESHOLD = 90;

// Sobel Filter ACC
static char *const SOBELFILTER_START_ADDR = (char *const)(0x73000000);
static char *const SOBELFILTER_READ_ADDR = (char *const)(0x73000004);
// Sobel Filter1 ACC
static char *const SOBELFILTER1_START_ADDR = (char *const)(0x74000000);
static char *const SOBELFILTER1_READ_ADDR = (char *const)(0x74000004);

// DMA
static volatile uint32_t *const DMA_SRC_ADDR = (uint32_t *const)0x70000000;
static volatile uint32_t *const DMA_DST_ADDR = (uint32_t *const)0x70000004;
static volatile uint32_t *const DMA_LEN_ADDR = (uint32_t *const)0x70000008;
static volatile uint32_t *const DMA_OP_ADDR = (uint32_t *const)0x7000000C;
static volatile uint32_t *const DMA_STAT_ADDR = (uint32_t *const)0x70000010;
static const uint32_t DMA_OP_MEMCPY = 1;

bool _is_using_dma = true;

int sem_init(uint32_t *__sem, uint32_t count) __THROW
{
  *__sem = count;
  return 0;
}

int sem_wait(uint32_t *__sem) __THROW
{
  uint32_t value, success; // RV32A
  __asm__ __volatile__(
      "\
L%=:\n\t\
     lr.w %[value],(%[__sem])            # load reserved\n\t\
     beqz %[value],L%=                   # if zero, try again\n\t\
     addi %[value],%[value],-1           # value --\n\t\
     sc.w %[success],%[value],(%[__sem]) # store conditionally\n\t\
     bnez %[success], L%=                # if the store failed, try again\n\t\
"
      : [value] "=r"(value), [success] "=r"(success)
      : [__sem] "r"(__sem)
      : "memory");
  return 0;
}

int sem_post(uint32_t *__sem) __THROW
{
  uint32_t value, success; // RV32A
  __asm__ __volatile__(
      "\
L%=:\n\t\
     lr.w %[value],(%[__sem])            # load reserved\n\t\
     addi %[value],%[value], 1           # value ++\n\t\
     sc.w %[success],%[value],(%[__sem]) # store conditionally\n\t\
     bnez %[success], L%=                # if the store failed, try again\n\t\
"
      : [value] "=r"(value), [success] "=r"(success)
      : [__sem] "r"(__sem)
      : "memory");
  return 0;
}

int barrier(uint32_t *__sem, uint32_t *__lock, uint32_t *counter, uint32_t thread_count)
{
  sem_wait(__lock);
  if (*counter == thread_count - 1)
  { // all finished
    *counter = 0;
    sem_post(__lock);
    for (int j = 0; j < thread_count - 1; ++j)
      sem_post(__sem);
  }
  else
  {
    (*counter)++;
    sem_post(__lock);
    sem_wait(__sem);
  }
  return 0;
}

// dma lock for dma access
uint32_t dma_lock;

void write_data_to_ACC(char *ADDR, unsigned char *buffer, int len)
{
  if (_is_using_dma)
  {
    // Using DMA
    //sem_wait(&dma_lock);
    *DMA_SRC_ADDR = (uint32_t)(buffer);
    *DMA_DST_ADDR = (uint32_t)(ADDR);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR = DMA_OP_MEMCPY;
    //sem_post(&dma_lock);
  }
  else
  {
    // Directly Send
    memcpy(ADDR, buffer, sizeof(unsigned char) * len);
  }
}
void read_data_from_ACC(char *ADDR, unsigned char *buffer, int len)
{
  if (_is_using_dma)
  {
    // Using DMA
    //sem_wait(&dma_lock);
    *DMA_SRC_ADDR = (uint32_t)(ADDR);
    *DMA_DST_ADDR = (uint32_t)(buffer);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR = DMA_OP_MEMCPY;
    //sem_post(&dma_lock);
  }
  else
  {
    // Directly Read
    memcpy(buffer, ADDR, sizeof(unsigned char) * len);
  }
}

const float x_input_signal[128] = {0.500, 0.525, 0.549, 0.574, 0.598, 0.622, 0.646, 0.670,
                                   0.693, 0.715, 0.737, 0.759, 0.780, 0.800, 0.819, 0.838,
                                   0.856, 0.873, 0.889, 0.904, 0.918, 0.931, 0.943, 0.954,
                                   0.964, 0.972, 0.980, 0.986, 0.991, 0.995, 0.998, 1.000,
                                   1.000, 0.999, 0.997, 0.994, 0.989, 0.983, 0.976, 0.968,
                                   0.959, 0.949, 0.937, 0.925, 0.911, 0.896, 0.881, 0.864,
                                   0.847, 0.829, 0.810, 0.790, 0.769, 0.748, 0.726, 0.704,
                                   0.681, 0.658, 0.634, 0.610, 0.586, 0.562, 0.537, 0.512,
                                   0.488, 0.463, 0.438, 0.414, 0.390, 0.366, 0.342, 0.319,
                                   0.296, 0.274, 0.252, 0.231, 0.210, 0.190, 0.171, 0.153,
                                   0.136, 0.119, 0.104, 0.089, 0.075, 0.063, 0.051, 0.041,
                                   0.032, 0.024, 0.017, 0.011, 0.006, 0.003, 0.001, 0.000,
                                   0.000, 0.002, 0.005, 0.009, 0.014, 0.020, 0.028, 0.036,
                                   0.046, 0.057, 0.069, 0.082, 0.096, 0.111, 0.127, 0.144,
                                   0.162, 0.181, 0.200, 0.220, 0.241, 0.263, 0.285, 0.307,
                                   0.330, 0.354, 0.378, 0.402, 0.426, 0.451, 0.475, 0.500};

const float y_input_signal[128] = {0.500, 0.525, 0.549, 0.574, 0.598, 0.622, 0.646, 0.670,
                                   0.693, 0.715, 0.737, 0.759, 0.780, 0.800, 0.819, 0.838,
                                   0.856, 0.873, 0.889, 0.904, 0.918, 0.931, 0.943, 0.954,
                                   0.964, 0.972, 0.980, 0.986, 0.991, 0.995, 0.998, 1.000,
                                   1.000, 0.999, 0.997, 0.994, 0.989, 0.983, 0.976, 0.968,
                                   0.959, 0.949, 0.937, 0.925, 0.911, 0.896, 0.881, 0.864,
                                   0.847, 0.829, 0.810, 0.790, 0.769, 0.748, 0.726, 0.704,
                                   0.681, 0.658, 0.634, 0.610, 0.586, 0.562, 0.537, 0.512,
                                   0.488, 0.463, 0.438, 0.414, 0.390, 0.366, 0.342, 0.319,
                                   0.296, 0.274, 0.252, 0.231, 0.210, 0.190, 0.171, 0.153,
                                   0.136, 0.119, 0.104, 0.089, 0.075, 0.063, 0.051, 0.041,
                                   0.032, 0.024, 0.017, 0.011, 0.006, 0.003, 0.001, 0.000,
                                   0.000, 0.002, 0.005, 0.009, 0.014, 0.020, 0.028, 0.036,
                                   0.046, 0.057, 0.069, 0.082, 0.096, 0.111, 0.127, 0.144,
                                   0.162, 0.181, 0.200, 0.220, 0.241, 0.263, 0.285, 0.307,
                                   0.330, 0.354, 0.378, 0.402, 0.426, 0.451, 0.475, 0.500};

// Total number of cores
// static const int PROCESSORS = 3;
#define PROCESSORS 2 // 2 processors to do the 3 channel operations
// the barrier synchronization objects
uint32_t barrier_counter = 0;
uint32_t barrier_lock;
uint32_t barrier_sem;
// the mutex object to control "global" related variable
uint32_t lock;
// print synchronication semaphore (print in core order)
uint32_t print_sem[PROCESSORS];

int main(unsigned hart_id)
{

  unsigned char buffer[4] = {0};
  //word data;
  int total;
  printf("Start processing...\n");
  printf("hart id = %d\n", hart_id);

  // create a barrier object with a count of PROCESSORS
  /*sem_init(&barrier_lock, 1);
  sem_init(&dma_lock, 1);
  sem_init(&barrier_sem, 0); // lock all cores initially
  for (int i = 0; i < PROCESSORS; ++i)
  {
    sem_init(&print_sem[i], 0); // lock printing initially
  }
  // Create mutex lock
  sem_init(&lock, 1);

  // barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);
  if (hart_id == 0)
    sem_post(&print_sem[1]);
  else
    sem_wait(&print_sem[1]);
*/
  if (hart_id == 0)
  {
    for (int i = 0; i < 128; i++)
    {
      printf("Start sending...(%d)\n", i);
      buffer[0] = x_input_signal[i];
      buffer[1] = y_input_signal[i];
      buffer[2] = 0;
      buffer[3] = 0;
      // barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS)
      // sem_wait(&lock);
      // printf("write data to core0 acc...(%d)\n", i);
      write_data_to_ACC(SOBELFILTER_START_ADDR, buffer, 4);
      printf("write data to core0 acc...(%d)\n", i);
      // sem_post(&lock);
    }
    // printf("writing address = %d \n", SOBELFILTER_START_ADDR);
    // sem_wait(&lock);
    read_data_from_ACC(SOBELFILTER_READ_ADDR, buffer, 4);
    printf("read data from core0 to acc...\n");
    //memcpy(data.uc, buffer, 4);
    // sem_post(&lock);
    double y_output_signal_0[256];
    for (int i = 0; i < 256; i++)
    {
      printf("output dataing...(%d)\n", i);
      y_output_signal_0[i] = buffer[0];
    }
  }
  else
  {
    for (int i = 0; i < 128; i++)
    {
      buffer[0] = x_input_signal[i];
      buffer[1] = y_input_signal[i];
      buffer[2] = 0;
      buffer[3] = 0;
      // sem_wait(&lock);
      write_data_to_ACC(SOBELFILTER1_START_ADDR, buffer, 4);
      printf("write data to core1 acc...(%d)\n", i);
      // printf("writing address = %d \n", SOBELFILTER1_START_ADDR);
      // sem_post(&lock);
    }
    // sem_wait(&lock);
    read_data_from_ACC(SOBELFILTER1_READ_ADDR, buffer, 4);
    printf("read data from core1 to acc...\n");
    //memcpy(data.uc, buffer, 4);
    // sem_post(&lock);
    double y_output_signal_1[256];
    for (int i = 0; i < 256; i++)
    {
      printf("output dataing...(%d)\n", i);
      y_output_signal_1[i] = buffer[0];
    }
  }
  // barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);

  if (hart_id == 0)
  { // Core 0 print first and then others
    printf("core%d is finished\n", hart_id);
    // sem_post(&print_sem[1]); // Allow Core 1 to print
  }
  else
  {
    // sem_wait(&print_sem[1]);
    printf("core%d, finished\n", hart_id);
  }

  printf("output done\n");
  return 0;
}
