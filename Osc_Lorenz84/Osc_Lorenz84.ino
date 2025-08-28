// Lorenz 84 chaotic attractor as oscillator - pin PD0 audio PWM output //

#include <HardwareTimer.h>

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 44100

  float a = 1.111f;
  float b = 1.479f;
  float f = 4.494f;
  float g = 0.44f;

  float x = 1.0f;
  float y = 1.0f;
  float z = 1.0f;

  float dt = 0.135f;


void timerHandler(void) {

	Timer.pause();

  float nx = x;
  float ny = y;
  float nz = z;
    
  x = nx + dt * (-a * nx - powf(ny, 2.0f) - powf(nz, 2.0f) + a * f);
  y = ny + dt * (-ny + nx * ny - b * nx * nz + g);
  z = nz + dt * (-nz + b * nx * ny + nx * nz);

	TIM1->CH1CVR = 128 + (41.0f * x);
  
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