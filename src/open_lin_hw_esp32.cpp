#include "ESP32-SoftwareLIN/src/SoftwareLin.h"
extern SoftwareLin swLin;

#ifdef __cplusplus
extern "C" {
#endif

#include "open_lin_types.h"

l_u32 open_lin_hw_get_time_us()
{
    return micros();
}

l_bool open_lin_hw_break_reg = false;
l_bool open_lin_hw_check_for_break(void)
{
    if (open_lin_hw_break_reg) {
        open_lin_hw_break_reg = false;
        return true;
    }
    else {
        return false;
    }
}

l_bool open_lin_hw_tx_break(void)
{
    swLin.sendBreak();
    return true;
}

l_bool open_lin_hw_tx_byte(l_u8 data)
{
    return swLin.write((const uint8_t*)&data, 1);
}

l_u8 open_lin_hw_tx_data(l_u8* data, l_u8 len)
{
    return swLin.write((const uint8_t*)data, len);
}

l_bool open_lin_hw_rx_byte(l_u8 *data, l_u32 timeout_us)
{
    int bytes_read = 0;
    l_u32 start_us = open_lin_hw_get_time_us();
    do {
        bytes_read = swLin.read(data, 1);
    }while (bytes_read <= 0 && timeout_us > open_lin_hw_get_time_us() - start_us);
    return bytes_read;
}

l_u8 open_lin_hw_rx_data(l_u8 *data, l_u8 len, l_u32 timeout_us)
{
    int bytes_read = 0;
    l_u32 start_us = open_lin_hw_get_time_us();
    for (int i = 0; i < len; ++i) {
        do {
            bytes_read += swLin.read(data, 1);
        }while (bytes_read <= i && timeout_us > open_lin_hw_get_time_us() - start_us);
        if (timeout_us > open_lin_hw_get_time_us() - start_us) {
            break;
        }
    }
    return bytes_read;
}

static l_bool rx_enabled = l_false;
void open_lin_set_rx_enabled(l_bool status)
{
    if (rx_enabled == status) {
        return;
    }

    if (status == l_true) {
        swLin.flush(); // flush Rx buffer
        swLin.enableRx(true);
        rx_enabled = l_true;
    }
    else {
        swLin.enableRx(false);
        swLin.flush(); // flush Rx buffer
        rx_enabled = l_false;
    }
}

#ifdef __cplusplus
}
#endif
