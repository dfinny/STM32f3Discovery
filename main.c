
#include "stm32f3_discovery.h"
#include "stm32f3_discovery_lsm303dlhc.h"
//#include "stm32f30x_i2c.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"


void i2c_init();
void Acc_Init();
void  LCD_contrast(int level);
void  LCD_backlight(int level);
void  LCD_write(int row, int col, char data);
//void SysTick_Handler(void);

FlagStatus Flag = RESET;
uint8_t SysTick_Flag = 0;

void main()
{
  uint8_t buffer[3]; 
  char str1[16];
  char str2[16];
  char str3[16];
  
  SysTick_Config(SystemCoreClock / 12);      

  Acc_Init();
  
  STM_EVAL_LEDInit(LED3);
  
  i2c_init();  
  while(I2C_GetFlagStatus(I2C2, I2C_ISR_BUSY) != RESET);
  
  LCD_contrast(35);
  LCD_backlight(2);
    
//  BusConfigure();
//  Write_Lcd_Instr(ON);
//  Write_Lcd_Instr(FUNCTION_SET);
  
  
  while(1)
  { 
  //  Write_Lcd_Instr(CLEAR_SCREEN);
    LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_X_H_A, buffer, 1);
    LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_Y_H_A, buffer+1, 1);
    LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_Z_H_A, buffer+2, 1);

    int factor = 0;
    if(buffer[0] <= 64) 
    {
      sprintf(str1, "X: %i ", buffer[0]);
      factor = buffer[0];
    }
    else
    {
      sprintf(str1, "X: %i ", buffer[0]-256);
      factor = buffer[0] - 256;
    }
    if(buffer[1] <= 64) 
      sprintf(str2, "Y: %i ", buffer[1]);
    else
      sprintf(str2, "Y: %i ", buffer[1]-256);

    if(buffer[2] <= 64) 
      sprintf(str3, "Z: %i ", buffer[2]);
    else
      sprintf(str3, "Z: %i ", buffer[2]-256);
    


    if(SysTick_Flag == 1)
    {
      STM_EVAL_LEDToggle(LED3);
      SysTick_Flag = 0;
      SysTick_Config(SystemCoreClock / (3 + abs(factor)));  
      int i,j,k;
      for(i=0; i<strlen(str1); i++)
        LCD_write(0, i, str1[i]);
      for(j=0; j<strlen(str2); j++)
        LCD_write(0, i+j, str2[j]);
      for(k=0; k<strlen(str3); k++)
        LCD_write(0, i+j+k, str3[k]);
    }
  }
  
}

void  LCD_write(int row, int col, char data) {
  
        
        // Move to sepcified row, col
  
        //while(I2C_GetFlagStatus(I2C2, I2C_ISR_BUSY) != RESET);
          
        I2C_TransferHandling(I2C2, 0x50 , 3, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
      
        I2C_SendData(I2C2, 0xFE);
               
        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
      
        I2C_SendData(I2C2, 0x45);
        
  
        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);        
        if (!row)               // if row == 0
            I2C_SendData(I2C2, col);
         else                  // else row asumed to be 1
            I2C_SendData(I2C2, (0x40 + col));       
         
        
         
        I2C_TransferHandling(I2C2, 0x50 , 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
          
       
        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);                        
        I2C_SendData(I2C2, data);

}         


//
//      Set LCD Contrast - Level should be 1..50 (Seems to work best if > 35)
//

void  LCD_contrast(int level) {
  
        //while(I2C_GetFlagStatus(I2C2, I2C_ISR_BUSY) != RESET);
          
        I2C_TransferHandling(I2C2, 0x50 , 3, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
      
        I2C_SendData(I2C2, 0xFE);
               
        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
      
        I2C_SendData(I2C2, 0x52);
  
        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET); 
        I2C_SendData(I2C2, level); 
}         

//
//      Set LCD Backlight - Level should be 1..8 (Seems to work best if > 1)
//

void  LCD_backlight(int level) {
  
        //while(I2C_GetFlagStatus(I2C2, I2C_ISR_BUSY) != RESET);
          
        I2C_TransferHandling(I2C2, 0x50 , 3, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
      
        I2C_SendData(I2C2, 0xFE);
               
        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
      
        I2C_SendData(I2C2, 0x53);
  
        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
      
        I2C_SendData(I2C2, level);
    
}    

void i2c_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef  I2C_InitStructure;
  
  /* Enable the I2C periph */
  RCC_I2CCLKConfig(RCC_I2C2CLK_SYSCLK);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
  
  /* Enable SCK and SDA GPIO clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_4);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_4);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* I2C SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* I2C SDA pin configuration */
  //GPIO_InitStructure.GPIO_Pin =  LSM303DLHC_I2C_SDA_PIN;
  //GPIO_Init(LSM303DLHC_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
  
  /* I2C configuration -------------------------------------------------------*/
  I2C_DeInit(I2C2);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0x00;
  //I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_Timing = 0xC062121F;
  
  /* Apply LSM303DLHC_I2C configuration after enabling it */
  I2C_Init(I2C2, &I2C_InitStructure);
  
  /* LSM303DLHC_I2C Peripheral Enable */
  I2C_Cmd(I2C2, ENABLE);
}

