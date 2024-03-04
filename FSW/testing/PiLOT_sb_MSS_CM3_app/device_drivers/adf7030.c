#include "adf7030.h"

ADF_SPI_INSTANCE_t *adf_spi;

void set_adf_spi_instance(ADF_SPI_INSTANCE_t *instance) {
    adf_spi = instance;
}
uint8_t radio_memory_configuration[ ] = {
    #include "Settings_adf7030_3.txt"
//    //The below 0x00 is included because an "expected expression" error occurs if its deleted. Need to find out why?
//    0x00
};

uint8_t callibrations_config[] = {
    #include "OfflineCalibrations.cfg"
};

//variable to store the length of radio_memory_configuration array
static uint32_t config_length_header;

//Pointer to store the beginning of the SPI commands in radio_memory_configuration
uint8_t *radio_memory_configuration_start_spi_command;

uint8_t apply_file(uint8_t *file) {
    uint32_t size = sizeof(file);
    uint32_t array_position = 0;
    uint8_t *pSeqData;
    ADF_SPI_SLAVE_SELECT(adf_spi,ADF_SPI_SLAVE);
    do 
    { 
      // Calculate the number of bytes to write
      uint32_t length =  (*(file + array_position ) << 16) | 
                         (*(file + array_position + 1) << 8) |
                         (*(file + array_position + 2));
      
      if(length > 0xFFFF)
      {
         return ERR_LENGTH_OVERFLOW;
      } 
      
      // Write the SPI data pointed to location (MEMORY_FILE + array_position) with specified length (length)
      pSeqData = (file + array_position + 3);
      
      // Transfer the Configuration sequence
      ADF_SPI_BLOCK_WRITE(adf_spi,pSeqData,1,(pSeqData+1),(length-4));

      // Update the array position to point to the next block
      array_position += length;
    
    }while(array_position < size); // Continue operation until full data file has been written
    //ADF_SPI_SLAVE_SELECT(adf_spi,0);
    //using new drivers
    ADF_SPI_SLAVE_CLEAR(adf_spi,ADF_SPI_SLAVE);

    return 0;

}

uint8_t config_adf7030() {
//    uint32_t size = sizeof(radio_memory_configuration);
    //Not using PDMA
#if ADF_USE_PDMA == 0

    // uint32_t array_position = 0, i;
    
    // ADF_SPI_SLAVE_SELECT(adf_spi,ADF_SPI_SLAVE);
    // do 
    // { 
    //   // Calculate the number of bytes to write
    //   uint32_t length =  (*(radio_memory_configuration + array_position ) << 16) | 
    //                      (*(radio_memory_configuration + array_position + 1) << 8) |
    //                      (*(radio_memory_configuration + array_position + 2));
      
    //   if(length > 0xFFFF)
    //   {
    //      return ERR_LENGTH_OVERFLOW;
    //   } 
      
    //   // Write the SPI data pointed to location (MEMORY_FILE + array_position) with specified length (length)
    //   uint8_t * pSeqData = (radio_memory_configuration + array_position + 3);
      
    //   // Transfer the Configuration sequence
    //   MSS_GPIO_set_output(MSS_GPIO_3, 0);
    //   ADF_SPI_BLOCK_WRITE(adf_spi,pSeqData,1,(pSeqData+1),(length-4));
    //   MSS_GPIO_set_output(MSS_GPIO_3, 1);
      
    //   for(i=0; i<1000; i++){

    //   }

    //   // Update the array position to point to the next block
    //   array_position += length;
    
    // }while(array_position < size); // Continue operation until full data file has been written

    uint8_t ret_val;
    uint8_t en_calib_array[] = {EN_CALIB >> 24,(EN_CALIB >> 16) & 0xFF, (EN_CALIB >> 8) & 0xFF, EN_CALIB & 0xFF};
    uint8_t en_calib_ar[] = {EN_CALIB & 0xFF, (EN_CALIB >> 8) & 0xFF, (EN_CALIB >> 16) & 0xFF, EN_CALIB >> 24 };
    uint8_t dis_calib_array[] = {DIS_CALIB >> 24,(DIS_CALIB >> 16) & 0xFF, (DIS_CALIB >> 8) & 0xFF, DIS_CALIB & 0xFF};
    uint8_t dis_calib_ar[] = {DIS_CALIB & 0xFF, (DIS_CALIB >> 8) & 0xFF, (DIS_CALIB >> 16) & 0xFF, DIS_CALIB >> 24};
    uint8_t read_reg[6] = {0x00,0x00,0x00,0x00,0x00,0x00};

    //Apply the configuration file
    ret_val = apply_file(radio_memory_configuration);
    if(ret_val) {
        return ERR_CONFIG_FILE_FAILED;
    }

//    Apply the calibration file
//    ret_val = apply_file(callibrations_config);
//    if(ret_val) {
//        return ERR_CALIB_FILE_FAILED;
//    }

    //Enable calibration
//    adf_write_to_memory(WMODE_1,SM_DATA_CALIBRATION,en_calib_ar,4);

//    Issue CMD_CONFIG_DEV command
//    return 0;
    ret_val = adf_send_cmd(CMD_CFG_DEV);

//    if(ret_val) {
//        adf_write_to_memory(WMODE_1,SM_DATA_CALIBRATION,dis_calib_ar,4);
//        return (ret_val | 0x80);
//    }
//
    while(1) {
	   ret_val = adf_get_state();
	   if(ret_val == PHY_OFF) {
		   break;
	   }
   }

//    adf_in_idle();
    //Issue CMD_PHY_ON
    ret_val = adf_send_cmd(CMD_PHY_ON);
    if(ret_val) {
//        adf_write_to_memory(WMODE_1,SM_DATA_CALIBRATION,dis_calib_ar,4);
        return (ret_val | 0xC0);
    }
    while(1) {
    	ret_val = adf_get_state();
	   if(ret_val == PHY_ON) {
		   break;
	   }
   }
    //Issue CMD_DO_CAL
//    ret_val = adf_get_state();
//    ret_val = adf_send_cmd(CMD_DO_CAL);
//    if(ret_val) {
////        adf_write_to_memory(WMODE_1,SM_DATA_CALIBRATION,dis_calib_ar,4);
//        return (ret_val | 0xE0);
//    }

    //Wait until adf returns to PHY_ON
//    while(1) {
//        ret_val = adf_get_state();
//        if(ret_val == PHY_ON) {
//            break;
//        }
//    }

    //Check for successful calibration
//    adf_read_from_memory(RMODE_1,PROFILE_RADIO_CAL_CFG1,read_reg,4);
//    if((read_reg[2] & 0x20) == 0) {
////        adf_write_to_memory(WMODE_1,SM_DATA_CALIBRATION,dis_calib_ar,4);
//        return ERR_CALIB_FAILED;
//    }

    //Disable calibration
//    adf_write_to_memory(WMODE_1,SM_DATA_CALIBRATION,dis_calib_ar,4);

    return 0;

#else

#endif
}


