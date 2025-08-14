// Karplus-Strong filtered distortion - pin PD0 audio PWM output //

#include <HardwareTimer.h>

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 22050
#define SIZE        128
#define OFFSET      8
#define BPM         120

  int16_t out = 0;
  int16_t last = 0;
  int16_t curr = 0;
  int16_t lp = 0;
  int16_t bp = 0;
  int16_t delaymem[SIZE];
  uint8_t locat = 0;
  uint8_t bound = SIZE;
  bool trig = false;

int16_t fuzz(int16_t x, uint8_t clip, uint8_t gain) {
  
  float y = x * gain;            
  if(y > clip) y = clip;
  else if(y < -clip) y = -clip;
  return y;

}


void timerHandler(void) {

	Timer.pause();

  if(trig) {

    for (int i = 0; i < SIZE; i++) delaymem[i] = random(-32767, 32767);
    trig = false;

  } else {

	  delaymem[locat++] = -out;
    if (locat >= bound) locat = 0;
    curr = delaymem[locat];
    out = (last>>1) + (curr>>1);
    last = curr;

    bp += (out - (bp>>4) - lp) / bound;
    lp += fuzz(bp, 64, 8); 

	  TIM1->CH1CVR = 128 + ((lp+bp) >> 7);

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

  trig = true;
  
  bound = random(OFFSET, SIZE);
   
  int tempo = 60000 / BPM;
  delay(tempo / 3);

}