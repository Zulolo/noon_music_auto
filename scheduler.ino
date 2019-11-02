#include <WiFi.h>
#include <time.h>
#include "CronAlarms.h"

const char* ssid       = "zulolo";
const char* password   = "leon0873";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 28800;
const int   daylightOffset_sec = 0;

CronId id;

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup()
{
  Serial.begin(115200);
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
//  WiFi.disconnect(true);
//  WiFi.mode(WIFI_OFF);

  // create the alarms, to trigger at specific times
  Cron.create("0 30 8 * * *", MorningAlarm, false);  // 8:30am every day
  Cron.create("0 30 18 * * *", EveningAlarm, false); // 6:30pm every day
  Cron.create("30 30 8 * * 6", WeeklyAlarm, false);  // 8:30:30 every Saturday

  // create timers, to trigger relative to when they're created
  Cron.create("*/15 * * * * *", Repeats, false);           // timer for every 15 seconds
  id = Cron.create("*/2 * * * * *", Repeats2, false);      // timer for every 2 seconds
  Cron.create("*/10 * * * * *", OnceOnly, true);           // called once after 10 seconds
  
}

void loop()
{
//  delay(14400);
  Cron.delay(3600000);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

// functions to be called when an alarm triggers:
void MorningAlarm() {
  Serial.println("Alarm: - turn lights off");
}

void EveningAlarm() {
  Serial.println("Alarm: - turn lights on");
}

void WeeklyAlarm() {
  Serial.println("Alarm: - its Saturday Morning");
}

void ExplicitAlarm() {
  Serial.println("Alarm: - this triggers only at the given date and time");
}

void Repeats() {
  Serial.println("15 second timer");
}

void Repeats2() {
  Serial.println("2 second timer");
}

void OnceOnly() {
  Serial.println("This timer only triggers once, stop the 2 second timer");
  // use Cron.free(id) to disable a timer and recycle its memory.
  Cron.free(id);
  // optional, but safest to "forget" the ID after memory recycled
  id = dtINVALID_ALARM_ID;
  // you can also use Cron.disable() to turn the timer off, but keep
  // it in memory, to turn back on later with Alarm.enable().
}
