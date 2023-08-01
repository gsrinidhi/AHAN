
#include"test.h"

void FabricIrq0_IRQHandler(void)
{
    I2C_isr(&g_core_i2c0);
}

void FabricIrq1_IRQHandler(void)
{
    I2C_isr(&g_core_i2c1);
}

void FabricIrq2_IRQHandler(void)
{
    I2C_isr(&g_core_i2c2);
}

void FabricIrq3_IRQHandler(void)
{
    I2C_isr(&g_core_i2c3);
}

void FabricIrq4_IRQHandler(void)
{
    I2C_isr(&g_core_i2c4);
}

void FabricIrq5_IRQHandler(void)
{
    I2C_isr(&g_core_i2c5);
}

void write_handler(i2c_instance_t* this_i2c, uint8_t* rx_buffer, uint8_t n){
    uint8_t i;
    if(this_i2c == &g_core_i2c0){
        for(i=0;i<n;i++){
            g0_slave_tx_buffer[i] = rx_buffer[i];
        }

    }

    else if(this_i2c == &g_core_i2c1){
            for(i=0;i<n;i++){
                g1_slave_tx_buffer[i] = rx_buffer[i];
            }

        }

    else if(this_i2c == &g_core_i2c2){
                for(i=0;i<n;i++){
                    g2_slave_tx_buffer[i] = rx_buffer[i];
                }

        }
    else if(this_i2c == &g_core_i2c3){
                for(i=0;i<n;i++){
                    g3_slave_tx_buffer[i] = rx_buffer[i];
                }

        }

}


