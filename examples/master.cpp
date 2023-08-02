#include "ESP32-SoftwareLIN/src/SoftwareLin.h"

extern "C" {
#include "open-LIN-c/open_lin_master_data_layer.h"
}

#define RX_PIN (4)
#define TX_PIN (5)

SoftwareLin swLin(RX_PIN, TX_PIN);

volatile t_open_lin_master_state master_state_prev = OPEN_LIN_MASTER_IDLE;
volatile t_open_lin_master_state master_state = OPEN_LIN_MASTER_IDLE;
void open_lin_master_state_callback(t_open_lin_master_state new_state)
{
    // Serial.printf("master_state = %d\n", new_state);

    master_state_prev = master_state;
    master_state = new_state;
    if (new_state == OPEN_LIN_MASTER_IDLE) {
        swLin.endFrame();
    }

    if (master_state != master_state_prev && master_state == OPEN_LIN_MASTER_IDLE) {
        // transitted to IDLE state
    }
}

extern "C" void app_main()
{
    Serial.begin(115200);
    swLin.begin(9600);

    l_u8 frame_data_length[] = {
        5,
        5
    };

    uint8_t master_data_buffer[][8] = {
        {'h', 'e', 'l', 'l', 'o'},
        {0, 0, 0, 0, 0}
    };
    t_master_frame_table_item master_frame_table[] = {
            {10, 0, {0x02, OPEN_LIN_FRAME_TYPE_TRANSMIT, frame_data_length[0], master_data_buffer[0]}},
            {10, 0, {0x03, OPEN_LIN_FRAME_TYPE_RECEIVE,  frame_data_length[1], master_data_buffer[1]}}
    };
    l_u8 table_size = sizeof(master_frame_table) / sizeof(master_frame_table[0]);
    open_lin_master_dl_init(master_frame_table, table_size);
    open_lin_master_dl_set_state_callback(open_lin_master_state_callback);
    
    unsigned long micro_prev = micros();
    while (1) {
        unsigned long micro_now = micros();

        if (master_state == OPEN_LIN_MASTER_DATA_RX) {
            uint8_t buf[8];
            
            const unsigned long timeout_us = 100000; // 100ms timeout
            unsigned long start_micro = micros();
            while (swLin.available() <= 0 && (micros() - start_micro) < timeout_us) // 100ms timeout
                ;
            int bytes_read = swLin.read(buf, sizeof(buf));

            for (int i = 0; i < bytes_read; ++i) {
                open_lin_master_dl_rx(buf[i]);
            }

            if (bytes_read <= 0) {
                Serial.printf("Master read timeout\n");
            }
        }

        open_lin_master_dl_handler((micro_now - micro_prev)/1000);
        micro_prev = micro_now;
        delay(1);
    }
}

#ifdef __cplusplus
extern "C" {
#endif

void open_lin_master_dl_rx_callback(open_lin_frame_slot_t* slot)
{
    Serial.printf("[open_lin_master_dl_rx_callback]\n\t");
    for (int i = 0; i < slot->data_length; ++i) {
        Serial.printf("0x%02X ", slot->data_ptr[i]);
    }
    Serial.printf("\n\n");
}

void open_lin_error_handler(t_open_lin_error error_code)
{
    Serial.printf("[open_lin_error_handler] error_code = %d\n", (int)error_code);
}

#ifdef __cplusplus
}
#endif
