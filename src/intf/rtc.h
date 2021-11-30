#pragma once
#include <stdint.h>

#define RTC_24H 0x02
#define RTC_UPDATE_INT 0x10
#define RTC_ALARM_INT 0x20
#define RTC_PERIOD_INT 0x40

#define RTC_REGISTER_YEAR 0x09
#define RTC_REGISTER_MONTH 0x08
#define RTC_REGISTER_DAY_MONTH 0x07
#define RTC_REGISTER_DAY_WEEK 0x06
#define RTC_REGISTER_HOUR 0x04
#define RTC_REGISTER_MINUTE 0x02
#define RTC_REGISTER_SECOND 0x00

#define RTC_BIN(data) (data & 0x04)

#define BCD_MASK 0x0F0F0F0F0F0F0F0F

struct time
{
    uint8_t century; //unused
    uint8_t year;
    uint8_t month;
    uint8_t day_month;
    uint8_t day_week;
    uint8_t hour;
    uint8_t minute; 
    uint8_t second;
};

typedef struct time time_t;

void init_rtc();
void rtc_handler();
uint8_t read_rtc(uint8_t reg);
void write_rtc(uint8_t reg, uint8_t value);
time_t read_rtc_time();