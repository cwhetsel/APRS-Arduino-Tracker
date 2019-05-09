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

// Mpide 22 fails to compile Arduino code because it stupidly defines ARDUINO 
// as an empty macro (hence the +0 hack). UNO32 builds are fine. Just use the
// real Arduino IDE for Arduino builds. Optionally complain to the Mpide
// authors to fix the broken macro.
#if (ARDUINO + 0) == 0
#error "Oops! We need the real Arduino IDE (version 22 or 23) for Arduino builds."
#error "See trackuino.pde for details on this"

// Refuse to compile on arduino version 21 or lower. 22 includes an 
// optimization of the USART code that is critical for real-time operation
// of the AVR code.
#elif (ARDUINO + 0) < 22
#error "Oops! We need Arduino 22 or 23"
#error "See trackuino.pde for details on this"

#endif

// Adafruit GPS Library - Version: Latest 
#include <Adafruit_GPS.h>
// Trackuino custom libs
#include "config.h"
#include "afsk_avr.h"
#include "afsk_pic32.h"
#include "aprs.h"
#include "gps.h"
#include "pin.h"
#include "power.h"

// Arduino/AVR libs
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif

// Module constants
static const uint32_t VALID_POS_TIMEOUT = 2000;  // ms

// Module variables
static int32_t next_aprs = 0;

#include <SoftwareSerial.h>
SoftwareSerial mySerial(5, 6);
Adafruit_GPS GPS(&mySerial);

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pin_write(LED_PIN, LOW);
  Serial.begin(GPS_BAUDRATE);
  GPS.begin(9600);
  Serial.println("SYSTEM SETUP");
  //GPS Set output and update rate for GPS at 10 times per second
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ);
  GPS.sendCommand(PMTK_API_SET_FIX_CTL_100_MILLIHERTZ);
  //Set the GPS to only RMC and GGA values
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  afsk_setup();
  // Do not start until we get a valid time reference
  // for slotted transmissions.
  char c = 0;
  if (APRS_SLOT >= 0) {
    do {
      while (! mySerial.available()) {
        //power_save();
        delay(10);
      }
      c = GPS.read();
#ifdef DEBUG_GPS
      if (c) UDR0 = c;  
#endif
    } while (! GPS.newNMEAreceived());
    
    next_aprs = millis() + 1000 *
      (APRS_PERIOD - (GPS.seconds + APRS_PERIOD - APRS_SLOT) % APRS_PERIOD);
  }
  else {
    next_aprs = millis();
  }  
  // TODO: beep while we get a fix, maybe indicating the number of
  // visible satellites by a series of short beeps?
}

void get_pos()
{
  // Get a valid position from the GPS
  uint32_t timeout = millis();

#ifdef DEBUG_GPS
  Serial.println("\nget_pos()");
#endif
  do {
    //if we have a new message from the GPS, parse it, 
    //else read from the serial port until timeout or a full message is found
    if (GPS.newNMEAreceived()) {
      #ifdef DEBUG_GPS
      Serial.println("Parsing Sentence");
      #endif
      //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
      if (!GPS.parse(GPS.lastNMEA()))  { // this also sets the newNMEAreceived() flag to false
        #ifdef DEBUG_GPS
        Serial.println("Failed to Parse Sentence");
        #endif
      }
      else {
#ifdef DEBUG_GPS
        //gps_print();
#endif
        return;  // we can fail to parse a sentence in which case we should just wait for another
      }
    }
    else {
      GPS.read();
    }
  } while ( (millis() - timeout < VALID_POS_TIMEOUT)) ;
}

void loop()
{
  char c;
  // Time for another APRS frame
  char lon[10];
  char lat[9];
  char gps_time_str[7];
  if ((int32_t) (millis() - next_aprs) >= 0) {
    get_pos();
    //these parse functions just format the GPS values to a char array
    parse_time(GPS.hour, GPS.minute, GPS.seconds, gps_time_str);
    parse_lon(GPS.longitude, lon);
    parse_lat(GPS.latitude, lat);
    aprs_send(gps_time_str, lat, lon, GPS.altitude);
    next_aprs += APRS_PERIOD * 1000L;
    while (afsk_flush()) {
      power_save();
    }
#ifdef DEBUG_MODEM
    // Show modem ISR stats from the previous transmission
    afsk_debug();
#endif
  }
  else {
    c = GPS.read();
#ifdef DEBUG_GPS
    if (c) UDR0 = c; 
#endif
    
  }
  //power_save(); // Incoming GPS data or interrupts will wake us up?
}

