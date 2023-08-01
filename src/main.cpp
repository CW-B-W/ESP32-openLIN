#include "ESP32-SoftwareLIN/src/SoftwareLin.h"

extern "C" {
#include "open-LIN-c/open_lin_data_layer.h"
#include "open-LIN-c/open_lin_master_data_layer.h"
}

#define RX_PIN (16)
#define TX_PIN (17)

SoftwareLin swLin(RX_PIN, TX_PIN);

void open_lin_master_state_callback(t_open_lin_master_state new_state)
{
    if (new_state == OPEN_LIN_MASTER_IDLE) {
        swLin.endFrame();
    }
}

extern "C" void app_main()
{
    swLin.begin(9600);

    while (1) {
        uint8_t master_data_buffer_tx[] = {'h', 'e', 'l', 'l', 'o'};
        t_master_frame_table_item master_frame_table[1] = {
                {10,0,{0x02,OPEN_LIN_FRAME_TYPE_TRANSMIT,sizeof(master_data_buffer_tx), master_data_buffer_tx}}
        };
        l_u8 table_size = sizeof(master_frame_table) / sizeof(t_master_frame_table_item);
        open_lin_master_dl_init(master_frame_table, table_size);
        open_lin_master_dl_set_state_callback(open_lin_master_state_callback);
        
        unsigned long micro_prev = micros();
        while (1) {
            unsigned long micro_now = micros();
            open_lin_master_dl_handler((micro_now - micro_prev)/1000);
            micro_prev = micro_now;
            delay(1);
        }
    }
}