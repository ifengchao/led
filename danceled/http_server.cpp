#include <ESP8266WebServer.h>
#include <Arduino.h>
#include "effects.h"

ESP8266WebServer server(80);


// -- Macro definitions -----------------
#define PARAM_MATCH(str, retval) \
  if (strcmp(param, str) == 0)   \
    return retval;

#define FILE_MATCH(fp, ret)  \
  if (filepath.endsWith(fp)) \
    return ret;


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
  WiFi.softAP("2021","88888888");
  WiFi.softAPConfig(local_IP, gateway, subnet);
  SPIFFS.begin();
}

void webserver_handle(){
  server.handleClient();
}  

int getEffectFromParams(const char *param)
{
  return atoi(param);
}

void change_color(){
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
    }
    else
    {
      Serial.printf("arg '%s' is not tied to any function", arg.c_str());
    }
  }

  server.send(200, "text/plain", errorMessage);
}

void webserver_init(){
  //server.on("/", homepage);
  server.on("/changeColor", change_color);
  server.on("/changeMode", change_mode);  
  server.on("/changeBrightness", change_brightness);
  server.onNotFound(searchFileSystem);
  server.begin();
}
