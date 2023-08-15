#include "ESP32-SoftwareLIN/src/SoftwareLin.h"

extern "C" {
#include "open-LIN-c/open_lin_master_data_layer.h"
}

#define RX_PIN (4)
#define TX_PIN (5)

SoftwareLin swLin(RX_PIN, TX_PIN);

const uint32_t frame_slot_time_us = 300000;

volatile t_open_lin_master_state master_state_prev = OPEN_LIN_MASTER_IDLE;
volatile t_open_lin_master_state master_state = OPEN_LIN_MASTER_IDLE;
void open_lin_master_state_callback(t_open_lin_master_state new_state)
{
    // Serial.printf("master_state = %d\n", new_state);

    master_state_prev = master_state;
    master_state = new_state;
    if (new_state == OPEN_LIN_MASTER_IDLE) {
        // DO NOT use swLin.flush()
        // swLin.flush() is for flushing Rx buffer
        swLin.endFrame();
    }

    if (master_state != master_state_prev && master_state == OPEN_LIN_MASTER_IDLE) {
        // transitted to IDLE state
        
        int32_t delay_us = frame_slot_time_us - (micros() - open_lin_master_dl_get_frame_start_time_us());
        // it's possible `delay_us < 0`
        uint32_t start_micros = micros();
        while (delay_us > 0 && delay_us > (micros() - start_micros))
        {
            ; // Wait until the frame slot time is consumed
        }
    }
}

extern "C" void app_main()
{
    Serial.begin(115200);
    swLin.begin(19200);

    l_u8 frame_data_length[] = {
        1,
        1,
        8
    };

    uint8_t master_data_buffer[][8] = {
        {0x00},
        {'X'},
        {'h', 'e', 'l', 'l', 'o', 'y', 'o', 'u'}
    };
    t_master_frame_table_item master_frame_table[] = {
        {frame_slot_time_us, {0x01, OPEN_LIN_FRAME_TYPE_RECEIVE,   frame_data_length[0], master_data_buffer[0]}},
        {frame_slot_time_us, {0x02, OPEN_LIN_FRAME_TYPE_TRANSMIT,  frame_data_length[1], master_data_buffer[1]}},
        {frame_slot_time_us, {0x03, OPEN_LIN_FRAME_TYPE_TRANSMIT,  frame_data_length[2], master_data_buffer[2]}}
    };
    l_u8 table_size = sizeof(master_frame_table) / sizeof(master_frame_table[0]);
    open_lin_master_dl_init(master_frame_table, table_size);
    open_lin_master_dl_set_state_callback(open_lin_master_state_callback);
    
    while (1) {
        open_lin_master_dl_handler();
    }
}

#ifdef __cplusplus
extern "C" {
#endif

void open_lin_master_dl_rx_callback(open_lin_frame_slot_t* slot)
{
    Serial.printf("[open_lin_master_dl_rx_callback] PID=%d\n\t", (int)slot->pid);
    for (int i = 0; i < slot->data_length; ++i) {
        Serial.printf("0x%02X ", slot->data_ptr[i]);
    }
    Serial.printf("\n\n");
}

void open_lin_error_handler(t_open_lin_error error_code)
{
    Serial.printf("[open_lin_error_handler] error_code = %d\n", (int)error_code);

    switch (error_code) {
        case OPEN_LIN_NO_ERROR:
            Serial.printf("\t%s\n", "OPEN_LIN_NO_ERROR");
            break;

        case OPEN_LIN_SLAVE_ERROR_INVALID_DATA_RX:
            Serial.printf("\t%s\n", "OPEN_LIN_SLAVE_ERROR_INVALID_DATA_RX");
            break;

        case OPEN_LIN_SLAVE_ERROR_INVALID_CHECKSUM:
            Serial.printf("\t%s\n", "OPEN_LIN_SLAVE_ERROR_INVALID_CHECKSUM");
            break;

        case OPEN_LIN_SLAVE_ERROR_PID_PARITY:
            Serial.printf("\t%s\n", "OPEN_LIN_SLAVE_ERROR_PID_PARITY");
            break;

        case OPEN_LIN_SLAVE_ERROR_INVALID_SYNCH:
            Serial.printf("\t%s\n", "OPEN_LIN_SLAVE_ERROR_INVALID_SYNCH");
            break;

        case OPEN_LIN_SLAVE_ERROR_INVALID_BREAK:
            Serial.printf("\t%s\n", "OPEN_LIN_SLAVE_ERROR_INVALID_BREAK");
            break;

        case OPEN_LIN_SLAVE_ERROR_ID_NOT_FOUND:
            Serial.printf("\t%s\n", "OPEN_LIN_SLAVE_ERROR_ID_NOT_FOUND");
            break;

        case OPEN_LIN_SLAVE_ERROR_HW_TX:
            Serial.printf("\t%s\n", "OPEN_LIN_SLAVE_ERROR_HW_TX");
            break;

        case OPEN_LIN_MASTER_ERROR_CHECKSUM:
            Serial.printf("\t%s\n", "OPEN_LIN_MASTER_ERROR_CHECKSUM");
            break;

        case OPEN_LIN_MASTER_ERROR_HEADER_TX:
            Serial.printf("\t%s\n", "OPEN_LIN_MASTER_ERROR_HEADER_TX");
            break;

        case OPEN_LIN_MASTER_ERROR_DATA_TX:
            Serial.printf("\t%s\n", "OPEN_LIN_MASTER_ERROR_DATA_TX");
            break;

        case OPEN_LIN_MASTER_ERROR_DATA_RX:
            Serial.printf("\t%s\n", "OPEN_LIN_MASTER_ERROR_DATA_RX");
            break;

        case OPEN_LIN_MASTER_ERROR_FRAME_SLOT_TIMEOUT:
            Serial.printf("\t%s\n", "OPEN_LIN_MASTER_ERROR_FRAME_SLOT_TIMEOUT");
            break;
        
        default:
            assert(0);
    }
}

#ifdef __cplusplus
}
#endif
