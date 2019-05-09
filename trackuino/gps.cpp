/* trackuino copyright (C) 2010  EA5HAV Javi
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "config.h"
#include "gps.h"
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif
#include <stdlib.h>
#include <string.h>

void parse_time(uint8_t hour, uint8_t minute, uint8_t seconds, char* val)
{
  val[6] = '\0';
  sprintf(val, "%02u%02u%02u", hour, minute, seconds);
#ifdef DEBUG_GPS
  Serial.println(val);
#endif
}

void parse_lat(float latitude, char* lat)
{
  dtostrf(latitude, 4,2,lat);
  lat[7]='N';
  lat[8] = '\0';
  // Parses  = latitude in the format "DD" + "MM" (+ ".M{...}M")
}


void parse_lon(float longitude, char* lon)
{
  if(longitude < 10000) {
    dtostrf(longitude, 4,2,lon+1);
    lon[0]='0';
  }
  else {
   dtostrf(longitude, 5,2,lon);
  }
  lon[8]='W';
  lon[9] = '\0';
  // Parses  = latitude in the format "DDD" + "MM" (+ ".M{...}M")
}

