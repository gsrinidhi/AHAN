#include "adf7030.h"

ADF_SPI_INSTANCE_t *adf_spi;

void set_adf_spi_instance(ADF_SPI_INSTANCE_t *instance) {
    adf_spi = instance;
}
uint8_t radio_memory_configuration[ ] = {
    #include "Settings_adf7030.txt"
//    //The below 0x00 is included because an "expected expression" error occurs if its deleted. Need to find out why?
//    0x00
};

//variable to store the length of radio_memory_configuration array
static uint32_t config_length_header;

//Pointer to store the beginning of the SPI commands in radio_memory_configuration
uint8_t *radio_memory_configuration_start_spi_command;

uint8_t config_adf7030() {
    uint32_t size = sizeof(radio_memory_configuration);
    //Not using PDMA
#if ADF_USE_PDMA == 0

    uint32_t array_position = 0;
    
    ADF_SPI_SLAVE_SELECT(adf_spi,ADF_SPI_SLAVE);
    do 
    { 
      // Calculate the number of bytes to write
      uint32_t length =  (*(radio_memory_configuration + array_position ) << 16) | 
                         (*(radio_memory_configuration + array_position + 1) << 8) |
                         (*(radio_memory_configuration + array_position + 2));
      
      if(length > 0xFFFF)
      {
         return 1;
      } 
      
      // Write the SPI data pointed to location (MEMORY_FILE + array_position) with specified length (length)
      uint8_t * pSeqData = (radio_memory_configuration + array_position + 3);
      
      // Transfer the Configuration sequence
      ADF_SPI_TRANSFER_BLOCK(adf_spi,pSeqData,0,(length - 4));
      
      
      // Update the array position to point to the next block
      array_position += length;
    
    }while(array_position < size); // Continue operation until full data file has been written

    //Send command to apply the configuration
    adf_send_cmd(CMD_CFG_DEV);
    ADF_SPI_SLAVE_CLEAR(adf_spi,0);
    return 0;

#else

#endif
}

uint8_t adf_write_to_memory(uint8_t mode,uint32_t addr,uint8_t *data,uint32_t size) {
    //Currently implementing mode1 for testing. Need to implement other modes
    if((mode & WMODE_1) == WMODE_1){
        uint8_t cmd_data[] = {mode,(addr >> 24),((addr >> 16) | 0xFF),((addr >> 8) | 0xFF),(addr | 0xFF )};
        ADF_SPI_SLAVE_SELECT(adf_spi,ADF_SPI_SLAVE);
        ADF_SPI_TRANSFER_BLOCK(adf_spi,cmd_data,0,5);
        ADF_SPI_TRANSFER_BLOCK(adf_spi,data,0,size);
        ADF_SPI_SLAVE_CLEAR(adf_spi,0);
    }

    return 0;
    

}

uint8_t* adf_read_from_memory(uint8_t mode,uint32_t addr,uint8_t *data,uint32_t size) {
    if((mode & WMODE_1) == WMODE_1){
        uint8_t cmd_data[] = {mode,(addr >> 24),((addr >> 16) | 0xFF),((addr >> 8) | 0xFF),(addr | 0xFF )};
        ADF_SPI_SLAVE_SELECT(adf_spi,ADF_SPI_SLAVE);
        ADF_SPI_TRANSFER_BLOCK(adf_spi,cmd_data,0,5);
        ADF_SPI_TRANSFER_BLOCK(adf_spi,0,data,size+RMODE1_OFFSET);
        ADF_SPI_SLAVE_CLEAR(adf_spi,0);
    }
    return (data+RMODE1_OFFSET);
}

uint8_t adf_send_cmd(uint8_t command) {
    ADF_SPI_SLAVE_SELECT(adf_spi,ADF_SPI_SLAVE);
    uint32_t check_val = 0;
    uint8_t tries = 0;
    //Send NOP command(0xFF) until adf is ready to receive command
    do {
        check_val = ADF_SPI_TRANSFER_FRAME(adf_spi,ADF_NOP,0,1);
        if((check_val & CMD_READY) != 0) {
            break;
        }
    }while(tries++ < 100);
    if(tries >= 100) {
        return check_val;
    }

    //Send the command
    ADF_SPI_TRANSFER_FRAME(adf_spi,command,0,1);

    return 0;

}
