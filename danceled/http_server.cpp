#include <ESP8266WebServer.h>
#include <Arduino.h>
#include "effects.h"
#include <ArduinoJson.h>
#include <FS.h>

ESP8266WebServer server(80);

#define CONFIG_PATH "/config.json"

// -- Macro definitions -----------------
#define PARAM_MATCH(str, retval) \
  if (strcmp(param, str) == 0)   \
    return retval;

#define FILE_MATCH(fp, ret)  \
  if (filepath.endsWith(fp)) \
    return ret;

    
int read_config(){
  if (SPIFFS.exists(CONFIG_PATH)){
    File fp = SPIFFS.open(CONFIG_PATH, "r");
    String settings = fp.readString();
    Serial.println(settings);
    
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, settings);
    JsonObject obj = doc.as<JsonObject>();

    int effects = obj[String("Effects")];
    int brightness = obj[String("maxBrightness")];
    int TriggerLevel = obj[String("TriggerLevel")];
    int maxColor = obj[String("Color")];

    set_effects(effects);
    set_brightness(brightness);
    set_trigger_level(TriggerLevel);
    set_color(maxColor);
    
    fp.close();
  }  
}

int set_config(){
  
  if (SPIFFS.exists(CONFIG_PATH)){
    
    File fp = SPIFFS.open(CONFIG_PATH, "w");
  
    char settings[1024] = {0};
    snprintf(settings, sizeof(settings), "{\"Effects\":%d, \"maxBrightness\":%d, \"TriggerLevel\":%d, \"Color\":%d}", get_effects(), get_brightness(), get_trigger_level(), get_color());
    fp.write(settings, sizeof(settings));
        
    fp.close();
  }  
}

String getFileContentType(String &filepath)
{

  //got the following from:
  //https://stackoverflow.com/questions/23714383/what-are-all-the-possible-values-for-http-content-type-header
  FILE_MATCH(".html", "text/html")
  FILE_MATCH(".txt", "text/plain")
  FILE_MATCH(".css", "text/css")
  FILE_MATCH(".js", "application/javascript")

  // the following is provided as a "just in case" -
  // it doesn't hurt to include them, but we haven't used them in this project (yet)
  FILE_MATCH(".mp4", "audio/mpeg")
  FILE_MATCH(".jpg", "image/jpeg")
  FILE_MATCH(".png", "image/png")

  //at the very least just return something
  return "text/plain";
}
    
// ---------------------------------------------------------------------------
void searchFileSystem()
{
  //server.uri() is the param accepted; ie:
  //    http://10.20.30.40/somefile.txt - uri is /somefile.txt
  // we will put it into a string for the string utility functions

  String filepath = server.uri();

  if (filepath.endsWith("/")) //is this a folder?
  {
    filepath += "index.html"; //index page of the folder being accessed
  }

  if (SPIFFS.exists(filepath))
  {
    String content = getFileContentType(filepath);

    Serial.printf("Serving file: %s as (%s)\n", filepath.c_str(), content.c_str());

    //Here, we've found that we have to deliver the stream manually.
    // this might be caused with an issue with the `streamFile()` function
    // so we manually implemented it for performance.

    // perhaps server.client().setNoDelay(true); solved the issue
    // in which we can change back to streamFile
    // so we will leave this up to the pull requests
    // to clean up this code
    File fp = SPIFFS.open(filepath, "r");
    size_t sent = 0;
    //write our own sending loop:
    char buf[2048];
    size_t total = sent = fp.size();
    server.client().setNoDelay(true);
    while (total > 0)
    {
      size_t len = _min((int)(sizeof(buf) - 1), total);
      fp.read((byte *)buf, len);
      server.client().write_P((const char *)buf, len);

      total -= len;
    }

    Serial.printf("sent %d of %d filesize\n", sent, fp.size());
  }
  else
  {
    Serial.printf("%s was not found, plaintext response", filepath.c_str());
    server.send(404, "text/plain", filepath + " File not found, have you uploaded data to the ESP correctly?");
  }
}


void homepage() {
  server.send(200, "text/plain", "test homepage !");
}

