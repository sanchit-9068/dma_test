/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include "xaxidma.h"
#include "xparameters.h"
#include "sleep.h"
#include "xil_cache.h"

u32 checkHalted(u32 baseAddress,u32 offset);

int main(){

	u32 a[] = {1,2,3,4,5,6,7,8};
	u32 b[8];
    u32 status;

	XAxiDma_Config *myDmaConfig;
	XAxiDma myDma;

	myDmaConfig = XAxiDma_LookupConfigBaseAddr(XPAR_AXI_DMA_0_BASEADDR);
	status = XAxiDma_CfgInitialize(&myDma, myDmaConfig);
	if(status != XST_SUCCESS){
		print("DMA initialization failed\n");
		return -1;
	}
	print("DMA initialization success..\n\r");
	status = checkHalted(XPAR_AXI_DMA_0_BASEADDR,0x4);
	xil_printf("Status before data transfer %0x\n\r",status);
	Xil_DCacheFlushRange((u32)a,8*sizeof(u32));
	status = XAxiDma_SimpleTransfer(&myDma, (u32)b, 8*sizeof(u32),XAXIDMA_DEVICE_TO_DMA);
	status = XAxiDma_SimpleTransfer(&myDma, (u32)a, 8*sizeof(u32),XAXIDMA_DMA_TO_DEVICE);//typecasting in C/C++
	if(status != XST_SUCCESS){
		print("DMA initialization failed\n\r");
		return -1;
	}
    status = checkHalted(XPAR_AXI_DMA_0_BASEADDR,0x4);
    while(status != 1){
    	status = checkHalted(XPAR_AXI_DMA_0_BASEADDR,0x4);
    }
    status = checkHalted(XPAR_AXI_DMA_0_BASEADDR,0x34);
    while(status != 1){
    	status = checkHalted(XPAR_AXI_DMA_0_BASEADDR,0x34);
    }
	print("DMA transfer success..\n\r");
	for(int i=0;i<8;i++)
		xil_printf("%0x\n",b[i]);
}


u32 checkHalted(u32 baseAddress,u32 offset){
	u32 status;
	status = (XAxiDma_ReadReg(baseAddress,offset))&XAXIDMA_HALTED_MASK;
	return status;
}
