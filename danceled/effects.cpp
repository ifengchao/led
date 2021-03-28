#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#include <Arduino.h>

#define LED_PIN    5   //灯带信号--引脚6
#define MIC_PIN    A0  //MAX9814 OUT引脚
#define NUMPIXELS 32
//灯数
#define delay_time 20      //闪灯间隔
#define SEG_4_NUM   4

#define COLOR_ORDER GRB    // 色序
#define LED_TYPE WS2812    // LED灯带型号

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
CRGB leds[NUMPIXELS];            // 建立光带leds

unsigned long drop_time;//触发掉落时机

int sig_max = 800;   //初始最大值

int si;
int pre_si;

int g_trigger_level = 300;    // 麦克风灵敏度，麦克风采样范围0-1024
int g_color = 255;              //起始颜色
int maxBrightness = 250;      //最大亮度
int effects = 0;              //默认灯效
int brightness_delta = int(maxBrightness / NUMPIXELS);
int color_delta = int(g_color / NUMPIXELS);
  
int drop_dot_color = 130;
int drop_dot_brightness = 255;
int start_dot = NUMPIXELS/2-3;


int set_brightness(int brightness){
  Serial.printf("maxBrightness = %d\n",maxBrightness);
  maxBrightness = brightness;
  brightness_delta = maxBrightness / NUMPIXELS;
  return 0;  
}

int get_brightness(){
  Serial.printf("maxBrightness = %d\n",maxBrightness);
  return maxBrightness;
}

int get_trigger_level(){
  Serial.printf("g_trigger_level = %d\n",g_trigger_level);
  return g_trigger_level;
}

int set_trigger_level(int level){
  Serial.printf("g_trigger_level = %d\n",g_trigger_level);
  g_trigger_level = level;
  return 0;  
}

int set_color(int color){
  Serial.printf("g_color = %d\n",g_color);
  g_color = color;
  color_delta = int(g_color / NUMPIXELS);
  return 0;
}

int get_color(){
  Serial.printf("g_color = %d\n",g_color);
  return g_color;
}

int get_effects(){
  //Serial.printf("effects = %d\n",effects);
  return effects;  
}

int set_effects( int effect){
  Serial.printf("effects = %d\n",effects);
  effects = effect;
  return 0;
}

void led_test(){
  
  fill_rainbow(leds, 32, 0, 5);
  FastLED.show();
  delay(1000);
  FastLED.clear(); 
  FastLED.show(); 
}

//上升，逐次点亮
void  led_init(){
  // LED init
  LEDS.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUMPIXELS);
  FastLED.setBrightness(maxBrightness);  
  led_test();
#if 1  
  pixels.begin();           // INITIALIZE NeoPixel pixels object (REQUIRED)
  pixels.clear();
  pixels.show();            // Turn OFF all pixels ASAP
  pixels.setBrightness(150); // Set BRIGHTNESS to about 1/5 (max = 255)
#endif

#if 0
  int delta = int(250 / NUMPIXELS);
  int j = 0;
  for (j = 0; j < NUMPIXELS; j++)
  {
    pixels.setPixelColor(j, pixels.Color(delta * j, 0, 255 - delta * j));
    //pixels.setPixelColor(j, pixels.Color(250, 0, 0));
    pixels.show();
    delay(5);//点亮速度，上升速度
    //delay(delay_time); ///
  }
  
  //pixels.clear();
  //pixels.show();
  //delay(300);

  for ( j; j >= 0; j--)
  {
    pixels.setPixelColor(j, pixels.Color(0, 0, 0));
    //pixels.setPixelColor(j, pixels.Color(250, 0, 0));
    pixels.show();
    delay(8);//灭灯速度
    //delay(delay_time); ///
  }
  pixels.clear();
  pixels.show();
#endif  
  delay(100);  

}

int drop_dot = 0;
int drop_dot2 = 1;
int pre_drop_dot = 0;

void clear_led(){
  //pixels.clear();
  //pixels.show();
  FastLED.clear();
  FastLED.show();
}

/*
音效描述：
1）HUE 色，气泡模式，气泡上升亮度和颜色渐变
*/
void music_pop(){

  int SEG_NUM = 10;
  int brightness_delta = maxBrightness/SEG_NUM;
  int color_delta = g_color /SEG_NUM;
  int sig = analogRead(MIC_PIN);//out引脚
  int start_dot = 0;
  //int delta = int(250 / NUMPIXELS);
  //int delta = int(120 / NUMPIXELS);
  int delta = random(0,250)/NUMPIXELS;//随机下颜色
  Serial.println(sig);
  
  if (sig >= g_trigger_level+50)
  //while(1)
  { 
    //Serial.println(sig);
    //int si = map(sig, triger_level, sig_max, 0, NUMPIXELS);
    for(int j = 0; j < NUMPIXELS; j++){
        for(int i = 0; i < SEG_NUM; i++){
            start_dot = j+i;
            int tmp_dot = start_dot%NUMPIXELS;// NUMPIXELS 为模，获取偏移位置
            if( tmp_dot >= SEG_NUM)
            {
              fill_solid(leds+tmp_dot-SEG_NUM, 1, CHSV(0, 255, 0));
            }
            
            fill_solid(leds+tmp_dot, 1, CHSV((g_color+i * color_delta)%255, 255, i * brightness_delta));
        }
        FastLED.show();
        delay(10);
    }
  }
}

