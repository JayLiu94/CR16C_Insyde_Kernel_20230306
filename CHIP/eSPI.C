/*----------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                              */
/* Copyright (c) 2016 by Nuvoton Technology Corporation                                                     */
/* All rights reserved                                                                                      */
/*----------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                           */
/*       eSPI.c - Enhanced Serial Peripheral Interface (eSPI)                                               */
/*                                                                                                          */
/* Project: Firmware for Nuvoton Notebook Embedded Controller Peripherals                                   */
/*----------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

#define ESPI_C

#include "swtchs.h"
#include "types.h"
#include "host_if.h"
#include "purdat.h"
#include "ps2.h"
#include "espi.h"
#include "icu.h"

#if ESPI_SUPPORTED
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           TYPES & DEFINITIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

const ESPI_PARAMS OOB_Params[] =
{
    {0x04002107, 0x0F010102},
    {0x04002107, 0x0F010202}
};


#define OOB_Params_Size (sizeof(OOB_Params) / sizeof(ESPI_PARAMS))

DWORD	OOB_Wdat_buf[20];

typedef enum {
	MASTER_TO_SLAVE = 0,
	SLAVE_TO_MASTER = 1,
}VW_MODE;

typedef struct {
	VW_MODE mode;
	BYTE    index;
}VW_MAP;

WORD MAFSRdCount;

const VW_MAP eSPI_VW_map_table[] =
{
	SLAVE_TO_MASTER, 0,	/* 4h: VWEVSM0*/
	SLAVE_TO_MASTER, 1,	/* 5h: VWEVSM1*/
	SLAVE_TO_MASTER, 2,	/* 6h: VWEVSM2*/
	SLAVE_TO_MASTER, 3,	/*40h: VWEVSM3*/
	SLAVE_TO_MASTER, 4,	/*45h: VWEVSM4*/
	SLAVE_TO_MASTER, 5,	/*46h: VWEVSM5*/
	MASTER_TO_SLAVE, 0,	/* 2h: VWEVMS0*/
	MASTER_TO_SLAVE, 1,	/* 3h: VWEVMS1*/
	MASTER_TO_SLAVE, 2,	/* 7h: VWEVMS2*/
	MASTER_TO_SLAVE, 3,	/*41h: VWEVMS3*/
	MASTER_TO_SLAVE, 4,	/*42h: VWEVMS4*/
	MASTER_TO_SLAVE, 5,	/*43h: VWEVMS5*/
	MASTER_TO_SLAVE, 6,	/*44h: VWEVMS6*/
	MASTER_TO_SLAVE, 7	/*47h: VWEVMS7*/
};

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
DWORD eSPI_VM_Read_VWEV_Reg(VW_MODE mode, BYTE idx)
{
    DWORD rval;
	if(mode == MASTER_TO_SLAVE)
	{
	    rval = VWEVMS(idx);
	    VWEVMS(idx) |= MODIFIED;
		//return ((VWEVMS(idx) | MODIFIED));
		return (rval);
	}
	else
		return (VWEVSM(idx));
}

