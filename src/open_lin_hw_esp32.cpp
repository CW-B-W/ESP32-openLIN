#include "ESP32-SoftwareLIN/src/SoftwareLin.h"
extern SoftwareLin swLin;

#ifdef __cplusplus
extern "C" {
#endif

#include "open_lin_types.h"

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

l_bool open_lin_hw_tx_data(l_u8* data, l_u8 len)
{
    return swLin.write((const uint8_t*)data, len);
}

void open_lin_set_rx_enabled(l_bool status)
{

}

#ifdef __cplusplus
}
#endif
