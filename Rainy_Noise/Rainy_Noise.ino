// Rainy noise generator - pin PD0 audio PWM output //

#include <HardwareTimer.h>

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 44100

int16_t lp1, lp2, bp2, lowns, sah, rndm, lfrt;

void timerHandler(void) {

	Timer.pause();

  rndm = rand();
    
  if (lfrt-- == 0) {

    lfrt = (rndm & 511) + 1024;
    sah = rndm;
    
  }

  bp2 = (((sah>>2) - (bp2>>3) - lp2)>>1) + bp2;
  lp2 = bp2 + lp2;                   
  lowns += ((rndm - lowns)>>3) + (rndm>>6);
  lp1 += ((rndm>>2) - lp1)>>5;

  int out = ((lp1>>1) + (bp2>>2) + (lowns>>10))>>5;

  TIM1->CH1CVR = 128 + out;
  
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

}