---
# Feel free to add content and custom Front Matter to this file.
# To modify the layout, see https://jekyllrb.com/docs/themes/#overriding-theme-defaults

layout: home
---
## Quienes Somos
Bienvenido a OptiShop, líderes en la transformación de la experiencia de compra en supermercados. Nos dedicamos a desarrollar soluciones innovadoras que abordan los desafíos comunes que los consumidores enfrentan al realizar sus compras diarias.

## Nuestra Misión
En OptiShop, nos enorgullece abordar los problemas actuales en las compras de supermercado mediante la implementación de tecnologías avanzadas y enfoques inteligentes. Nuestra misión es proporcionar a los usuarios una experiencia de compra más eficiente, agradable y personalizada, utilizando soluciones que marcan la pauta en la industria.

## Nuestro Porducto
Nos especializamos en el desarrollo de carritos inteligentes para supermercados, integrando tecnología de vanguardia para optimizar la distribución de productos, ofrecer promociones personalizadas y agilizar el proceso de servicio en los mostradores. Con un equipo dedicado y una visión clara, estamos dando forma al futuro de las compras minoristas.

![foto-terminada](/assets/foto-terminada.jpg)


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
