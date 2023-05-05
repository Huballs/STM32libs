#pragma once

#include "stm32.h"

#if defined (STM32F1)
enum class IOConf { 
	/*------------------------------------Output CNF and MODE----------------------------------*/

	// General purpose output Push-pull Port bit conf
	OUTPUT_PP_10M   =   0x01,
	OUTPUT_PP_2M    =   0x02,
	OUTPUT_PP_50M   =   0x03,

	// General purpose output Open-drain Port bit conf
	OUTPUT_OD_10M   =   0x05,
	OUTPUT_OD_2M    =   0x06,
	OUTPUT_OD_50M   =   0x07,

	// Alternate Function output Push-pull
	OUTPUT_AF_PP_10M =  0x09,
	OUTPUT_AF_PP_2M	 =  0x0A,
	OUTPUT_AF_PP_50M =  0x0B,

	// Alternate Function output Open-drain
	OUTPUT_AF_OD_10M =  0x0D,
	OUTPUT_AF_OD_2M  =  0x0E,
	OUTPUT_AF_OD_50M =  0x0F,

	/*-------------------------------------Input CNF and MODE-----------------------------------*/
	INPUT_ANALOG    =   0x00,
	INPUT_FLOAT     =   0x04,
	INPUT_PULLUP    =   0x08,		// Allows to set pull up or down via PxODR register
	INPUT_PULLDOWN  =   0x18,
};

inline void _set_pin(GPIO_TypeDef* port_, uint32_t set_mask){
	port_->BSRR = set_mask;
}

inline void _reset_pin(GPIO_TypeDef* port_, uint32_t reset_mask){
	port_->BSRR = reset_mask;
}
#endif // STM32F1

#if defined (STM32F3)
	enum class IOConf { 
		/*
		*  Order of config bits:
		*  MODER	OTYPER	OSPEEDR	PUPDR
		*  [6:5]	[4]		[3:2]	[1:0]
		*/
			// General purpose output Push-pull Port bit conf
		OUTPUT_PP_2M	=	0x20,
		OUTPUT_PP_10M	=	0x24,
		OUTPUT_PP_50M	=	0x2C,

		// General purpose output Open-drain Port bit conf
		OUTPUT_OD_2M	=	0x30,
		OUTPUT_OD_10M	=	0x34,
		OUTPUT_OD_50M	=	0x3C,

		// Alternate Function output Push-pull
		OUTPUT_AF_PP_2M	=	0x40,
		OUTPUT_AF_PP_10M=	0x44,
		OUTPUT_AF_PP_50M=	0x4C,

		// Alternate Function output Open-drain
		OUTPUT_AF_OD_2M	=	0x50,
		OUTPUT_AF_OD_10M=	0x54,
		OUTPUT_AF_OD_50M=	0x5C,

		// Inputs
		INPUT_FLOAT	=	0x00,
		INPUT_PULLUP=	0x01,
		INPUT_PULLDOWN=	0x02,
		INPUT_ANALOG=	0x60,

		OUTPUT_ANALOG=	0x60,

		AF0		=	0x01,
		AF1		=	0x02,
		AF2		=	0x03,
		AF3		=	0x04,
		AF4		=	0x05,
		AF5		=	0x06,
		AF6		=	0x07,
		AF7		=	0x08,
		AF8		=	0x09,
		AF9		=	0x10,
		AF10	=	0x11,
		AF11	=	0x12,
		AF12	=	0x13,
		AF13	=	0x14,
		AF14	=	0x15,
		AF15	=	0x16,
	}
#endif // STM32F3

class GPIOPIN {

public:
	GPIOPIN() = delete;

    GPIOPIN(const GPIOPIN& other);

	GPIOPIN(GPIO_TypeDef* port, uint8_t pin, IOConf mode);

    void setMode(IOConf mode);
	void setMode();
	
	inline void set(){
		_set_pin(port_, set_read_mask_);
	}
	inline void reset(){
		_reset_pin(port_, reset_mask_);
	}
	inline bool read() const{
		return port_->IDR & set_read_mask_;
	}
	void toggle();

	GPIO_TypeDef *port_;
	uint16_t pin_;

private:
	uint32_t set_read_mask_	 = 1 << pin_;
	uint32_t reset_mask_ = 1 << (pin_ + 16);
	IOConf mode_;
};

class ButtonDebouncer {
public:
	ButtonDebouncer() = delete;

	ButtonDebouncer (TIM_TypeDef *timer, uint16_t timer_prescaler, uint16_t debounce_time)
	 : timer_(timer), timer_prescaler_(timer_prescaler), debounce_time_(debounce_time) {};

	void configure();

	inline void start(){
		timer_->SR = 0;
		timer_->CR1 |= TIM_CR1_CEN;
	}
	inline bool isDone() const {
		return (timer_->SR & TIM_SR_UIF);
	}
private:
	TIM_TypeDef *timer_;
	uint16_t timer_prescaler_;
	uint16_t debounce_time_;
};

extern ButtonDebouncer button_debouncer;

class BUTTON {
public:
	BUTTON() = delete;

	BUTTON(GPIOPIN gpio_pin, uint8_t active_state, uint16_t long_press_time = 0);

	void checkState();

	inline void configurePin(IOConf mode){
		gpio_pin_.setMode(mode);
	}
	inline void configurePin(){
		gpio_pin_.setMode();
	}

	bool wasPressed(){
		if(waspressed){
			waspressed = false;
			return true;
		} 
		return false;
	}
	bool wasReleased(){
		if(wasreleased){
			wasreleased = false;
			return true;
		} 
		return false;
	}
	inline bool isLongPress(){
		return longstate;
	}

	inline void _setWasPressed(){
		prevstate = active_state_;
		waspressed = true;
	}
	inline void _setWasReleased(){
		prevstate = !active_state_;
		wasreleased = true;
	}
	inline void _setLongPress(){
		longstate = true;
	}
	inline void _resetLongPress(){
		longstate = false;
	}
	inline bool _readPin(){
		return gpio_pin_.read();
	}
	inline bool _getActiveState(){
		return active_state_;
	}
	inline bool _getPrevState(){
		return prevstate;
	}
	inline uint16_t _getLongPressTime(){
		return long_press_time_;
	}
private:
	GPIOPIN gpio_pin_;
	uint8_t active_state_;
	uint16_t long_press_time_;
	bool waspressed = false;
	bool wasreleased = false;
	bool longstate = false;
	bool prevstate;

};

class ENCODER {
public:
	ENCODER() = delete;

	ENCODER(GPIOPIN pina, GPIOPIN pinb, bool active_state) 
		: pina_(pina), pinb_(pinb), active_state_(active_state){
	}

	void checkState();

	inline void resetData(){
		encdata = 0;
	}

	inline bool getActiveState(){
		return active_state_;
	}
	inline uint8_t getPrevState(){
		return prev_state;
	}
	inline void setPrevState(uint8_t state){
		prev_state = state;
	}

	GPIOPIN pina_;
	GPIOPIN pinb_;
	uint8_t encdata = 0;
	int dir = 0;
private:
	bool active_state_;
	uint8_t prev_state;
};

inline void GPIO_enable_clock(GPIO_TypeDef *gpio){
	if(gpio == GPIOA){
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	} else if(gpio == GPIOB){
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	} else if(gpio == GPIOC){
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	} else if(gpio == GPIOD){
		RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
	} else if(gpio == GPIOE){
		RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;
	} 
	/*
	else if(gpio == GPIOF){
		RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;
	} else if(gpio == GPIOG){
		RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;
	}*/
}