/*
音效描述：
1）HUE 色，4点跳跃模式
*/
void music_dot_dance(){

  int SEG_NUM = 8;
  int brightness_delta = maxBrightness/SEG_4_NUM;
  int color_delta = g_color /SEG_NUM;
  int sig = analogRead(MIC_PIN);//out引脚

  int delta = random(0,250)/NUMPIXELS;//随机下颜色
  //Serial.println(sig);
  int trigger = g_trigger_level/5;//灵敏度
  if (sig >= 400 && sig - 400 > trigger ){
    si = sig - 400;
    Serial.printf("up %d\n",sig - 400);
  }else if(sig <= 400 && 400 - sig > trigger){
    si = 400 - sig;
    Serial.printf("down %d\n",400 - sig);
  }else{
    si = 0;  
  }

  if(si > trigger){
    
    if(si > pre_si){// 声音增大
  
      int move_step = map(si, 0, 200, 0, SEG_4_NUM);//15 步
      Serial.printf("move_step %d\n",move_step);      
      
      for(int y = 0; y < move_step; y++){
          // 关掉上次开始位置 要移动的几个点
         fill_solid(leds+(start_dot+y)%NUMPIXELS, 1, CHSV(0, 255, 0));         
      }
      FastLED.show();
      delay(50);
      if(start_dot+move_step <= NUMPIXELS-SEG_4_NUM){
        start_dot += move_step;
      
        for(int x = 0; x < SEG_4_NUM; x++){     
            //fill_solid(leds+(start_dot+x)%NUMPIXELS, 1, CHSV((g_color+x * color_delta)%255, 255, x * brightness_delta));
            fill_solid(leds+(start_dot+x)%NUMPIXELS, 1, CHSV(g_color, 255, (x+1) * brightness_delta));
        }
        FastLED.show();
        delay(50);
    
        start_dot = start_dot % NUMPIXELS;
        Serial.printf("start_dot %d\n",start_dot);
      }    
      
      pre_si = si;
    }else{  //声音变小
      int move_step = map(si, 0, 200, 0, SEG_4_NUM);//15 步
         for(int y = 0; y < move_step; y++){
          // 关掉要移动的几个点
         fill_solid(leds+(start_dot+SEG_4_NUM-y)%NUMPIXELS, 1, CHSV(0, 255, 0));
      }    
      FastLED.show();
      delay(50);
      
      if(start_dot - move_step >=0){
        start_dot -= move_step;
      
          for(int x = 0; x < SEG_4_NUM; x++){     
              //fill_solid(leds+(start_dot+x)%NUMPIXELS, 1, CHSV((g_color+x * color_delta)%255, 255, (SEG_4_NUM-x) * brightness_delta));
              fill_solid(leds+(start_dot+x)%NUMPIXELS, 1, CHSV(g_color, 255, (SEG_4_NUM-x) * brightness_delta));
          }
          FastLED.show();
          delay(50); 
          start_dot = start_dot % NUMPIXELS;     
     }     
            
      Serial.printf("start_dot %d\n",start_dot);
      pre_si = si;
    }
      
  } 
 
}

/*
音效描述：
1）HUE 色，上推+掉落效果
*/
void music_drop_pro(){
        
  int sig = analogRead(MIC_PIN);//out引脚
  
  int delta = int(g_color / NUMPIXELS);
  //int delta = int(120 / NUMPIXELS);
  //int delta = random(0,250)/NUMPIXELS;//随机下颜色
  if (sig > sig_max)
  {
    sig = sig_max;// 防止超范围
  }
  int trigger = 50;
  if (sig >= 400 && sig - 400 > trigger ){
    //si = sig - 400;
    int sig_delta = sig - 400;
    si = map(sig_delta, 0, 200, 0, NUMPIXELS-2);//最高点留给掉落点,最大值30
    //Serial.printf("up %d\n",sig - 400);
  }else if(sig <= 400 && 400 - sig > trigger){
    int sig_delta = 400 - sig;
    si = map(sig_delta, 0, 200, 0, NUMPIXELS-2);//最高点留给掉落点,最大值30
    //Serial.printf("down %d\n",400 - sig);
  }else{
    si = 0;  
  }

  if(si > 0)
  { 
    Serial.println(si);
    if(si > NUMPIXELS -2) si = NUMPIXELS-2;
    
    //int si = map(sig, g_trigger_level , sig_max, 0, NUMPIXELS-2);//最高点留给掉落点,最大值30
    if(si > pre_si){//音量更大
      
      int j = 0;

      if(si > drop_dot){ //如果这次音量最大值没有达到上次掉落的最小值，就不管它，继续掉落，反之，重新上升掉落点

        drop_dot = si+1;//掉落点要高于本次音量
 
        //if(millis() - drop_time > 300)//掉落间隔
        { 
          //Serial.printf("si  %d\n", si); 
          Serial.printf("drop_dot %d\n", drop_dot); 
          if(pre_drop_dot != drop_dot){
            fill_solid(leds + pre_drop_dot, 1, CHSV(drop_dot_color, 255, 0));
          }
          fill_solid(leds + drop_dot, 1, CHSV(drop_dot_color, 255, maxBrightness));
          FastLED.show();
          //drop_time = millis();
        }        
      }
      if(si == drop_dot){ 
          si--;//不能覆盖了掉落点
          //Serial.printf("drop_dot = si %d\n", drop_dot); 
      }
      for(j = pre_si; j <= si; j++){
        //if(j < si)
        {          
          fill_solid(leds + j, 1, CHSV((g_color+j * color_delta)%255, 255, (NUMPIXELS-j) * brightness_delta));        
          //fill_solid(leds + j, 1, CHSV(j * delta, 255, maxBrightness));
          
        }
      }
      FastLED.show();          
      delay(5);    
      pre_si = si;  
    }else{//音量变小
      for(int j = pre_si; j >= si && j > 0; j--){
        fill_solid(leds + j, 1, CHSV(j * delta, 255, 0));
        FastLED.show();
        delay(10);
      }
      pre_si = si; 
    }
  }
  
  {//一直掉落
      //for(int i = 0;drop_dot >= si; i ++)
      unsigned long interval = map(pre_si - si, 0, 31, 800, 5);//取5-100ms的掉落速度，两次音量落差越大，降速越快
      //Serial.printf("time %ld,%ld\n",millis(),drop_time); 
      if(millis() - drop_time > 200)//掉落间隔
      {        
        if(drop_dot>0)
        {
          fill_solid(leds + drop_dot--, 1, CHSV(130, 255, 0));
          fill_solid(leds + drop_dot, 1, CHSV(drop_dot_color, 255, maxBrightness));
          FastLED.show();
          //Serial.printf("drop_dot %d\n", drop_dot); 
        }
        //Serial.printf("drop_dot %d\n",drop_dot); 
        pre_drop_dot = drop_dot;
        drop_time = millis();
      }
  }
  delay(5);          
}


