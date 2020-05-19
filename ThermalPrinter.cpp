/*
 * ThermalPrinter.cpp - Copyright (c) 2014-2020 - Olivier Poncet
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
#include "ThermalPrinter.h"

// ---------------------------------------------------------------------------
// ASCII
// ---------------------------------------------------------------------------

namespace {

struct ASCII
{
    static constexpr char NUL = 0x00; // null character
    static constexpr char SOH = 0x01; // start of heading
    static constexpr char STX = 0x02; // start of text
    static constexpr char ETX = 0x03; // end of text
    static constexpr char EOT = 0x04; // end of transmission
    static constexpr char ENQ = 0x05; // enquiry
    static constexpr char ACK = 0x06; // acknowledge
    static constexpr char BEL = 0x07; // bell
    static constexpr char BS  = 0x08; // backspace
    static constexpr char HT  = 0x09; // horizontal tab
    static constexpr char LF  = 0x0a; // new line
    static constexpr char VT  = 0x0b; // vertical tab
    static constexpr char FF  = 0x0c; // form feed
    static constexpr char CR  = 0x0d; // carriage return
    static constexpr char SO  = 0x0e; // shift out
    static constexpr char SI  = 0x0f; // shift in
    static constexpr char DLE = 0x10; // data link escape
    static constexpr char DC1 = 0x11; // device control 1
    static constexpr char DC2 = 0x12; // device control 2
    static constexpr char DC3 = 0x13; // device control 3
    static constexpr char DC4 = 0x14; // device control 4
    static constexpr char NAK = 0x15; // negative ackowledge
    static constexpr char SYN = 0x16; // synchronous idle
    static constexpr char ETB = 0x17; // end of transmission block
    static constexpr char CAN = 0x18; // cancel
    static constexpr char EM  = 0x19; // end of medium
    static constexpr char SUB = 0x1a; // substitute
    static constexpr char ESC = 0x1b; // escape
    static constexpr char FS  = 0x1c; // file separator
    static constexpr char GS  = 0x1d; // group separator
    static constexpr char RS  = 0x1e; // record separator
    static constexpr char US  = 0x1f; // unit separator
};

}

// ---------------------------------------------------------------------------
// Traits
// ---------------------------------------------------------------------------

namespace {

struct Traits
{
    struct Printer
    {
        static constexpr unsigned long firmwareVersion = THERMAL_PRINTER_FIRMWARE;
        static constexpr unsigned long serialBaudRate  = THERMAL_PRINTER_BAUDRATE;
        static constexpr unsigned long serialByteTime  = THERMAL_PRINTER_BYTETIME;
        static constexpr uint16_t      dotsPerLine     = 384;
    };

    struct FontA
    {
        static constexpr uint8_t charWidth  = 12;
        static constexpr uint8_t charHeight = 24;
    };

    struct FontB
    {
        static constexpr uint8_t charWidth  =  9;
        static constexpr uint8_t charHeight = 17;
    };

    struct FontC
    {
        static constexpr uint8_t charWidth  = 24;
        static constexpr uint8_t charHeight = 24;
    };

    static uint8_t charWidth(const bool hasFontBit, const bool hasDoubleWidthBit)
    {
        const uint8_t baseCharWidth = (hasFontBit ? FontB::charWidth : FontA::charWidth);
        const uint8_t realCharWidth = (baseCharWidth << (hasDoubleWidthBit ? 1 : 0));

        return realCharWidth;
    }

    static uint8_t charHeight(const bool hasFontBit, const bool hasDoubleHeightBit)
    {
        const uint8_t baseCharHeight = (hasFontBit ? FontB::charHeight : FontA::charHeight);
        const uint8_t realCharHeight = (baseCharHeight << (hasDoubleHeightBit ? 1 : 0));

        return realCharHeight;
    }
};

};

// ---------------------------------------------------------------------------
// Command
// ---------------------------------------------------------------------------

namespace {

struct Command
{
    struct PrintTestPage
    {
        static constexpr uint8_t control  = ASCII::DC2;
        static constexpr uint8_t function = 'T';
    };

    struct Reset
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = '@';
    };

    struct Wake
    {
        static constexpr uint8_t control  = 0xff;
        static constexpr uint8_t function = 0x00;
    };

    struct Sleep
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = '8';

        static uint8_t n1(const uint8_t lsb)
        {
            return lsb;
        }

        static uint8_t n2(const uint8_t msb)
        {
            return msb;
        }
    };

    struct PrintAndFeedLines
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = 'd';

        static uint8_t n1(const uint8_t lines)
        {
            return lines;
        }
    };

    struct PrintAndFeedRows
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = 'J';

        static uint8_t n1(const uint8_t rows)
        {
            return rows;
        }
    };

    struct SetJustification
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = 'a';

        static uint8_t n1(const uint8_t justification)
        {
            switch(justification) {
                case 0x0: // left
                case 0x1: // center
                case 0x2: // right
                case '0': // left
                case '1': // center
                case '2': // right
                    break;
                case 'L': // left
                    return 0x0;
                case 'C': // center
                    return 0x1;
                case 'R': // right
                    return 0x2;
                default:
                    return 0x0;
            }
            return justification;
        }
    };

    struct SetPrintingParameters
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = '7';

        static uint8_t n1(const uint8_t dots)
        {
            return dots;
        }

        static uint8_t n2(const uint8_t time)
        {
            return time;
        }

        static uint8_t n3(const uint8_t interval)
        {
            return interval;
        }
    };

    struct SetPrintingDensity
    {
        static constexpr uint8_t control  = ASCII::DC2;
        static constexpr uint8_t function = '#';

        static uint8_t n1(const uint8_t density, const uint8_t breaktime)
        {
            const uint8_t msb   = ((breaktime >  7 ?  7 : breaktime) << 5);
            const uint8_t lsb   = ((density   > 15 ? 15 : density  ) << 0);
            const uint8_t value = (msb | lsb);

            return value;
        }
    };

    struct SetPeripheralDevice
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = '=';

        static uint8_t n1(const uint8_t online)
        {
            if(online > 1) {
                return 1;
            }
            return online;
        }
    };

    struct SetPrintMode
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = '!';
        static constexpr uint8_t bit0     = (1 << 0);
        static constexpr uint8_t bit1     = (1 << 1);
        static constexpr uint8_t bit2     = (1 << 2);
        static constexpr uint8_t bit3     = (1 << 3);
        static constexpr uint8_t bit4     = (1 << 4);
        static constexpr uint8_t bit5     = (1 << 5);
        static constexpr uint8_t bit6     = (1 << 6);
        static constexpr uint8_t bit7     = (1 << 7);

        static uint8_t n1(const uint8_t mode)
        {
            return mode;
        }
    };

    struct SetReverseMode
    {
        static constexpr uint8_t control  = ASCII::GS;
        static constexpr uint8_t function = 'B';

        static uint8_t n1(const uint8_t enabled)
        {
            if(enabled > 1) {
                return 1;
            }
            return enabled;
        }
    };

    struct SetUpsideDownMode
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = '{';

        static uint8_t n1(const uint8_t enabled)
        {
            if(enabled > 1) {
                return 1;
            }
            return enabled;
        }
    };

    struct SetUnderlineMode
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = '-';

        static uint8_t n1(const uint8_t thickness)
        {
            if(thickness > 2) {
                return 2;
            }
            return thickness;
        }
    };

    struct SetDoubleStrikeMode
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = 'G';

        static uint8_t n1(const uint8_t enabled)
        {
            if(enabled > 1) {
                return 1;
            }
            return enabled;
        }
    };

    struct SetCharacterSet
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = 'R';

        static uint8_t n1(const uint8_t charset)
        {
            if(charset > 15) {
                return 0;
            }
            return charset;
        }
    };

    struct SetCharacterPage
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = 't';

        static uint8_t n1(const uint8_t charpage)
        {
            if(charpage > 47) {
                return 0;
            }
            return charpage;
        }
    };

    struct SetCharacterSpacing
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = ' ';

        static uint8_t n1(const uint8_t spacing)
        {
            return spacing;
        }
    };

    struct SetLineSpacing
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = '3';

        static uint8_t n1(const uint8_t spacing)
        {
            return spacing;
        }
    };

    struct SetBarcodeLabel
    {
        static constexpr uint8_t control  = ASCII::GS;
        static constexpr uint8_t function = 'H';

        static uint8_t n1(const uint8_t label)
        {
            switch(label) {
                case 0x0: // not printed
                case 0x1: // above
                case 0x2: // below
                case 0x3: // both
                case '0': // not printed
                case '1': // above
                case '2': // below
                case '3': // both
                    break;
                default:
                    return 0x0;
            }
            return label;
        }
    };

    struct SetBarcodeWidth
    {
        static constexpr uint8_t control  = ASCII::GS;
        static constexpr uint8_t function = 'w';

        static uint8_t n1(const uint8_t width)
        {
            if((width < 2) || (width > 6)) {
                return 3;
            }
            return width;
        }
    };

    struct SetBarcodeHeight
    {
        static constexpr uint8_t control  = ASCII::GS;
        static constexpr uint8_t function = 'h';

        static uint8_t n1(const uint8_t height)
        {
            if((height < 1) || (height > 254)) {
                return 162;
            }
            return height;
        }
    };

    struct PrintBarcode
    {
        static constexpr uint8_t control  = ASCII::GS;
        static constexpr uint8_t function = 'k';

        static uint8_t n1(const uint8_t type)
        {
            return type;
        }

        static uint8_t n2(const uint8_t length)
        {
            return length;
        }
    };

    struct GetPrinterStatus
    {
        static constexpr uint8_t control  = ASCII::ESC;
        static constexpr uint8_t function = 'v';

        static uint8_t n1(const uint8_t value)
        {
            return value;
        }
    };
};

}

// ---------------------------------------------------------------------------
// ThermalPrinter
// ---------------------------------------------------------------------------

ThermalPrinter::ThermalPrinter(Stream& stream)
    : _stream(stream)
    , _deadline(0UL)
    , _printMode(0)
{
}

void ThermalPrinter::begin()
{
    reset();
}

void ThermalPrinter::reset()
{
    typedef Command::Reset command_traits;

    send ( command_traits::control
         , command_traits::function );

    /* reset properties */ {
        _printMode = 0;
    }
}

