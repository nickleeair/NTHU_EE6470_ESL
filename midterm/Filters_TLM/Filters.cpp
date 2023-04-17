#include <cmath>

#include "Filters.h"

DecimationFilter::DecimationFilter(sc_module_name n) : sc_module(n), t_skt("t_skt")
{
    SC_THREAD(do_filter);
    t_skt.register_b_transport(this, &DecimationFilter::blocking_transport);
}

const double mask[MASK_X] = {1 / 2, 1 / 3, 1 / 6};

void DecimationFilter::do_filter()
{

    for (int i = 0; i < 129; i++)
    {
        x_row[i] = 0;
        wait(1);
    }

    for (int i = 0; i < 129; i++)
    {
        if (i == 0)
        {
            x_row[i] = 0;
        }
        else
        {
            x_row[i] = i_x.read();
        }
        wait(1);
    }
    for (int i = 0; i < 129; i++)
        y_val = (x_row[i + 2] * mask[0] + x_row[i + 1] * mask[1] + x_row[i] * mask[2]);

    // std::cout << "r_val_mean: " << r_val << std::endl;
    oy_result.write(y_val);
    // std::cout << "r_val_mean_input: " << r_val << std::endl;

    wait(1); // emulate module delay
}

void DecimationFilter::blocking_transport(tlm::tlm_generic_payload &payload,
                                          sc_core::sc_time &delay)
{
    sc_dt::uint64 addr = payload.get_address();
    addr = addr - base_offset;
    unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    unsigned char *data_ptr = payload.get_data_ptr();
    word buffer;
    switch (payload.get_command())
    {
    case tlm::TLM_READ_COMMAND:
        switch (addr)
        {
        case SOBEL_FILTER_RESULT_ADDR:
            buffer.uint = o_result.read();
            break;
        case SOBEL_FILTER_CHECK_ADDR:
            buffer.uint = o_result.num_available();
            break;
        default:
            std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
            break;
        }
        data_ptr[0] = buffer.uc[0];
        break;

    case tlm::TLM_WRITE_COMMAND:
        switch (addr)
        {
        case SOBEL_FILTER_R_ADDR:
            if (mask_ptr[0] == 0xff)
            {
                i_x.write(data_ptr[0]);
            }
            break;
        default:
            std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
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
