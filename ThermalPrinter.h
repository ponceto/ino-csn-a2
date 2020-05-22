/*
 * ThermalPrinter.h - Copyright (c) 2014-2020 - Olivier Poncet
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
#ifndef __INO_CSN_A2_ThermalPrinter_h__
#define __INO_CSN_A2_ThermalPrinter_h__

// ---------------------------------------------------------------------------
// ThermalPrinter defines
// ---------------------------------------------------------------------------

#ifndef THERMAL_PRINTER_FIRMWARE
#define THERMAL_PRINTER_FIRMWARE 216
#endif

#ifndef THERMAL_PRINTER_BAUDRATE
#define THERMAL_PRINTER_BAUDRATE 19200UL
#endif

#ifndef THERMAL_PRINTER_BYTETIME
#define THERMAL_PRINTER_BYTETIME (((8UL + 2UL) * 1000000UL) / (THERMAL_PRINTER_BAUDRATE))
#endif

// ---------------------------------------------------------------------------
// ThermalPrinter Character Set
// ---------------------------------------------------------------------------

struct ThermalPrinterCharacterSet
{
    static constexpr uint8_t CS_DEFAULT      =  0;
    static constexpr uint8_t CS_USA          =  0;
    static constexpr uint8_t CS_FRANCE       =  1;
    static constexpr uint8_t CS_GERMANY      =  2;
    static constexpr uint8_t CS_UK           =  3;
    static constexpr uint8_t CS_DENMARK1     =  4;
    static constexpr uint8_t CS_SWEDEN       =  5;
    static constexpr uint8_t CS_ITALY        =  6;
    static constexpr uint8_t CS_SPAIN1       =  7;
    static constexpr uint8_t CS_JAPAN        =  8;
    static constexpr uint8_t CS_NORWAY       =  9;
    static constexpr uint8_t CS_DENMARK2     = 10;
    static constexpr uint8_t CS_SPAIN2       = 11;
    static constexpr uint8_t CS_LATINAMERICA = 12;
    static constexpr uint8_t CS_KOREA        = 13;
    static constexpr uint8_t CS_SLOVENIA     = 14;
    static constexpr uint8_t CS_CROATIA      = 14;
    static constexpr uint8_t CS_CHINA        = 15;
};

// ---------------------------------------------------------------------------
// ThermalPrinter Character Page
// ---------------------------------------------------------------------------

struct ThermalPrinterCharacterPage
{
    static constexpr uint8_t CP_DEFAULT      =  0;
    static constexpr uint8_t CP_CP437        =  0;
    static constexpr uint8_t CP_KATAKANA     =  1;
    static constexpr uint8_t CP_CP850        =  2;
    static constexpr uint8_t CP_CP860        =  3;
    static constexpr uint8_t CP_CP863        =  4;
    static constexpr uint8_t CP_CP865        =  5;
    static constexpr uint8_t CP_WCP1251      =  6;
    static constexpr uint8_t CP_CP866        =  7;
    static constexpr uint8_t CP_MIK          =  8;
    static constexpr uint8_t CP_CP755        =  9;
    static constexpr uint8_t CP_IRAN         = 10;
    static constexpr uint8_t CP_RESERVED1    = 11;
    static constexpr uint8_t CP_RESERVED2    = 12;
    static constexpr uint8_t CP_RESERVED3    = 13;
    static constexpr uint8_t CP_RESERVED4    = 14;
    static constexpr uint8_t CP_CP862        = 15;
    static constexpr uint8_t CP_WCP1252      = 16;
    static constexpr uint8_t CP_WCP1253      = 17;
    static constexpr uint8_t CP_CP852        = 18;
    static constexpr uint8_t CP_CP858        = 19;
    static constexpr uint8_t CP_IRAN2        = 20;
    static constexpr uint8_t CP_LATVIAN      = 21;
    static constexpr uint8_t CP_CP864        = 22;
    static constexpr uint8_t CP_ISO_8859_1   = 23;
    static constexpr uint8_t CP_CP737        = 24;
    static constexpr uint8_t CP_WCP1257      = 25;
    static constexpr uint8_t CP_THAI         = 26;
    static constexpr uint8_t CP_CP720        = 27;
    static constexpr uint8_t CP_CP855        = 28;
    static constexpr uint8_t CP_CP857        = 29;
    static constexpr uint8_t CP_WCP1250      = 30;
    static constexpr uint8_t CP_CP775        = 31;
    static constexpr uint8_t CP_WCP1254      = 32;
    static constexpr uint8_t CP_WCP1255      = 33;
    static constexpr uint8_t CP_WCP1256      = 34;
    static constexpr uint8_t CP_WCP1258      = 35;
    static constexpr uint8_t CP_ISO_8859_2   = 36;
    static constexpr uint8_t CP_ISO_8859_3   = 37;
    static constexpr uint8_t CP_ISO_8859_4   = 38;
    static constexpr uint8_t CP_ISO_8859_5   = 39;
    static constexpr uint8_t CP_ISO_8859_6   = 40;
    static constexpr uint8_t CP_ISO_8859_7   = 41;
    static constexpr uint8_t CP_ISO_8859_8   = 42;
    static constexpr uint8_t CP_ISO_8859_9   = 43;
    static constexpr uint8_t CP_ISO_8859_15  = 44;
    static constexpr uint8_t CP_THAI2        = 45;
    static constexpr uint8_t CP_CP856        = 46;
    static constexpr uint8_t CP_CP874        = 47;
};

// ---------------------------------------------------------------------------
// ThermalPrinter Barcode System
// ---------------------------------------------------------------------------

struct ThermalPrinterBarcodeSystem
{
    static constexpr uint8_t BS_UPC_A        = 65;
    static constexpr uint8_t BS_UPC_E        = 66;
    static constexpr uint8_t BS_EAN_13       = 67;
    static constexpr uint8_t BS_EAN_8        = 68;
    static constexpr uint8_t BS_CODE_39      = 69;
    static constexpr uint8_t BS_ITF          = 70;
    static constexpr uint8_t BS_CODABAR      = 71;
    static constexpr uint8_t BS_CODE_93      = 72;
    static constexpr uint8_t BS_CODE_128     = 73;

    static constexpr uint8_t BS_TEXT_NONE    = 48;
    static constexpr uint8_t BS_TEXT_ABOVE   = 49;
    static constexpr uint8_t BS_TEXT_BELOW   = 50;
    static constexpr uint8_t BS_TEXT_BOTH    = 51;

    static constexpr uint8_t BS_WIDTH_TINY   = 2;
    static constexpr uint8_t BS_WIDTH_SMALL  = 3;
    static constexpr uint8_t BS_WIDTH_MEDIUM = 4;
    static constexpr uint8_t BS_WIDTH_LARGE  = 5;
    static constexpr uint8_t BS_WIDTH_HUGE   = 6;
};

// ---------------------------------------------------------------------------
// ThermalPrinter
// ---------------------------------------------------------------------------

class ThermalPrinter
    : public Print
{
public: // public interface
    ThermalPrinter(Stream& stream);

    void begin();
    void reset();
    void end();

    void printTestPage         ( );

    void wakePrinter           ( );

    void sleepPrinter          ( const uint16_t seconds );

    void feedLines             ( const uint8_t lines );

    void feedRows              ( const uint8_t rows );

    void setJustification      ( const uint8_t justification );

    void setCharacterSet       ( const uint8_t number );

    void setCharacterPage      ( const uint8_t number );

    void setCharacterSpacing   ( const uint8_t spacing );

    void setLineSpacing        ( const uint8_t spacing );

    void setPrintingParameters ( const uint8_t dots
                               , const uint8_t time
                               , const uint8_t interval );

    void setPrintingDensity    ( const uint8_t density
                               , const uint8_t breaktime );

    void setOnline             ( const bool online );

    void setPrintMode          ( const uint8_t printMode );

    void setSmallMode          ( const bool enabled );

    void setReverseMode        ( const bool enabled );

    void setUpsideDownMode     ( const bool enabled );

    void setEmphasizedMode     ( const bool enabled );

    void setDoubleHeightMode   ( const bool enabled );

    void setDoubleWidthMode    ( const bool enabled );

    void setDeleteLineMode     ( const bool enabled );

    void setDoubleStrikeMode   ( const bool enabled );

    void setUnderlineMode      ( const uint8_t mode );

    void setBarcodeLabel       ( const uint8_t label );

    void setBarcodeWidth       ( const uint8_t width );

    void setBarcodeHeight      ( const uint8_t height );

    void printBarcode          ( const char *text, const uint8_t type );

    bool hasPaper              ( );

    void waitReady             ( );

    virtual size_t write(uint8_t character) override;

protected: // protected interface
    bool waitAndSetNextTimeout(const unsigned long timeout);

    bool send ( const uint8_t a );

    bool send ( const uint8_t a
              , const uint8_t b );

    bool send ( const uint8_t a
              , const uint8_t b
              , const uint8_t c );

    bool send ( const uint8_t a
              , const uint8_t b
              , const uint8_t c
              , const uint8_t d );

    bool send ( const uint8_t a
              , const uint8_t b
              , const uint8_t c
              , const uint8_t d
              , const uint8_t e );

private: // private data
    Stream&       _stream;
    unsigned long _deadline;
    uint8_t       _printMode;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __INO_CSN_A2_ThermalPrinter_h__ */