/*
音效描述：
1）HUE 色，段落跳跃模式
*/
void music_dance(){

  int SEG_NUM = 8;
  int brightness_delta = maxBrightness/SEG_NUM;
  int color_delta = g_color /SEG_NUM;
  int sig = analogRead(MIC_PIN);//out引脚

  int delta = random(0,250)/NUMPIXELS;//随机下颜色
  //Serial.println(sig);
  int trigger = 50;
  if (sig >= 400 && sig - 400 > trigger ){
    si = sig - 400;
    Serial.printf("up %d\n",sig - 400);
  }else if(sig <= 400 && 400 - sig > trigger){
    si = 400 - sig;
    Serial.printf("down %d\n",400 - sig);
  }else{
    si = 0;  
  }

  if(si > trigger){
    
    if(si > pre_si){// 声音增大
  
      int move_step = map(si, 0, 200, 0, SEG_NUM);//15 步
      Serial.printf("move_step %d\n",move_step);      
      
      for(int y = 0; y < move_step; y++){
          // 关掉上次开始位置 要移动的几个点
         fill_solid(leds+(start_dot+y)%NUMPIXELS, 1, CHSV(0, 255, 0));
      }
      FastLED.show();
      delay(50);
      if(start_dot+move_step <= NUMPIXELS-SEG_NUM){
        start_dot += move_step;
      
        for(int x = 0; x < SEG_NUM; x++){     
            fill_solid(leds+(start_dot+x)%NUMPIXELS, 1, CHSV((g_color+x * color_delta)%255, 255, x * brightness_delta));
        }
        FastLED.show();
        delay(50);
    
        start_dot = start_dot % NUMPIXELS;
        Serial.printf("start_dot %d\n",start_dot);
      }    
      
      pre_si = si;
    }else{  //声音变小
      int move_step = map(si, 0, 200, 0, SEG_NUM);//15 步
         for(int y = 0; y < move_step; y++){
          // 关掉要移动的几个点
         fill_solid(leds+(start_dot+SEG_NUM-y)%NUMPIXELS, 1, CHSV(0, 255, 0));

      }    
      FastLED.show();
      delay(50);
      
      if(start_dot - move_step >=0){
        start_dot -= move_step;
      
          for(int x = 0; x < SEG_NUM; x++){     
              fill_solid(leds+(start_dot+x)%NUMPIXELS, 1, CHSV((g_color+x * color_delta)%255, 255, x * brightness_delta));
          }
          FastLED.show();
          delay(50); 
          start_dot = start_dot % NUMPIXELS;     
     }
      Serial.printf("start_dot %d\n",start_dot);
      pre_si = si;
    }
      
  } 
 
}
/*
音效描述：
1）HUE 色，段落跳跃模式，单色模式
*/
void music_dance_single_color(){

  int SEG_NUM = 8;
  int brightness_delta = maxBrightness/SEG_NUM;
  int color_delta = g_color /SEG_NUM;
  int sig = analogRead(MIC_PIN);//out引脚

  int delta = random(0,250)/NUMPIXELS;//随机下颜色
  //Serial.println(sig);
  int trigger = 50;
  if (sig >= 400 && sig - 400 > trigger ){
    si = sig - 400;
    Serial.printf("up %d\n",sig - 400);
  }else if(sig <= 400 && 400 - sig > trigger){
    si = 400 - sig;
    Serial.printf("down %d\n",400 - sig);
  }else{
    si = 0;  
  }

  if(si > trigger){
    
    if(si > pre_si){// 声音增大
  
      int move_step = map(si, 0, 200, 0, SEG_NUM);//15 步
      Serial.printf("move_step %d\n",move_step);      
      
      for(int y = 0; y < move_step; y++){
          // 关掉上次开始位置 要移动的几个点
         fill_solid(leds+(start_dot+y)%NUMPIXELS, 1, CHSV(0, 255, 0));
         
      }
      FastLED.show();
      delay(30);
      if(start_dot+move_step <= NUMPIXELS-SEG_NUM){
        start_dot += move_step;
      
        for(int x = 0; x < SEG_NUM; x++){     
            fill_solid(leds+(start_dot+x)%NUMPIXELS, 1, CHSV(g_color, 255, x * brightness_delta));
        }
        FastLED.show();
        delay(30);
    
        start_dot = start_dot % NUMPIXELS;
        Serial.printf("start_dot %d\n",start_dot);
      }    
      
      pre_si = si;
    }else{  //声音变小
      int move_step = map(si, 0, 200, 0, SEG_NUM);//15 步
         for(int y = 0; y < move_step; y++){
          // 关掉上次开始位置 要移动的几个点
         fill_solid(leds+(start_dot+SEG_NUM-y)%NUMPIXELS, 1, CHSV(0, 255, 0));
      }    
      FastLED.show();
      delay(30);
      
      if(start_dot - move_step >=0){
        start_dot -= move_step;
      
          for(int x = 0; x < SEG_NUM; x++){     
              fill_solid(leds+(start_dot+x)%NUMPIXELS, 1, CHSV(g_color, 255, x * brightness_delta));
          }
          FastLED.show();
          delay(30); 
          start_dot = start_dot % NUMPIXELS;
     }     
            
      Serial.printf("start_dot %d\n",start_dot);
      pre_si = si;
    }
      
  } 
 
}
/*
音效描述：
1）HUE 色，由中心向外渐变扩展模式
*/
long last_drop_time = 0;
void music_extend(){
  
  int sig = analogRead(MIC_PIN);//out引脚

  int delta = int(g_color / NUMPIXELS);
  //int delta = int(120 / NUMPIXELS);
  //int delta = random(0,250)/NUMPIXELS;//随机下颜色
  if (sig > sig_max)
  {
    sig_max = sig;
  }

  if (sig >= g_trigger_level)
  { 
    //Serial.println(sig);
    int si = map(sig, g_trigger_level, sig_max, 0, NUMPIXELS/2);
    //Serial.printf("si %d\n", si); 
    if(si > pre_si){//音量更大 
      int m = si;
      for(int j = NUMPIXELS/2-1; m >= 0 && j >=0; j--, m--){
          Serial.printf("si %d\n", j); 
          if(j == 0){
            fill_solid(leds + j, 1, CHSV(delta%255, 255, brightness_delta)); 
            fill_solid(leds + NUMPIXELS-1 - j, 1, CHSV(delta%255, 255, brightness_delta));//17 18
          }
          else{
            fill_solid(leds + j, 1, CHSV((g_color+j * delta)%255, 255, j*2 * brightness_delta));//16 15 14 13
            fill_solid(leds + NUMPIXELS-1 - j, 1, CHSV((g_color+j * delta)%255, 255, j* 2 * brightness_delta));//17 18
          }
      }
      FastLED.show();          
      delay(10);
      pre_si = si;  
      last_drop_time = millis();
    }
    //else
    {// 不管音量变不变，都继续掉落
#if 1      
      for(int j = NUMPIXELS/2-1 - pre_si; j < NUMPIXELS/2 -1 -si; j++){
        
        fill_solid(leds + j, 1, CHSV(j * delta, 255, 0));
        fill_solid(leds + NUMPIXELS-1 - j, 1, CHSV(j * delta, 255, 0));
      }
      FastLED.show();
      delay(10);
#endif      
      pre_si = si; 
      last_drop_time = millis();
    }   
  }
  //长时间没动静，就关掉所有LED
  if(millis()- last_drop_time > 500){
    for(int j = 0; j <= NUMPIXELS/2-1; j++){    
      fill_solid(leds + j, 1, CHSV(j * delta, 255, 0));
      fill_solid(leds + NUMPIXELS-1 - j, 1, CHSV(j * delta, 255, 0));
    }  
    FastLED.show();
    delay(20);
    last_drop_time = millis();
  }
  //delay(5);       
}

