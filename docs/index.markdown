---
# Feel free to add content and custom Front Matter to this file.
# To modify the layout, see https://jekyllrb.com/docs/themes/#overriding-theme-defaults

layout: home
---

Esta es la página principal. La pueden ajustar a su gusto usando [markdown](https://www.markdownguide.org/basic-syntax/)

## Ejemplo

Así se agrega una imagen. En general se guardan en la carpeta `assets`:

![mtstmichel](/assets/mtstmichel.jpg)


## Código incluido

Pueden incluir código y que lo pinte adecuadamente:

```c++
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    value = random(100);

    //Send value as telemetry
    DynamicJsonDocument resp(256);
    resp["value"] = value;
    char buffer[256];
    serializeJson(resp, buffer);
    
    client.publish("v1/devices/me/telemetry", buffer);
    
    Serial.print("Publish message [telemetry]: ");
    Serial.println(buffer);
    
  }
}
```

## Otras páginas

Creando archivos `.markdown` en la carpeta principal pueden agregar "tabs" de navegación a la página. Por ejemplo [la del equipo](/equipo) y [otra de prueba](/prueba)

## Configuración del sitio

El archivo `_config.yml` contiene la configuración básica del sitio. Edítenlo para ajustarlo a su proyecto.

## Posts

En la carpeta `_posts`` pueden poner posts para crear la bitácora del proyecto. En el primer post está cómo debe ser el formato para que les quede ordenado.