// floating point Karplus-Strong like cymbal - pin PD0 audio PWM output //

#include <HardwareTimer.h>

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 44100
#define SIZE        96
#define OFFSET      4
#define BPM         120

  float out = 0.0f;
  float last = 0.0f;
  float curr = 0.0f;
  float tmp = 0.0f;
  float delaymem[SIZE];
  uint8_t locat = 0;
  uint8_t bound = SIZE;
  float lowpass = 0.0f;
  bool trig = false;

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);}  

void timerHandler(void) {

	Timer.pause();

  if(trig) {

    for (int i = 0; i < SIZE; i++) delaymem[i] =  randomf(-0.99f, 0.99f);
    trig = false;

  } else {

	  delaymem[locat++] -= out * lowpass;
    if (locat >= bound) locat = 0;
    curr = delaymem[locat];
    tmp = 0.5f * (last + curr);
    last = curr;
    out = out - (lowpass * (out - tmp));

	  TIM1->CH1CVR = 128 + 127 * out;

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
  lowpass = randomf(0.049f, 0.99f);

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}