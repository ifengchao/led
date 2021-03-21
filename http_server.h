#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H

#include <Arduino.h>


void wifi_init();

void homepage();
void handleNotFound();

void webserver_init();
void webserver_handle();

#endif