uint8_t adf_write_to_memory(uint8_t mode,uint32_t addr,uint8_t *data,uint32_t size) {
    //Currently implementing mode1 for testing. Need to implement other modes
    if((mode & WMODE_1) == WMODE_1){
        uint8_t cmd_data[] = {mode,(addr >> 24),((addr >> 16) & 0xFF),((addr >> 8) & 0xFF),(addr & 0xFF )};
        ADF_SPI_SLAVE_SELECT(adf_spi,ADF_SPI_SLAVE);
        ADF_SPI_BLOCK_WRITE(adf_spi,cmd_data,5,data,size);
        ADF_SPI_SLAVE_CLEAR(adf_spi, ADF_SPI_SLAVE);
    }

    return 0;
    

}

uint8_t* adf_read_from_memory(uint8_t mode,uint32_t addr,uint8_t *data,uint32_t size) {
    if((mode & RMODE_1) == RMODE_1){
        uint8_t cmd_data[] = {mode,(addr >> 24),((addr >> 16) & 0xFF),((addr >> 8) & 0xFF),(addr & 0xFF )};
        ADF_SPI_SLAVE_SELECT(adf_spi,ADF_SPI_SLAVE);
        ADF_SPI_BLOCK_READ(adf_spi,cmd_data,5,data,size+RMODE1_OFFSET);
        ADF_SPI_SLAVE_CLEAR(adf_spi, ADF_SPI_SLAVE);
    }
    return (data+RMODE1_OFFSET);
}

uint8_t adf_send_cmd(uint8_t command) {
    ADF_SPI_SLAVE_SELECT(adf_spi,ADF_SPI_SLAVE);
    uint8_t check_val = 0,nop = ADF_NOP;
    uint8_t tries = 0;
    //Send NOP command(0xFF) until adf is ready to receive command
   do {
       ADF_SPI_BLOCK_READ(adf_spi,&nop,1,&check_val,1);
       if((check_val & CMD_READY) != 0) {
           break;
       }
   }while(tries++ < 100);
   if(tries >= 100) {
       return ERR_CMD_FAILED;
   }

    //Send the command
    ADF_SPI_BLOCK_WRITE(adf_spi,&command, 1, &check_val, 1);
    //Using old drivers
    //ADF_SPI_SLAVE_SELECT(adf_spi,0);
    //Using new drivers
    ADF_SPI_SLAVE_CLEAR(adf_spi,ADF_SPI_SLAVE);

    return 0;

}

