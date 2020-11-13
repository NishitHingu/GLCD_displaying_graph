#include <reg51.h>															/* Include reg51 header file */
#include <intrins.h>		
#include <stdlib.h>															/* Include intrinsic header file */		
#define Data_Port P3														/* Define data port for GLCD */


sbit RS = P2^0;																	/* Set control bits pins */
sbit RW = P2^1;
sbit E = P2^2;
sbit CS1 = P2^3;
sbit CS2 = P2^4;
sbit RST = P2^5;


void delay(k)																		/* Delay of msec with xtal = 11.0592MHz */
{
	int i,j;
	for (i=0;i<k;i++)
		for (j=0;j<112;j++);
}

void GLCD_Command(char Command)									/* GLCD command function */
{
	Data_Port = Command;													/* Copy command on data pin */
	RS = 0;																				/* Make RS LOW to select command register */
	RW = 0;																				/* Make RW LOW to select write operation */
	E = 1;																				/* Make HIGH to LOW transition on Enable pin */
	_nop_ ();
	E = 0;														
	_nop_ ();
}

void GLCD_Data(char Data)												/* GLCD data function */
{
	Data_Port = Data;															/* Copy data on data pin */
	RS = 1;																				/* Make RS HIGH to select data register */
	RW = 0;																				/* Make RW LOW to select write operation */
	E = 1;																				/* Make HIGH to LOW transition on Enable pin */
	_nop_ ();
	E = 0;
	_nop_ ();
}

void GLCD_Init()																/* GLCD initialize function */
{
	CS1 = 0; CS2 = 0;															/* Select both left & right half of display */
	RST = 1;																			/* Keep reset pin high */
	delay(20);
	GLCD_Command(0x3E);														/* Display OFF */
	GLCD_Command(0x40);														/* Set Y address (column=0) */
	GLCD_Command(0xB8);														/* Set x address (page=0) */
	GLCD_Command(0xC0);														/* Set z address (start line=0) */
	GLCD_Command(0x3F);														/* Display ON */
}

void GLCD_ClearAll()														/* GLCD all display clear function */
{
	int i,j;
	CS1 = 0; CS2 = 0;															/* Select both left & right half of display */
	for(i=0;i<8;i++)
	{
		GLCD_Command((0xB8)+i);											/* Increment page each time after 64 column */
		for(j=0;j<64;j++)
		{
			GLCD_Data(0);															/* Write zeros to all 64 column */
		}
	}
	GLCD_Command(0x40);														/* Set Y address (column=0) */
	GLCD_Command(0xB8);														/* Set x address (page=0) */
}


void GLCD_Display(int val[])										/* GLCD string write function */
{
	int column,page,page_add=0xB8;
	float page_inc=0;
	CS1 = 0; CS2 = 1;															/* Select first left half of display to start print */
	for(page=0;page<8;page++)									
	{
		if (page%2 == 0)
		{
			GLCD_Command(0x49);
			if(val[page/2] > 50){
				for(column=0;column<55;column++)
					{
						GLCD_Data(0xFF); 						
					}
				CS1 = ~CS1; CS2 = ~CS2;
				page_inc=page_inc+0.5;										
				GLCD_Command((page_add+page_inc));				/* Increment page address by 1 each after two page successfull write */
				GLCD_Command(0x40);
				for(column=0;column<(val[page/2]-50)*1.1;column++)
				{
					GLCD_Data(0xFF); 										
				}
				for(column;column<64;column++){
					GLCD_Data(0x00);
				}
				page_inc=page_inc+0.5;										
				GLCD_Command((page_add+page_inc));						
				CS1 = ~CS1; CS2 = ~CS2;
			}
			else{
				GLCD_Command(0x49);
				for(column=0;column<val[page/2]*1.1;column++)
					{
						GLCD_Data(0xFF); 						
					}
				for(column;column<56;column++){
					GLCD_Data(0x00);
				}
				CS1 = ~CS1; CS2 = ~CS2;
				page_inc=page_inc+0.5;										
				GLCD_Command((page_add+page_inc));				
				GLCD_Command(0x40);
				for(column=0;column<64;column++){
					GLCD_Data(0x00);
				}
				page_inc=page_inc+0.5;										
				GLCD_Command((page_add+page_inc));								
				CS1 = ~CS1; CS2 = ~CS2;
			}
		}
		else{
			page_inc=page_inc+1;										
			GLCD_Command((page_add+page_inc));				
			
		}
	}
	GLCD_Command(0x40);														/* Set Y address (column=0) */
	GLCD_Command(0xB8);														/* Set x address (page=0) */
}


void main()
{
	int i,n=10;
	int value[10][4] = {{40,70,80,20},{50,60,100,10},{60,50,80,20},{50,20,90,40},{30,35,87,48},{20,40,50,70},
										 {30,42,40,60},{40,50,35,50},{20,40,45,60},{30,55,65,40}};
		
	GLCD_Init();								/* Initialize GLCD */	
	GLCD_ClearAll();
	while(1){
		for(i=0;i<n;i++){
			GLCD_Display(value[i]);
			delay(100);
		}
	}
}