void eSPI_FLASH_Send(ESPI_FLASH_T type, DWORD addr, WORD leng, DWORD *p_dat)
{
    unsigned int i;
	DWORD	flash_wdat, flash_hdat;
	FLASHCTL &= (DWORD) ~AMTEN;
	FLASHCTL &= (DWORD) ~STRPHDR;
	if((FLASHCTL & FLASH_TX_AVAIL) == 0)
	{
	    MAFSRdCount = leng;
        flash_hdat = ((DWORD) leng << 24) | ((DWORD) type << 8) | 0x07;
        if (type == FLASH_WRITE)
        {
            flash_hdat += leng;
            for (i = 0; i < leng/4; i++)
	        {
			    flash_wdat=(DWORD)(*(p_dat+i));
			    FLASHTXBUF((i + 2)) = flash_wdat;
		    }
        }
        FLASHTXBUF(0) = flash_hdat;
        FLASHTXBUF(1) = ((addr & 0xFF) << 24) + ((addr & 0xFF00) << 8)
                         + ((addr & 0xFF0000) >> 8) + ((addr & 0xFF000000) >> 24);
		/*Enqueue the packet for transmission*/
        FLASHCTL |= FLASH_TX_AVAIL;
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: eSPI_FLASH_Get                                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/*                                                                                                         */
/* Returns:     DWORD pointer (OOB_Wdat_buf)                                                               */
/*              OOB_Wdat_buf[0] --> Completion code bit31 ~ 8 valid                                        */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
DWORD*	eSPI_FLASH_Get(void)
{
	BYTE i;
	BYTE rdCount = 1;
	DWORD   Completed = FLASHRXBUF(0);
	//idx = ((OOBRXBUF(0) >> 24) & 0xFF)/4 + 1 + ((((OOBRXBUF(0) >> 24) & 0xFF)%4) ? 1 : 0);
    if ((Completed & 0x0F00) == 0x0F00)
    {
        rdCount = (BYTE) (Completed >> 24) + 1;
    }
    for (i = 0; i < rdCount; i++)
    {
    	OOB_Wdat_buf[i]= FLASHRXBUF(i);
    }
    return (OOB_Wdat_buf);
}

void eSPI_OOB_Send(BYTE idx, DWORD *p_dat)
{
	BYTE	cnt;
	DWORD	oob_wdat;
	cnt=0;
	if((OOBCTL & OOB_AVAIL) == 0)
	{
		while(cnt<idx)
		{
			oob_wdat=(DWORD)(*(p_dat+cnt));
			//REG_WRITE(OOBTXBUF(cnt), oob_wdat);
			OOBTXBUF(cnt) = oob_wdat;
			cnt++;
		}
		/*Enqueue the packet for transmission*/
		OOBCTL |= OOB_AVAIL;
	}
}

DWORD*	eSPI_OOB_Get(BYTE idx)
{
	BYTE	cnt;
	DWORD	oob_rdat;
	cnt=0;
	//idx = (((OOBRXBUF(0) >> 24) & 0xFF) + 4)/4 + (((((OOBRXBUF(0) >> 24) & 0xFF) + 3)%4) ? 1 : 0);
	idx = ((OOBRXBUF(0) >> 24) & 0xFF)/4 + 1 + ((((OOBRXBUF(0) >> 24) & 0xFF)%4) ? 1 : 0);
    while(cnt<  idx)
    {
    	OOB_Wdat_buf[cnt]= OOBRXBUF(cnt);
    	cnt++;
    }
    OOBCTL |= OOB_FREE;
    return (OOB_Wdat_buf);
}

void eSPI_VM_Write_VWEV_Reg(VW_MODE mode, BYTE idx, DWORD data)
{
	if(mode == MASTER_TO_SLAVE)
		VWEVMS(idx) = data;
	else
        VWEVSM(idx) = data;
}

void eSPI_VM_Write(BYTE eSPI_Idx, BYTE data)
{
	BYTE wire_mask,valid_mask;
	BYTE vw_wr_val;
	DWORD wdata;
	VW_MAP map;

	//vw write value
	vw_wr_val = (data & 0x80) >> 7;

	//eSPI index and wire mask
	wire_mask  = data & 0x0F;
	valid_mask = wire_mask;

	//Get VW mapping
	map = eSPI_VW_map_table[eSPI_Idx & 0x0F];

	//Send VW events
	wdata = eSPI_VM_Read_VWEV_Reg(map.mode, map.index);
	if(vw_wr_val)
	{
		//wdata = (wdata & 0xFFFFFF00L) | (valid_mask<<4) | wire_mask;
        wdata = (wdata | (valid_mask<<4)) | wire_mask;

	}
	else
	{
		//wdata = (wdata & 0xFFFFFF00L) | (valid_mask<<4) & (~wire_mask);
		wdata = (wdata | (valid_mask<<4)) & (~wire_mask);
	}
	eSPI_VM_Write_VWEV_Reg(map.mode, map.index, wdata);
}

DWORD eSPI_VM_Read(BYTE index)
{
	VW_MAP map;

	//Get VW mapping
	map = eSPI_VW_map_table[index & 0x0F];

	//Read VW events and compare it
	return(eSPI_VM_Read_VWEV_Reg(map.mode, map.index));
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:                                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/*                                                                                                         */
/* Returns:         None.                                                                                  */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void eSPI_Configuration(ESPI_DIR_T eSPI_ch, FLAG en_flag)
{

	if(en_flag)
	{
		switch(eSPI_ch)
		{
			// Enable Flash/OOB/VW/Peripheral channel
		    // This bit is reflected in Flash/OOB/VW/Peripheral channel
			// Reflects the Channel Enable bit of Channel n Capabilities and Configurations register.
			case ESPI_CHANNEL_PERIPHERAL:
				ESPICFG |= (PCHANEN | PCCHN_SUPP);
				break;

			case ESPI_CHANNEL_VW:
				ESPICFG |= (VWCHANEN | VWCHN_SUPP);
				break;

			case ESPI_CHANNEL_OOB:
				ESPICFG |= (DWORD)(OOBCHANEN | OOBCHN_SUPP);
				break;

			case ESPI_CHANNEL_FLASH:
				ESPICFG |= (DWORD)(FLASHCHANEN | FLASHCHN_SUPP);
				break;
		}

	}
	else
	{
		switch(eSPI_ch)
		{
			case ESPI_CHANNEL_PERIPHERAL:
                ESPICFG &= (DWORD)~(PCHANEN | PCCHN_SUPP);
				break;

			case ESPI_CHANNEL_VW:
				ESPICFG &= (DWORD)~(VWCHANEN | VWCHN_SUPP);
				break;

			case ESPI_CHANNEL_OOB:
				ESPICFG &= (DWORD)~(OOBCHANEN | OOBCHN_SUPP);
				break;

			case ESPI_CHANNEL_FLASH:
				ESPICFG &= (DWORD)~(FLASHCHANEN | FLASHCHN_SUPP);
				break;
		}
	}
}

/* ---------------------------------------------------------------
   Name: eSPI_Init

   Initialize eSPI registers.
   Operation Frequency are 20MHz, 25MHz, 33MHz, 50MHz or 66MHz.
   I/O mode are Single I/O, Dual I/O or Quad I/O

   --------------------------------------------------------------- */
void eSPI_Init(void)
{
    ESPISTS = ESPISTS;
    /* Disable EN_DPWROK_RST VW0 ~ 3 for SLP_S3, SLP_S4 and SLP_S5. */
    VWEVMS(0) &= ~0x380000;
    /* Enable Interrupt when Index 7 is written. */
    VWEVMS(1) |= (1l << 18);
    VWEVMS(2) |= (1l << 18);
    VWEVMS(3) |= (1l << 18);
    // Set freq as 33MHz and I/O mode as single I/O.
	ESPICFG = (ESPICFG & ~(mskMAXFREQ | mskIOMODE)) | MAXFREQ_33M | ALL_IO_SUPPORT;

	ESPIIE = IBRSTIE | BERRIE | CFGUPDIE | VWUPDIE | ESPIRSTIE | ESPIRSTIE2 | PLTRSTIE | PLTRSTIE2 | OOBRXIE | FLASHRXIE;
	//ESPISTS = ESPISTS;
    ICU_Enable_Irq(ICU_EINT_ESPI);


}
void Microsecond_Delay(WORD delay);


void eSPI_IntHandler(void)
{
    DWORD espi_sts;
    DWORD VwRegTemp = 0;

    if ((ESPISTS & ESPIIE) != 0)
    {
        espi_sts = ESPISTS;

        if ((espi_sts & PLTRST_DEASSERT) != 0)
        {   /* PLTRST dessert. */
            ESPISTS = PLTRST_DEASSERT;
            eSPI_Configuration(ESPI_CHANNEL_PERIPHERAL,1);
            eSPI_Configuration(ESPI_CHANNEL_OOB, 1);
            eSPI_Configuration(ESPI_CHANNEL_FLASH, 1);

        }

        else if ((espi_sts & PLTRST) != 0)
        {   /* PLTRST activate. */
            ESPISTS = PLTRST;
        }

        if ((espi_sts & ESPIRST_DEASSERT) != 0)
        {   /* eSPIRST dessert. */
            ESPISTS = ESPIRST_DEASSERT;
            ESPICFG |= PCCHN_SUPP | VWCHN_SUPP | OOBCHN_SUPP | FLASHCHN_SUPP;
            eSPI_VM_Write(VW_IDX_6, 0x87);
        }
        else if ((espi_sts & ESPIRST) != 0)
        {   /* eSPI is activate */
            ESPISTS = ESPIRST;
        }
        if ((espi_sts & BERR) != 0)
        {
            ESPIERR = ESPIERR;
            ESPISTS = BERR;
        }

        if ((espi_sts & VWUPDIE) != 0)
        {   /* VIRTUAL WIRE updated interrupt triggered. */
            ESPISTS = VWUPDIE;
            /* Check HOST_RST_WARN. */
            VwRegTemp = eSPI_VM_Read(VW_IDX_7);
            if ((VwRegTemp & MODIFIED) == MODIFIED)
            {   /* VIRTUAL WIRE updated interrupt triggered. */
                if ((VwRegTemp & 0x01l) == 0x01l)
                {   /* HOST_RST_WARN asserted and response HOST_RST_ACK. */
                    eSPI_VM_Write(VW_IDX_6, 0x88l);
                }
                else
                {   /* HOST_RST_WARN deasserted and release HOST_RST_ACK. */
                    eSPI_VM_Write(VW_IDX_6, 0x08l);
                }
            }
            /* Check SUS_WARM. */
            VwRegTemp = eSPI_VM_Read(VW_IDX_41);
            if ((VwRegTemp & MODIFIED) == MODIFIED)
            {   /* eSPI Index 4 was updated. */
                if ((VwRegTemp & 0x01l) == 0x01l)
                {   /* SUS_WARN is asserted and response SUS_ACK. */
                    eSPI_VM_Write(VW_IDX_40, 0x81l);
                }
                else
                {   /* SUS_WARN is deasserted and release SUS_ACK. */
                    eSPI_VM_Write(VW_IDX_40, 0x01l);
                }
            }
            /* Check OOB_RST_WARM. */
            VwRegTemp = eSPI_VM_Read(VW_IDX_3);
            if ((VwRegTemp & MODIFIED) == MODIFIED)
            {   /* Virtual Wire Index 3 was updated. */
                if ((VwRegTemp & 0x04l) == 0x04l)
                {   /* HOST_RST_WARN asserted and response OOB_RST_ACK. */
                    eSPI_VM_Write(VW_IDX_4, 0x81l);
                }
                else
                {   /* HOST_RST_WARN deasserted and release OOB_RST_ACK. */
                    eSPI_VM_Write(VW_IDX_4, 0x01l);
                }
            }
        }

        if ((espi_sts & CFGUPD) != 0)
        {   /* Configuration and capability register had been updated. */
            ESPISTS = CFGUPD;
            if (ESPICFG & HVWCHANEN)
            {   /* Host updates VIRTUAL WIRE channel is enabled, slave set READY bit. */
                eSPI_Configuration(ESPI_CHANNEL_VW, 1);
                eSPI_VM_Write(VW_IDX_5, 0x89);
            }
            if (ESPICFG & HPCHANEN)
            {   /* Host updates PERIPHERAL channel is enabled, slave set READY bit. */
                eSPI_Configuration(ESPI_CHANNEL_PERIPHERAL,1);
            }
            if (ESPICFG & HOOBCHANEN)
            {   /* Host updates OOB channel is enabled, slave set READY bit. */
                eSPI_Configuration(ESPI_CHANNEL_OOB, 1);
            }
            if (ESPICFG & HFLASHCHANEN)
            {   /* Host updates FLASH channel is enabled, slave set READY bit. */
                eSPI_Configuration(ESPI_CHANNEL_FLASH, 1);
            }
        }
        if ((espi_sts & IBRST) != 0)
        {
            ESPISTS = IBRST;
        }
        if ((espi_sts & OOBRXIE) != 0)
        {   /* OOB channel receive interrupt triggered. */
		    ESPISTS |= OOBRXIE;
            eSPI_OOB_Rcvd();
        }
        if ((espi_sts & FLASHRXIE) != 0)
        {   /* FLASH channel receive interrupt triggered. */
		    ESPISTS |= FLASHRXIE;
            eSPI_FLASH_Rcvd();
        }
    }
}

BYTE OOB_INDX;
void eSPI_OOB_Test(void)
{
    DWORD OOB_Temp[2];
    //if (ESPISTS & ESPIRST_DEASSERT)
    {
        if (OOB_INDX < OOB_Params_Size)
        {
            eSPI_OOB_Send(2, (DWORD *)&OOB_Params[OOB_INDX]);
            OOB_INDX++;
        }
        else
        {
            OOB_INDX = 0;
        }
    }
}

DWORD RTC_Data[4];
DWORD PCH_Temp[3];
void eSPI_OOB_Rcvd(void)
{
    int i;
    DWORD* OOB_Pntr;
    OOB_Pntr = eSPI_OOB_Get(2);
    if (((OOB_Pntr[1] >> 8) & 0xFF) == 0x02)
    {
        for (i = 0; i < 4; i++)
        {
            RTC_Data[i] = OOB_Pntr[i];
        }
    }
    else if (((OOB_Pntr[1] >> 8) & 0xFF) == 0x01)
    {
        for (i = 0; i < 3; i++)
        {
            PCH_Temp[i] = OOB_Pntr[i];
        }

    }

}

DWORD FLASH_Data[16];
void eSPI_FLASH_Test(ESPI_FLASH_T type, DWORD addr, WORD leng)
{
    if (addr)
    {
        eSPI_FLASH_Send(type, addr, leng, FLASH_Data);
    }
}



void eSPI_FLASH_Rcvd(void)
{

    BYTE i;
    BYTE rdCount = 1;

    DWORD* FLASH_Pntr;
    FLASH_Pntr = eSPI_FLASH_Get();
    if ((FLASH_Pntr[0] & 0x0F00) == 0x0F00)
    {
        rdCount = (BYTE) (FLASH_Pntr[0] >> 24) + 1;
    }
    for (i = 0; i < rdCount; i++)
    {
        FLASH_Data[i] = FLASH_Pntr[i];
    }
}

#endif // ESPI_SUPPORTED