void handleNotFound() {                                     
  server.send(404, "text/plain", "404: Not found");
}

IPAddress local_IP(192,168,4,1);//手动设置的开启的网络的ip地址
IPAddress gateway(192,168,4,1);  //手动设置的网关IP地址
IPAddress subnet(255,255,255,0); //手动设置的子网掩码

void wifi_init(){
  WiFi.mode(WIFI_AP);
  WiFi.softAP("2021","88888888",1,0,4);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  SPIFFS.begin();
  read_config();
}

void webserver_handle(){
  server.handleClient();
}  

int getEffectFromParams(const char *param)
{
  return atoi(param);
}

void change_color(){
  
  String errorMessage = "Sent to lights!"; //lets set an error message to send if there's anything wrong.

  Serial.println("Received Request:");

  for (int i = 0; i < server.args(); i++)
  {

    const String arg = server.argName(i);
    const char *param = server.arg(i).c_str();

    //just for internal debugging
    Serial.printf("%s:%s\n", arg.c_str(), param);

    if (arg == "color")
    {
      int color = getEffectFromParams(param);
      set_color(color);
      set_config();
    }
    else
    {
      Serial.printf("arg '%s' is not tied to any function", arg.c_str());
    }
  }

  server.send(200, "text/plain", errorMessage);  
}

void change_mode(){
  
  String errorMessage = "Sent to lights!"; //lets set an error message to send if there's anything wrong.

  Serial.println("Received Request:");

  for (int i = 0; i < server.args(); i++)
  {

    const String arg = server.argName(i);
    const char *param = server.arg(i).c_str();

    //just for internal debugging
    Serial.printf("%s:%s\n", arg.c_str(), param);

    if (arg == "mode")
    {
      int effects = getEffectFromParams(param);
      set_effects(effects);
      set_config();
    }
    else
    {
      Serial.printf("arg '%s' is not tied to any function", arg.c_str());
    }
  }

  server.send(200, "text/plain", errorMessage);
}

void change_brightness(){
  
  String errorMessage = "Sent to lights!"; //lets set an error message to send if there's anything wrong.

  Serial.println("Received Request:");

  for (int i = 0; i < server.args(); i++)
  {

    const String arg = server.argName(i);
    const char *param = server.arg(i).c_str();

    //just for internal debugging
    Serial.printf("%s:%s\n", arg.c_str(), param);

    if (arg == "brightness")
    {
      int brightness = getEffectFromParams(param);
      set_brightness(brightness);
      set_config();
    }
    else
    {
      Serial.printf("arg '%s' is not tied to any function", arg.c_str());
    }
  }

  server.send(200, "text/plain", errorMessage);
}

void change_trigger_level(){
  
  String errorMessage = "Sent to lights!"; //lets set an error message to send if there's anything wrong.

  Serial.println("Received Request:");

  for (int i = 0; i < server.args(); i++)
  {

    const String arg = server.argName(i);
    const char *param = server.arg(i).c_str();

    //just for internal debugging
    Serial.printf("%s:%s\n", arg.c_str(), param);

    if (arg == "level")
    {
      int level = getEffectFromParams(param);
      set_trigger_level(level);
      set_config();
    }
    else
    {
      Serial.printf("arg '%s' is not tied to any function\n", arg.c_str());
    }
  }

  server.send(200, "text/plain", errorMessage);
}

void get_state(){
    char errorMessage[1024] = "Sent to lights!";
    snprintf(errorMessage, sizeof(errorMessage), "{\"Effects\":%d, \"maxBrightness\":%d, \"TriggerLevel\":%d, \"Color\":%d}", get_effects(), get_brightness(), get_trigger_level(), get_color());
    server.send(200, "text/json", errorMessage);
}

void webserver_init(){
  //server.on("/", homepage);
  server.on("/getState", get_state);
  server.on("/changeColor", change_color);
  server.on("/changeMode", change_mode);  
  server.on("/changeBrightness", change_brightness);
  server.on("/changeTriggerLevel", change_trigger_level);
  server.onNotFound(searchFileSystem);
  server.begin();
}
