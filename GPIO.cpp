#include "GPIO.hpp"

#if defined (STM32F1)

static void _GPIO_set_mode(GPIO_TypeDef* port_, uint16_t pin_, IOConf mode){
	uint32_t crreg ;

	if (mode == IOConf::INPUT_PULLUP){
		crreg = 1 << pin_;
		port_->ODR |= crreg;
	} else if (mode == IOConf::INPUT_PULLDOWN){
		crreg = 1 << pin_;
		port_->ODR &= ~crreg;
		mode = IOConf::INPUT_PULLUP;	// CNF 10 - input pullup/pulldown
	}

	if (pin_ < 8){
		crreg = 0xF << (pin_ * 4);		// set to 0 CNF and mode
		port_->CRL &= ~crreg;
		crreg = static_cast<uint32_t>(mode) << (pin_ * 4);
		port_->CRL |= crreg;
	}	else {
		uint8_t _pin = pin_- 8;
		crreg = 0xF << (_pin * 4);		// set to 0 CNF and mode
		port_->CRH &= ~crreg;
		crreg = static_cast<uint32_t>(mode)<< (_pin * 4);
		port_->CRH |= crreg;
	}
}
#endif // STM32F1

static void Button_check(BUTTON *button){
	bool state = button->_readPin();
	static uint16_t _long_press_time;

 	// press
	if ((state == button->_getActiveState())
		&& (button->_getPrevState() != state)
		&& (button->debouncer_->isDone()))
	{
		button->debouncer_->start();
		button->_setWasPressed();
	}

	// release
	if ((state != button->_getActiveState())
		&& (button->_getPrevState() == button->_getActiveState())
		&&  (button->debouncer_->isDone()))
	{
		button->debouncer_->start();
		button->_setWasReleased();
		_long_press_time = 0;
		button->_resetLongPress();
	}

	// long press
	if ((state == button->_getActiveState())
		&& (button->_getPrevState() == button->_getActiveState())
		&& (button->debouncer_->isDone())
		&& (button->isLongPress() == false))
	{
		_long_press_time += 1;
		if (_long_press_time >= button->_getLongPressTime())  button->_setLongPress();
	} 
}

static void check_encoder_state(ENCODER *encoder){
	uint8_t state = encoder->pina_.read();
	state = state | (encoder->pinb_.read() << 1);
	if (encoder->_getActiveState() == 1) {
		state = ~state;
		state &= 0x3;
	}/*
	switch(encoder->PREVSTATE)
		{
		case 2:
			{
			if(state == 3) encoder->ENCDATA++;
			if(state == 0) encoder->ENCDATA--;
			break;
			}
		case 0:
			{
			if(state == 2) encoder->ENCDATA++;
			if(state == 1) encoder->ENCDATA--;
			break;
			}
		case 1:
			{
			if(state == 0) encoder->ENCDATA++;
			if(state == 3) encoder->ENCDATA--;
			break;
			}
		case 3:
			{
			if(state == 1) encoder->ENCDATA++;
			if(state == 2) encoder->ENCDATA--;
			break;
			}
		}
*/
	if (encoder->_getPrevState() == 0x3){
		if(state == 2) {encoder->encdata++; encoder->dir =  1; encoder->_was_turned_right = true;}
		if(state == 1) {encoder->encdata--; encoder->dir = -1; encoder->_was_turned_left = true;}
	}

	encoder->_setPrevState(state);
}


#if defined (STM32F3)
	
#endif // STM32F3

/*-------------------------------------Class GPIOPIN start---------------------------------*/
/*GPIOPIN::GPIOPIN(GPIO_TypeDef* port, uint8_t pin)
    : port_(port), pin_(pin) {
}*/

GPIOPIN::GPIOPIN(const GPIOPIN& other) 
	: port_(other.port_), pin_(other.pin_), mode_(other.mode_){
}

GPIOPIN::GPIOPIN(GPIO_TypeDef* port, uint8_t pin, IOConf mode)
    : port_(port), pin_(pin), mode_(mode) {
		GPIO_enable_clock(port);
		setMode();
}

void GPIOPIN::setMode(IOConf mode){
    _GPIO_set_mode(port_, pin_, mode);
}
void GPIOPIN::setMode(){
    _GPIO_set_mode(port_, pin_, mode_);
}

 void GPIOPIN::toggle(){
    if(read()){
        reset();
    } else {
        set();
    }
}
/*-------------------------------------Class GPIOPIN end-----------------------------------*/


/*-------------------------------------Class BUTTON start----------------------------------*/
BUTTON::BUTTON(GPIOPIN gpio_pin, uint8_t active_state, ButtonDebouncer* debouncer, uint16_t long_press_time)
    : debouncer_(debouncer), gpio_pin_(gpio_pin), active_state_(active_state)
	, long_press_time_(long_press_time), prevstate(!active_state_){

}

void BUTTON::checkState(){
    Button_check(this);
}
/*
void BUTTON::configurePin(IOConf mode){
	gpio_pin_.set_mode(mode);
}
void BUTTON::configurePin(){
	gpio_pin_.set_mode();
}*/

/*-------------------------------------Class BUTTON end--------------------------------*/

/*-------------------------------------Class ENCODER start-----------------------------*/
void ENCODER::checkState(){
	check_encoder_state(this);
}
/*-------------------------------------Class ENCODER end-------------------------------*/