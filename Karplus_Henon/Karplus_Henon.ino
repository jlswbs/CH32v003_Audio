// Dual Karplus-Strong and Henon fractal - pin PD0 audio PWM output //

#include <HardwareTimer.h>

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 22050
#define SIZE        128
#define OFFSET      8
#define BPM         120

  int out1;
  int last1 = 0;
  int curr1 = 0;
  int8_t delaymem1[SIZE];
  uint8_t locat1 = 0;
  uint8_t bound1 = SIZE;
  int accum1 = 0;
  int lowpass1 = 2;

  int out2;
  int last2 = 0;
  int curr2 = 0;
  int8_t delaymem2[SIZE];
  uint8_t locat2 = 0;
  uint8_t bound2 = SIZE;
  int accum2 = 0;
  int lowpass2 = 2;

  float a = 1.4f;
  float b = 0.3f;
  float x = 1.0f;
  float y = 1.0f;

  bool trig1 = false;
  bool trig2 = false;

void timerHandler(void) {

	Timer.pause();

  if(trig1) {

    for (int i = 0; i < SIZE; i++) delaymem1[i] = random(-127, 127);
    trig1 = false;

  } else {
  
    delaymem1[locat1++] = out1;
    if (locat1 >= bound1) locat1 = 0;
    curr1 = delaymem1[locat1];
    out1 = accum1 >> lowpass1;
    accum1 = accum1 - out1 + ((last1>>1) + (curr1>>1));
    last1 = curr1;

  }

  if(trig2) {

    for (int i = 0; i < SIZE; i++) delaymem2[i] = random(-127, 127);
    trig2 = false;

  } else {

    delaymem2[locat2++] = -out2;
    if (locat2 >= bound2) locat2 = 0;
    curr2 = delaymem2[locat2];
    out2 = accum2 >> lowpass2;
    accum2 = accum2 - out2 + ((last2>>1) + (curr2>>1));
    last2 = curr2;
  
  }

	TIM1->CH1CVR = 128 + (out1 + out2);
  
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

  trig1 = true;
  
  float nx = x;
  float ny = y;
  
  x = 1.0f + ny - a * powf(nx, 2.0f);
  y = b * nx;

  int xout = 1000 * x;
  int yout = 1000 * y;
  
  trig1 = true;
  lowpass2 = random(1, 3);
  bound1 = map(xout, -1500, 1500, OFFSET, SIZE);

  int tempo = 60000 / BPM;
  delay(tempo / 3);
  
  trig2 = true;
  lowpass2 = random(1, 3);
  bound2 = map(yout, -400, 400, OFFSET, SIZE);
 
  delay(tempo / 4);

}