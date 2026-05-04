#ifndef SH110X_BRIVER_H
#define SH110X_BRIVER_H
#include "../../common/IDisplay.h"
#include <Adafruit_SH110X.h>

namespace Drivers {
    class SH110X_Driver : public IDisplay {
    private:
        Adafruit_SH1106G _gfx;
        uint8_t _addr;

    public:
        SH110X_Driver(int16_t w, int16_t h, uint8_t addr = 0x3C) 
            : _gfx(w, h, &Wire, -1), _addr(addr) {}

        bool begin() override {
            if (!_gfx.begin(_addr, true)) return false;
            _gfx.clearDisplay();
            _gfx.display();
            return true;
        }

        void clear() override { _gfx.clearDisplay(); }
        void display() override { _gfx.display(); }
        void setContrast(uint8_t contrast) override { _gfx.setContrast(contrast); }
        void setPower(bool on) override { on ? _gfx.oled_command(SH110X_DISPLAYON) : _gfx.oled_command(SH110X_DISPLAYOFF); }
        void setInvert(bool invert) override { _gfx.invertDisplay(invert); }
        void setRotation(uint8_t r) override { _gfx.setRotation(r); }

        void drawPixel(int16_t x, int16_t y, Color c) override { _gfx.drawPixel(x, y, (uint16_t)c); }
        void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, Color c) override { _gfx.drawLine(x0, y0, x1, y1, (uint16_t)c); }
        void drawVLine(int16_t x, int16_t y, int16_t h, Color c) override {_gfx.drawFastVLine(x, y, h, (uint16_t)c);};
        void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, Color c) override { _gfx.drawRect(x, y, w, h, (uint16_t)c); }
        virtual void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t c) override { _gfx.drawRoundRect(x, y, w, h, r, c); }
        void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, Color c) override { _gfx.fillRect(x, y, w, h, (uint16_t)c); }
        void drawCircle(int16_t x, int16_t y, int16_t r, Color c) override { _gfx.drawCircle(x, y, r, (uint16_t)c); }
        void fillCircle(int16_t x, int16_t y, int16_t r, Color c) override { _gfx.fillCircle(x, y, r, (uint16_t)c); }

        void setCursor(int16_t x, int16_t y) override { _gfx.setCursor(x, y); }
        void setTextColor(Color c) override { _gfx.setTextColor((uint16_t)c); }
        void setTextSize(uint8_t s) override { _gfx.setTextSize(s); }
        void print(const String& t) override { _gfx.print(t); }
        void print(const char* t) override { _gfx.print(t); }
        void println(const String& t) override { _gfx.println(t); }
        void println(const char* t) override { _gfx.println(t); }
        void println() override { _gfx.println(); }

        void print(int32_t n, uint8_t base = 10) override { _gfx.print(n, base); }
        void print(double n, uint8_t digits = 2) override { _gfx.print(n, digits); }
        void print(int n) override { _gfx.print(n); }
        void print(unsigned int n) override { _gfx.print(n); }
        void print(long n) override { _gfx.print(n); }
        void print(uint32_t n) override { _gfx.print(n); }

        int16_t width() const override { return _gfx.width(); }
        int16_t height() const override { return _gfx.height(); }

        void getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) override { _gfx.getTextBounds(string, x, y, x1, y1, w, h); }
        void getTextBounds(const String &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) override { _gfx.getTextBounds(str, x, y, x1, y1, w, h); }

        void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t c) override { _gfx.drawBitmap(x, y, bitmap, w, h, c); }
    };
}
#endif