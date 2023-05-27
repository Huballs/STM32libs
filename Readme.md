## Examples

GPIO:
```
#include "../libs/GPIO.hpp"

GPIOPIN PIN_LED(GPIOA, 9, IOConf::OUTPUT_PP_2M);    // clock is enabled in constructor

TimerDebouncer debouncer(TIM4, 4048, 2000);

BUTTON BUT({GPIOA, 1, IOConf::INPUT_PULLUP}, 0, &debouncer, 7000);

int main(void) {

    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;     // button debounce timer

    while(1){
        BUT.checkState();

        if(BUT.wasPressed()){
            PIN_LED.toggle();
        }
    }

}

```

7segment:
```
#include "../libs/7-segment.hpp"

SevenSegment<4>::digits_t digits_pins = {GPIOPIN{GPIOC, 14, IOConf::OUTPUT_PP_2M},
                                         GPIOPIN{GPIOB, 11, IOConf::OUTPUT_PP_2M},
                                         GPIOPIN{GPIOB, 10, IOConf::OUTPUT_PP_2M},
                                         GPIOPIN{GPIOA, 7, IOConf::OUTPUT_PP_2M}};

SevenSegment<4>::segm_t segment_pins = {GPIOPIN{GPIOA, 4, IOConf::OUTPUT_PP_2M},
                                        GPIOPIN{GPIOA, 2, IOConf::OUTPUT_PP_2M},
                                        GPIOPIN{GPIOB, 1, IOConf::OUTPUT_PP_2M},
                                        GPIOPIN{GPIOA, 6, IOConf::OUTPUT_PP_2M},
                                        GPIOPIN{GPIOA, 5, IOConf::OUTPUT_PP_2M},
                                        GPIOPIN{GPIOA, 3, IOConf::OUTPUT_PP_2M},
                                        GPIOPIN{GPIOB, 0, IOConf::OUTPUT_PP_2M},
                                        GPIOPIN{GPIOB, 2, IOConf::OUTPUT_PP_2M},};

SevenSegment<4> display(&digits_pins, &segment_pins);   


int main(void){

    int counter = 0;
    int i = 0;

    while(1){

        counter++;
        if((counter % 48000) == 0)
            i++;

        display.DisplayNumber(i);
    }
}

```