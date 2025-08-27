// Grain effect tuned IIR filter oscillator - pin PD0 audio PWM output //

#include <HardwareTimer.h>

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 44100
#define SIZE        100
#define BPM         120

  long A = 0x7e66;
  long y[3] = {0, 0x1209, 0};

  float r = 3.65f;
  float x = 0.1f;

  const unsigned int d_size = SIZE;
  int d_time, i, j;
  bool rev = true;
  int8_t delay_1[d_size + 1] = { NULL };
  int8_t delay_2[d_size + 1] = { NULL };

int grain(int d_in) {

  int d_out;
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

  y[0] = ((A * y[1]) >> 14) - y[2];
  y[2] = y[1];
  y[1] = y[0];

	TIM1->CH1CVR = 128 + grain((y[ 0 ] >> 9));
  
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

  float nx = x;
  x = r * nx * (1.0f - nx);
  d_time = SIZE * x;

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}