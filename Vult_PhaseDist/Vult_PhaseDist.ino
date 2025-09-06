// Vult DSP phase distortion oscillator - pin PD0 audio PWM output //

#include <HardwareTimer.h>
#include "Phasedist.h"

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 22050
#define BPM         120

Phasedist_process_type phasedist;

  uint16_t cv;      // control voltage
  uint16_t detune;  // detune

void timerHandler(void) {

  int sample = Phasedist_process(phasedist, cv , detune);

  TIM1->CH1CVR = 128 + (sample >> 9);

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

  cv = random(16384, 32768);
  detune = 32768;
   
  int tempo = 60000 / BPM;
  delay(tempo / 4);

}