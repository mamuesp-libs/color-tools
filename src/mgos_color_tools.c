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

#include "mgos_color_tools.h"

bool mgos_color_tools_init(void)
{

    if (!mgos_sys_config_get_color_tools_enable()) {
        return true;
    }

    return true;
}

tools_rgb_array *tools_config_get_color_arr(const char* data, const char separator)
{
    char** str_colors;
    tools_rgb_array *result = calloc(1, sizeof(tools_rgb_array));
    result->len = tools_str_split(data, separator, &str_colors);
    result->data = result->len > 0 ? calloc(result->len, sizeof(tools_rgb_data)) : NULL;
    for (int i = 0; i < result->len; i++) {
        char** str_rgb;
        if (tools_str_split(str_colors[i], ',', &str_rgb) == 3) {
            tools_set_color(&result->data[i], atoi(str_rgb[0]), atoi(str_rgb[1]), atoi(str_rgb[2]), 1);
            tools_str_split_free(str_rgb, 3);
        }
    }
    tools_str_split_free(str_colors, result->len);
    return result;
}

typedef struct {
    double r; // a fraction between 0 and 1
    double g; // a fraction between 0 and 1
    double b; // a fraction between 0 and 1
} rgb;

typedef struct {
    double h; // angle in degrees
    double s; // a fraction between 0 and 1
    double v; // a fraction between 0 and 1
} hsv;

static hsv rgb2hsv(rgb in);
static rgb hsv2rgb(hsv in);

static hsv rgb2hsv(rgb in)
{
    /*
    hsv out;

    double max = maxval(in.r, in.g, in.b);
    double min = minval(in.r, in.g, in.b);

    out.h = max;
    out.s = max;
    out.v = max;

    double d = max - min;
    out.s = (max == 0.0) ? 0.0 : (d / max);
    if (max == min) {
        out.h = 0; // achromatic
    } else {
        if (max == in.r) {
            out.h = (in.g - in.b) / (d + (in.g < in.b ? 6.0 : 0.0));
        } else if (max == in.g) {
            out.h = (in.b - in.r) / (d + 2.0);
        } else if (max == in.b) {
            out.h = (in.r - in.g) / (d + 4.0);
        }
        out.h = out.h / 6.0;
    }

    return out;
*/
    hsv out;
    double min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min < in.b ? min : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max > in.b ? max : in.b;

    out.v = max; // v
    delta = max - min;
    if (delta < 0.00001) {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max); // s
    } else {
        // if max is 0, then r = g = b = 0
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN; // its now undefined
        return out;
    }
    if (in.r >= max) // > is bogus, just keeps compilor happy
        out.h = (in.g - in.b) / delta; // between yellow & magenta
    else if (in.g >= max)
        out.h = 2.0 + (in.b - in.r) / delta; // between cyan & yellow
    else
        out.h = 4.0 + (in.r - in.g) / delta; // between magenta & cyan

    out.h *= 60.0; // degrees

    if (out.h < 0.0)
        out.h += 360.0;

    return out;
}

static rgb hsv2rgb(hsv in)
{
    rgb out;

    // range of 0.0 .. 1.0 needed ...
    in.h /= 360.0;

    int i = (int)floor(in.h * 6.0);
    double f = in.h * 6.0 - i;
    double p = in.v * (1.0 - in.s);
    double q = in.v * (1.0 - f * in.s);
    double t = in.v * (1.0 - (1.0 - f) * in.s);

    switch (i % 6) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;
    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }

    return out;
}

void tools_rgb_to_hsv(tools_rgb_data in, double* h, double* s, double* v)
{
    rgb d_in;
    hsv out;

    d_in.r = in.r / 255.0;
    d_in.g = in.g / 255.0;
    d_in.b = in.b / 255.0;

    out = rgb2hsv(d_in);

    *h = out.h;
    *s = out.s;
    *v = out.v;
}

tools_rgb_data tools_hsv_to_rgb(double h, double s, double v)
{
    tools_rgb_data out;
    rgb d_out;
    hsv in;

    in.h = h;
    in.s = s;
    in.v = v;

    d_out = hsv2rgb(in);

    out.r = (uint8_t)round(d_out.r * 255.0);
    out.g = (uint8_t)round(d_out.g * 255.0);
    out.b = (uint8_t)round(d_out.b * 255.0);

    return out;
}

