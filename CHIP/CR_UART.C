/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     CR_UART.C
 *
 * Project:
 *     Firmware for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include "swtchs.h"
#include "types.h"
#include "com_defs.h"
#include "icu.h"
#include "cr_uart.h"
#include "regs.h"
#include "sib.h"
#include "purdat.h"
#include <yvals.h>
#include <stdarg.h>
#include <string.h>

#if CR_UART_SUPPORTED
BYTE RxBuff[RX_BUFF_SIZE];
BYTE TxBuff[TX_BUFF_SIZE];
BYTE Rx_Index, Tx_Index_IN, Tx_Index_OUT, *argv[5];
static WORD crsmae = 0x1000;

#define Get_Cmd ((*prefix[i] > 0x60) ? (*prefix[i]++ - 0x57) : ((*prefix[i] > 0x40) ? (*prefix[i]++ - 0x37) : (*prefix[i]++ - 0x30)))
/* Marco funcs for console commands */
#define CONSOLE_CMD_ITEM(name) \
	{_STRINGX(name), cmd_##name}

#define  CONSOLE_CMD_END() \
	{"",NULL}

/*
* Customized console commands.
*/

/* Console commands array */
const CONSOLE_CMD_DECLARED Console_CMD_Mcb[] =
{
	CONSOLE_CMD_ITEM(kx),
	CONSOLE_CMD_ITEM(dreg),
	CONSOLE_CMD_ITEM(wreg),
	CONSOLE_CMD_ITEM(help),
	CONSOLE_CMD_END(),
};
const WORD uCommandUsed = ARRAY_SIZE(Console_CMD_Mcb);

/*---------------------------------------------------------------------------------------------------------*/
/* Function: cmd_help                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*    This function show console help.                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    argc                                                                                                 */
/*    *argv[]                                                                                              */
/* Return value:                                                                                           */
/*    None                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void cmd_help(BYTE argc, BYTE *argv[])
{
	const int cols = 5; /* printing in five columns */
	const int rows = (uCommandUsed + cols - 1) / cols;
	BYTE i, j;

	for (i = 0; i < rows; i++) {
		DEBUG_PRINTF(" Supporting Commands:\n ");
		//DEBUG_PRINTF("  ");
		for (j = 0; j < cols; j++) {
			int index = j * rows + i;
			if (index >= uCommandUsed)
				break;
			DEBUG_PRINTF("%-10s", Console_CMD_Mcb[index].cmd_name);
		}
		DEBUG_PRINTF("\n");
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: CR_UART_init                                                                                     */
/*                                                                                                         */
/* Description:                                                                                            */
/*    This function init PORT module                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*    None                                                                                                 */
/*                                                                                                         */
/* Return value:                                                                                           */
/*    None                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void CR_UART_init(void)
{
	/* Configure pins from GPIOs to UART */
	DEVALT1 |= URTO1_SL;
	DEVALT1 |= URTI_SL;

	/* Disable interrupt. */
	Disable_UART_INT();

	/* Enable rx interrupt. */
	SET_BIT(UICTRL, UICTRL_ERI);

	UMDSL = 0x0;          // asynchronous mode
	UPSR  = 0x10;         // Pre-Sacle Register
	UBAUD = (0x09-1);	    // Baud Rate = 25M/(16*9*9) ~= 115200
	UFRS  = CHAR_8BIT | ONE_STOP_BIT | ODD_PARITY ;	//# 8bits,Odd parity,One stop bit

	/* Enable interrupt. */
	Enable_UART_INT();

	/* Initialize ring buffer in console task */
	Rx_Index = 0;
	Tx_Index_IN = 0;
	Tx_Index_OUT = 0;
}


void Enable_UART_INT(void)
{
    ICU_Clr_Pending_Irq(ICU_EINT_UART);
    ICU_Enable_Irq(ICU_EINT_UART);
}

void Disable_UART_INT(void)
{
    ICU_Clr_Pending_Irq(ICU_EINT_UART);
    ICU_Disable_Irq(ICU_EINT_UART);
}

int uart_get_int_type()
{
	if(IS_BIT_SET(UICTRL, UICTRL_ERI) &&
			IS_BIT_SET(UICTRL, UICTRL_RBF))
		return UART_RX_INT;
	if(IS_BIT_SET(UICTRL, UICTRL_ETI) &&
			IS_BIT_SET(UICTRL, UICTRL_TBE))
		return UART_TX_INT;
	return UART_NONE_INT;
}

void ConsoleProcessOutputFromISR(void)
{
	if (Tx_Index_OUT == Tx_Index_IN)
		uart_enable_tx_int(0);
	else {
		uart_put_char(TxBuff[Tx_Index_OUT++]);
		Tx_Index_OUT %= TX_BUFF_SIZE;
	}
}

void ConsoleCommandProcess(void)
{
    BYTE i, argc;
    WORD irq_saved;

    argc = Parsing_Console_CMD(RxBuff, Rx_Index, argv);
    if (argc != 0)
    {
        for (i = 0;; i++) {
		    if (Console_CMD_Mcb[i].fnc == NULL)
			    break;
		    if (!strcmp(argv[0], Console_CMD_Mcb[i].cmd_name)) {
			    if (Console_CMD_Mcb[i].fnc != NULL)
				    Console_CMD_Mcb[i].fnc(argc, argv);
			    break;
		    }
		}
	}
    Rx_Index = 0;
}

void ConsoleProcessInputFromISR(void)
{
	BYTE tmp;
	BYTE argc, i;
	tmp = uart_get_char();
	//echo input
	if ((tmp == 0x08) && (Rx_Index != 0)) { //BS
		uart_put_char(tmp);
		Rx_Index--;
		uart_put_char(0x20);
		uart_put_char(tmp);
	} else if ((tmp != 0x08) && (tmp != 0x0A)) {
		uart_put_char(tmp);
		if (Rx_Index < RX_BUFF_SIZE)
			RxBuff[Rx_Index++] = tmp;
		if (tmp == 0x0D) { // got new line input
			uart_put_char(0x0A);
			Service.bit.CORE_7 = 1;
            //ConsoleCommandProcess();
		}
	}


}

BYTE Parsing_Console_CMD(BYTE *ptr, BYTE len, BYTE *argv[])
{
	BYTE cnt, i;

	for (i = 0; i < len; i++) {
		if ((ptr[i] == 0x20) || (ptr[i] == 0x0D))
			ptr[i] = 0;
	}
	cnt = 0;
	for (i = 0; i < len;) {
		if (ptr[i] == 0) {
			i++;
			continue;
		} else {
			argv[cnt++] = &ptr[i];
			for (; i < len; i++) {
				if (ptr[i] == 0)
					break;
			}
		}
	}
	return (cnt);
}

void cmd_dreg(BYTE argc, BYTE *argv[])
{
    BYTE cmd = 0, data, i = 0;
    BYTE *prefix[8];
    DWORD Addr;
    for (i = 0; i < argc - 1; i++)
    {
        prefix[i] = argv[i+1];
    }
    i = 0;
    Addr = (DWORD)(((DWORD)Get_Cmd << 20) | ((DWORD)Get_Cmd << 16) | ((DWORD)Get_Cmd << 12) | ((DWORD)Get_Cmd << 8) | ((DWORD)Get_Cmd << 4) | (Get_Cmd & 0x0F));
    DEBUG_PRINTF("Register content 0x%04x\n", *(BYTE *)Addr);

}

void cmd_wreg(BYTE argc, BYTE *argv[])
{
    BYTE cmd = 0, data, i = 0;
    BYTE *prefix[8];
    DWORD Addr;
    for (i = 0; i < argc - 1; i++)
    {
        prefix[i] = argv[i+1];
    }
    for (i = 0; i < argc - 1; i++)
    {
        if ( i == 0)
        {
            Addr = (DWORD)(((DWORD)Get_Cmd << 20) | ((DWORD)Get_Cmd << 16) | ((DWORD)Get_Cmd << 12) | ((DWORD)Get_Cmd << 8) | ((DWORD)Get_Cmd << 4) | (Get_Cmd & 0x0F));
        }
        else if (i == 1)
        {
            data = ((Get_Cmd << 4) | (Get_Cmd & 0x0F));
            *(BYTE *)Addr = data;
        }
    }
}


void Write2SIB(WORD module, BYTE data, BYTE port)
{
    CRSMAE = module;
    SIBCTRL |= CSAE;
    while (SIBCTRL & CSWR) ;
    IHIOA = port & 0x0F;
    IHD = data;
    while (SIBCTRL & CSWR) ;
    SIBCTRL &= ~CSAE;
}

BYTE ReadFromSIB(WORD module, BYTE port)
{
    BYTE data;
    CRSMAE = module;
    SIBCTRL |= CSAE;
    while (SIBCTRL & CSWR) ;
    IHIOA = port & 0x0F;
    SIBCTRL |= CSRD;
    while (SIBCTRL & CSRD) ;
    data  = IHD;
    SIBCTRL &= ~CSAE;
    return data;
}


void cmd_kx(BYTE argc, BYTE *argv[])
{
    BYTE cmd = 0, data, i = 0;
    BYTE *prefix[8];

    for (i = 0; i < argc - 1; i++)
    {
        prefix[i] = argv[i+1];
    }

    if (argc == 1)
    {
        DEBUG_PRINTF("Please fill in i8042 command and/or data.\n");
        DEBUG_PRINTF("Example: i8042 c60 d47 .\n");
    }
    else if (argc > 9)
    {
        DEBUG_PRINTF("Wrong pamaters.\n");
        DEBUG_PRINTF("Example: i8042 c60 d47 .\n");
    }
    else
    {
        for (i = 0; i < argc - 1; i++)
        {
            if ((*prefix[i] == 'c') || (*prefix[i] == 'C'))
            {
                prefix[i]++;
                cmd = (BYTE)((BYTE)(Get_Cmd << 4) | (Get_Cmd & 0x0F));
                while (ReadFromSIB(0x800, 0x64) & 0x02) ;
                Write2SIB(0x800, cmd, 0x64);
                //DEBUG_PRINTF("0x%02x Wrtie to Port 64h\n", cmd);
            }
            else if ((*prefix[i] == 'd') || (*prefix[i] == 'D'))
            {
                prefix[i]++;
                data = (BYTE)((BYTE)(Get_Cmd << 4) | (Get_Cmd & 0x0F));
                while (ReadFromSIB(0x800, 0x64) & 0x02) ;
                Write2SIB(0x800, data, 0x60);
                //DEBUG_PRINTF("0x%02x Write to Port 60h\n", data);
            }

            else if ((*prefix[i] == 's') || (*prefix[i] == 'S'))
            {
                prefix[i]++;
                DEBUG_PRINTF("Port 64h status is : 0x%02x\n", ReadFromSIB(0x800, 0x64));
            }

            else if ((*prefix[i] == 'p') || (*prefix[i] == 'P'))
            {
                prefix[i]++;
                cmd = (BYTE)((BYTE)(Get_Cmd << 4) | (Get_Cmd & 0x0F));
                while (ReadFromSIB(crsmae, 0x01) & 0x02) ;
                Write2SIB(crsmae, cmd, 0x01);
                //DEBUG_PRINTF("0x%02x Wrtie to Port 66h\n", cmd);
            }
            else if ((*prefix[i] == 'q') || (*prefix[i] == 'Q'))
            {
                prefix[i]++;
                data = (BYTE)((BYTE)(Get_Cmd << 4) | (Get_Cmd & 0x0F));
                while (ReadFromSIB(crsmae, 0x01) & 0x02) ;
                Write2SIB(crsmae, data, 0x00);
                //DEBUG_PRINTF("0x%02x Write to Port 62h\n", data);
            }

            else if ((*prefix[i] == 't') || (*prefix[i] == 'T'))
            {
                prefix[i]++;
                DEBUG_PRINTF("Port 66h status is : 0x%02x\n", ReadFromSIB(crsmae, 0x01));
            }
            else if ((*prefix[i] == 'w') || (*prefix[i] == 'W'))
            {
                prefix[i]++;
                crsmae = (WORD)((WORD)(Get_Cmd << 12) | (WORD)(Get_Cmd << 8) | (WORD)(Get_Cmd << 4) | (Get_Cmd & 0x0F));
                DEBUG_PRINTF("CRSMAE is set as 0x%04x\n", crsmae);
            }

        }
    }
}

void  ConsoleCommandHandling(void)
{
    /* Polling Output Buffer Full */
    /* EC emulate Host to polling keyboard/mouse data */
    if ((ReadFromSIB(0x800, 0x64) & 0x01) != 0)
    {
        DEBUG_PRINTF("KBC: 0x%02x\n", ReadFromSIB(0x800, 0x60));
    }
    /* EC emulate Host to polling PM1 data */
    if ((ReadFromSIB(0x1000, 0x01) & 0x01) != 0)
    {
        DEBUG_PRINTF("PM1: 0x%02x\n", ReadFromSIB(0x1000, 0x00));
    }
    /* EC emulate Host to polling PM2 data */
    if ((ReadFromSIB(0x2000, 0x01) & 0x01) != 0)
    {
        DEBUG_PRINTF("PM2: 0x%02x\n", ReadFromSIB(0x2000, 0x00));
    }
    /* EC emulate Host to polling PM3 data */
    if ((ReadFromSIB(0x4000, 0x01) & 0x01) != 0)
    {
        DEBUG_PRINTF("PM3: 0x%02x\n", ReadFromSIB(0x4000, 0x00));
    }
    /* EC emulate Host to polling PM4 data */
    if ((ReadFromSIB(0x10, 0x01) & 0x01) != 0)
    {
        DEBUG_PRINTF("PM4: 0x%02x\n", ReadFromSIB(0x10, 0x00));
    }

    /* Data following command or data handling. */

}



void uart_enable_tx_int(BYTE enable)
{
	if (enable)
		SET_BIT(UICTRL, UICTRL_ETI);
	else
		CLEAR_BIT(UICTRL, UICTRL_ETI);
}

void uart_put_char(BYTE c)
{
	/* Wait for space in transmit FIFO. */
	while (!uart_tx_ready())
		;

	UTBUF = c;
}

int uart_get_char(void)
{
	return URBUF;
}

int uart_tx_ready()
{
	return UICTRL & 0x01;	/*if TX FIFO is empty return 1*/
}

int DEBUG_PRINTF(const BYTE *format, ...)
{
	va_list args;

    va_start( args, format );
    return print( 0, format, args );
}

static int print(BYTE **out, const BYTE *format, va_list args )
{
	int width, pad;
	int pc = 0;
	BYTE scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%')
			{
				printchar (out, *format);
				++pc;
			}
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				BYTE *s = (BYTE *)va_arg( args, long );
//				pc += prints (out, s? s:"(null)", width, pad);
				pc += prints (out, s, width, pad);
				continue;
			}
			if( *format == 'd' ) {
				pc += printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				/* char are converted to int then pushed on the stack */
				scr[0] = (BYTE)va_arg( args, int );
				scr[1] = '\0';
				pc += prints (out, scr, width, pad);
				continue;
			}
		}
		else {
//		out:
			printchar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end( args );
	return pc;
}

static int prints(BYTE **out, const BYTE *string, int width, int pad)
{
	int pc = 0, padchar = ' ';

	if (width > 0) {
		int len = 0;
		const BYTE *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			printchar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		printchar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		printchar (out, padchar);
		++pc;
	}

	return pc;
}

static int printi(BYTE **out, int i, int b, int sg, int width, int pad, int letbase)
{
	BYTE print_buf[PRINT_BUF_LEN];
	BYTE *s;
	int t, neg = 0, pc = 0;
	unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}
	return pc + prints (out, s, width, pad);
}

static void printchar(BYTE **str, int c)
{
	if (str)
	{
		**str = c;
		++(*str);
	}
	else
	{
		PutConsole(c);
	}
}

void PutConsole(int c)
{
	if (Tx_Index_OUT == ((Tx_Index_IN + 1) % TX_BUFF_SIZE))
		return; //TxBuffer , abort output
	else {
		TxBuff[Tx_Index_IN++] = (BYTE) c;
		Tx_Index_IN %= TX_BUFF_SIZE;
		uart_enable_tx_int(1);
	}
}

#endif // CR_UART_SUPPORTED




