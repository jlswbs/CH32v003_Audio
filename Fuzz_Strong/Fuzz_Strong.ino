// Fuzz-Strong filter and fuzz distortion - pin PD0 audio PWM output //

#include <HardwareTimer.h>

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 22050
#define SIZE        256
#define OFFSET      16
#define BPM         120

  int out = 0;
  int last = 0;
  int curr = 0;
  uint8_t delaymem[SIZE];
  uint8_t locat = 0;
  uint8_t bound = SIZE;
  int accum = 0;
  int lowpass = 0;
  bool trig = false;

int8_t fuzz(int8_t x, uint8_t clip, uint8_t gain) {
  
  float y = x * gain;            
  if(y > clip) y = clip;
  else if(y < -clip) y = -clip;
  return y;

}

void timerHandler(void) {

	Timer.pause();

  if(trig) {

    for (int i = 0; i < SIZE; i++) delaymem[i] = random(-127, 127);
    trig = false;

  } else {

	  delaymem[locat++] = fuzz(-out, 64, 8);
    if (locat >= bound) locat = 0;
    curr = delaymem[locat];
    out = accum >> lowpass;
    accum = accum - out + ((last>>1) + (curr>>1));
    last = curr;

	  TIM1->CH1CVR = 128 + out;

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
  lowpass = random(0, 4);
   
  int tempo = 60000 / BPM;
  delay(tempo / 2);

}