#ifndef _EFFECTS_H
#define _EFFECTS_H

#include <Arduino.h>

void led_init();
/*
音效描述：
1）HUE 色，气泡模式，气泡上升亮度和颜色渐变
*/
void music_pop();

/*
音效描述：
1）HUE 色，掉落效果
*/
void music_drop();

void music_RGB_drop();

void music_double_drop();

void music_extend();

void music_gathered();

void music_up_down();

void musicWrite3();

void musicWrite2();

void musicWrite1();

int get_effects();

int set_effects( int effect);

int set_brightness(int brightness);

int get_brightness();

int get_trigger_level();

int set_trigger_level(int level);

int get_color();

int set_color(int color);

#endif
