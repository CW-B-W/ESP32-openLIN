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
        // DO NOT use swLin.flush()
        // swLin.flush() is for flushing Rx buffer
        swLin.endFrame();
    }
    
    if (slave_state != slave_state_prev && slave_state == OPEN_LIN_SLAVE_IDLE) {
        // transitted to IDLE state
    }
}

extern "C" void app_main()
{
    Serial.begin(115200);

// #define LIN_AUTOBAUD

#ifdef LIN_AUTOBAUD
    const uint32_t command_baud[] = {1200, 2400, 4800, 9600, 14400, 19200};
    const uint32_t LIN_BAUD_MAX = 20000;
    swLin.begin(LIN_BAUD_MAX);
#else
    swLin.begin(19200);
#endif

    l_u8 frame_data_length[] = {
        1,
        1,
        8
    };

    uint8_t slave_data_buffer[][8] = {
        {'X'},
        {0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
    };
    open_lin_frame_slot_t frame_slot[] = {
        {0x01, OPEN_LIN_FRAME_TYPE_TRANSMIT,  frame_data_length[0], slave_data_buffer[0]},
        {0x02, OPEN_LIN_FRAME_TYPE_RECEIVE,   frame_data_length[1], slave_data_buffer[1]},
        {0x03, OPEN_LIN_FRAME_TYPE_RECEIVE,   frame_data_length[2], slave_data_buffer[2]}
    };
    l_u8 slot_size = sizeof(frame_slot) / sizeof(frame_slot[0]);
    open_lin_net_init(frame_slot, slot_size);

    open_lin_slave_init();
    open_lin_slave_set_state_callback(open_lin_slave_state_callback);

    while (1) {
        if (swLin.checkBreak()) {
            // sw_lin.checkBreak() blocks until UART ISR gives the semaphore

            // In `open_lin_slave_rx_header` it checks for break,
            // but the swLin.checkBreak() has done the check.
            // Therefore `open_lin_hw_break_reg` is reg to notify
            // that the break has been detected.
            open_lin_hw_break_reg = true;
            open_lin_slave_rx_header(0); // To notify that the break has detected

#ifdef LIN_AUTOBAUD
            uint32_t autobaud = swLin.setAutoBaud(command_baud, sizeof(command_baud)/sizeof(command_baud[0]));
            if (autobaud) {
                Serial.printf("autobaud detection succeeded. Set baud = %u\n\n", autobaud);
                open_lin_slave_rx_header(0x55); // setAutoBaud() has successfully recognized the SYNC
            }
            else {
                Serial.printf("autobaud detection failed. baud is not changed = %u\n\n", swLin.baudRate());
                open_lin_slave_rx_header(0x00); // setAutoBaud() failed to recognize SYNC. 0x00 is not expected, thus the slave is reset.
            }
#else
            
#endif

            const unsigned long read_timeout_us = 300000 - 500;
            unsigned long start_micro = micros();
            while (slave_state != OPEN_LIN_SLAVE_IDLE) {
                while (swLin.available() <= 0 && (micros() - start_micro) < read_timeout_us)
                    ;
                uint8_t val = 0;
                int bytes_read = swLin.read(&val, 1);

                if (bytes_read <= 0) {
                    open_lin_slave_reset();
                    Serial.printf("Slave read timeout, resetting slave\n");
                    break;
                }

                open_lin_slave_rx_header(val);
            }
        }
    }
}

#ifdef __cplusplus
extern "C" {
#endif

void open_lin_on_rx_frame(open_lin_frame_slot_t *slot)
{
    Serial.printf("[open_lin_on_rx_frame] PID=%d\n\t", (int)slot->pid);
    for (int i = 0; i < slot->data_length; ++i) {
        Serial.printf("0x%02X ", slot->data_ptr[i]);
    }
    Serial.printf("\n\n");
}

void open_lin_on_tx_frame(open_lin_frame_slot_t *slot)
{
    Serial.printf("[open_lin_on_tx_frame] PID=%d\n\n", (int)slot->pid);
    slot->data_ptr[0]++;
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
    Serial.println("\n");
}

#ifdef __cplusplus
}
#endif
