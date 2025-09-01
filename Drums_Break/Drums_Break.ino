// Drum samples with clip distortion and samplerate tune - pin PD0 audio PWM output //

#include <HardwareTimer.h>
#include "BD.h"
#include "OH.h"
#include "RD.h"
#include "RS.h"
#include "SD.h"

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 44100
#define BPM         135

  int8_t out;
  int cnt, size, type, lim, rate;
  bool trig = false;

int8_t clip(int8_t x, uint8_t clip) {
        
  if(x > clip) x = clip;
  else if(x < -clip) x = -clip;
  return x;

}

void timerHandler(void) {

	Timer.pause();

  if (trig) {
    
    cnt = 0;
    trig = false;
    
  } else { 

    if (type == 0) { out = BD[cnt]; size = BDLen; }
    if (type == 1) { out = RD[cnt]; size = RDLen; }
    if (type == 2) { out = RS[cnt]; size = RSLen; }
    if (type == 3) { out = SD[cnt]; size = SDLen; } 
    if (type == 4) { out = OH[cnt]; size = OHLen; }
  
    if(cnt >= size) cnt = size;
    cnt++;

    out = 128 - out;

    TIM1->CH1CVR = 128 + clip(out, lim);

  }

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

  rate = random(4000, 24000);
  Timer.setOverflow(rate, HERTZ_FORMAT);
  trig = true;
  type = random(0, 5);
  lim = random(10, 50);
   
  int tempo = 60000 / BPM;
  delay(tempo / 3);

}