tools_rgb_data tools_color_wheel(double wheel_pos, double base)
{
    tools_rgb_data color;
    rgb out_rgb;
    hsv in_hsv;

    in_hsv.h = (wheel_pos / base) * 360.0;
    in_hsv.s = 1.0;
    in_hsv.v = 1.0;
    out_rgb = hsv2rgb(in_hsv);

    color.r = (uint8_t)round(out_rgb.r * 255.0);
    color.g = (uint8_t)round(out_rgb.g * 255.0);
    color.b = (uint8_t)round(out_rgb.b * 255.0);

    return color;
}

tools_rgb_data tools_fade_color(tools_rgb_data start, double fade)
{
    tools_rgb_data color;
    double h, s, v;
    tools_rgb_to_hsv(start, &h, &s, &v);
    color = tools_hsv_to_rgb(h, s, v * fade);
    return color;
}

double tools_check_color_distance(tools_rgb_data* start, uint16_t count, double h_test)
{
    tools_rgb_data* col;
    double h, s, v, dist, min = 9999.0;

    for (int i = 0; i < count; i++) {
        col = start + (i * sizeof(tools_rgb_data));
        tools_rgb_to_hsv(*col, &h, &s, &v);
        dist = fabs(h - h_test);
        min = (dist < min) ? dist : min;
    }

    return min;
}

tools_rgb_data tools_get_random_color(tools_rgb_data start, tools_rgb_data* test, uint16_t count, double min_dist)
{
    return tools_get_random_color_fade(start, test, count, min_dist, 1.0, 0.4);
}

tools_rgb_data tools_get_random_color_fade(tools_rgb_data start, tools_rgb_data* test, uint16_t count, double min_dist, double s_new, double v_new)
{
    double h, s, v, new_h;
    tools_rgb_to_hsv(start, &h, &s, &v);

    if ((start.r + start.g + start.b) < 10.0) {
        h = (double)tools_get_random(0, 360);
    } else {
        tools_rgb_to_hsv(start, &h, &s, &v);
    }

    do {
        tools_get_random(0, 360);
        new_h = (double)tools_get_random(0, 360);
    } while (tools_check_color_distance(test, count, new_h) < 30.0);

    LOG(LL_DEBUG, ("Generate random color: Diff %03.02f, Old H %03.02f, New H %03.02f", fabs(h - new_h), h, new_h));
    LOG(LL_DEBUG, ("Generate random color from R: 0x%02X, G: 0x%02X, B: 0x%02X", start.r, start.g, start.b));

    h = new_h;
    s = s_new;
    v = v_new;
    LOG(LL_DEBUG, ("Generate random color with H: %.02f, S: %.02f, V: %.02f", h, s, v));
    tools_rgb_data result = tools_hsv_to_rgb(h, s, v);
    LOG(LL_DEBUG, ("Generated random color with R: 0x%02X, G: 0x%02X, B: 0x%02X", result.r, result.g, result.b));

    return result;
}

void tools_set_color(tools_rgb_data* color, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
}

void tools_config_get_color(char* fmt, char* key, tools_rgb_data* color)
{
    size_t elems;
    char** conf_col = tools_config_get_dyn_arr(fmt, key, &elems);
    if (conf_col) {
        if (elems == 3) {
            color->r = atoi(conf_col[0]);
            color->g = atoi(conf_col[1]);
            color->b = atoi(conf_col[2]);
        }
        if (elems == 4) {
            color->a = atoi(conf_col[3]);
        }
        tools_str_split_free(conf_col, elems);
    }
}

tools_rgb_data tools_hexcolor_str_to_rgb(char* hex_val)
{
    uint32_t number = (int)strtol(hex_val, NULL, 0);
    return tools_hexcolor_to_rgb(number);
}

tools_rgb_data tools_hexcolor_to_rgb(uint32_t hex_val)
{
    tools_rgb_data color;

    if (hex_val > 0xFFFFFF) {
        color.r = ((hex_val & 0xFF000000) >> 24) & 0xFF;
        color.g = ((hex_val & 0x00FF0000) >> 16) & 0xFF;
        color.b = ((hex_val & 0x0000FF00) >> 8) & 0xFF;
        color.a = ((hex_val & 0x000000FF)) & 0xFF;
    } else {
        color.r = ((hex_val & 0xFF0000) >> 16) & 0xFF;
        color.g = ((hex_val & 0x00FF00) >> 8) & 0xFF;
        color.b = ((hex_val & 0x0000FF)) & 0xFF;
        color.a = 255;
    }

    return color;
}
