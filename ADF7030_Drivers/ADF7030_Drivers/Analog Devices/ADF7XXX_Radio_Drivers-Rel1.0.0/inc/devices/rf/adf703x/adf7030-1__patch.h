/*!
 *****************************************************************************
  @file:	adf7030-1__patch.h 
  @brief:	...  
  @version:	$Revision: 
  @date:	$Date:
 -----------------------------------------------------------------------------
Copyright (c) 2017, Analog Devices, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted (subject to the limitations in the disclaimer below) provided that
the following conditions are met:
  - Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation 
    and/or other materials provided with the distribution.
  - Neither the name of Analog Devices, Inc. nor the names of its contributors 
    may be used to endorse or promote products derived from this software without
    specific prior written permission.

NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY 
THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT 
NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
OF THE POSSIBILITY OF SUCH DAMAGE.

*****************************************************************************/


#ifndef _ADF7030_1__PATCH_H_
#define _ADF7030_1__PATCH_H_


/** \addtogroup adf7030-1 adf7030-1 Driver
 *  @{
 */

/** \addtogroup adf7030-1__patch Patch Interface
 *  @{
 */


/** Generic Type to describes the Radio PHY Patch Information data.
*/
typedef struct {
    //char patch_name[69];
    char patch_name[73];
    uint16_t build_number;
    char family_str[4];
    uint16_t adiid;
    uint16_t chipid;
    uint8_t full_layer_num;
    uint8_t eco_num;
    uint16_t svn_version;
} PATCH_INFO_Type;

typedef struct PATCH_SECTION_Type PATCH_SECTION_Type;
typedef struct PATCH_CHECKSUM_DESCRIPTOR_Type PATCH_CHECKSUM_DESCRIPTOR_Type;
typedef PATCH_INFO_Type version_info_t; //\todo: Remove the legacy typedef declaration

struct PATCH_SECTION_Type {
    uint16_t            Size;
    uint16_t            Addr;
};

typedef union crc_config_type{
    uint16_t            u16_val;  
    struct{
      uint16_t          enable           : 1;   //0->Disabled, 1->Enabled
      uint16_t          mode             : 1;   //0->Non-Direct, 1->Direct
      uint16_t          type             : 2;   //see "patch_selfcheck.c" line 182
      uint16_t          bitdir           : 1;   //0->LSB first, 1->MSB first
      uint16_t          calculate_only   : 1;   //Only calculate CRC
      uint16_t          set_irq          : 1;   //Set IRQs once finished
      uint16_t          inject_ok        : 1;   //Automatic Inject "patch" on valid CRC
      uint16_t          register_cmd     : 1;   //0->Unregister CRC special command, 1->Register CRC special command
      uint16_t          crc_len          : 7;   //lenght of the CRC polynomial
    };
}CRC_CONFIG_Type;

struct PATCH_CHECKSUM_DESCRIPTOR_Type{
    uint8_t             Size;
    uint8_t             nbBlock;
    CRC_CONFIG_Type     crc_config;
    uint32_t            crc_poly;
    uint32_t            crc_seed;
    uint32_t            crc_output;
    uint32_t            GoldenCRC;
    PATCH_SECTION_Type  Sections[];             
};

struct PATCH_DESCRIPTOR_Type{
    char		                Name[16];	///String with name of Patch
    uint16_t 	                        Size;		///Total size of the Patch
    uint16_t                            nbBlock;        ///Number of data blocks
    uint16_t                            InfoBlockId;    ///Index of Patch info structure in the PHY
    uint16_t                            CheckBlockId;   ///Index of of the SelfCheck section in the PHY
    uint16_t                            CheckSeq;       ///Selfchecking patch sequence
    uint16_t                            InjectSeq;      ///Enable patch sequence
    uint16_t                            EjectSeq;       ///Disable patch sequence
    PATCH_CHECKSUM_DESCRIPTOR_Type *    pChecksum;      ///Pointer to patch checksum configuration
    DATA_BLOCK_DESCRIPTOR_Type          BLOCKS[];
};
  
/* Write patch data to Radio PHY */
ADI_ADF7030_1_RESULT adf7030_1__WritePatch(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,                            
    PATCH_DESCRIPTOR_Type *     pPATCH                                
);

/* Read patch data from Radio PHY */
ADI_ADF7030_1_RESULT adf7030_1__ReadPatch(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,                           
    PATCH_DESCRIPTOR_Type *     pPATCH                                
);

