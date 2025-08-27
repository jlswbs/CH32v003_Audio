// fixed point Lorenz chaotic oscillator - pin PD0 audio PWM output //

#include <HardwareTimer.h>

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 44100

  int32_t X = 1024;
  int32_t Y = 0;
  int32_t Z = 0;
  int dt = 20; // 0.02 * 1024
  int A = 768; // 0.75 * 1024

void timerHandler(void) {

	Timer.pause();

  int32_t nx = X;
  int32_t ny = Y;
  int32_t nz = Z;
  
  X = nx + ((dt * ((ny * nz) >> 10)) >> 10);
  Y = ny + ((dt * (nx - ny)) >> 10);
  Z = nz + ((dt * (A - ((nx * ny) >> 10))) >> 10);

	TIM1->CH1CVR = map(X, -4500, 4500, 0, 255);
  
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