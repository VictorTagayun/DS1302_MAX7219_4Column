// DS1302_Serial_Easy (C)2010 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// Adopted for DS1302RTC library by Timur Maksimov 2014
//
// A quick demo of how to use my DS1302-library to
// quickly send time and date information over a serial link
//
// I assume you know how to connect the DS1302.
// DS1302:  CE pin    -> Arduino Digital 27
//          I/O pin   -> Arduino Digital 29
//          SCLK pin  -> Arduino Digital 31
//          VCC pin   -> Arduino Digital 33
//          GND pin   -> Arduino Digital 35

#include <TimeLib.h>
#include <DS1302RTC.h>
// Set pins:  CE, IO,CLK
DS1302RTC RTC(8, 9, 10);

// MAX7219
#include "LedControl.h" //  need the library
#define PIN_EYES_DIN 13
#define PIN_EYES_CS 12
#define PIN_EYES_CLK 11
LedControl lc = LedControl(PIN_EYES_DIN, PIN_EYES_CLK, PIN_EYES_CS, 2);
int ledintensity = 1; // LED Intensity 0-15

#include "fontTiny.h"
int present_minute = 0;
int previous_minute = 0;
int present_hour = 0;
int previous_hour = 0;
char minute_ones[4];
int c_minute_ones = 100;
char minute_tens[4];
int c_minute_tens = 100;
char hour_ones[4];
int c_hour_ones = 100;
char hour_tens[4];
int c_hour_tens = 100;
char temp_num[8];
char temp_char;

// Timer Interval
#include <SimpleTimer.h>
SimpleTimer timer;

void setup()
{
  Serial.begin(9600);
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.shutdown(1, false);
  lc.setIntensity(0, ledintensity); // sets brightness (0~15 possible values)
  lc.setIntensity(1, ledintensity);
  lc.clearDisplay(0);// clear screen
  lc.clearDisplay(1);
  Serial.println("");
  Serial.println("");

  if (RTC.haltRTC()) {
    Serial.println("The DS1302 is stopped.  Please run the SetTime");
    Serial.println("example to initialize the time and begin running.");
    Serial.println();
  }
  if (!RTC.writeEN()) {
    Serial.println("The DS1302 is write protected. This normal.");
    Serial.println();
  }

  delay(2000);
  timer.setInterval(1000, read_RTC_1000ms);
}

void display_minute_ones()
{
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_minute_ones][7 - cntr]; // copy to temp variable
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      minute_ones[col_cntr] = minute_ones[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      minute_ones[col_cntr] = minute_ones[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }

  //  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  //  {
  //    lc.setRow(0, 7 - col_cntr, minute_ones[col_cntr]); // 180deg mirror
  //  }

  char c_bit;
  for (int row_cntr = 0; row_cntr < 8; row_cntr++)
  {
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      lc.setLed(0, 7 - col_cntr, 7 - row_cntr, true );
    }
    delay(150);
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      // lc.setLed(0, 0, 0, true); // lower left
      // lc.setLed(0, 7, 0, true); // lower right
      c_bit = minute_ones[col_cntr] >> row_cntr;
      c_bit = c_bit & 0x01;
      lc.setLed(0, 7 - col_cntr, 7 - row_cntr, c_bit ); // lower left
    }
  }
}

void display_minute_tens()
{
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_minute_tens][7 - cntr]; // copy to temp variable
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      minute_tens[col_cntr] = minute_tens[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      minute_tens[col_cntr] = minute_tens[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }

  //  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  //  {
  //    lc.setRow(0, 3 - col_cntr, minute_tens[col_cntr]); // 180deg mirror
  //  }

  char c_bit;
  for (int row_cntr = 0; row_cntr < 8; row_cntr++)
  {
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      lc.setLed(0, 3 - col_cntr, 7 - row_cntr, true );
    }
    delay(150);
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      // lc.setLed(0, 0, 0, true); // lower left
      // lc.setLed(0, 7, 0, true); // lower right
      c_bit = minute_tens[col_cntr] >> row_cntr;
      c_bit = c_bit & 0x01;
      lc.setLed(0, 3 - col_cntr, 7 - row_cntr, c_bit ); // lower left
    }
  }

}

void display_hour_ones()
{
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_hour_ones][7 - cntr]; // copy to temp variable
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      hour_ones[col_cntr] = hour_ones[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      hour_ones[col_cntr] = hour_ones[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }

  //  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  //  {
  //    lc.setRow(1, 7 - col_cntr, hour_ones[col_cntr]); // 180deg mirror
  //  }

  char c_bit;
  for (int row_cntr = 0; row_cntr < 8; row_cntr++)
  {
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      lc.setLed(1, 7 - col_cntr, 7 - row_cntr, true );
    }
    delay(150);
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      // lc.setLed(0, 0, 0, true); // lower left
      // lc.setLed(0, 7, 0, true); // lower right
      c_bit = hour_ones[col_cntr] >> row_cntr;
      c_bit = c_bit & 0x01;
      lc.setLed(1, 7 - col_cntr, 7 - row_cntr, c_bit ); // lower left
    }
  }
}

void display_hour_tens()
{
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_hour_tens][7 - cntr]; // copy to temp variable
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      hour_tens[col_cntr] = hour_tens[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      hour_tens[col_cntr] = hour_tens[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }

  //  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  //  {
  //    lc.setRow(1, 3 - col_cntr, hour_tens[col_cntr]); // 180deg mirror
  //  }

  char c_bit;
  for (int row_cntr = 0; row_cntr < 8; row_cntr++)
  {
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      lc.setLed(1, 3 - col_cntr, 7 - row_cntr, true );
    }
    delay(150);
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      // lc.setLed(0, 0, 0, true); // lower left
      // lc.setLed(0, 7, 0, true); // lower right
      c_bit = hour_tens[col_cntr] >> row_cntr;
      c_bit = c_bit & 0x01;
      lc.setLed(1, 3 - col_cntr, 7 - row_cntr, c_bit ); // lower left
    }
  }
}