/*
音效描述：
1）HUE 色，两端向中心渐变聚拢
*/

void music_gathered_debug(){
  
  int sig = analogRead(MIC_PIN);//out引脚
  
  //int delta = int(250 / NUMPIXELS);
  int delta = int(g_color / NUMPIXELS);
  //int delta = random(0,250)/NUMPIXELS;//随机下颜色
  if (sig > sig_max)
  {
    sig_max = sig;
  }
  
  int trigger = 50;
  if (sig >= 400 && sig - 400 > trigger ){
    si = sig - 400;
    Serial.printf("up %d\n",sig - 400);
  }else if(sig <= 400 && 400 - sig > trigger){
    si = 400 - sig;
    Serial.printf("down %d\n",400 - sig);
  }else{
    si = 0;  
  }

  if(si > trigger)
  //if (sig >= g_trigger_level)
  { 
    //Serial.println(sig);
    int si = map(sig, g_trigger_level, sig_max, 0, NUMPIXELS/2-1);//15
    Serial.printf("si %d\n", si); 
    if(si > pre_si){//音量更大
      int m = si;
      for(int j = 0; j <= si; j++){        
          fill_solid(leds + j, 1, CHSV((g_color+j * color_delta), 255, (NUMPIXELS - j)* brightness_delta));
          fill_solid(leds + NUMPIXELS-1 - j, 1, CHSV((g_color+j * color_delta), 255, (NUMPIXELS - j) * brightness_delta));
        FastLED.show();          
        delay(5);
      }

      pre_si = si;  
      last_drop_time = millis();
    }else{//音量变小
#if 1      
      for(int j = pre_si; j >= 0; j--){
        
        fill_solid(leds + j, 1, CHSV(j * delta, 255, 0));
        fill_solid(leds + NUMPIXELS-1 - j, 1, CHSV(j * delta, 255, 0));
        FastLED.show();
        delay(5);
      }
#endif      
      pre_si = si; 
      last_drop_time = millis();
    }   
  }
  //长时间没动静，就关掉所有LED
  if(millis()- last_drop_time > 200){
    for(int j = 0; j < NUMPIXELS/2; j++){    
      fill_solid(leds + j, 1, CHSV(j * delta, 255, 0));
      fill_solid(leds + NUMPIXELS-1 - j, 1, CHSV(j * delta, 255, 0));
      FastLED.show();
      delay(15);
    }  
    last_drop_time = millis();
  }
  //delay(5);       
}

/*
音效描述：
1）HUE 色，两端向中心渐变聚拢
*/