void ThermalPrinter::end()
{
    reset();
}

void ThermalPrinter::printTestPage()
{
    typedef Command::PrintTestPage command_traits;

    send ( command_traits::control
         , command_traits::function );
}

void ThermalPrinter::wakePrinter()
{
    typedef Command::Wake command_traits;

    send ( command_traits::control
         , command_traits::function );
}

void ThermalPrinter::sleepPrinter(const uint16_t seconds)
{
    typedef Command::Sleep command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1((seconds >> 0) & 0xff)
         , command_traits::n2((seconds >> 8) & 0xff) );
}

void ThermalPrinter::feedLines(const uint8_t lines)
{
    typedef Command::PrintAndFeedLines command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(lines) );
}

void ThermalPrinter::feedRows(const uint8_t rows)
{
    typedef Command::PrintAndFeedRows command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(rows) );
}

void ThermalPrinter::setJustification(const uint8_t justification)
{
    typedef Command::SetJustification command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(justification) );
}

void ThermalPrinter::setCharacterSet(const uint8_t number)
{
    typedef Command::SetCharacterSet command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(number) );
}

void ThermalPrinter::setCharacterPage(const uint8_t number)
{
    typedef Command::SetCharacterPage  command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(number) );
}

void ThermalPrinter::setCharacterSpacing(const uint8_t spacing)
{
    typedef Command::SetCharacterSpacing  command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(spacing) );
}

