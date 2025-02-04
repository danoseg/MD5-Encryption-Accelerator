/* COE838 - System-on-Chip
 * Lab 4 - Custom IP for HPS/FPGA Systems
 * main.c
 *	
 *  Created on: 2014-11-15
 *  Author: Anita Tino
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"

#define LW_SIZE 0x00200000
#define LWHPS2FPGA_BASE 0xff200000

volatile uint32_t *md5_control = NULL;
volatile uint32_t *md5_data = NULL;

void reset_system(){
	int reset_signal;

	alt_write_word(md5_control+2, 0x1); //assert reset
	reset_signal = alt_read_word(md5_control+2);
	printf("\nReset signal: 0x%08x ", reset_signal);
	//while(!(alt_read_word(md5_control+2) & 0x1));
	
	alt_write_word(md5_control+2, 0x0); //deassert reset
	reset_signal = alt_read_word(md5_control+2);
	printf("\nReset signal: 0x%08x", reset_signal);
	/*printf("Reset done. Deasserting signal\n");
	while((alt_read_word(md5_control+2) & 0x1));//deassert*/
}

void write_data(){
int i;
int inputData[16] = {0x1680208, 0x13ab80bb, 0xcb8b2c30, 0xb9657582, 0xa3793c48, 0x103f26be, 0x0b78dac4, 0x5c433348,
					 0x4de99287, 0xeff0be7c, 0x00808533, 0x00000000, 0x00000000, 0x00000000, 0x00000150,
					 0x00000000};
int write_signal;
int input_test;
int inputaddress_test;

	//alt_write_word(md5_data, 0x10); //m_write
	alt_write_word(md5_control, 0x1); //write enable signal
	write_signal = alt_read_word(md5_control);
	printf("\nWrite signal: 0x%08x", write_signal);

	/*while(!(alt_read_word(md5_control) & 0x1));
	printf("Write signal on.\n");*/

	for(i = 0; i <= 15 ; i++){  //write address
	//alt_write_word(md5_data+3,i);	// activate engine signal
	alt_write_word(md5_data, i);
	alt_write_word(md5_data+1, inputData[i]); //write data into address
	input_test = alt_read_word(md5_data+1);
	inputaddress_test = alt_read_word(md5_data);
	printf("\nInput Data is: 0x%08x " ,input_test);
	printf("\nInput Address is: 0x%08x ", inputaddress_test);
	}

	//alt_write_word(md5_data, 0x00); //m_write
	alt_write_word(md5_control, 0x0); //write enable signal
	write_signal = alt_read_word(md5_control);
	printf("\nWrite signal: 0x%08x", write_signal);
/*	while((alt_read_word(md5_control) & 0x1));//deassert
	printf("Write signal disabled.\n");*/

}
void md5_conversion(){
int start_signal;

	alt_write_word(md5_control+1, 0x1); //start signal
	start_signal = alt_read_word(md5_control+1);
	printf("\nStart signal: 0x%08x", start_signal);

	/*while(!(alt_read_word(md5_control+1) & 0x1)); //double check that start was asserted
	printf("Start signal activated \n");*/


	alt_write_word(md5_control+1, 0x0); //start conversion signal
	start_signal = alt_read_word(md5_control+1);
	printf("\nStart signal: 0x%08x", start_signal);

	/*while((alt_read_word(md5_control+1) & 0x1));//deassert
	printf("Start signal cleared\n");*/

}

void retrieve_digest(){
	uint32_t digest0, digest1, digest2, digest3;
	int done_signal;
	int write_signal, start_signal, reset_signal;
	int input_test;
	int inputaddress_test;

	/*reset_signal = alt_read_word(md5_control+2);
	printf("\nReset signal: 0x%08x ", reset_signal);
	write_signal = alt_read_word(md5_control);
	printf("\nWrite signal: 0x%08x", write_signal);
	start_signal = alt_read_word(md5_control+1);
	printf("\nStart signal: 0x%08x", start_signal);
	input_test = alt_read_word(md5_data+1);
	inputaddress_test = alt_read_word(md5_data);
	printf("\nInput Data is: 0x%08x " ,input_test);
	printf("\nInput Address is: 0x%08x ", inputaddress_test);*/

	done_signal = alt_read_word(md5_control+3);
	printf("\nDone signal: 0x%08x",done_signal);

	printf("\nWaiting for done\n");
	while((alt_read_word(md5_control+3) & 0x0)); // assert done signal
	//printf("Done signal activated\n");
	done_signal = alt_read_word(md5_control+3);
	printf("\nDone signal: 0x%08x",done_signal);

	alt_write_word(md5_data+2,0);
	digest0 = alt_read_word(md5_data+2);

	alt_write_word(md5_data+2,1);
	digest1 = alt_read_word(md5_data+2);

	alt_write_word(md5_data+2,2);
	digest2 = alt_read_word(md5_data+2);

	alt_write_word(md5_data+2,3);
	digest3 = alt_read_word(md5_data+2);

	printf("\nDigest 3: 0x%08x",digest0);
	printf(" Digest 2: 0x%08x", digest1);
	printf(" Digest 1: 0x%08x", digest2);
	printf(" Digest 0: 0x%08x", digest3);
	printf("\nDigest: 0x%08x,0x%08x,0x%08x,0x%08x", digest3, digest2, digest1, digest0);
	printf("\nDigest is complete.\n");
}

int main(int argc, char **argv){
	int fd;
	void *virtual_base; 
	
	//map address space of fpga for software to access here
	if((fd = open("/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}

	virtual_base =  mmap( NULL, LW_SIZE, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, LWHPS2FPGA_BASE);

	if( virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return(1);
	}

	//initialize the addresses
	md5_control = virtual_base + ((uint32_t)( MD5_CONTROL_0_BASE) );
	md5_data = virtual_base + ((uint32_t)(MD5_DATA_0_BASE));
	
	printf("------>Finished initializing HPS/FPGA system<-------\n");

	reset_system();
	write_data();
	md5_conversion();
	retrieve_digest();


	// clean up our memory mapping and exit
	if( munmap( virtual_base, LW_SIZE) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	}

	close( fd );

	
	return 0;

}
