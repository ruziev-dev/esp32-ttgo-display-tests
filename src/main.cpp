#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();


void espDelay(int ms) {
  esp_sleep_enable_timer_wakeup(ms * 1000);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
  esp_light_sleep_start();
}

void setup()
{
  tft.init();
  tft.fillScreen(TFT_RED);
  tft.setTextColor(TFT_CYAN);
  Serial.begin(9600);
  Serial.println("It Works!");
}

void loop()
{

  int i = 0;

  while (i <= TFT_HEIGHT)
  {
    tft.drawString("Fuck you!", 10, i, 4);
    i++;
    espDelay(20);
    tft.fillScreen(TFT_BLACK);
  }
}

