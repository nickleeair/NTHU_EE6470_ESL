#ifndef FILTERS_H_
#define FILTERS_H_
#include <systemc>
using namespace sc_core;

#include "filter_def.h"

class MedianFilter : public sc_module
{
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  sc_fifo_in<unsigned char> i_r;
  sc_fifo_in<unsigned char> i_g;
  sc_fifo_in<unsigned char> i_b;
  sc_fifo_out<double> or_result;
  sc_fifo_out<double> og_result;
  sc_fifo_out<double> ob_result;

  SC_HAS_PROCESS(MedianFilter);
  MedianFilter(sc_module_name n);
  ~MedianFilter() = default;

private:
  void do_filter();
  char r_row[9] = {0};
  char g_row[9] = {0};
  char b_row[9] = {0};
  char r_sorted_pixels[9] = {0};
  char g_sorted_pixels[9] = {0};
  char b_sorted_pixels[9] = {0};

  double r_val;
  double g_val;
  double b_val;
};

class MeanFilter : public sc_module
{
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  sc_fifo_in<double> i_r;
  sc_fifo_in<double> i_g;
  sc_fifo_in<double> i_b;
  sc_fifo_out<double> or_result;
  sc_fifo_out<double> og_result;
  sc_fifo_out<double> ob_result;

  SC_HAS_PROCESS(MeanFilter);
  MeanFilter(sc_module_name n);
  ~MeanFilter() = default;

private:
  void do_filter();

  double r_row[9] = {0};
  double g_row[9] = {0};
  double b_row[9] = {0};

  double r_val;
  double g_val;
  double b_val;
};
#endif
