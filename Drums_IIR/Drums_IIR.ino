// floating point IIR filter fold drum - pin PD0 audio PWM output //

#include <HardwareTimer.h>

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 44100
#define BPM         120

float randomf(float minf, float maxf) {return minf + random(1UL << 31)*(maxf - minf) / (1UL << 31);}

class Synth {
public:

  float osc = 0.0f;
  float A = 1.975f;
  float y[3];
	float pitch = 0.0f;
  float lim = 1.0f;
  bool gate = false;
  bool type = false;
  float decay = 0.0f;
  float d = 0.0f;

	float calculate();

};

float Synth::calculate() {
	
  if (gate == true) {

    d = 1.0f;

    y[0] = 0.0f;
    y[1] = 0.25f;
	  y[2] = 0.0f;
	
    gate = false;

  } else {

    y[0] = ((A + pitch) * y[1]) - y[2];
	  y[2] = y[1];
	  y[1] = y[0];

    d = d - decay;
    if (d <= 0.0f) d = 0.0f;

    if (type == true) osc = y[rand() % 3];
    else osc = y[2];

    while (osc > lim || osc < -lim) {
    
      if (osc > lim) osc = lim - (osc - lim);
      else if (osc < -lim) osc = -lim + (-osc - lim);
    
    }

    osc = d * osc;

  }

	return osc;

}

Synth drum;

void timerHandler(void) {

	Timer.pause();

	TIM1->CH1CVR = 128 + (127.0f * drum.calculate());
  
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
  
  drum.gate = true;
  drum.type = rand() % 2;
  drum.lim = randomf(0.3f, 0.9f);
  drum.decay = randomf(0.001f, 0.005f);
  drum.pitch = randomf(0.001f, 0.02f);

  int tempo = 60000 / BPM;
  delay(tempo / 3);

}