/* Verify patch integrity */
ADI_ADF7030_1_RESULT adf7030_1__VerifyPatch(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,                             
    PATCH_DESCRIPTOR_Type *     pPATCH                                
);
    
/* Selfchecking patch integrity directly on the PHY Radio */
ADI_ADF7030_1_RESULT adf7030_1__SelfCheckPatch(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,                              
    PATCH_DESCRIPTOR_Type *             pPATCH,
    bool                                bCHECK,
    ADI_ADF7030_1_INTPIN                eIntPin,
    ADI_ADF7030_1_RADIO_STATE           eCmd,
    PATCH_CHECKSUM_DESCRIPTOR_Type *    pCHECKSUM_CFG
);

/* Setup Patch sequence */
ADI_ADF7030_1_RESULT adf7030_1__SetupPatch(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,                               
    uint16_t                    SetupSeq,
    bool                        bCHECK                                
);

/* Write a single Patch block to the PHY */
ADI_ADF7030_1_RESULT adf7030_1__WritePatchBlock(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,                               
    DATA_BLOCK_DESCRIPTOR_Type *        pBlock
);

/* Read a single Patch block to the PHY */
ADI_ADF7030_1_RESULT adf7030_1__ReadPatchBlock(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,                                
    DATA_BLOCK_DESCRIPTOR_Type *        pBlock
);                             

/* Verify content of a single Patch block in the PHY */
ADI_ADF7030_1_RESULT adf7030_1__VerifyPatchBlock(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    DATA_BLOCK_DESCRIPTOR_Type *        pBlock
);

/* Transfers Patch between Host shadow and the radio PHY */
ADI_ADF7030_1_RESULT adf7030_1__XferPatch(
    ADI_ADF7030_1_SPI_INFO *    pSPIDevInfo,
    PATCH_DESCRIPTOR_Type *     pPATCH,
    bool                        bREAD_nWRITE,
    bool                        bCHECK_ONLY
);

/* Transfers Patch subblock between Host and the Radio PHY */
ADI_ADF7030_1_RESULT adf7030_1__XferPatchBlock(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo, 
    DATA_BLOCK_DESCRIPTOR_Type *        pBLOCK,
    bool                                bREAD_nWRITE,
    bool                                bCHECK_ONLY
);

/* Transfers Patch checksum configuration */
ADI_ADF7030_1_RESULT adf7030_1__XferChecksum(
    ADI_ADF7030_1_SPI_INFO *            pSPIDevInfo,
    PATCH_DESCRIPTOR_Type *             pPATCH,
    PATCH_CHECKSUM_DESCRIPTOR_Type *    pPATCH_CHECKSUM,
    bool                                bREAD_nWRITE,
    bool                                bCHECK_ONLY
);

#define adf7030_1__WritePatchCheckumInfo( pSPIDevInfo, pPATCH, pPATCH_CHECKSUM) adf7030_1__XferChecksum( pSPIDevInfo, \
                                                                                                         (PATCH_DESCRIPTOR_Type *)pPATCH, \
                                                                                                         (PATCH_CHECKSUM_DESCRIPTOR_Type *)pPATCH_CHECKSUM, \
                                                                                                         (bool)PHY_MEM_WRITE, \
                                                                                                         false )
#define adf7030_1__ReadPatchCheckumInfo( pSPIDevInfo, pPATCH, pPATCH_CHECKSUM) adf7030_1__XferChecksum( pSPIDevInfo, \
                                                                                                         (PATCH_DESCRIPTOR_Type *)pPATCH, \
                                                                                                         (PATCH_CHECKSUM_DESCRIPTOR_Type *)pPATCH_CHECKSUM, \
                                                                                                         (bool)PHY_MEM_READ, \
                                                                                                         false )
#define adf7030_1__VerifyPatchCheckumInfo( pSPIDevInfo, pPATCH, pPATCH_CHECKSUM) adf7030_1__XferChecksum( pSPIDevInfo,  \
                                                                                                         (PATCH_DESCRIPTOR_Type *)pPATCH, \
                                                                                                         (PATCH_CHECKSUM_DESCRIPTOR_Type *)pPATCH_CHECKSUM, \
                                                                                                         (bool)PHY_MEM_READ, \
                                                                                                         true )
/** @} */ /* End of group adf7030-1__patch Patch Interface */
/** @} */ /* End of group adf7030-1 adf7030-1 Driver */

#endif /* _ADF7030_1__PATCH_H_ */