void music_gathered_old(){
  
  int sig = analogRead(MIC_PIN);//out引脚
  
  //int delta = int(250 / NUMPIXELS);
  int delta = int(g_color / NUMPIXELS);
  //int delta = random(0,250)/NUMPIXELS;//随机下颜色
  if (sig > sig_max)
  {
    sig_max = sig;
  }

  if (sig >= g_trigger_level)
  { 
    //Serial.println(sig);
    int si = map(sig, g_trigger_level, sig_max, 0, NUMPIXELS/2-1);//15
    Serial.printf("si %d\n", si); 
    if(si > pre_si){//音量更大
      int m = si;
      for(int j = 0; j <= si; j++){        
          fill_solid(leds + j, 1, CHSV((g_color+j * color_delta), 255, (NUMPIXELS - j)* brightness_delta));
          fill_solid(leds + NUMPIXELS-1 - j, 1, CHSV((g_color+j * color_delta), 255, (NUMPIXELS - j) * brightness_delta));
        FastLED.show();          
        delay(10);
      }

      pre_si = si;  
      last_drop_time = millis();
    }else{//音量变小
#if 1      
      for(int j = pre_si; j >= 0; j--){
        
        fill_solid(leds + j, 1, CHSV(j * delta, 255, 0));
        fill_solid(leds + NUMPIXELS-1 - j, 1, CHSV(j * delta, 255, 0));
        FastLED.show();
        delay(10);
      }
#endif      
      pre_si = si; 
      last_drop_time = millis();
    }   
  }
  //长时间没动静，就关掉所有LED
  if(millis()- last_drop_time > 200){
    for(int j = 0; j < NUMPIXELS/2; j++){    
      fill_solid(leds + j, 1, CHSV(j * delta, 255, 0));
      fill_solid(leds + NUMPIXELS-11 - j, 1, CHSV(j * delta, 255, 0));
      FastLED.show();
      delay(20);
    }  
    last_drop_time = millis();
  }
  //delay(5);       
}

/*
音效描述：
1）HUE 色，上推+掉落效果
*/
void music_drop(){
        
  int sig = analogRead(MIC_PIN);//out引脚
  Serial.println(sig);
  int delta = int(g_color / NUMPIXELS);
  //int delta = int(120 / NUMPIXELS);
  //int delta = random(0,250)/NUMPIXELS;//随机下颜色
  if (sig > sig_max)
  {
    sig = sig_max;// 防止超范围
  }

  if (sig >= g_trigger_level)
  { 
    //Serial.println(sig);
    int si = map(sig, g_trigger_level, sig_max, 0, NUMPIXELS-2);//最高点留给掉落点,最大值28
    if(si > pre_si){//音量更大
      
      int j = 0;

      if(si > drop_dot){ //如果这次音量最大值没有达到上次掉落的最小值，就不管它，继续掉落，反之，重新上升掉落点

        drop_dot = si+1;//掉落点要高于本次音量
 
        //if(millis() - drop_time > 300)//掉落间隔
        { 
          //Serial.printf("si  %d\n", si); 
          //Serial.printf("drop_dot %d\n", drop_dot); 
          if(pre_drop_dot != drop_dot){
            fill_solid(leds + pre_drop_dot, 1, CHSV(drop_dot_color, 255, 0));
          }
          fill_solid(leds + drop_dot, 1, CHSV(drop_dot_color, 255, maxBrightness));
          FastLED.show();
          //drop_time = millis();
        }        
      }
      if(si == drop_dot){ 
          si--;//不能覆盖了掉落点
          //Serial.printf("drop_dot = si %d\n", drop_dot); 
      }
      for(j = pre_si; j <= si; j++){
        //if(j < si)
        {          
          fill_solid(leds + j, 1, CHSV((g_color+j * color_delta)%255, 255, (NUMPIXELS-j) * brightness_delta));        
          //fill_solid(leds + j, 1, CHSV(j * delta, 255, maxBrightness));
          
        }
      }
      FastLED.show();          
      delay(5);    
      pre_si = si;  
    }else{//音量变小
      for(int j = pre_si; j >= si && j > 0; j--){
        fill_solid(leds + j, 1, CHSV(j * delta, 255, 0));
        FastLED.show();
        delay(10);
      }
      pre_si = si; 
    }
  }
  
  {//一直掉落
      //for(int i = 0;drop_dot >= si; i ++)
      unsigned long interval = map(pre_si - si, 0, 31, 800, 5);//取5-100ms的掉落速度，两次音量落差越大，降速越快
      //Serial.printf("time %ld,%ld\n",millis(),drop_time); 
      if(millis() - drop_time > 200)//掉落间隔
      {        
        if(drop_dot>0)
        {
          fill_solid(leds + drop_dot--, 1, CHSV(130, 255, 0));
          fill_solid(leds + drop_dot, 1, CHSV(drop_dot_color, 255, maxBrightness));
          FastLED.show();
          //Serial.printf("drop_dot %d\n", drop_dot); 
        }
        pre_drop_dot = drop_dot;
        drop_time = millis();
      }
  }
  delay(5);          
}

int last_brightness = 0;
int last_color = 0;

/*
音效描述：
1）HUE 色，变色+亮度渐变
*/
void music_fade(){
        
  int sig = analogRead(MIC_PIN);//out引脚

  int delta = int(g_color / NUMPIXELS);
  //int delta = int(120 / NUMPIXELS);
  //int delta = random(0,250)/NUMPIXELS;//随机下颜色
  if (sig > sig_max)
  {
    sig = sig_max;// 防止超范围
  }

  if (sig >= g_trigger_level)
  { 
    //Serial.println(sig);
    int si = map(sig, g_trigger_level, sig_max, 0, NUMPIXELS-1);
    if(si > pre_si){//音量更大
      int j = 0;
      for(j = pre_si; j <= si; j++){
          fill_solid(leds, NUMPIXELS, CHSV((g_color+j * color_delta)%255, 255, j*brightness_delta));
          FastLED.show();
          delay(5);
      }  
      last_brightness = (j-1)*brightness_delta;
      last_color = (j-1) * delta;
      pre_si = si;
    }else{//音量变小
      int j = 0;
      for(j = pre_si; j >= si && j > 0; j--){
        fill_solid(leds, NUMPIXELS, CHSV(j * delta, 255, j*brightness_delta));
        FastLED.show();
        delay(5);
      }
      last_color = (j-1) * delta;
      last_brightness = (j-1)*brightness_delta;
      pre_si = si; 
    }
  }
  
  {//一直fade
      //for(int i = 0;drop_dot >= si; i ++)
      unsigned long interval = map(pre_si - si, 0, 31, 800, 5);//取5-100ms的掉落速度，两次音量落差越大，降速越快
      //Serial.printf("last_color %ld,%ld\n",last_color,drop_time); 
      if(millis() - drop_time > 200)//掉落间隔
      {
        if(last_brightness>0)
        {
          fill_solid(leds, NUMPIXELS, CHSV(last_color, 255, last_brightness-=brightness_delta));
          FastLED.show();
        }
        
        drop_time = millis();
      }
  }
  delay(5);          
}

