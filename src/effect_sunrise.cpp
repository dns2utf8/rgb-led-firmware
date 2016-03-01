/**
 * Effect: Controllable RGB values
 *
 * Copyright (c) 2014--2015 Coredump Rapperswil
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

// Get arduino headers
#include <Arduino.h>

// Get pin definitions
#include "pins.h"


// Get poti values and map them to 0..255
uint8_t normalized_read(int pin) {
  int v = analogRead(pin) / 4;
  v = 255 - v;

  if (v < 0) {
    return 0;
  }
  if (v > 255) {
    return 255;
  }
  return v;
}

struct NColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

static NColor current{ r: 0, g: 0, b: 0 }, input{}, target{}, dark_red{ r: 50, g: 0, b: 0 }, red{ r: 200, 70, 23 };
static boolean first_run = true;
static int resume_count_down = 0;
static int max_resume_count_down = 420, init_speed = 666;

template<typename T>
boolean within_range(T a, T b) {
  return abs(a - b) > 13;
}

boolean has_changed_userinput(uint8_t r, uint8_t g, uint8_t b) {
  return within_range(r, input.r) || within_range(g, input.g) || within_range(b, input.b);
}

void set_colors(void) {
    // Set colors
    analogWrite(LED_R, current.r);
    analogWrite(LED_G, current.g);
    analogWrite(LED_B, current.b);
}

// Main loop
void effect_sunrise(void) {
    if (first_run) {
      set_colors();
      delay(init_speed);
      
      current.r = 255;
      set_colors();
      delay(init_speed);
      
      current.g = 255;
      current.r = 0;
      set_colors();
      delay(init_speed);
      
      current.b = 255;
      current.g = 0;
      set_colors();
      delay(init_speed);
      
      current.b = 0;
      set_colors();
    }
    
    uint8_t r = normalized_read(POT_1);
    uint8_t g = normalized_read(POT_2);
    uint8_t b = normalized_read(POT_3);

    if (first_run) {
      input.r = r;
      input.g = g;
      input.b = b;
      
      first_run = false;

      current = dark_red;
    }

    if (has_changed_userinput(r, g, b)) {
      resume_count_down = max_resume_count_down;
      input.r = r;
      input.g = g;
      input.b = b;
    }
    if (resume_count_down > 0) {
      current.r = r;
      current.g = g;
      current.b = b;

      --resume_count_down;
    } else {
      current = red;
    }

    set_colors();
  
    // Sleep
    delay(13);
}

