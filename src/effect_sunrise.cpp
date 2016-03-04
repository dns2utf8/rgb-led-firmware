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

  boolean operator==(NColor const o) const {
    return r == o.r && g == o.g && b == o.b;
  }
};

static NColor current{ r: 0, g: 0, b: 0 }, input{};
static const NColor off{ r: 0, g: 0, b: 0 }, night{ r: 0, g: 0, b: 23 }, dark_red{ r: 50, g: 0, b: 0 }, red{ r: 170, g: 0, b: 0 }, orange{ 180, 100, 0 }, morning{ r: 200, 70, 23 }, day{ r: 255, g: 255, b: 255 };
static const uint8_t wheel_length = 9;
static const NColor wheel[wheel_length] = { off, dark_red, red, orange, morning, day, morning, orange, dark_red };

static boolean first_run = true;
static int resume_count_down = 0;
static const int max_resume_count_down = 420, init_speed = 666;

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

void sunrise_step(void);

// Main loop
void effect_sunrise(void) {
    if (first_run) {
      set_colors();
      /*delay(init_speed);
      
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
      set_colors();*/
    }
    
    uint8_t r = normalized_read(POT_1);
    uint8_t g = normalized_read(POT_2);
    uint8_t b = normalized_read(POT_3);

    if (first_run) {
      input.r = r;
      input.g = g;
      input.b = b;
      
      first_run = false;
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
      sunrise_step();
    }

    set_colors();
  
    // Sleep
    delay(13);
}

static const int8_t max_step_size = 1;
int8_t safe_step(uint8_t c, uint8_t t) {
  if (c < t && c < 255) {
    return 1;
  }
  if (c > t && c > 0) {
    return -1;
  }
  return 0;
}

static uint8_t wheel_step = 0;
static NColor target = wheel[0];
void sunrise_step(void) {
  if (current == target) {
    wheel_step = (wheel_step +1) % wheel_length;
    target = wheel[ wheel_step ];
    
  } else {
    current.r += safe_step(current.r, target.r);
    current.g += safe_step(current.g, target.g);
    current.b += safe_step(current.b, target.b);
  }

  delay(66);
}