/*
音效描述：
1）HUE 色，单色+亮度渐变
*/
void music_fade_single_color(){
        
  int sig = analogRead(MIC_PIN);//out引脚

  //int delta = int(g_color / NUMPIXELS);
  //int delta = int(120 / NUMPIXELS);
  //int delta = random(0,250)/NUMPIXELS;//随机下颜色
  if (sig > sig_max)
  {
    sig = sig_max;// 防止超范围
  }

  if (sig >= g_trigger_level)
  { 
    //Serial.println(sig);
    int si = map(sig, g_trigger_level, sig_max, 0, NUMPIXELS-1);
    if(si > pre_si){//音量更大
      int j = 0;
      for(j = 0; j <= si; j++){
          fill_solid(leds, NUMPIXELS, CHSV(g_color, 255, j*brightness_delta));
          FastLED.show();
          delay(5);
      }  
      last_brightness = (j-1)*brightness_delta;
      //last_color = (j-1) * delta;
      pre_si = si;
    }else{//音量变小
      int j = 0;
      for(j = pre_si; j >= si && j > 0; j--){
        fill_solid(leds, NUMPIXELS, CHSV(g_color, 255, j*brightness_delta));
        FastLED.show();
        delay(5);
      }
      //last_color = (j-1) * delta;
      last_brightness = (j-1)*brightness_delta;
      pre_si = si; 
      drop_time = millis();
    }
  }
  
  {//一直fade
      //for(int i = 0;drop_dot >= si; i ++)
      unsigned long interval = map(pre_si - si, 0, 31, 800, 5);//取5-100ms的掉落速度，两次音量落差越大，降速越快
      //Serial.printf("last_color %ld,%ld\n",last_color,drop_time); 
      if(millis() - drop_time > 10)//掉落间隔
      {
        if(last_brightness>0)
        {
          fill_solid(leds, NUMPIXELS, CHSV(g_color, 255, last_brightness--));
          FastLED.show();
        }
        
        drop_time = millis();
      }
  }
  delay(5);          
}


void music_RGB_drop(){
        
  int sig = analogRead(MIC_PIN);//out引脚
  
  int delta = int(g_color / NUMPIXELS);
  //int delta = int(120 / NUMPIXELS);
  //int delta = random(0,250)/NUMPIXELS;//随机下颜色
  if (sig > sig_max)
  {
    sig = sig_max;// 防止超范围
  }

  if (sig >= g_trigger_level)
  { 
    //Serial.println(sig);
    int si = map(sig, g_trigger_level, sig_max, 0, NUMPIXELS-2);//最高点留给掉落点,最大值30
    if(si > pre_si){//音量更大
      
      int j = 0;

      if(si > drop_dot){ //如果这次音量最大值没有达到上次掉落的最小值，就不管它，继续掉落，反之，重新上升掉落点

        drop_dot = si+1;//掉落点要高于本次音量
 
        //if(millis() - drop_time > 300)//掉落间隔
        { 
          Serial.printf("si  %d\n", si); 
          Serial.printf("drop_dot %d\n", drop_dot); 
          if(pre_drop_dot != drop_dot){
            pixels.setPixelColor(pre_drop_dot, pixels.Color(0, 0, 0));
            //pixels.show();
          }
          
          pixels.setPixelColor(drop_dot, pixels.Color(0, 0, 255));          
          pixels.show();
          //drop_time = millis();
        }
      }
      if(si == drop_dot){ 
          si--;//不能覆盖了掉落点
          //Serial.printf("drop_dot = si %d\n", drop_dot); 
      }
      for(j = pre_si; j <= si; j++){
        //if(j < si)
        {          
          pixels.setPixelColor(j, pixels.Color(255, 0, 0));          
          pixels.show();
          
          delay(5);
        }
      }      
      pre_si = si;  
    }else{//音量变小
      for(int j = pre_si; j >= si && j > 0; j--){
          pixels.setPixelColor(j, pixels.Color(0, 0, 0)); 
          pixels.show();
        delay(10);
      }
      pre_si = si; 
    }
  }
  
  {//一直掉落
      //for(int i = 0;drop_dot >= si; i ++)
      unsigned long interval = map(pre_si - si, 0, 31, 800, 5);//取5-100ms的掉落速度，两次音量落差越大，降速越快
      //Serial.printf("time %ld,%ld\n",millis(),drop_time); 
      if(millis() - drop_time > 200)//掉落间隔
      {        
        if(drop_dot>0)
        {
          pixels.setPixelColor(drop_dot--, pixels.Color(0, 0, 0));          
          pixels.setPixelColor(drop_dot, pixels.Color(0, 0, 255));
          pixels.show();
          Serial.printf("drop_dot %d\n", drop_dot); 
        }
        pre_drop_dot = drop_dot;
        drop_time = millis();
      }
  }
  delay(5);            
}

