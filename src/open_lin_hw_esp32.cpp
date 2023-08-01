#include "ESP32-SoftwareLIN/src/SoftwareLin.h"

extern SoftwareLin swLin;

#ifdef __cplusplus
extern "C" {
#endif

#include "open_lin_types.h"
#include "open_lin_transport_layer.h"

open_lin_NAD_t open_lin_NAD;
l_u16 open_lin_function_id;
l_u16 open_lin_supplier_id;

void open_lin_error_handler(t_open_lin_error error_code)
{

}

l_bool open_lin_hw_check_for_break(void)
{
    return swLin.checkBreak();
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

#ifdef OPEN_LIN_DYNAMIC_IDS
open_lin_id_translation_item_t open_lin_id_translation_tab[1];
#endif

void open_lin_master_dl_rx_callback(open_lin_frame_slot_t* slot)
{

}

void open_lin_on_rx_frame(open_lin_frame_slot_t *slot)
{

}

void open_lin_set_rx_enabled(l_bool status)
{

}

void open_lin_sid_callback(open_lin_frame_slot_t* slot)
{

}

#ifdef __cplusplus
}
#endif
