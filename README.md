# ESP8266 Analog distance meter

Allows you to measure absolute & relative distance from an analog 0-10V or 4-20mA sensor and send it to MQTT.
It also provides HomeAssistant sensor autodiscovery.

This project was inspired by [WaterLevelSensorMQTT](https://github.com/portfedh/WaterLevelSensorMQTT).

## Required components

* NodeMCU ESP8266 Development Board ~4EUR
  * Recommend buying the one with built-in 0.96in display
* Any analog sensor, I used SWK-LT100
* 4-20mA to 3V3 signal converter or 0-10V signal converter

![ESP8266](./doc/nodemcu.png)
![JSN-SR04T](./doc/sensor.png)

Signal converters

![](./doc/signal_converter_1.png)
![](./doc/signal_converter_2.png)

Final result

![](./doc/final_result.jpg)

## Setup

1. Connect the sensor's `TRIG` pin to `D1` and `ECHO` pin to `D2`. Take 5V from the `VU` pin. Connect `Ground` to any `G` pin.
1. Optional 128x64 OLED display can be connected - `D5` as `SCL` and `D6` as `SDA`. With abuilt-in display it works out of the box.
1. At first, the `WiFiManager` will set up an AccessPoint, connect to `ESP_distance_meter` WiFi, and the configure connection to your local network. Restart the NodeMCU when done.
1. Navigate with the browser to the device's IP address. It is displayed on the LCD or printed to Serial.
1. Enter all needed parameters in the admin dashboard.

![Admin](./doc/admin_dashboard.png)

### Configuring distances

The meter has to know some heights in advance; it uses them to calculate the percentage filled or absolute column height.

#### Distance to sensor when "empty"

Provide the distance in centimeters from the bottom or your defined minimum of "something" to the sensor.
It means that at this distance the container is "empty".

#### Distance to sensor when "full"

Provide the distance in centimeters from the top or your defined maximum of "something" to the sensor.
It means that at this distance the container is "full".

## Measurements

HomeAssintant's autoconfiguration message is published to MQTT. You should be able to find the Entity under MQTT integration.

By default, the current state will be published to the `esp_distance_meter/stat/distance` topic. It contains a JSON with the following structure:

* `relative` - a percentage of how much the container is filled.
* `absolute` - calculated height in meters of "something" in the container.
* `measured` - a raw distance measured in meters from the sensor to the surface.

```json
{
    "relative": 0.73,
    "absolute": 1.12,
    "measured": 1.51
}
```

The example above shows that the container is filled to 73%, and the height of something is 1.12 meters. The raw measured distance from the sensor to something is 1.51 meters.