/*
音效描述：
1）HUE 色, 2 个掉落效果点
*/
void music_double_drop(){
       
  int sig = analogRead(MIC_PIN);//out引脚
  
  int delta = int(g_color / NUMPIXELS);
  //int delta = int(120 / NUMPIXELS);
  //int delta = random(0,250)/NUMPIXELS;//随机下颜色
  if (sig > sig_max)
  {
    sig_max = sig;
  }

  if (sig >= g_trigger_level)
  { 
    //Serial.println(sig);
    int si = map(sig, g_trigger_level, sig_max, 0, NUMPIXELS-2);//最高点留给掉落点,最大值28
    if(si > pre_si){//音量更大
      
      int j = 0;

      if(si > drop_dot){ //如果这次音量最大值没有达到上次掉落的最小值，就不管它，继续掉落，反之，重新上升掉落点

        drop_dot = si+1;//掉落点要高于本次音量
        drop_dot2 = drop_dot+1;//掉落点要高于本次音量
        //if(millis() - drop_time > 300)//掉落间隔
        { 
          Serial.printf("si  %d\n", si); 
          Serial.printf("drop_dot %d\n", drop_dot); 
          if(pre_drop_dot != drop_dot){
            fill_solid(leds + pre_drop_dot, 1, CHSV(drop_dot_color, 255, 0));
          }
          fill_solid(leds + drop_dot, 1, CHSV(drop_dot_color, 255, maxBrightness));
          fill_solid(leds + drop_dot2, 1, CHSV(drop_dot_color, 255, maxBrightness));          
          
          FastLED.show();
          drop_time = millis();
        }
       
      }
      if(si == drop_dot){ 
          si--;//不能覆盖了掉落点
          Serial.printf("drop_dot = si %d\n", drop_dot); 
      }
      for(j = pre_si; j <= si; j++){
        //if(j < si)
        {          
          fill_solid(leds + j, 1, CHSV((g_color+j * delta)%255, 255, (NUMPIXELS-j) * brightness_delta));        
          FastLED.show();          
          delay(5);
        }
      }      
      pre_si = si;  
    }else{//音量变小
      for(int j = pre_si; j >= si && j > 0; j--){
        fill_solid(leds + j, 1, CHSV((g_color+j * delta)%255, 255, 0));
        FastLED.show();
        delay(10);
      }
      pre_si = si; 
    }
  }
  
  {   //继续掉落
      //for(int i = 0;drop_dot >= si; i ++)
      unsigned long interval = map(pre_si - si, 0, 31, 800, 5);//取5-100ms的掉落速度，两次音量落差越大，降速越快
      if(millis() - drop_time > 300)//掉落间隔
      {        
        if(drop_dot>0)
        {
          fill_solid(leds + drop_dot--, 1, CHSV(130, 255, 0));
          fill_solid(leds + drop_dot, 1, CHSV(drop_dot_color, 255, maxBrightness));
          fill_solid(leds + drop_dot2--, 1, CHSV(130, 255, 0));
          fill_solid(leds + drop_dot2, 1, CHSV(drop_dot_color, 255, maxBrightness));
          FastLED.show();
        }
        pre_drop_dot = drop_dot;
        drop_time = millis();
      }
  }
  delay(5);          
}

/*
音效描述：
1）HUE 色，上推+掉落效果
*/
void music_up_down(){
        
  int sig = analogRead(MIC_PIN);//out引脚
  
  int delta = int(g_color / NUMPIXELS);
  //int delta = int(120 / NUMPIXELS);
  //int delta = random(0,250)/NUMPIXELS;//随机下颜色
  if (sig > sig_max)
  {
    sig_max = sig;
  }

  if (sig >= g_trigger_level)
  { 
    //Serial.println(sig);
    int si = map(sig, g_trigger_level, sig_max, 0, NUMPIXELS);
    if(si > pre_si){//音量更大
      for(int j = 0; j < si; j++){
          //pixels.setPixelColor(j, pixels.Color(128, 128, 0));
          //pixels.show();
          //fill_solid(leds + j, 1, CHSV((NUMPIXELS-j) * delta, 255, maxBrightness));
          //fill_solid(leds + j, 1, CHSV(j * delta, 255, maxBrightness));
          Serial.printf("j %d", j); 
          fill_solid(leds + j, 1, CHSV((g_color+j * color_delta)%255, 255, (NUMPIXELS-j) * brightness_delta));          
          FastLED.show();          
          delay(1);
      }
           
      pre_si = si;  
    }else{//音量变小
      for(int j = pre_si; j >= si; j--){
        //pixels.setPixelColor(j, pixels.Color(0, 0, 0));
        //pixels.show();
        fill_solid(leds + j, 1, CHSV(j * delta, 255, 0));
        FastLED.show();
        delay(10);
      }
      pre_si = si; 
    }
  }
  delay(5);          
}

