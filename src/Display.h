#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

class Display
{
public:
    void init();
    void setStateInfo(String ipAddr, String macAddr);
    void setRequestInfo(String info);

private:
    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite request = TFT_eSprite(&tft);
    TFT_eSprite state = TFT_eSprite(&tft);
};