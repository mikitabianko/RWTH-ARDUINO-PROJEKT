#ifndef IDISPLAY_H
#define IDISPLAY_H

#include <Arduino.h>

namespace Drivers {
    enum Color : uint16_t {
        Black = 0,
        White = 1,
        Inverse = 2
    };

    class IDisplay {
    public:
        virtual ~IDisplay() {}

        virtual bool begin() = 0;
        virtual void clear() = 0;
        virtual void display() = 0; 
        virtual void setContrast(uint8_t contrast) = 0;
        virtual void setPower(bool on) = 0;
        virtual void setInvert(bool invert) = 0;
        virtual void setRotation(uint8_t rotation) = 0;

        virtual void drawPixel(int16_t x, int16_t y, Color color) = 0;
        virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, Color color) = 0;
        virtual void drawVLine(int16_t x, int16_t y, int16_t h, Color color) = 0;
        virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, Color color) = 0;
        virtual void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) = 0;
        virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, Color color) = 0;
        virtual void drawCircle(int16_t x, int16_t y, int16_t r, Color color) = 0;
        virtual void fillCircle(int16_t x, int16_t y, int16_t r, Color color) = 0;

        virtual void setCursor(int16_t x, int16_t y) = 0;
        virtual void setTextColor(Color color) = 0;
        virtual void setTextSize(uint8_t size) = 0;
        virtual void print(const String& text) = 0;
        virtual void print(const char* text) = 0;
        virtual void println(const String& text) = 0;
        virtual void println(const char* text) = 0;
        virtual void println() = 0;

        virtual void print(int32_t n, uint8_t base = 10) = 0;
        virtual void print(double n, uint8_t digits = 2) = 0;
        virtual void print(int n) = 0;
        virtual void print(unsigned int n) = 0;
        virtual void print(long n) = 0;
        virtual void print(uint32_t n) = 0;

        virtual int16_t width() const = 0;
        virtual int16_t height() const = 0;

        virtual void getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) = 0;
        virtual void getTextBounds(const String &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) = 0;

        virtual void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) = 0;
    };
}

#endif