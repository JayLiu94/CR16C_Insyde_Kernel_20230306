/*----------------------------------------------------------------------------------------------------------*/
/* Copyright (c) 2006-2010 by Nuvoton Technology Corporation                                                */
/* All rights reserved.                                                                                     */
/*<<<-------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                           */
/*     CR_UART.h - CORE UNIVERSAL ASYNCHRONOUS RECEIVER-TRANSMITTER (CR_UART) driver                                                               */
/*                                                                                                          */
/* Project:                                                                                                 */
/*     Driver Set for uRider Notebook Embedded Controller Peripherals                                     */
/*------------------------------------------------------------------------------------------------------->>>*/

#ifndef CR_UART_H
#define CR_UART_H

#include <yvals.h>

/* Uart module initialization */
void CR_UART_init(void);
void Enable_UART_INT(void);
void Disable_UART_INT(void);
void ConsoleProcessOutputFromISR(void);
void ConsoleProcessInputFromISR(void);
void ConsoleCommandHandling(void);
BYTE Parsing_Console_CMD(BYTE *ptr, BYTE len, BYTE *argv[]);
void cmd_kx(BYTE argc, BYTE *argv[]);
void cmd_help(BYTE argc, BYTE *argv[]);
void cmd_dreg(BYTE argc, BYTE *argv[]);
void cmd_wreg(BYTE argc, BYTE *argv[]);
/* Enable/Disable tx interrupt of  UART module */
void uart_enable_tx_int(BYTE enable);
/* Put one character to UART module */
void uart_put_char(BYTE c);
/* Get one character from UART module */
int uart_get_char(void);
int uart_get_int_type();
int uart_tx_ready();
int DEBUG_PRINTF(const BYTE *format, ...);
static int print(BYTE **out, const BYTE *format, va_list args );
static int prints(BYTE **out, const BYTE *string, int width, int pad);
static int printi(BYTE **out, int i, int b, int sg, int width, int pad, int letbase);
static void printchar(BYTE **str, int c);
void PutConsole(int c);

typedef struct console_cmd_s{
	BYTE	cmd_name[10];
	void 	(*fnc)(BYTE argc, BYTE *argv[]);
}CONSOLE_CMD_DECLARED;

typedef enum UART_INT_
{
	UART_TX_INT,
	UART_RX_INT,
	UART_NONE_INT
} UART_INT;

typedef struct _REG_DECARED{
	WORD	RegOffset;
	BYTE 	RegType;
}REG_DECALRED;

typedef struct _REGACC{
	DWORD	        RegBase;
	REG_DECALRED     *RegTbl;
}REGACC;


//frame format
#define CHAR_8BIT           0x00
#define CHAR_7BIT           0x01
#define CHAR_9BIT           0x02
#define CHAR_9BIT_LOOPBACK  0x02
#define ONE_STOP_BIT        0x00
#define TWO_STOP_BIT        0x04
#define BIT9_0              0x00
#define BIT9_1              0x08
#define ODD_PARITY          0x00
#define EVEN_PARITY         0x10
#define PARITY_ENABLE       0x40
#define PARITY_DISABLE      0x00

/* UART receive buffer size in bytes. */
#define RX_BUFF_SIZE    64
/* UART transmit buffer size in bytes. */
#define TX_BUFF_SIZE    256

#define PAD_RIGHT 1
#define PAD_ZERO 2

#define PRINT_BUF_LEN 12

#endif // CR_UART_H

