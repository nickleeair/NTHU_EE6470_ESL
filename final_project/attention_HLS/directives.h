#ifndef _DIRECTIVES_H_
#define _DIRECTIVES_H_

// Define a group of macros for each configuration
#ifdef UNROLL
#   define LOOP_UNROLL  HLS_UNROLL_LOOP( COMPLETE, 3, "unroll"  )
#   define LOOP_PIPELINE
#   define LOOP_LATENCY
#endif

#ifdef PIPELINE
#   define LOOP_PIPELINE  HLS_PIPELINE_LOOP( SOFT_STALL, 1, "pipeline"  )
#   define LOOP_UNROLL
#   define LOOP_LATENCY
#endif

#ifdef LATENCY
#   define LOOP_LATENCY  HLS_CONSTRAIN_LATENCY(0,3,"latency")
#   define LOOP_UNROLL
#   define LOOP_PIPELINE
#endif

#ifdef UNROLL_PIPE
#   define LOOP_UNROLL  HLS_UNROLL_LOOP( COMPLETE, 3, "unroll"  )
#   define LOOP_PIPELINE  HLS_PIPELINE_LOOP( SOFT_STALL, 1, "pipeline"  )
#   define LOOP_LATENCY
#endif

#ifdef LATENCY_PIPE
#   define LOOP_UNROLL
#   define LOOP_PIPELINE  HLS_PIPELINE_LOOP( SOFT_STALL, 1, "pipeline"  )
#   define LOOP_LATENCY  HLS_CONSTRAIN_LATENCY(0,20,"latency")
#endif


#ifndef LOOP_UNROLL
#   define LOOP_UNROLL
#endif
#ifndef LOOP_PIPELINE
#   define LOOP_PIPELINE
#endif
#ifndef LOOP_LATENCY
#   define LOOP_LATENCY
#endif


#endif // _DIRECTIVES_H_