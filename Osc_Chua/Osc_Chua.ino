// Chua chaotic oscillator - pin PD0 audio PWM output //

#include <HardwareTimer.h>

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 44100

  float x = 0.7f;
  float y = 0.0f;
  float z = 0.0f;
  float dt = 0.01f;
  float alpha = 15.6f * dt;
  float beta = -28.0f * dt; 
  float m0 = -1.143f;
  float m1 = -0.714f;
  float m2 = 0.5f * (m0 - m1);
  float m3 = 2.0f * m2;
  float h = 0.0f;


void timerHandler(void) {

	Timer.pause();

  float a = y - x;
  
  float xn = alpha * (a - h); 
  float yn = (z - a) * dt; 
  float zn = (beta) * y;

  if (x <= -1.0f) h = (m1 * x) - m3; 
  else if (x >= 1.0f) h = (m1 * x) + m3; 
  else h = (m1 + m3) * x;

  x = x + xn;  
  y = y + yn;
  z = z + zn;

	TIM1->CH1CVR = 128 + (55.0f * x);
  
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