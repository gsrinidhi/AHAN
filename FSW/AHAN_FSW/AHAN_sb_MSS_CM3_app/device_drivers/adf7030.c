#include "adf7030.h"

uint8_t radio_memory_configuration[ ] = {
    #include "Settings_adf7030.txt"
    //The below 0x00 is included because an "expected expression" error occurs if its deleted. Need to find out why?
    0x00
};

//variable to store the length of radio_memory_configuration array
static uint32_t config_length_header;

//Pointer to store the beginning of the SPI commands in radio_memory_configuration
uint8_t *radio_memory_configuration_start_spi_command;

uint8_t config_adf7030(uint32_t size) {
    //Not using PDMA
#if ADF_USE_PDMA == 0

    uint32_t array_position = 0;
    
    ADF_SPI_SLAVE_SELECT(&ADF_SPI_INSTANCE,ADF_SPI_SLAVE);
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
      ADF_SPI_TRANSFER_BLOCK(&ADF_SPI_INSTANCE,pSeqData,(length - 4),0,0);
      
      
      // Update the array position to point to the next block
      array_position += length;
    
    }while(array_position < size); // Continue operation until full data file has been written
    ADF_SPI_SLAVE_CLEAR(&ADF_SPI_INSTANCE,ADF_SPI_SLAVE);
    return 0;

#else

#endif
}

uint8_t adf_write_to_memory(uint8_t mode,uint32_t addr,uint8_t *data,uint32_t size) {
    //Currently implementing mode1 for testing. Need to implement other modes
    if((mode & WMODE_1) == WMODE_1){
        uint8_t cmd_data[] = {mode,(addr >> 24),((addr >> 16) | 0xFF),((addr >> 8) | 0xFF),(addr | 0xFF )};
        ADF_SPI_SLAVE_SELECT(&ADF_SPI_INSTANCE,ADF_SPI_SLAVE);
        ADF_SPI_TRANSFER_BLOCK(&ADF_SPI_INSTANCE,cmd_data,5,0,0);
        ADF_SPI_TRANSFER_BLOCK(&ADF_SPI_INSTANCE,data,size,0,0);
        ADF_SPI_SLAVE_CLEAR(&ADF_SPI_INSTANCE,ADF_SPI_SLAVE);
    }

    return 0;
    

}