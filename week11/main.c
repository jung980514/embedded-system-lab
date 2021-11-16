#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "lcd.h"
#include "touch.h"
#include "misc.h"


int color[12] = {WHITE,CYAN,BLUE,RED,MAGENTA,LGRAY,GREEN,YELLOW,BROWN,BRRED,GRAY};
uint16_t value = 100;
int flag = 1;
int ledCount = 0;

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef       TIM_OCInitStructure;
uint16_t prescale;

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void NVIC_Configure(void);

//---------------------------------------------------------------------------------------------------

void RCC_Configure(void)
{
      /* LCD */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // port C RCC ENABLE
      /* PWM */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // TIM2
      /* LED */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); // Port D

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure3;
    GPIO_InitStructure3.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // led1,2
    GPIO_InitStructure3.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure3.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure3); // led Ȱ��ȭ
}

void NVIC_Configure(void) {

   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
   NVIC_InitTypeDef NVIC_InitStructure;

   /* Enable TIM2 Global Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
}

void TIM2_IRQHandler(void) {
  if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
      if (ledCount % 2 == 0) { //led 1(pin2) �� 1�ʸ���
      GPIO_SetBits(GPIOD, GPIO_Pin_2);
      } else{
      GPIO_ResetBits(GPIOD, GPIO_Pin_2);
      }

      if(ledCount == 0) { //led2(pin3)�� 5�ʸ���
      GPIO_ResetBits(GPIOD, GPIO_Pin_3);
      } else if(ledCount == 5) {
      GPIO_SetBits(GPIOD, GPIO_Pin_3);
      }

      ledCount++;
      ledCount %= 10;

      TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}

void PWM_Configure()
{
prescale = (uint16_t) (SystemCoreClock / 1000000);
TIM_TimeBaseStructure.TIM_Period = 20000;
TIM_TimeBaseStructure.TIM_Prescaler = prescale;
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_Pulse = 1500; // us
TIM_OC3Init(TIM3, &TIM_OCInitStructure);
TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
TIM_ARRPreloadConfig(TIM3, ENABLE);
TIM_Cmd(TIM3, ENABLE);
}

//void PWM_Configure()
//{
//   prescale = (uint16_t) (SystemCoreClock / 10000);
//   TIM_TimeBaseStructure.TIM_Period = 10000;
//   TIM_TimeBaseStructure.TIM_Prescaler = prescale;
//   TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
//
//   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//   TIM_ARRPreloadConfig(TIM2, ENABLE);
//   TIM_Cmd(TIM2, ENABLE);
//}


int main(void)
{

    SystemInit();
    RCC_Configure();
    GPIO_Configure();
    PWM_Configure();
    NVIC_Configure();

    //-----------------

    LCD_Init(); // LCD �ʱ�ȭ
    Touch_Configuration(); // ��ġ ����
    Touch_Adjust(); // ȭ�� ��ġ ���� ���߱�
    LCD_Clear(WHITE); // LCD ��� �ʱ�ȭ

    //-----------------

    uint16_t pos_temp[2]; // x, y��ǥ�� ���� �迭

   LCD_ShowString(40, 10, "THU_TEAM07", MAGENTA, WHITE); // ���� ���
   LCD_ShowString(90, 50, "OFF", RED, WHITE); // ����Ʈ�� OFF
   LCD_DrawRectangle(40, 80, 80, 120); // �簢�� ���
   LCD_ShowString(60,100, "BTN", MAGENTA, WHITE); // "Button" ���� ���

    while (1) {
         Touch_GetXY(&pos_temp[0], &pos_temp[1], 1); // ��ġ ��ǥ �޾Ƽ� �迭�� �Է�
         Convert_Pos(pos_temp[0],pos_temp[1],&pos_temp[0],&pos_temp[1]); // ���� ��ǥ�� LCD ũ�⿡ �°� ��ȯ
         printf("%d, %d\n", pos_temp[0], pos_temp[1]);

         if ((uint16_t)40 < pos_temp[0] && pos_temp[0] < (uint16_t)80 && \
             (uint16_t)80 < pos_temp[1] && pos_temp[1] < (uint16_t)120) {
            if (flag) {
               LCD_ShowString(90, 50, "    ", RED, WHITE);
               LCD_ShowString(40, 50, "ON", RED, WHITE);
               TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // interrupt enable
            } else {
               LCD_ShowString(90, 50, "OFF", RED, WHITE);
               LCD_ShowString(40, 50, "    ", RED, WHITE);
               TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE); // interrupt enable
            }
            flag++;
            flag %= 2;
         }
    }
    return 0;
}