void musicWrite3(){
  int sig = analogRead(MIC_PIN);//out引脚
  
  int delta = int(250 / NUMPIXELS);
  if (sig > sig_max)
  {
    sig_max = sig;
  }
  if (sig >= g_trigger_level)
  { 
    Serial.println(sig);
    int si = map(sig, g_trigger_level, sig_max, 0, NUMPIXELS);
    if(si > pre_si){//音量更大
      for(int j = 0; j <= si; j++){
        if(si < NUMPIXELS -3){
          pixels.setPixelColor(j, pixels.Color(128, 128, 0));
          pixels.show();
          //delay(1);
        }
      }

      drop_dot = si + 1;
      for(int i = 0;i < 3; i ++){
        drop_dot ++;
        
        if(pre_drop_dot)
          pixels.setPixelColor(pre_drop_dot, pixels.Color(0, 0, 0));
        
        pixels.setPixelColor(drop_dot, pixels.Color(255, 0, 0));
        pre_drop_dot = drop_dot;
        
        pixels.show();
        delay(5);
      }
      pre_si = si;  
    }else{//音量变小
      for(int j = pre_si; j >= si; j--){
        pixels.setPixelColor(j, pixels.Color(0, 0, 0));
        pixels.show();
        delay(1);
      }

      //for(int i = 0;drop_dot >= si; i ++)
      {
        drop_dot --;
        
        if(pre_drop_dot>=0)
          pixels.setPixelColor(pre_drop_dot, pixels.Color(0, 0, 0));

        if(drop_dot>=0){
          pixels.setPixelColor(drop_dot, pixels.Color(255, 0, 0));
          pre_drop_dot = drop_dot;
        }
        pixels.show();
        delay(1);
      }
      
      pre_si = si; 
    }
  }else{  //继续上次
      //继续掉落
      //for(int i = 0;drop_dot >= si; i ++)
      {
        drop_dot --;
        
        if(pre_drop_dot>=0)
          pixels.setPixelColor(pre_drop_dot, pixels.Color(0, 0, 0));

        if(drop_dot>=0){
          pixels.setPixelColor(drop_dot, pixels.Color(255, 0, 0));
          pre_drop_dot = drop_dot;
        }
        pixels.show();
        delay(10);
      }
  }
  delay(1);
}

void musicWrite2()//音乐（月光模式）此程序适用于灯珠较少直条效果
{
  int sig = analogRead(MIC_PIN);//out引脚
  //Serial.println(sig);
  int delta = int(250 / NUMPIXELS);


  if (sig > sig_max)
  {
    sig_max = sig;
  }

  if (sig >= g_trigger_level)
  {
    int si = map(sig, g_trigger_level, sig_max, 0, NUMPIXELS);
    Serial.println(si);
    if (si > pre_si)
    {
      //Serial.println("+++++++++++++++");
      for (int j = 0; j < si + 1; j++)
      {
        //pixels.setPixelColor(j, pixels.Color(delta * j, 0, 255 - delta * j));
        pixels.setPixelColor(j, pixels.Color(250, 0, 0));
        pixels.show();
        delay(4);//上升
        //delay(delay_time); ///
      }
      //pixels.show();//
      //delay(delay_time); ///
      pre_si = si;
    }

    if (si < pre_si)
    {
      //Serial.println("------------");
      for (int k = pre_si; k > si; k--)
      {
        pixels.setPixelColor(k, pixels.Color(0, 0, 0));
        pixels.show();
        delay(8); //
      }
      //pixels.show();//
      //delay(delay_time); ///
      pre_si = si;
    }

  }else{
    pixels.clear();
    pixels.show();
    //delay(3);  
  }
  
}



void musicWrite1() // 掉落模式
{
  int sig = analogRead(MIC_PIN);//out引脚
  //Serial.println(sig);
  int delta = int(250 / NUMPIXELS);

  if (sig > sig_max)
  {
    sig_max = sig;
  }

  if (sig >= g_trigger_level)
  {
    
    int si = map(sig, g_trigger_level, sig_max, 0, NUMPIXELS);
    //Serial.println(si);
    if (si > pre_si)
    {
      int j = 0;
      //Serial.println("+++++++++++++++");
      for ( j= 0; j < si + 1; j++)
      {
        //pixels.setPixelColor(j, pixels.Color(delta * j, 0, 255 - delta * j));
        //pixels.setPixelColor(j, pixels.Color(250, 0, 0));

        if(j+3 <= NUMPIXELS){
          //drop_dot = j+3;
          //pixels.setPixelColor(drop_dot, pixels.Color(0, 0, 255));
          pixels.setPixelColor(j, pixels.Color(250, 0, 0));
        }else{
          //drop_dot = NUMPIXELS;
          //pixels.setPixelColor(drop_dot, pixels.Color(0, 0, 255));
          pixels.setPixelColor(NUMPIXELS-3, pixels.Color(250, 0, 0));
        }

        pixels.show();
        delay(5);//上升
        
        //delay(delay_time); ///
      }
      
      if(j+3 <= NUMPIXELS){
          if(drop_dot <= j)
            drop_dot = j+3;
            
          pixels.setPixelColor(drop_dot, pixels.Color(0, 0, 255));
          //pixels.setPixelColor(j, pixels.Color(250, 0, 0));
        }else{
          drop_dot = NUMPIXELS;
          pixels.setPixelColor(drop_dot, pixels.Color(0, 0, 255));
          //pixels.setPixelColor(NUMPIXELS-3, pixels.Color(250, 0, 0));
        }
        Serial.println(drop_dot);
        pixels.show();//
      delay(5); ///
      pre_si = si;
    }

    if (si < pre_si)
    {
      Serial.println("------------");
      for (int k = pre_si; k > si; k--)
      {
        //pixels.setPixelColor(drop_dot, pixels.Color(0, 0, 0));
        //pixels.setPixelColor(drop_dot-1, pixels.Color(0, 0, 255));
        
        if(k%3 == 0){
          if(k == 0) 
          {  
            pixels.setPixelColor(drop_dot, pixels.Color(0, 0, 0));
            drop_dot = 0;
          }else {
            pixels.setPixelColor(drop_dot, pixels.Color(0, 0, 0));
            drop_dot = drop_dot-1;
            pixels.setPixelColor(drop_dot, pixels.Color(0, 0, 255));
          }
        }
        
        pixels.setPixelColor(k, pixels.Color(0, 0, 0));
        pixels.show();
        delay(6); //
      }
      //pixels.show();//
      //delay(delay_time); ///
      pre_si = si;
    }

  }else{
    pixels.clear();
    pixels.show();
    //delay(3);  
  }
  
}
