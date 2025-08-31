// Techno-Glitch drum samples with grain effect and samplerate tune - pin PD0 audio PWM output //

#include <HardwareTimer.h>
#include "BD.h"
#include "OH.h"
#include "RD.h"
#include "RS.h"
#include "SD.h"

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 44100
#define BPM         120

  int8_t out;
  int cnt, size, type, rate;
  bool trig = false;

  const unsigned int d_size = 256;
  int d_time, i, j;
  bool rev = true;
  uint8_t delay_1[d_size + 1] = { NULL };
  uint8_t delay_2[d_size + 1] = { NULL };

uint8_t grain(uint8_t d_in) {

  uint8_t d_out;
  i++; if (i  > d_time) i = 0;
  delay_1[i] = d_in;
  if (i == d_time) j = 0;
  delay_2[i]  = delay_1[i];
  j++; if (j > d_time) j = 0;
  if (!rev) d_out = delay_2[j];
  if (rev) d_out = delay_2[d_time - j];
  return d_out;

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
  
    if(cnt >= size) cnt = 0;
    cnt++;

    TIM1->CH1CVR = grain(out);

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
  d_time = random(1, d_size);
   
  int tempo = 60000 / BPM;
  delay(tempo / (1+type));

}