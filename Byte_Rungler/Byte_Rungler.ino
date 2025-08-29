// Byte-Rungler a Rob Hordijk pattern generator - pin PD0 audio PWM output //

#include <HardwareTimer.h>

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 8000
#define BPM         120

  uint8_t tune;
  uint16_t feedback;
  uint16_t data;
  uint16_t shiftReg;

void timerHandler(void) {

	Timer.pause();

	feedback = shiftReg >> tune;
  feedback ^= data > 8000;
  shiftReg = shiftReg << 1;
  shiftReg |= feedback & 1;

  TIM1->CH1CVR = shiftReg & 0xFF;
  
  Timer.resume();

}

void PWM_init() {

	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_TIM1;
	GPIOD->CFGLR &= ~(0xf<<(4*0));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_Mode_AF_PP)<<(4*0);
	RCC->APB2PRSTR |= RCC_APB2Periph_TIM1;
	RCC->APB2PRSTR &= ~RCC_APB2Periph_TIM1;
	TIM1->PSC = 0x0000;
	TIM1->ATRLR = 255;
	TIM1->SWEVGR |= TIM_UG;
	TIM1->CCER |= TIM_CC1NE | TIM_CC1NP;
	TIM1->CCER |= TIM_CC4E | TIM_CC4P;
	TIM1->CHCTLR1 |= TIM_OC1M_2 | TIM_OC1M_1;
	TIM1->CH1CVR = 128;
	TIM1->BDTR |= TIM_MOE;
	TIM1->CTLR1 |= TIM_CEN;

}

void setup() {

	PWM_init();
	
	Timer.setOverflow(SAMPLE_RATE, HERTZ_FORMAT);
  Timer.attachInterrupt(timerHandler);
  Timer.resume();

}

void loop() {

	data += 32768;  // 1-65535 different pattern
  tune = 10;      // 1-15 value like bit reduction tune

  int tempo = 60000 / BPM;
  delay(tempo / 64);

}