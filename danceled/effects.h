#ifndef _EFFECTS_H
#define _EFFECTS_H

#include <Arduino.h>

void led_init();
void clear_led();

/*
音效描述：
1）HUE 色，气泡模式，气泡上升亮度和颜色渐变
*/
void music_pop();
void led_test();
/*
音效描述：
1）HUE 色，掉落效果
*/
void music_drop();
void music_drop_pro();
void music_dance();

void music_dot_dance();

void music_dance_single_color();

void music_RGB_drop();

void music_double_drop();

void music_extend();

void music_gathered();
void music_gathered_debug();
void music_up_down();

void music_fade();
void music_fade_single_color();
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
