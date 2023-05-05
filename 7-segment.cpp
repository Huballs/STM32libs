#include "7-segment.hpp"
#include "GPIO.hpp"
#include <array>
#include <algorithm>


void SevenSegment::split (int number){
    
    int d;
    _out_digits_N = 0;
    int i = 0;

    while(number){
        d = number % _base;
        number /= _base;
        _out_digits[i++] = d;
        _out_digits_N++;
        if(i == _n_digits) break;
    }
}

void SevenSegment::ResetSegments(){
    for(auto& pin : segment_pins){
        _OFF_SEG(pin);
    }
}

void SevenSegment::ResetDigits(){
    for(auto& pin : digits_pins){
        _OFF_DIG(pin);
    }
}

/* set zero indexed digit dot point*/
void SevenSegment::SetDP(int digit){
    _out_dp[digit] = 1;
}

/* reset zero indexed digit dot point*/
void SevenSegment::ResetDP(int digit){
    _out_dp[digit] = 0;
}

void SevenSegment::SetSegments(int digit){
    switch(digit){
        case 0: 
            _ON_SEG(segment_pins[0]); _ON_SEG(segment_pins[1]); _ON_SEG(segment_pins[2]);
            _ON_SEG(segment_pins[3]); _ON_SEG(segment_pins[4]); _ON_SEG(segment_pins[5]);
            break;
        case 1: 
            _ON_SEG(segment_pins[1]); _ON_SEG(segment_pins[2]);
            break;
        case 2: 
            _ON_SEG(segment_pins[0]); _ON_SEG(segment_pins[1]); _ON_SEG(segment_pins[6]);
            _ON_SEG(segment_pins[3]); _ON_SEG(segment_pins[4]);
            break;
        case 3: 
            _ON_SEG(segment_pins[0]); _ON_SEG(segment_pins[1]); _ON_SEG(segment_pins[2]);
            _ON_SEG(segment_pins[3]); _ON_SEG(segment_pins[6]);
            break;
        case 4: 
            _ON_SEG(segment_pins[1]); _ON_SEG(segment_pins[2]);
            _ON_SEG(segment_pins[6]); _ON_SEG(segment_pins[5]);
            break;
        case 5: 
            _ON_SEG(segment_pins[0]); _ON_SEG(segment_pins[5]); _ON_SEG(segment_pins[6]);
            _ON_SEG(segment_pins[2]); _ON_SEG(segment_pins[3]);
            break;
        case 6: 
            _ON_SEG(segment_pins[0]); _ON_SEG(segment_pins[6]); _ON_SEG(segment_pins[2]);
            _ON_SEG(segment_pins[3]); _ON_SEG(segment_pins[4]); _ON_SEG(segment_pins[5]);
            break;
        case 7: 
            _ON_SEG(segment_pins[0]); _ON_SEG(segment_pins[1]); _ON_SEG(segment_pins[2]);
            break;
        case 8: 
            _ON_SEG(segment_pins[0]); _ON_SEG(segment_pins[1]); _ON_SEG(segment_pins[2]);
            _ON_SEG(segment_pins[3]); _ON_SEG(segment_pins[4]); _ON_SEG(segment_pins[5]);
            _ON_SEG(segment_pins[6]);
            break;
        case 9: 
            _ON_SEG(segment_pins[0]); _ON_SEG(segment_pins[1]); _ON_SEG(segment_pins[2]);
            _ON_SEG(segment_pins[3]); _ON_SEG(segment_pins[6]); _ON_SEG(segment_pins[5]);
            break;
    }
}

void SevenSegment::DisplayNumber(int number){
    
    if(number != _old_number){
        /*  split digits into an array - _out_digits (inverted)
            _out_digits[4]: 4 1 - -
                number:        14 */
        split(number);

        if(_align_right)
            /* restore order (- - 1 4) for easier indexing when right aligned */
            std::reverse(_out_digits.begin(), _out_digits.end());
        _old_number = number;
    }

    if(_current_digit >= _n_digits){
        _current_digit = 0;
    }

    ResetDigits();
    ResetSegments();

    if(_align_right && (_n_digits - _current_digit <= _out_digits_N))
        SetSegments(_out_digits[_current_digit]);
    else if(_align_right && _leading_zeroes)
        SetSegments(0);

    if(!_align_right && (_current_digit < _out_digits_N))
        SetSegments(_out_digits[_out_digits_N - _current_digit - 1]);

    _ON_DIG(digits_pins[_current_digit]);

    ++_current_digit;
}