void ThermalPrinter::setLineSpacing(const uint8_t spacing)
{
    typedef Command::SetLineSpacing  command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(spacing) );
}

void ThermalPrinter::setPrintingParameters(const uint8_t dots, const uint8_t time, const uint8_t interval)
{
    typedef Command::SetPrintingParameters command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(dots)
         , command_traits::n2(time)
         , command_traits::n3(interval) );
}

void ThermalPrinter::setPrintingDensity(const uint8_t density, const uint8_t breaktime)
{
    typedef Command::SetPrintingDensity command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(density, breaktime) );
}

void ThermalPrinter::setOnline(const bool online)
{
    typedef Command::SetPeripheralDevice command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(online) );
}

void ThermalPrinter::setPrintMode(const uint8_t printMode)
{
    typedef Command::SetPrintMode command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(printMode) );

    /* adjust the shadow value */ {
        _printMode = printMode;
    }
}

void ThermalPrinter::setSmallMode(const bool enabled)
{
    typedef Command::SetPrintMode command_traits;

    setPrintMode ( enabled != false
                 ? _printMode |  command_traits::bit0
                 : _printMode & ~command_traits::bit0 );
}

void ThermalPrinter::setReverseMode(const bool enabled)
{
    typedef Command::SetPrintMode command_traits;

    setPrintMode ( enabled != false
                 ? _printMode |  command_traits::bit1
                 : _printMode & ~command_traits::bit1 );

    if(Traits::Printer::firmwareVersion >= 216) {
        typedef Command::SetReverseMode command_traits;

        send ( command_traits::control
             , command_traits::function
             , command_traits::n1(enabled) );
    }
}

