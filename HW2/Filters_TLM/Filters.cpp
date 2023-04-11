#include <cmath>
#include <iomanip>

#include "Filters.h"

const double mask[MASK_X][MASK_Y] = {{1, 1, 1}, {1, 2, 1}, {1, 1, 1}};

Filters::Filters(sc_module_name n)
    : sc_module(n), t_skt("t_skt"), base_offset(0)
{
    SC_THREAD(do_filter);

    t_skt.register_b_transport(this, &Filters::blocking_transport);
}

void Filters::do_filter()
{
    while (true)
    {
        r_val = 0;
        g_val = 0;
        b_val = 0;
        std::cout << "r_val_initiate: " << r_val << std::endl;
        //do median_filter save in a huge buffer
            for (unsigned int v = 0; v < MASK_Y; ++v)
                {
                    for (unsigned int u = 0; u < MASK_X; ++u)
                    {
                        r_val += i_r.read() * mask[u][v];
                        g_val += i_g.read() * mask[u][v];
                        b_val += i_b.read() * mask[u][v];
                        std::cout << "u': " << u << std::endl;
                        std::cout << "v': " << v << std::endl;
                        //wait(1);
                    }
                }
            r_val = r_val/10;
            g_val = g_val/10;
            b_val = b_val/10;
            
            std::cout << "r_val_mean: " << r_val << std::endl;
            or_result.write(r_val);
            std::cout << "r_val_mean_input: " << r_val << std::endl;
            
            og_result.write(g_val);
            std::cout << "g_val_mean_input: " << g_val << std::endl;
            
            ob_result.write(b_val);
            std::cout << "b_val_mean_input: " << b_val << std::endl;
            //wait(1); // emulate module delay
    }
}

void Filters::blocking_transport(tlm::tlm_generic_payload &payload,
                                      sc_core::sc_time &delay)
{
    sc_dt::uint64 addr = payload.get_address();
    addr -= base_offset;
    unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    unsigned char *data_ptr = payload.get_data_ptr();
    word buffer1,buffer2,buffer3;
    switch (payload.get_command())
    {
    case tlm::TLM_READ_COMMAND:
        switch (addr)
        {
        case FILTERS_RESULT_ADDR:
            buffer1.uint = or_result.read();
            buffer2.uint = og_result.read();
            buffer3.uint = ob_result.read();
            break;
        case FILTERS_CHECK_ADDR:
            buffer1.uint = or_result.num_available();
            buffer2.uint = og_result.num_available();
            buffer3.uint = ob_result.num_available();
            break;
        default:
            std::cerr << "Error! Filters::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
            break;
        }
        data_ptr[0] = buffer1.uc[0];
        data_ptr[1] = buffer2.uc[0];
        data_ptr[2] = buffer3.uc[0];
        data_ptr[3] = 0;
        break;
    case tlm::TLM_WRITE_COMMAND:
        switch (addr)
        {
        case FILTERS_R_ADDR:
            if (mask_ptr[0] == 0xff)
            {
                i_r.write(data_ptr[0]);
            }
            if (mask_ptr[1] == 0xff)
            {
                i_g.write(data_ptr[1]);
            }
            if (mask_ptr[2] == 0xff)
            {
                i_b.write(data_ptr[2]);
            }
            break;
        default:
            std::cerr << "Error! Filters::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
            break;
        }
        break;
    case tlm::TLM_IGNORE_COMMAND:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
    default:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
    }
    payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
}

