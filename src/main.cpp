#include "ESP32-SoftwareLIN/src/SoftwareLin.h"

extern "C" {
#include "open-LIN-c/open_lin_data_layer.h"
#include "open-LIN-c/open_lin_master_data_layer.h"
#include "open-LIN-c/open_lin_slave_data_layer.h"
}

#define RX_PIN (16)
#define TX_PIN (17)

SoftwareLin swLin(RX_PIN, TX_PIN);
extern l_bool open_lin_hw_break_reg;

// #define LIN_MASTER
#define LIN_SLAVE

#ifdef LIN_MASTER
volatile t_open_lin_master_state master_state_prev = OPEN_LIN_MASTER_IDLE;
volatile t_open_lin_master_state master_state = OPEN_LIN_MASTER_IDLE;
void open_lin_master_state_callback(t_open_lin_master_state new_state)
{
    master_state_prev = master_state;
    master_state = new_state;
    if (new_state == OPEN_LIN_MASTER_IDLE) {
        swLin.endFrame();
    }

    if (master_state != master_state_prev && master_state == OPEN_LIN_MASTER_IDLE) {
        // delay(1000);
    }
}
#endif

#ifdef LIN_SLAVE
volatile t_open_lin_slave_state slave_state_prev = OPEN_LIN_SLAVE_IDLE;
volatile t_open_lin_slave_state slave_state = OPEN_LIN_SLAVE_IDLE;
void open_lin_slave_state_callback(t_open_lin_slave_state new_state)
{
    slave_state_prev = slave_state;
    slave_state = new_state;
    if (new_state == OPEN_LIN_SLAVE_IDLE) {
        swLin.endFrame();
    }
    
    if (slave_state != slave_state_prev && slave_state == OPEN_LIN_MASTER_IDLE) {
        // delay(1000);
    }
}
#endif

extern "C" void app_main()
{
    Serial.begin(115200);
    swLin.begin(9600);

    uint8_t master_data_buffer_tx[] = {'h', 'e', 'l', 'l', 'o'};
    t_master_frame_table_item master_frame_table[1] = {
            {10,0,{0x02,OPEN_LIN_FRAME_TYPE_TRANSMIT,sizeof(master_data_buffer_tx), master_data_buffer_tx}}
    };
    l_u8 table_size = sizeof(master_frame_table) / sizeof(master_frame_table[0]);

#ifdef LIN_MASTER
    open_lin_master_dl_init(master_frame_table, table_size);
    open_lin_master_dl_set_state_callback(open_lin_master_state_callback);
    
    unsigned long micro_prev = micros();
    while (1) {
        unsigned long micro_now = micros();
        open_lin_master_dl_handler((micro_now - micro_prev)/1000);
        micro_prev = micro_now;
        delay(1);
    }
#endif

#ifdef LIN_SLAVE
    uint8_t slave_data_buffer_rx[5];
    open_lin_frame_slot_t frame_slot[1] = {0x02,OPEN_LIN_FRAME_TYPE_RECEIVE,sizeof(slave_data_buffer_rx), slave_data_buffer_rx};
    l_u8 slot_size = sizeof(frame_slot) / sizeof(frame_slot[0]);
    open_lin_net_init(frame_slot, slot_size);

    open_lin_slave_init();
    open_lin_slave_set_state_callback(open_lin_slave_state_callback);

    while (1) {
        if (swLin.checkBreak()) {
            uint8_t buf[3 + sizeof(master_data_buffer_tx)/sizeof(master_data_buffer_tx[0])];
            int bytes_to_read = sizeof(buf);
            int bytes_read = 0;
            while (bytes_read < bytes_to_read) {
                while (swLin.available() <= 0)
                    ;
                bytes_read += swLin.read(buf+bytes_read, bytes_to_read - bytes_read);
            }

            open_lin_slave_reset();

            // In `open_lin_slave_rx_header` it checks for break,
            // but the swLin.checkBreak() has done the check.
            // Therefore `open_lin_hw_break_reg` is reg to notify
            // that the break has been detected.
            open_lin_hw_break_reg = true;
            open_lin_slave_rx_header(0); // To notify that the break has detected

            for (int i = 0; i < sizeof(buf)/sizeof(buf[0]); ++i) {
                open_lin_slave_rx_header(buf[i]);
            }

            swLin.endFrame();

            delay(10);
        }
    }
#endif
}