void ThermalPrinter::setUpsideDownMode(const bool enabled)
{
    typedef Command::SetPrintMode command_traits;

    setPrintMode ( enabled != false
                 ? _printMode |  command_traits::bit2
                 : _printMode & ~command_traits::bit2 );

    if(Traits::Printer::firmwareVersion >= 216) {
        typedef Command::SetUpsideDownMode command_traits;

        send ( command_traits::control
             , command_traits::function
             , command_traits::n1(enabled) );
    }
}

void ThermalPrinter::setEmphasizedMode(const bool enabled)
{
    typedef Command::SetPrintMode command_traits;

    setPrintMode ( enabled != false
                 ? _printMode |  command_traits::bit3
                 : _printMode & ~command_traits::bit3 );
}

void ThermalPrinter::setDoubleHeightMode(const bool enabled)
{
    typedef Command::SetPrintMode command_traits;

    setPrintMode ( enabled != false
                 ? _printMode |  command_traits::bit4
                 : _printMode & ~command_traits::bit4 );
}

void ThermalPrinter::setDoubleWidthMode(const bool enabled)
{
    typedef Command::SetPrintMode command_traits;

    setPrintMode ( enabled != false
                 ? _printMode |  command_traits::bit5
                 : _printMode & ~command_traits::bit5 );
}

void ThermalPrinter::setDeleteLineMode(const bool enabled)
{
    typedef Command::SetPrintMode command_traits;

    setPrintMode ( enabled != false
                 ? _printMode |  command_traits::bit6
                 : _printMode & ~command_traits::bit6 );
}

void ThermalPrinter::setDoubleStrikeMode(const bool enabled)
{
    typedef Command::SetDoubleStrikeMode command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(enabled) );
}

void ThermalPrinter::setUnderlineMode(const uint8_t mode)
{
    typedef Command::SetUnderlineMode command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(mode) );
}

void ThermalPrinter::setBarcodeLabel(const uint8_t label)
{
    typedef Command::SetBarcodeLabel command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(label) );
}

void ThermalPrinter::setBarcodeWidth(const uint8_t width)
{
    typedef Command::SetBarcodeWidth command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(width) );
}

void ThermalPrinter::setBarcodeHeight(const uint8_t height)
{
    typedef Command::SetBarcodeHeight command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(height) );
}

