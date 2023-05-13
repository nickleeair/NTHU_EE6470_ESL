#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

#include "Filters.h"
#include "directives.h"

Filters::Filters( sc_module_name n ): sc_module( n )
{
	SC_THREAD( do_filter );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
        
#ifndef NATIVE_SYSTEMC
	i_rgb.clk_rst(i_clk, i_rst);
	or_result.clk_rst(i_clk, i_rst);
	og_result.clk_rst(i_clk, i_rst);
	ob_result.clk_rst(i_clk, i_rst);
#endif
}

Filters::~Filters() {}

// sobel mask
const int mask[MASK_X][MASK_Y] = {{1, 1, 1}, {1, 2, 1}, {1, 1, 1}};

void Filters::do_filter() {
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_rgb.reset();
		or_result.reset();
		og_result.reset();
		ob_result.reset();
#endif
		wait();
	}
	while (true) {
		r_val = 0;
		g_val = 0;
		b_val = 0;
		for (unsigned int v = 0; v<MASK_Y; ++v) {
			LOOP_UNROLL;
			for (unsigned int u = 0; u<MASK_X; ++u) {
				LOOP_UNROLL;
				sc_dt::sc_uint<24> rgb;
#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("input");
					rgb = i_rgb.get();
					wait();
				}
#else
				rgb = i_rgb.read();
#endif
				LOOP_PIPELINE;
				r_val += rgb.range(7, 0) 	* mask[u][v] 	;
				g_val += rgb.range(15, 8) 	* mask[u][v] 	;
				b_val += rgb.range(23, 16) 	* mask[u][v] ;
			}
		}
		r_val = r_val/10;
		g_val = g_val/10;
		b_val = b_val/10;
#ifndef NATIVE_SYSTEMC
		{
			HLS_DEFINE_PROTOCOL("output");
			or_result.put(r_val);
			og_result.put(g_val);
			ob_result.put(b_val);
			wait();
		}
#else
		or_result.put(r_val);
		og_result.put(g_val);
		ob_result.put(b_val);
#endif
	}
}