void Acc_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
//  EXTI_InitTypeDef EXTI_InitStructure;
  I2C_InitTypeDef  I2C_InitStructure;
  
  /* Enable the I2C periph */
  RCC_APB1PeriphClockCmd(LSM303DLHC_I2C_CLK, ENABLE);
  
  /* Enable SCK and SDA GPIO clocks */
  RCC_AHBPeriphClockCmd(LSM303DLHC_I2C_SCK_GPIO_CLK | LSM303DLHC_I2C_SDA_GPIO_CLK , ENABLE);
  
  /* Enable INT1 GPIO clock */
  RCC_AHBPeriphClockCmd(LSM303DLHC_I2C_INT1_GPIO_CLK, ENABLE);
  
  /* Enable INT2 GPIO clock */
  RCC_AHBPeriphClockCmd(LSM303DLHC_I2C_INT2_GPIO_CLK, ENABLE);
  
  /* Enable DRDY clock */
  RCC_AHBPeriphClockCmd(LSM303DLHC_DRDY_GPIO_CLK, ENABLE);
  
  GPIO_PinAFConfig(LSM303DLHC_I2C_SCK_GPIO_PORT, LSM303DLHC_I2C_SCK_SOURCE, LSM303DLHC_I2C_SCK_AF);
  GPIO_PinAFConfig(LSM303DLHC_I2C_SDA_GPIO_PORT, LSM303DLHC_I2C_SDA_SOURCE, LSM303DLHC_I2C_SDA_AF);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* I2C SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = LSM303DLHC_I2C_SCK_PIN;
  GPIO_Init(LSM303DLHC_I2C_SCK_GPIO_PORT, &GPIO_InitStructure);
  
  /* I2C SDA pin configuration */
  GPIO_InitStructure.GPIO_Pin =  LSM303DLHC_I2C_SDA_PIN;
  GPIO_Init(LSM303DLHC_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
  
  /* Mems DRDY */
/*
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
*/ 
  /* Mems DRDY pin configuration */
/*
  GPIO_InitStructure.GPIO_Pin = LSM303DLHC_DRDY_PIN;
  GPIO_Init(LSM303DLHC_DRDY_GPIO_PORT, &GPIO_InitStructure);
*/ 
  /* Connect EXTI Line to Mems DRDY Pin */
/*
  SYSCFG_EXTILineConfig(LSM303DLHC_DRDY_EXTI_PORT_SOURCE, LSM303DLHC_DRDY_EXTI_PIN_SOURCE);
  
  EXTI_InitStructure.EXTI_Line = LSM303DLHC_DRDY_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  
  EXTI_Init(&EXTI_InitStructure);
*/ 
  /* I2C configuration -------------------------------------------------------*/
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0x00;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_Timing = 0x00902025;
  
  /* Apply LSM303DLHC_I2C configuration after enabling it */
  I2C_Init(LSM303DLHC_I2C, &I2C_InitStructure);
  
  /* LSM303DLHC_I2C Peripheral Enable */
  I2C_Cmd(LSM303DLHC_I2C, ENABLE);
  
  /* Configure GPIO PINs to detect Interrupts */
/*
  GPIO_InitStructure.GPIO_Pin = LSM303DLHC_I2C_INT1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(LSM303DLHC_I2C_INT1_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = LSM303DLHC_I2C_INT2_PIN;
  GPIO_Init(LSM303DLHC_I2C_INT2_GPIO_PORT, &GPIO_InitStructure);
*/
  LSM303DLHCAcc_InitTypeDef LSM303DLHCAcc_InitStruct;
  LSM303DLHCAcc_InitStruct.Power_Mode = LSM303DLHC_NORMAL_MODE;
  LSM303DLHCAcc_InitStruct.AccOutput_DataRate = LSM303DLHC_ODR_50_HZ;
  LSM303DLHCAcc_InitStruct.Axes_Enable = LSM303DLHC_AXES_ENABLE;
  LSM303DLHCAcc_InitStruct.High_Resolution = LSM303DLHC_HR_ENABLE;
  LSM303DLHCAcc_InitStruct.BlockData_Update = LSM303DLHC_BlockUpdate_Continous;
  LSM303DLHCAcc_InitStruct.Endianness = LSM303DLHC_BLE_LSB;
  LSM303DLHCAcc_InitStruct.AccFull_Scale = LSM303DLHC_FULLSCALE_2G;

  LSM303DLHC_AccInit(&LSM303DLHCAcc_InitStruct);
}  

/*
void EXTI2_TS_IRQHandler()
{
  Flag = SET;
  EXTI_ClearITPendingBit(EXTI_Line2);
}
*/
void SysTick_Handler(void)
{
  SysTick_Flag = 1; 
}
