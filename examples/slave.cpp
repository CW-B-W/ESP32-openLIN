#include "ESP32-SoftwareLIN/src/SoftwareLin.h"

extern "C" {
#include "open-LIN-c/open_lin_network_layer.h"
#include "open-LIN-c/open_lin_slave_data_layer.h"
}

#define RX_PIN (4)
#define TX_PIN (5)

SoftwareLin swLin(RX_PIN, TX_PIN);
extern l_bool open_lin_hw_break_reg;

volatile t_open_lin_slave_state slave_state_prev = OPEN_LIN_SLAVE_IDLE;
volatile t_open_lin_slave_state slave_state = OPEN_LIN_SLAVE_IDLE;
void open_lin_slave_state_callback(t_open_lin_slave_state new_state)
{
    // Serial.printf("slave_state = %d\n", new_state);

    slave_state_prev = slave_state;
    slave_state = new_state;
    if (new_state == OPEN_LIN_SLAVE_IDLE) {
        swLin.endFrame();
    }
    
    if (slave_state != slave_state_prev && slave_state == OPEN_LIN_SLAVE_IDLE) {
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

    uint8_t slave_data_buffer[][8] = {
        {0, 0, 0, 0, 0},
        {'S', 'L', 'A', 'V', 'E'}
    };
    open_lin_frame_slot_t frame_slot[] = {
        {0x02, OPEN_LIN_FRAME_TYPE_RECEIVE,  frame_data_length[0], slave_data_buffer[0]},
        {0x03, OPEN_LIN_FRAME_TYPE_TRANSMIT, frame_data_length[1], slave_data_buffer[1]}
    };
    l_u8 slot_size = sizeof(frame_slot) / sizeof(frame_slot[0]);
    open_lin_net_init(frame_slot, slot_size);

    open_lin_slave_init();
    open_lin_slave_set_state_callback(open_lin_slave_state_callback);

    while (1) {
        if (swLin.checkBreak()) {
            // In `open_lin_slave_rx_header` it checks for break,
            // but the swLin.checkBreak() has done the check.
            // Therefore `open_lin_hw_break_reg` is reg to notify
            // that the break has been detected.
            open_lin_hw_break_reg = true;
            open_lin_slave_rx_header(0); // To notify that the break has detected

            uint8_t buf[3 + 8];
            while (slave_state != OPEN_LIN_SLAVE_IDLE) {
                const unsigned long timeout_us = 100000; // 100ms timeout
                unsigned long start_micro = micros();
                while (swLin.available() <= 0 && (micros() - start_micro) < timeout_us)
                    ;
                int bytes_read = swLin.read(buf, sizeof(buf));

                for (int i = 0; i < bytes_read; ++i) {
                    open_lin_slave_rx_header(buf[i]);
                }

                if (bytes_read <= 0) {
                    open_lin_slave_reset();
                    Serial.printf("Slave read timeout, resetting slave\n");
                }
            }

            delay(10);
        }
    }
}

#ifdef __cplusplus
extern "C" {
#endif

void open_lin_on_rx_frame(open_lin_frame_slot_t *slot)
{
    Serial.printf("[open_lin_on_rx_frame]\n\t");
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
