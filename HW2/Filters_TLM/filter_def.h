#ifndef FILTER_DEF_H_
#define FILTER_DEF_H_

const int MASK_N = 2;
const int MASK_X = 3;
const int MASK_Y = 3;

const int MEDIAN_FILTER_R_ADDR = 0x00000000;
const int MEDIAN_FILTER_RESULT_ADDR = 0x00000004;
const int MEDIAN_FILTER_CHECK_ADDR = 0x00000008;

const int MEAN_FILTER_R_ADDR = 0x00000000;
const int MEAN_FILTER_RESULT_ADDR = 0x00000004;
const int MEAN_FILTER_CHECK_ADDR = 0x00000008;

const int FILTERS_R_ADDR = 0x00000000;
const int FILTERS_RESULT_ADDR = 0x00000004;
const int FILTERS_CHECK_ADDR = 0x00000008;

union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

#endif
