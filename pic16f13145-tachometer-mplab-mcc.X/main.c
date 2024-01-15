 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
*/

/*
© [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"

#define ENABLE_CLOCKWISE_VALUE         0b01
#define ENABLE_COUNTERCLOCKWISE_VALUE  0b11
#define DISABLE_VALUE                  0
#define TMR_VALUE_STEPS(steps)         (0xFFFF - 3*(steps - 1))
#define MAX_STEPS                      315                          // The maximum number of steps for this type of stepper
                                                                    // is 315, added 1 due to TMR_VALUE_STEPS formula

/*
    Main application
*/

void stepper_cw_steps(uint16_t steps);
void stepper_ccw_steps(uint16_t steps);
void stepper_zero_position(void);
void stepper_max_position(void);
void needle_sweep(void);

uint16_t read_position(void);

int main(void)
{
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts 
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts 
    // Use the following macros to: 

    // Enable the Global Interrupts 
    //INTERRUPT_GlobalInterruptEnable(); 

    // Disable the Global Interrupts 
    //INTERRUPT_GlobalInterruptDisable(); 

    // Enable the Peripheral Interrupts 
    //INTERRUPT_PeripheralInterruptEnable(); 

    // Disable the Peripheral Interrupts 
    //INTERRUPT_PeripheralInterruptDisable();
    
    printf("Needle sweep on-going! Please wait!\n\r");    
    needle_sweep(); 
    
    uint16_t actual_position = 0;
    uint16_t needed_position = 0;    
    
    while(1)
    {
        printf("Actual position: %d \n\r", actual_position);
        printf("Please enter the new position [0-315]! \n\r");
        needed_position = read_position();
        
        if( needed_position <= 315 )
        {
            if( needed_position != actual_position )
            {
                if ( needed_position < actual_position )
                    stepper_ccw_steps( actual_position - needed_position );
                else
                    stepper_cw_steps ( needed_position - actual_position );

                actual_position = needed_position;
            }
        }
        else
            printf("The value must be between [0-315]! \n\r");
    }    
}

void stepper_cw_steps(uint16_t steps)
{

    TMR1_Write( TMR_VALUE_STEPS(steps) );
      
    while(CLBCONbits.BUSY)
        ;
    CLBSWINL = ENABLE_CLOCKWISE_VALUE;
    
    while(CLBCONbits.BUSY)
        ;
    CLBSWINLbits.CLBSWIN0 = DISABLE_VALUE;   
    
    while(!PIR7bits.CLB1IF0)
        ;
    PIR7bits.CLB1IF0 = 0;
 
}

void stepper_ccw_steps(uint16_t steps)
{    
    
    TMR1_Write( TMR_VALUE_STEPS(steps) );

    while(CLBCONbits.BUSY)
        ;
    CLBSWINL = ENABLE_COUNTERCLOCKWISE_VALUE;
    
    while(CLBCONbits.BUSY)
        ;
    CLBSWINLbits.CLBSWIN0 = DISABLE_VALUE;
 
    while(!PIR7bits.CLB1IF0)
        ;
    PIR7bits.CLB1IF0 = 0;

}

void stepper_zero_position(void)
{
    stepper_ccw_steps(MAX_STEPS);
}

void stepper_max_position(void)
{
    stepper_cw_steps(MAX_STEPS);
}

void needle_sweep(void)
{
    stepper_zero_position();
    stepper_max_position();
    stepper_zero_position();
}

uint16_t read_position(void)
{
    char vector_number[4] = {0};
    
    uint8_t i = 0;
    uint8_t temp = 0;
    uint16_t result = 0;
    
    temp = (uint8_t) getch();
    while(temp != '\n')
    {
        vector_number[i++] = temp;
        temp = (uint8_t) getch();
    }
    
    result = (uint16_t) atoi(vector_number);
    return result;
}