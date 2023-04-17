#include <cmath>
#include <iomanip>

#include "Filters.h"

MeanFilter::MeanFilter(sc_module_name n)
    : sc_module(n), t_skt("t_skt"), base_offset(0)
{
    SC_THREAD(do_filter);

    t_skt.register_b_transport(this, &MeanFilter::blocking_transport);
}

const double mask[MASK_X] = {1 / 6, 1 / 3, 1 / 2};

void MeanFilter::do_filter()
{
    while (true)
    {
        y_val = 0;
        for (int i = 0; i != 129; i++)
        {
            if (i == 0)
            {
                X[i] = 0;
            }
            else
            {
                X[i] = i_r.read();
            }
        }
        std::cout << "r_val_decimation_initial: " << r_val << std::endl;
        for (int v = 0; v < 129; ++v)
        {
            for (int u = 0; i < 3; ++i)
            {
                y_val += X[v + u] * mask[u];
            }
        }

        // std::cout << "r_val_mean: " << r_val << std::endl;
        or_result.write(y_val);
        std::cout << "r_val_mean_input: " << r_val << std::endl;
    }
}

void MeanFilter::blocking_transport(tlm::tlm_generic_payload &payload,
                                    sc_core::sc_time &delay)
{
    sc_dt::uint64 addr = payload.get_address();
    addr -= base_offset;
    unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    unsigned char *data_ptr = payload.get_data_ptr();
    word buffer1, buffer2, buffer3;
    switch (payload.get_command())
    {
    case tlm::TLM_READ_COMMAND:
        switch (addr)
        {
        case MEAN_FILTER_RESULT_ADDR:
            buffer1.uint = or_result.read();
            break;
        case MEAN_FILTER_CHECK_ADDR:
            buffer1.uint = or_result.num_available();
            break;
        default:
            std::cerr << "Error! MeanBlurFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
        }
        data_ptr[0] = buffer1.uc[0];
        data_ptr[1] = 0;
        data_ptr[2] = 0;
        data_ptr[3] = 0;
        break;
    case tlm::TLM_WRITE_COMMAND:
        switch (addr)
        {
        case MEAN_FILTER_R_ADDR:
            if (mask_ptr[0] == 0xff)
            {
                i_r.write(data_ptr[0]);
            }
            break;
        default:
            std::cerr << "Error! MeanBlurFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
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