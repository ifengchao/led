#include <ESP8266WiFi.h>
#include <FS.h>
#include "http_server.h"
#include "effects.h"

void setup() {
  Serial.begin(115200);
  
//LED init
  led_init();

// WIFI init  
  wifi_init();
  
//WEB init
  webserver_init();

}

void loop()
{
  //dance1();
  //musicWrite2();
  //musicWrite3();
  //musicWrite4();
  //music_drop();
  //music_pop();
  //music_double_drop();
  //music_extend();
  int effects = get_effects();
  switch(effects){
    case 0:
        music_drop();
        break;
    case 1:
        music_pop();
        break;
    case 2:
        music_double_drop();
        break;
    case 3:
        music_extend();
        break;
    case 4:
        music_RGB_drop();
        break;
    case 5:
        music_gathered();
        break;
    case 6:
        music_up_down();
        break;        
        
    default:
      break;
  }
  
  //监听网页请求并处理
  webserver_handle();
}
