/*
 * ThermalPrinterTest.ino - Copyright (c) 2014-2020 - Olivier Poncet
 *
 * This file is part of the CSN-A2 library
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ThermalPrinter.h>

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------

struct Setup
{
    static constexpr unsigned long SERIAL_SPEED   = 9600UL;
    static constexpr unsigned long LOOP_DELAY     = 9000UL;
    static constexpr uint8_t       PRINTER_RX_PIN = 6; // the yellow wire
    static constexpr uint8_t       PRINTER_TX_PIN = 5; // the green wire
    static constexpr uint8_t       ARDUINO_RX_PIN = PRINTER_TX_PIN;
    static constexpr uint8_t       ARDUINO_TX_PIN = PRINTER_RX_PIN;
};

// ---------------------------------------------------------------------------
// Serial port auto-detection macros
// ---------------------------------------------------------------------------

#if !defined(PRINTER_SERIAL_PORT) && defined(ARDUINO_AVR_LEONARDO)
#define USE_HARDWARE_SERIAL 1
#define PRINTER_SERIAL_PORT Serial1
#endif

#if !defined(PRINTER_SERIAL_PORT) && defined(ARDUINO_AVR_MEGA2560)
#define USE_HARDWARE_SERIAL 1
#define PRINTER_SERIAL_PORT Serial3
#endif

#if !defined(PRINTER_SERIAL_PORT)
#define USE_SOFTWARE_SERIAL 1
#define PRINTER_SERIAL_PORT SoftwareSerial1
#endif

// ---------------------------------------------------------------------------
// globals
// ---------------------------------------------------------------------------

#ifdef USE_SOFTWARE_SERIAL
SoftwareSerial SoftwareSerial1 ( Setup::ARDUINO_RX_PIN
                               , Setup::ARDUINO_TX_PIN );
#endif

auto&          printerSerial(PRINTER_SERIAL_PORT);
ThermalPrinter thermalPrinter(printerSerial);

// ---------------------------------------------------------------------------
// MainLoop
// ---------------------------------------------------------------------------

class MainLoop
{
public: // public interface
    MainLoop(const unsigned long timeout)
        : _deadline(::micros() + (timeout * 1000UL))
    {
    }

   ~MainLoop()
    {
        wait();
    }

    void begin();
    void loop();

protected: // protected interface
    void wait() const;

private: // private data
    const unsigned long _deadline;
};

// ---------------------------------------------------------------------------
// setup
// ---------------------------------------------------------------------------

void setup()
{
    MainLoop mainloop(Setup::LOOP_DELAY);

    /* setup serial */ {
        Serial.begin(Setup::SERIAL_SPEED);
    }
    /* setup software serial */ {
        printerSerial.begin(THERMAL_PRINTER_BAUDRATE);
    }
    /* setup thermal printer */ {
        thermalPrinter.begin();
    }
    /* print the test page */ {
        mainloop.begin();
    }
}

// ---------------------------------------------------------------------------
// loop
// ---------------------------------------------------------------------------

void loop()
{
    MainLoop mainloop(Setup::LOOP_DELAY);

    mainloop.loop();
}

// ---------------------------------------------------------------------------
// MainLoop::wait
// ---------------------------------------------------------------------------

