## Stratus

- cd stratus
- BASIC: make sim_V_BASIC
- UNROLL: make sim_V_UNROLL
- PIPELINE: make sim_V_PIPELINE
- UNROLL+PIPELINE: make sim_V_UNROLL_PIPE

## riscv-vp
- cd riscv-vp/vp/build
- cmake ..
- make install
- cd riscv-vp/sw
- cd basic-sobel
- make
- make sim
