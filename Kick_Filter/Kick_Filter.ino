// Lo-Fi kick drum and filter distortion - pin PD0 audio PWM output //

#include <HardwareTimer.h>

HardwareTimer Timer(TIM2);

#define SAMPLE_RATE 22050
#define BPM         120

class Synth {
public:

  bool trig = 0;
  int wave = 0;
  int clip = 0;
  int i = 0;
  int lp = 0;
  int bp = 0;
  int filt = 0;
  float k = 1.0f;
  int harm = 0;
  int decay = 0;

  const int16_t fand[180] = { 0, 17, 34, 52, 69, 86, 103, 120, 137, 154, 171, 187, 203, 219, 234, 250, 264, 279, 293, 307, 321, 334, 347, 359, 371,  383,  394,  404,  414,  424,  433,  441,  449,  456,  463,  469,  475,  480,  485,  489,  492,  495,  497,  498,  499,  500,  499,  498,  497,  495,  492,  489,  485,  480,  475,  469,  463,  456,  449,  441,  433,  424,  414,  404,  394,  383,  371,  359,  347,  334,  321,  307, 293, 279, 264, 250, 234, 219, 203, 187, 171, 154, 137, 120, 103, 86, 69, 52, 34, 17, 0, -18, -35, -53, -70, -87, -104, -121, -138, -155, -172, -188, -204, -220, -235, -250, -265, -280, -294, -308, -322, -335, -348, -360, -372, -384, -395, -405, -415, -425, -434, -442, -450, -457, -464, -470, -476, -481, -486, -490, -493, -496, -498, -499, -500, -500, -500, -499, -498, -496, -493, -490, -486, -481, -476, -470, -464, -457, -450, -442, -434, -425, -415, -405, -395, -384, -372, -360, -348, -335, -322, -308, -294, -280, -265, -250, -235, -220, -204, -188, -172, -155, -138, -121, -104, -87, -70, -53, -35, -18 };
  const int16_t harmo[180] = { 0, 416, 365, 321, 334, 307, 279, 279, 254, 232, 227, 203, 185, 178, 155, 140, 131, 110, 97, 88, 68, 57, 48, 30, 21, 11, -4, -11,  -21,  -34,  -40,  -48,  -60,  -63,  -71,  -80,  -82,  -88,  -95,  -95,  -101, -104, -103, -107, -109, -106, -109, -107, -103, -104, -101, -95,  -95,  -88,  -82,  -80,  -71,  -63,  -60,  -48,  -40,  -34,  -21,  -11,  -4, 11, 21, 30, 48, 57, 68, 88, 97, 110,  131,  140, 155, 178, 185, 203, 227, 232, 254, 279, 279, 307, 334, 321, 365, 416, 0, -417, -366, -322, -335, -308, -280, -280, -255, -233, -228, -204, -186, -179, -156, -141, -132, -111, -98,  -89,  -69,  -58,  -49,  -31,  -22,  -12,  3,  10, 20, 33, 39, 47, 59, 62, 70, 79, 81, 87, 94, 94, 100,  103,  102,  106,  108,  105,  108,  106,  102,  103,  100,  94, 94, 87, 81, 79, 70, 62, 59, 47, 39, 33, 20, 10, 3,  -12,  -22,  -31,  -49,  -58,  -69,  -89,  -98,  -111, -132, -141, -156, -179, -186, -204, -228, -233, -255, -280, -280, -308, -335, -322, -366, -417 };

	int calculate();

};

int Synth::calculate() {

  if (trig == 1) {

    i = 0;
    k = 1.0f;
    trig = 0;

  }

  if (i > 179) {

    i = 0;
    k =  k * (128 - (64 - decay)) / 128 - 10 / 128;

  }

  wave = fand[i] * 64 + harmo[i] * (64 - harm);

  i ++;

  int x = (int)(wave * k);

  bp += (x - (bp>>4) - lp) / filt;
  lp += bp;

  int y = (lp + bp) >> 8;

  if(y > clip) y = clip;
  else if(y < -clip) y = -clip;

  return y;

}

Synth kick;

void timerHandler(void) {

	Timer.pause();

  TIM1->CH1CVR = 128 + kick.calculate();
  
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

	kick.trig = 1;
  kick.decay = random(8, 63);
  kick.harm = random(1, 63);
  kick.filt = random(1, 128);
  kick.clip = 100;

  int tempo = 60000 / BPM;
  delay(tempo / 2);

}