uint8_t adf_send_cmd_without_ready(uint8_t command) {
    ADF_SPI_SLAVE_SELECT(adf_spi,ADF_SPI_SLAVE);
    uint8_t check_val = 0,nop = ADF_NOP;
    uint8_t tries = 0;
    //Send NOP command(0xFF) until adf is ready to receive command
//   do {
//       ADF_SPI_BLOCK_READ(adf_spi,&nop,1,&check_val,1);
//       if((check_val & CMD_READY) != 0) {
//           break;
//       }
//   }while(tries++ < 100);
//   if(tries >= 100) {
//       return ERR_CMD_FAILED;
//   }

    //Send the command
    ADF_SPI_BLOCK_WRITE(adf_spi,&command, 1, &check_val, 1);
    //Using old drivers
    //ADF_SPI_SLAVE_SELECT(adf_spi,0);
    //Using new drivers
    ADF_SPI_SLAVE_CLEAR(adf_spi,ADF_SPI_SLAVE);

    return 0;

}

uint8_t cmd_ready_set() {
    uint8_t check_val = 0,nop = ADF_NOP;
    uint8_t tries = 0;
    //Send NOP command(0xFF) until adf is ready to receive command
    do {
        ADF_SPI_BLOCK_READ(adf_spi,&nop,1,&check_val,1);
        if((check_val & CMD_READY) != 0) {
            break;
        }
    }while(tries++ < 100);
    if(tries >= 100) {
        return check_val;
    }
    return 0;
}

uint8_t adf_in_idle() {
    uint8_t check_val = 0,nop = ADF_NOP;
    uint8_t tries = 0;
    //Send NOP command(0xFF) until adf is ready to receive command
    do {
        ADF_SPI_BLOCK_READ(adf_spi,&nop,1,&check_val,1);
        if((check_val & 0x04) != 0) {
            break;
        }
    }while(tries++ < 100);
    if(tries >= 100) {
        return check_val;
    }
    return 0;
}

void adf_spi_trans_read( spi_instance_t * this_spi,
    uint8_t * cmd_buffer,
    size_t cmd_byte_size,
    uint8_t * rd_buffer,
    size_t rd_byte_size){

	uint16_t i;
	uint8_t r_buf[6];
	r_buf[0] = 0x00;
	//Using old drivers
//	MSS_GPIO_set_output(MSS_GPIO_3, 0);
//	SPI_block_read(this_spi, cmd_buffer, cmd_byte_size, rd_buffer, rd_byte_size);
//	MSS_GPIO_set_output(MSS_GPIO_3, 1);
	//Using new drivers
//	SPI_set_slave_select(this_spi,ADF_SPI_SLAVE);


		SPI_transfer_block(this_spi,cmd_buffer,cmd_byte_size,r_buf,rd_byte_size);

//	SPI_transfer_block(this_spi,0,0,rd_buffer,rd_byte_size);
//	SPI_clear_slave_select(this_spi,ADF_SPI_SLAVE);
	for(i=0;i<1000;i++){
		if(i<rd_byte_size){
			rd_buffer[i] = r_buf[i];
		}
	}
}

void adf_spi_trans_write( spi_instance_t * this_spi,
    uint8_t * cmd_buffer,
    size_t cmd_byte_size,
    uint8_t * wr_buffer,
    size_t wr_byte_size){

	uint8_t data[300];
	uint16_t i = 0;

	for(;i<cmd_byte_size;i++){
		data[i] = cmd_buffer[i];
	}
	for(;i<cmd_byte_size + wr_byte_size;i++){
		data[i] = wr_buffer[i-cmd_byte_size];
	}
	//Using old drivers
//	MSS_GPIO_set_output(MSS_GPIO_3, 0);
//	SPI_block_write(this_spi, cmd_buffer, cmd_byte_size, wr_buffer, wr_byte_size);
//	MSS_GPIO_set_output(MSS_GPIO_3, 1);
	//Using new drivers
//	SPI_set_slave_select(this_spi,ADF_SPI_SLAVE);
	SPI_transfer_block(this_spi,data,cmd_byte_size + wr_byte_size,0,0);
//	SPI_transfer_block(this_spi,wr_buffer,wr_byte_size,0,0);
//	SPI_clear_slave_select(this_spi,ADF_SPI_SLAVE);
	for(i=0;i<1000;i++){

	}
}

uint8_t adf_get_state() {
    uint8_t misc_fw[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
    uint8_t curr_mode = 0;
    adf_read_from_memory(RMODE_1,MISC_FW,misc_fw,4);
    curr_mode = misc_fw[4] & 0x3F;
    return curr_mode;
}

void adf_config_gpio(){


	uint8_t data1[4] = {0x18, 0x19, 0x1A, 0x1B};
//	uint8_t	data2[4] = {0x1C, 0x1D, 0x1E, 0x1F};


	adf_write_to_memory(WMODE_1, GPIO_CONFIG_ADDR1, data1, sizeof(data1));


}

uint8_t adf_start_rx(){


	adf_config_gpio();

				// Enter the GPIO Component to be Enabled Or disabled
									// Power On LNA connected to GPIO_0 in older design and GPIO_1 in newer version.
	adf_send_cmd(CMD_PHY_RX);		// After ADF gets into PHY_RX it waits for Preamble to get detected.
									// Once the Preamble is detected, ADF generates interrupt



}