void ThermalPrinter::printBarcode(const char* data, const uint8_t type)
{
    typedef Command::PrintBarcode command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(type)
         , command_traits::n2(::strlen(data)) );

    /* send characters */ {
        int sent = 0;
        while(sent < 255) {
            const char character = *data;
            if(character == ASCII::NUL) {
                break;
            }
            else {
                send(character);
                ++sent;
                ++data;
            }
        }
    }
}

bool ThermalPrinter::hasPaper()
{
    typedef Command::GetPrinterStatus command_traits;

    send ( command_traits::control
         , command_traits::function
         , command_traits::n1(0x00) );

    bool has_paper = false;
    /* fetch status */ {
        int status = -1;
        constexpr int retry_count = 10;
        constexpr int retry_delay = 50;
        for(int retry = 0; retry < retry_count; ++retry) {
            if(_stream.available()) {
                status = _stream.read();
                break;
            }
            else {
                ::delay(retry_delay);
            }
        }
        if((status != -1) && ((status & 0b00000100) == 0)) {
            has_paper = true;
        }
    }
    return has_paper;
}

void ThermalPrinter::waitReady()
{
    (void) waitAndSetNextTimeout(0UL);
}

bool ThermalPrinter::waitAndSetNextTimeout(const unsigned long timeout)
{
    unsigned long       now      = ::micros();
    unsigned long const deadline = _deadline;

    /* waiting for the previous timeout */ {
        while(static_cast<long>(now - deadline) < 0L) {
            now = ::micros();
        }
    }
    /* set the next timeout */ {
        _deadline = (now + timeout);
    }
    return true;
}

bool ThermalPrinter::send(const uint8_t a)
{
    constexpr unsigned long bytetime  = Traits::Printer::serialByteTime;
    constexpr unsigned long bytecount = 1UL;
    constexpr unsigned long timeout   = (bytetime * bytecount);

    if(waitAndSetNextTimeout(timeout)) {
        _stream.write(a);
    }
    return true;
}

bool ThermalPrinter::send(const uint8_t a, const uint8_t b)
{
    constexpr unsigned long bytetime  = Traits::Printer::serialByteTime;
    constexpr unsigned long bytecount = 2UL;
    constexpr unsigned long timeout   = (bytetime * bytecount);

    if(waitAndSetNextTimeout(timeout)) {
        _stream.write(a);
        _stream.write(b);
    }
    return true;
}

bool ThermalPrinter::send(const uint8_t a, const uint8_t b, const uint8_t c)
{
    constexpr unsigned long bytetime  = Traits::Printer::serialByteTime;
    constexpr unsigned long bytecount = 3UL;
    constexpr unsigned long timeout   = (bytetime * bytecount);

    if(waitAndSetNextTimeout(timeout)) {
        _stream.write(a);
        _stream.write(b);
        _stream.write(c);
    }
    return true;
}

bool ThermalPrinter::send(const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d)
{
    constexpr unsigned long bytetime  = Traits::Printer::serialByteTime;
    constexpr unsigned long bytecount = 4UL;
    constexpr unsigned long timeout   = (bytetime * bytecount);

    if(waitAndSetNextTimeout(timeout)) {
        _stream.write(a);
        _stream.write(b);
        _stream.write(c);
        _stream.write(d);
    }
    return true;
}

bool ThermalPrinter::send(const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d, const uint8_t e)
{
    constexpr unsigned long bytetime  = Traits::Printer::serialByteTime;
    constexpr unsigned long bytecount = 5UL;
    constexpr unsigned long timeout   = (bytetime * bytecount);

    if(waitAndSetNextTimeout(timeout)) {
        _stream.write(a);
        _stream.write(b);
        _stream.write(c);
        _stream.write(d);
        _stream.write(e);
    }
    return true;
}

size_t ThermalPrinter::write(uint8_t character)
{
    constexpr unsigned long bytetime  = Traits::Printer::serialByteTime;
    constexpr unsigned long bytecount = 1UL;
    constexpr unsigned long timeout   = (bytetime * bytecount);

    if(character == ASCII::CR) {
        return 1;
    }
    if(waitAndSetNextTimeout(timeout)) {
        _stream.write(character);
    }
    return 1;
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
