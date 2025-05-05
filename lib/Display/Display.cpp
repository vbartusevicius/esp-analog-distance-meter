#include "Display.h"
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, D6, D5);

Display::Display()
{
    u8g2.begin();

    this->displayWidth = u8g2.getDisplayWidth();
}

void Display::displayFirstStep(const char* appName)
{
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_6x10_tr);
        u8g2.drawStr(
            0, 
            this->cursorOffset + 14,
            "Welcome! (1/2)"
        );
        u8g2.setFont(u8g2_font_5x7_tr);

        u8g2.drawStr(
            0, 
            this->cursorOffset + 28,
            "Connect to AP:"
        );
        u8g2.drawStr(
            0, 
            this->cursorOffset + 38,
            appName
        );
    } while (u8g2.nextPage());
}

void Display::displaySecondStep(const char* ipAddress)
{
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_6x10_tr);
        u8g2.drawStr(
            0, 
            this->cursorOffset + 14,
            "Welcome! (2/2)"
        );
        u8g2.setFont(u8g2_font_5x7_tr);

        u8g2.drawStr(
            0, 
            this->cursorOffset + 28,
            "Configure device on:"
        );
        u8g2.drawStr(
            0, 
            this->cursorOffset + 38,
            ipAddress
        );
    } while (u8g2.nextPage());
}

void Display::run(Stats* stats)
{
    u8g2.firstPage();
    do {
        this->renderProgress(stats);
        this->renderNetwork(stats);
        this->renderBoolStatus("MQTT", stats->mqttConnected);
        this->renderBoolStatus("Sensor", stats->sensorConnected);
        this->renderUptime(stats);
    } while (u8g2.nextPage());
}

void Display::renderProgress(Stats* stats)
{
    const int progressHeight = 16;
    const char* value = "N/A";
    int boxWidth = 0;

    if (stats->sensorConnected) {
        boxWidth = (this->displayWidth - 2) * stats->fractionalDistance;
        value = stats->relativeDistance.c_str();
    }

    u8g2.drawFrame(0, 0, this->displayWidth, progressHeight);
    u8g2.drawBox(1, 1, boxWidth, progressHeight - 2);

    u8g2.setFontMode(1);
    u8g2.setDrawColor(2);
    u8g2.setFont(u8g2_font_5x7_tr);
    auto charHeight = u8g2.getAscent();
    auto strWidth = u8g2.getStrWidth(value);
    u8g2.drawStr(
        (this->displayWidth - strWidth) / 2, 
        charHeight + floor((progressHeight - charHeight) / 2) - 1, 
        value
    );

    this->cursorOffset = progressHeight;
}

void Display::renderNetwork(Stats* stats)
{
    auto signalStrength = atoi(stats->wifiSignal.c_str());
    // no signal
    unsigned int signalGlyph = 57887;
    // poor
    if (signalStrength >= -95) {
        signalGlyph = 57888;
    }
    // good
    if (signalStrength >= -85) {
        signalGlyph = 57889;
    }
    // very good
    if (signalStrength >= -75) {
        signalGlyph = 57890;
    }

    u8g2.setFont(u8g2_font_siji_t_6x10);
    u8g2.drawGlyph(
        this->displayWidth - 10,
        this->cursorOffset + 14,
        signalGlyph
    );

    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(
        0, 
        this->cursorOffset + 7, 
        ("SSID: " + stats->network).c_str()
    );
    u8g2.drawStr(
        0, 
        this->cursorOffset + 14 + 1, 
        ("IP: " + stats->ipAddress).c_str()
    );
    u8g2.drawHLine(0, this->cursorOffset + 16, this->displayWidth);

    this->cursorOffset += 17;
}

void Display::renderBoolStatus(String name, bool status)
{
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(0, this->cursorOffset + 8, (name + ":").c_str());

    const char* statusGlyph = "[ ]";
    if (status) {
        statusGlyph = "[+]";
    }

    auto glyphLength = u8g2.getStrWidth(statusGlyph);
    u8g2.drawStr(
        (int) (this->displayWidth / 2) - glyphLength,
        this->cursorOffset + 8,
        statusGlyph
    );

    this->cursorOffset += 8;
}

void Display::renderUptime(Stats* stats)
{
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(0, this->cursorOffset + 8, "Uptime:");

    u8g2.drawStr(
        49,
        this->cursorOffset + 8,
        stats->uptime.c_str()
    );

    this->cursorOffset += 8;
}