void MainLoop::begin()
{
    /* print title */ {
        thermalPrinter.setDoubleWidthMode(true);
        thermalPrinter.setDoubleHeightMode(true);
        thermalPrinter.setJustification('C');
        thermalPrinter.println("Hello World !");
        thermalPrinter.setJustification('L');
        thermalPrinter.setDoubleHeightMode(false);
        thermalPrinter.setDoubleWidthMode(false);
    }
    /* justify right */ {
        thermalPrinter.setJustification('R');
        thermalPrinter.println("Right Justify");
    }
    /* justify center */ {
        thermalPrinter.setJustification('C');
        thermalPrinter.println("Center Justify");
    }
    /* justify left */ {
        thermalPrinter.setJustification('L');
        thermalPrinter.println("Left Justify");
    }
    /* small mode */ {
        thermalPrinter.setSmallMode(true);
        thermalPrinter.println("Small Mode On");
        thermalPrinter.setSmallMode(false);
        thermalPrinter.println("Small Mode Off");
        thermalPrinter.setPrintMode(0);
    }
    /* reverse mode */ {
        thermalPrinter.setReverseMode(true);
        thermalPrinter.println("Reverse Mode On");
        thermalPrinter.setReverseMode(false);
        thermalPrinter.println("Reverse Mode Off");
        thermalPrinter.setPrintMode(0);
    }
    /* upside/down mode */ {
        thermalPrinter.setUpsideDownMode(true);
        thermalPrinter.println("Upside/Down Mode On");
        thermalPrinter.setUpsideDownMode(false);
        thermalPrinter.println("Upside/Down Mode Off");
        thermalPrinter.setPrintMode(0);
    }
    /* emphasized mode */ {
        thermalPrinter.setEmphasizedMode(true);
        thermalPrinter.println("Emphasized Mode On");
        thermalPrinter.setEmphasizedMode(false);
        thermalPrinter.println("Emphasized Mode Off");
        thermalPrinter.setPrintMode(0);
    }
    /* double width mode */ {
        thermalPrinter.setDoubleWidthMode(true);
        thermalPrinter.println("Double Width Mode On");
        thermalPrinter.setDoubleWidthMode(false);
        thermalPrinter.println("Double Width Mode Off");
        thermalPrinter.setPrintMode(0);
    }
    /* double height mode */ {
        thermalPrinter.setDoubleHeightMode(true);
        thermalPrinter.println("Double Height Mode On");
        thermalPrinter.setDoubleHeightMode(false);
        thermalPrinter.println("Double Height Mode Off");
        thermalPrinter.setPrintMode(0);
    }
    /* double width/height mode */ {
        thermalPrinter.setDoubleWidthMode(true);
        thermalPrinter.setDoubleHeightMode(true);
        thermalPrinter.println("Double Width/Height Mode On");
        thermalPrinter.setDoubleWidthMode(false);
        thermalPrinter.setDoubleHeightMode(false);
        thermalPrinter.println("Double Width/Height Mode Off");
        thermalPrinter.setPrintMode(0);
    }
    /* delete line mode */ {
        thermalPrinter.setDeleteLineMode(true);
        thermalPrinter.println("Delete Line Mode On");
        thermalPrinter.setDeleteLineMode(false);
        thermalPrinter.println("Delete Line Mode Off");
        thermalPrinter.setPrintMode(0);
    }
    /* double strike mode */ {
        thermalPrinter.setDoubleStrikeMode(true);
        thermalPrinter.println("Double Strike Mode On");
        thermalPrinter.setDoubleStrikeMode(false);
        thermalPrinter.println("Double Strike Mode Off");
        thermalPrinter.setPrintMode(0);
    }
    /* underline mode */ {
        thermalPrinter.setUnderlineMode(2);
        thermalPrinter.println("Underline Mode Thick");
        thermalPrinter.setUnderlineMode(1);
        thermalPrinter.println("Underline Mode Thin");
        thermalPrinter.setUnderlineMode(0);
        thermalPrinter.println("Underline Mode None");
        thermalPrinter.setPrintMode(0);
    }
    /* barcode 'CODE 39' */ {
        const ThermalPrinterBarcodeSystem BarcodeSystem;
        const char*       bc_data     = "BARCODE";
        constexpr uint8_t bc_type     = BarcodeSystem.BS_CODE_39;
        constexpr uint8_t bc_position = BarcodeSystem.BS_TEXT_BELOW;
        constexpr uint8_t bc_width    = BarcodeSystem.BS_WIDTH_TINY;
        constexpr uint8_t bc_height   = 150;
        constexpr uint8_t bc_justify  = 'C';

        thermalPrinter.setJustification(bc_justify);
        thermalPrinter.setBarcodeLabel(bc_position);
        thermalPrinter.setBarcodeWidth(bc_width);
        thermalPrinter.setBarcodeHeight(bc_height);
        thermalPrinter.printBarcode(bc_data, bc_type);
    }
    /* barcode 'UPC-A' */ {
        const ThermalPrinterBarcodeSystem BarcodeSystem;
        const char*       bc_data     = "123456789123";
        constexpr uint8_t bc_type     = BarcodeSystem.BS_UPC_A;
        constexpr uint8_t bc_position = BarcodeSystem.BS_TEXT_BELOW;
        constexpr uint8_t bc_width    = BarcodeSystem.BS_WIDTH_SMALL;
        constexpr uint8_t bc_height   = 150;
        constexpr uint8_t bc_justify  = 'C';

        thermalPrinter.setJustification(bc_justify);
        thermalPrinter.setBarcodeLabel(bc_position);
        thermalPrinter.setBarcodeWidth(bc_width);
        thermalPrinter.setBarcodeHeight(bc_height);
        thermalPrinter.printBarcode(bc_data, bc_type);
    }
    /* feed some lines */ {
        thermalPrinter.feedLines(5);
    }
}

// ---------------------------------------------------------------------------
// MainLoop::loop
// ---------------------------------------------------------------------------

void MainLoop::loop()
{
    const bool hasPaper(thermalPrinter.hasPaper());

    Serial.print(F("Thermal Printer has paper : "));
    Serial.println(hasPaper ? F("Yes") : F("No"));

    if(hasPaper != false) {
        thermalPrinter.print(F("Current Time : "));
        thermalPrinter.println(::micros());
    }
}

// ---------------------------------------------------------------------------
// MainLoop::wait
// ---------------------------------------------------------------------------

void MainLoop::wait() const
{
    unsigned long       now      = ::micros();
    unsigned long const deadline = _deadline;

    while(static_cast<long>(now - deadline) < 0L) {
        now = ::micros();
    }
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
