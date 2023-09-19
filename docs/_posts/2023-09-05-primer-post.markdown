---
layout: post
title:  "Primer post!"
date:   2023-09-05 14:14:20 -0300
categories: posts
---
Los posts se guardan en el directorio `_posts`. Agregando posts aquí (y editando este también) respetando el formato pueden hacer la bitácora del proyecto.

`YEAR-MONTH-DAY-title.markdown`

Se puede incluir código así:

```c++
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Update these with values suitable for your network.
const char* ssid = "NOMBRE_WIFI";
const char* password = "PASS_WIFI";
const char* mqtt_server = "demo.thingsboard.io";
const char* token = "TOKEN_DEVICE";

// Connection objects
WiFiClient espClient;
PubSubClient client(espClient);
```

E imágenes así:

![esp8266](https://upload.wikimedia.org/wikipedia/commons/thumb/7/7e/NodeMCU_DEVKIT_1.0.jpg/2880px-NodeMCU_DEVKIT_1.0.jpg)
