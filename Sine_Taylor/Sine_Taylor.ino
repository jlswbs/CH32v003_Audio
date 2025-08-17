// Taylor sine wave oscillator - pin PD0 audio PWM output //

#include <HardwareTimer.h>

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 44100

  float delta = -PI;

float taylor(float angle) {

  float a2 = angle * angle;
  float x = angle;

  float rv = x;

  x = x * (1.0f / 6.0f) * a2;
  rv -= x;

  x = x * (1.0f / 20.0f) * a2;
  rv += x;

  x = x * (1.0f / 42.0f) * a2;
  rv -= x;

  x = x * (1.0f / 72.0f) * a2;
  rv += x;

  x = x * (1.0f / 110.0f) * a2;
  rv -= x;

  return rv;

}

void timerHandler(void) {

	Timer.pause();

	TIM1->CH1CVR = 128 + (127.0f * taylor(delta));

  if (delta > PI) delta = -PI;

  delta = delta + 0.1f;
  
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