void read_RTC_1000ms()
{
  tmElements_t tm;

  if (!RTC.read(tm)) {
    present_minute = tm.Minute;
    present_hour = tm.Hour;

    if (present_minute != previous_minute) {
      Serial.print("  Time = ");
      Serial.print(present_hour);
      Serial.print(':');
      Serial.print(present_minute);
      Serial.println();

      if (c_minute_ones != (present_minute % 10)) {
        c_minute_ones = present_minute % 10;
        Serial.print("c_minute_ones : ");
        Serial.println(c_minute_ones);
        display_minute_ones();
      }

      if (c_minute_tens != (present_minute / 10)) {
        c_minute_tens = present_minute / 10;
        Serial.print("c_minute_tens : ");
        Serial.println(c_minute_tens);
        display_minute_tens();
      }

      if (c_hour_ones != (present_hour % 10)) {
        c_hour_ones = present_hour % 10;
        Serial.print("c_hour_ones : ");
        Serial.println(c_hour_ones);
        display_hour_ones();
      }

      if (c_hour_tens != (present_hour / 10)) {
        c_hour_tens = present_hour / 10;
        Serial.print("c_hour_tens : ");
        Serial.println(c_hour_tens);
        display_hour_tens();
      }
    }

  } else {
    Serial.println("DS1302 read error!  Please check the circuitry.");
    Serial.println();
    delay(9000);
  }

  previous_minute = present_minute;
  previous_hour = present_hour;

}

void loop()
{
  timer.run();
}

/*
  void display_time_1000ms()
  {

  display_minute_ones();
  display_minute_tens();
  display_hour_ones();
  display_hour_tens();

  }

  void display_minute_ones()
  {
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_minute_ones][7 - cntr]; // copy to temp variable
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      minute_ones[col_cntr] = minute_ones[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      minute_ones[col_cntr] = minute_ones[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }

  //  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  //  {
  //    // lc.setRow(0, 7 - col_cntr, minute_ones[3 - col_cntr]); // 180 deg
  //    // lc.setRow(0, col_cntr + 4, minute_ones[col_cntr]); // 180 deg
  //    lc.setRow(1, 3 - col_cntr, hour_tens[col_cntr]); // 180deg mirror
  //  }

  char c_bit;
  for (int row_cntr = 0; row_cntr < 8; row_cntr++)
  {
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      lc.setLed(0, 7 - col_cntr, 7 - row_cntr, true );
    }
    delay(150);
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      // lc.setLed(0, 0, 0, true); // lower left
      // lc.setLed(0, 7, 0, true); // lower right
      c_bit = minute_ones[col_cntr] >> row_cntr;
      c_bit = c_bit & 0x01;
      lc.setLed(0, 7 - col_cntr, 7 - row_cntr, c_bit ); // lower left
    }
  }
  }

  void display_minute_tens()
  {
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_minute_tens][7 - cntr]; // copy to temp variable
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      minute_tens[col_cntr] = minute_tens[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      minute_tens[col_cntr] = minute_tens[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    lc.setRow(0, 3 - col_cntr, minute_tens[col_cntr]); // 180deg mirror
  }
  }

  void display_hour_ones()
  {
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_hour_ones][7 - cntr]; // copy to temp variable
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      hour_ones[col_cntr] = hour_ones[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      hour_ones[col_cntr] = hour_ones[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    lc.setRow(1, 7 - col_cntr, hour_ones[col_cntr]); // 180deg mirror
  }
  }

  void display_hour_tens()
  {
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_hour_tens][7 - cntr]; // copy to temp variable
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      hour_tens[col_cntr] = hour_tens[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      hour_tens[col_cntr] = hour_tens[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    lc.setRow(1, 3 - col_cntr, hour_tens[col_cntr]); // 180deg mirror
  }
  }
  // for debugging

  //for (int cntr = 0; cntr < 8; cntr++)
  //{
  //  minute_ones[0] = minute_ones[0] << 1;
  //  Serial.print("minute_ones[0] << 1        :"); Serial.println(minute_ones[0], BIN);
  //  minute_ones[0] = minute_ones[0] & 0xfe;
  //  Serial.print("minute_ones[0] & 0xfe      :"); Serial.println(minute_ones[0], BIN);
  //  temp_char = temp_num[cntr] & 0x01;
  //  Serial.print("temp_num[cntr] & 0x01      :"); Serial.println(temp_char, BIN);
  //  minute_ones[0] = minute_ones[0] | temp_char;
  //  Serial.print("minute_ones[0] | temp_char :"); Serial.println(minute_ones[0], BIN);
  //  Serial.println("*******************");
  //  Serial.println(minute_ones[0], BIN);
  //}

  //for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  //{
  //  for (int row_cntr = 0; row_cntr < 8; row_cntr++)
  //  {
  //    minute_ones[col_cntr] = minute_ones[col_cntr] << 1; // initial shift
  //    minute_ones[col_cntr] = minute_ones[col_cntr] & 0xfe; // set zero the last bit because if last no is 1, it will be 1 also
  //    temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
  //    minute_ones[col_cntr] = minute_ones[col_cntr] | temp_char; // AND the bit to
  //    temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
  //  }
  //  Serial.print("col_cntr  :"); Serial.println(col_cntr);
  //  Serial.print("minute_ones[col_cntr] :"); Serial.println(minute_ones[col_cntr], BIN);
  //}

*/
