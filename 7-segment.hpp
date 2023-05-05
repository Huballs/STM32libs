#pragma once
#include <array>
#include <algorithm>

#include "GPIO.hpp"

#define _SEG_ACTIVE 1   // 0 - low, 1 - high
#define _DIG_ACTIVE 1

#if(_SEG_ACTIVE == 1)
    #define _ON_SEG(pin) (pin.set())
    #define _OFF_SEG(pin) (pin.reset())
#endif

#if(_SEG_ACTIVE == 0)
    #define _ON_SEG(pin) (pin.reset())
    #define _OFF_SEG(pin) (pin.set())
#endif

#if(_DIG_ACTIVE == 1)
    #define _ON_DIG(pin) (pin.set())
    #define _OFF_DIG(pin) (pin.reset())
#endif

#if(_DIG_ACTIVE == 0)
    #define _ON_DIG(pin) (pin.reset())
    #define _OFF_DIG(pin) (pin.set())
#endif

template <const int Ndigits>
class SevenSegment {

public:
    using digits_t = std::array<GPIOPIN, Ndigits>;
    // pins of digits 1 to 4
    digits_t *digits_pins;
    
    using segm_t = std::array<GPIOPIN, 8>;
    // pins of segments A to DP
    segm_t *segment_pins;

    explicit SevenSegment(digits_t *digits, segm_t *segments) 
        : digits_pins(digits), segment_pins(segments){
            _out_dp.fill(0);
        };
    
    SevenSegment() = delete;

    void DisplayNumber(int number){
        
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

        if(_current_digit >= Ndigits){
            _current_digit = 0;
        }

        ResetDigits();
        ResetSegments();

        if(_align_right && (Ndigits - _current_digit <= _out_digits_N))
            SetSegments(_out_digits[_current_digit]);
        else if(_align_right && _leading_zeroes)
            SetSegments(0);

        if(!_align_right && (_current_digit < _out_digits_N))
            SetSegments(_out_digits[_out_digits_N - _current_digit - 1]);

        _ON_DIG((*digits_pins)[_current_digit]);

        ++_current_digit;
    }

    void SetDP(int digit){
        _out_dp[digit] = 1;
    }

    void ResetDP(int digit){
        _out_dp[digit] = 0;
    }

private:

    void split (int number){
    
        int d;
        _out_digits_N = 0;
        int i = 0;

        while(number){
            d = number % _base;
            number /= _base;
            _out_digits[i++] = d;
            _out_digits_N++;
            if(i == Ndigits) break;
        }
    }

    void ResetSegments(){
        for(auto& pin : *segment_pins){
            _OFF_SEG(pin);
        }
    }

    void ResetDigits(){
        for(auto& pin : *digits_pins){
            _OFF_DIG(pin);
        }
    }

    void SetSegments(int digit){
        switch(digit){
            case 0: 
                _ON_SEG((*segment_pins)[0]); _ON_SEG((*segment_pins)[1]); _ON_SEG((*segment_pins)[2]);
                _ON_SEG((*segment_pins)[3]); _ON_SEG((*segment_pins)[4]); _ON_SEG((*segment_pins)[5]);
                break;
            case 1: 
                _ON_SEG((*segment_pins)[1]); _ON_SEG((*segment_pins)[2]);
                break;
            case 2: 
                _ON_SEG((*segment_pins)[0]); _ON_SEG((*segment_pins)[1]); _ON_SEG((*segment_pins)[6]);
                _ON_SEG((*segment_pins)[3]); _ON_SEG((*segment_pins)[4]);
                break;
            case 3: 
                _ON_SEG((*segment_pins)[0]); _ON_SEG((*segment_pins)[1]); _ON_SEG((*segment_pins)[2]);
                _ON_SEG((*segment_pins)[3]); _ON_SEG((*segment_pins)[6]);
                break;
            case 4: 
                _ON_SEG((*segment_pins)[1]); _ON_SEG((*segment_pins)[2]);
                _ON_SEG((*segment_pins)[6]); _ON_SEG((*segment_pins)[5]);
                break;
            case 5: 
                _ON_SEG((*segment_pins)[0]); _ON_SEG((*segment_pins)[5]); _ON_SEG((*segment_pins)[6]);
                _ON_SEG((*segment_pins)[2]); _ON_SEG((*segment_pins)[3]);
                break;
            case 6: 
                _ON_SEG((*segment_pins)[0]); _ON_SEG((*segment_pins)[6]); _ON_SEG((*segment_pins)[2]);
                _ON_SEG((*segment_pins)[3]); _ON_SEG((*segment_pins)[4]); _ON_SEG((*segment_pins)[5]);
                break;
            case 7: 
                _ON_SEG((*segment_pins)[0]); _ON_SEG((*segment_pins)[1]); _ON_SEG((*segment_pins)[2]);
                break;
            case 8: 
                _ON_SEG((*segment_pins)[0]); _ON_SEG((*segment_pins)[1]); _ON_SEG((*segment_pins)[2]);
                _ON_SEG((*segment_pins)[3]); _ON_SEG((*segment_pins)[4]); _ON_SEG((*segment_pins)[5]);
                _ON_SEG((*segment_pins)[6]);
                break;
            case 9: 
                _ON_SEG((*segment_pins)[0]); _ON_SEG((*segment_pins)[1]); _ON_SEG((*segment_pins)[2]);
                _ON_SEG((*segment_pins)[3]); _ON_SEG((*segment_pins)[6]); _ON_SEG((*segment_pins)[5]);
                break;
        }
    }


    const int _base = 10;
    const bool _align_right = true;
    const bool _leading_zeroes = true;
    
    int _out_digits_N;
    std::array<int, Ndigits> _out_digits;
    std::array<int, Ndigits> _out_dp; // [digit] = 1 - dot point on
    int _old_number;
    int _current_digit;

};
