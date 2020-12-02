/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "../../inc/MarlinConfig.h"

#if ENABLED(EXPERIMENTAL_UART)

#include "experimental_uart.h"

#if ENABLED(EX_SERIAL_HARDWARE)
#else
    #include "SoftwareSerial.h"
    SoftwareSerial EX_SERIAL (EX_SERIAL_RX, EX_SERIAL_TX);
#endif
ExperimentalUART experimental_uart;

ExperimentalUART::ExperimentalUART()
    : pw(0)
    , pr(0)
    , m_inited(false)
{
}

void ExperimentalUART::init()
{
    EX_SERIAL.begin (EX_SERIAL_BAUD_RATE);
    m_inited = true;
}

void ExperimentalUART::loop()
{
    while (EX_SERIAL.available() > 0)
    {
        unsigned npw ((pw + 1) % sbuf);
        if (npw==pr)
            return;
        buf[pw]=EX_SERIAL.read();

        #if ENABLED(EX_SERIAL_DEBUG)
            SERIAL_ECHO("External serial debug, read:");
            SERIAL_ECHOLN(buf[pw]);
        #endif

        pw = npw;
    }
}

void ExperimentalUART::write (const char* a_message)
{
    if (!m_inited)
    {
        #if ENABLED(EX_SERIAL_DEBUG)
            SERIAL_ECHOLN("Serial not inited");
        #endif
        return;
    }
    #if ENABLED(EX_SERIAL_DEBUG)
        SERIAL_ECHOLN("ExperimentalUART::write println");
        SERIAL_ECHOLN(a_message);
    #endif
    EX_SERIAL.println (a_message);
    #if ENABLED(EX_SERIAL_DEBUG)
        SERIAL_ECHOLN("ExperimentalUART::write flush");
    #endif
    EX_SERIAL.flush();
    #if ENABLED(EX_SERIAL_DEBUG)
        SERIAL_ECHOLN("ExperimentalUART::write finish");
    #endif
}

bool ExperimentalUART::available()
{
    //return EX_SERIAL.available() > 0;
    return pw != pr;
}

char ExperimentalUART::read()
{
    if (!available())
        return 0;
    char c = buf[pr];
    pr = (pr + 1) % sbuf;
    return c;
}


#endif // EXPERIMENTAL_UART
