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

#include "../../../inc/MarlinConfig.h"

#if ENABLED(EXPERIMENTAL_UART)

#include "../../gcode.h"
#include "../../../feature/experimental_uart/experimental_uart.h" 
	
/**
 * M262: Send data to device trouth UART
 *
 *  M262 <string>
 *
 * Example:
 * M262 "A8 R0 disable" ; Send string "A8 R0 disable" to device 
*/

void skip_command(char*& p, size_t& len)
{
  while (*p == ' ' && *p != 0 && len > 0)
    { p++; len--;}
  while (*p != ' ' && *p != 0 && len > 0)
    { p++; len--;}
  while (*p == ' ' && *p != 0 && len > 0)
    { p++; len--;}
}

void es_clear_string(char*& p, size_t& len)
{
  if (len==0)
    return;

  if (p[0] == '"')
    p++;

  char* pp (&p[len-1]);
  while (pp!=p && *pp == ' ')
  { 
    pp--; len--;
  }
  while (pp!=p)
  {
    bool br(*pp=='"');
    pp--; len--;
    if (br)
      break;
  }
}

void GcodeSuite::M262() 
{
    char* p (parser.string_arg);
    size_t len = strlen(p);
    es_clear_string(p,len);
    if (len == 0)
    {
      SERIAL_ERROR_MSG("Empty string");
      return;
    }
    if (len > EX_SERIAL_MAX_STRING_LENGTH)
    {
      SERIAL_ERROR_MSG("String too long");
      return;
    }

    char t_message[EX_SERIAL_MAX_STRING_LENGTH+1];
    memcpy (t_message,p,len);
    t_message[len] = 0;
    experimental_uart.write (t_message);
}

/**
 * M263: Read data from device trouth UART
 *
 */

void GcodeSuite::M263() 
{
    unsigned long t_wait = millis(); 
    #ifdef EX_SERIAL_ANSWER_TIMEOUT
        unsigned long t_start(t_wait);
    #endif

    char* p (parser.command_ptr);
    size_t len = strlen(p);
    skip_command(p,len);
    es_clear_string(p, len);
    char t_message[EX_SERIAL_MAX_STRING_LENGTH+1];
    memcpy (t_message,p,len);
    t_message[len] = 0;

    char t_answer[EX_SERIAL_MAX_STRING_LENGTH+1];
    t_answer[0] = 0;

    while (true)
    {
      size_t i(0);
      while (experimental_uart.available())
      {
        t_answer[i++]=experimental_uart.read();
        if (i==EX_SERIAL_MAX_STRING_LENGTH)
          break;
      }
      t_answer[i]=0;

      if (len==0)
      {
        SERIAL_ECHO("echo: ");
        SERIAL_ECHOLN(t_answer);
        break;
      }
      else
      {
        const bool exists = strstr(t_answer, t_message) != NULL;
        if (exists)
          break;
        //if (strcmp(t_message,t_answer) == 0)
        //    break;
      }

      unsigned long t_current (millis());
      if (t_current-t_wait >= 4000)
      {
        SERIAL_ECHOLN("External serial: wait answer");
        t_wait = t_current;
      }

      #ifdef EX_SERIAL_ANSWER_TIMEOUT
        if (t_current-t_start >= EX_SERIAL_ANSWER_TIMEOUT)
        {
          SERIAL_ECHOLN("External serial: answer timeout");

          #if ENABLED(ADVANCED_PAUSE_FEATURE)
          #if ENABLED(EX_SERIAL_ANSWER_TIMEOUT_M600)
            M600();
          #endif
          #endif

          break;
        }
      #endif


      KEEPALIVE_STATE(NOT_BUSY);
      dwell (100);
    }
}
#endif //EXPERIMENTAL_UART
