//embedded system lab group7 5th week mission
#include "stm32f10x.h"

#define RCC_APB2ENR *(volatile unsigned int *)0x40021018

//�����̸��
#define GPIOC_CRL *(volatile unsigned int *)0x40011000 //Joystick Configuration Regiester Low
#define GPIOC_CRH *(volatile unsigned int *)0x40011004 //Joystick Configuration Regiester Low
#define GPIOC_IDR *(volatile unsigned int *)0x40011008 //Joystick Input Data Register
#define GPIOC_BSRR *(volatile unsigned int *)0x40011010 //BIT SET, RESET WRITE

//LED, ��ư
#define GPIOD_CRL *(volatile unsigned int *)0x40011400 //LED Configuration Regiester Low
#define GPIOD_CRH *(volatile unsigned int *)0x40011404 //LED Configuration Regiester High
#define GPIOD_BSRR *(volatile unsigned int *)0x40011410 //LED Bit Set/Reset Register
#define GPIOD_IDR *(volatile unsigned int *)0X40011408 //LED LOW, HIGH READ

void init() {
  RCC_APB2ENR = 0x30; // Port C,D Clock Enable
 
  // LED PD7
  GPIOD_CRL = 0x44444444; // reset
  GPIOD_CRL = 0x30000000; // led PD 7 output push-pull
 
  //��ư: PD11(S1), PD12(S2)
  GPIOD_CRH = 0x44444444; // reset
  GPIOD_CRH = 0x00088000; // ��ư configuration input 
  GPIOD_IDR = 0x00000000; // ��ư INPUT DATA REGISTER RESET
  GPIOD_BSRR = 0x00000000; // ��ư bsrr ����
 
  //������ ���: PC8
  GPIOC_CRH = 0x44444444; // reset
  GPIOC_CRH = 0x00000003; // ������ PC8 ���ǱԾ�̼�, output  push-pull
  GPIOC_BSRR = 0x00000000; // ������ bsrr ����
}

void led7on() { 
  GPIOD_BSRR = 0x00000080;
}
void led7off() {   
  GPIOD_BSRR = 0x00800000; // led 7 reset
}
void delay(void) {
  int i=0;
  for(i=0;i<1000000;i++)   {}  //empty Loop
}

int main(void)
{
  init();
 
  while(1){
    if (~GPIOD_IDR & 0x00000800) { // pd 11�� ��ư �������� ���� �۵�
      GPIOC_BSRR = 0x0100; // pc8 set
      delay();
      GPIOC_BSRR = 0x01000000; // pc8 reset
    } else if (~GPIOD_IDR & 0x00001000) { //pd 12�� ��ư ������ ��  led ��
      led7on();
      delay();
      led7off();
    } 
  }
} 