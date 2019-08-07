/**
 *
 * Copyright (c) 2018 Manfred Mueller-Spaeth <fms1961@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 * A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **/
#ifndef __TOOLS_COLOR_TOOLS_H
#define __TOOLS_COLOR_TOOLS_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include "frozen.h"
#include "mgos.h"
#include "mgos_wifi.h"
#include "mgos_app.h"
#include "mgos_config.h"
#include "mgos_ro_vars.h"
#include "mgos_common_tools.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} tools_rgb_data;

typedef struct {
    tools_rgb_data* data;
    uint32_t len;
} tools_rgb_array;

tools_rgb_array* tools_create_rgb_array(uint32_t size);
void tools_free_rgb_arr(tools_rgb_array* arr);
tools_rgb_array *tools_config_get_color_arr(const char* data, const char separator);

void tools_rgb_to_hsv(tools_rgb_data in, double* h, double* s, double* v);
tools_rgb_data tools_hsv_to_rgb(double h, double s, double v);

tools_rgb_data tools_get_random_color(tools_rgb_data start, tools_rgb_data* test, uint16_t count, double min_dist);
tools_rgb_data tools_get_random_color_fade(tools_rgb_data start, tools_rgb_data* test, uint16_t count, double min_dist, double s_new, double v_new);
double tools_check_color_distance(tools_rgb_data* start, uint16_t count, double h_test);
void tools_config_get_color(char* fmt, char* key, tools_rgb_data* color);
tools_rgb_data tools_color_wheel(double wheel_pos, double base);
tools_rgb_data tools_fade_color(tools_rgb_data start, double fade);

tools_rgb_array *tools_config_get_color_arr(const char* data, const char separator);

void tools_set_color(tools_rgb_data* color, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
tools_rgb_data tools_hexcolor_to_rgb(uint32_t hex_val);
tools_rgb_data tools_hexcolor_str_to_rgb(char* hex_val);

bool tools_comlor_tools_init(void);

#endif // __TOOLS_COLOR_TOOLS_H
