#include "Display.h"

void Display::init()
{
    this->tft.init();
    this->request.createSprite(TFT_WIDTH, TFT_HEIGHT);
    this->state.createSprite(TFT_WIDTH, TFT_HEIGHT);
    this->request.setTextColor(TFT_GREEN);
}

void Display::setStateInfo(String ipAddr, String macAddr)
{
    this->state.fillScreen(TFT_BLACK);
    this->state.drawString("IP: " + ipAddr, 5, 0, 2);
    this->state.drawString("MAC: " + macAddr, 5, 20, 2);
    this->state.pushSprite(0, 0);
}

void Display::setRequestInfo(String request)
{
    this->request.fillScreen(TFT_BLACK);
    this->request.drawString("request: " + request, 5, 0, 2);
    this->request.pushSprite(0, 60);
}
