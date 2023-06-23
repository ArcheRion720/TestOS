#include "rtc.h"
#include "hal.h"
#include "interrupts.h"

uint8_t binary_read;
time_t boot_time;

uint8_t read_rtc(uint8_t reg)
{
    outport8(0x70, reg);
    return inport8(0x71);
}

void write_rtc(uint8_t reg, uint8_t value)
{
    outport8(0x70, reg);
    outport8(0x71, value);
}

time_t read_time()
{
    union
    {
        time_t time; 
        uint64_t ltime;
    } result;

    result.time.year = read_rtc(RTC_REGISTER_YEAR);
    result.time.month = read_rtc(RTC_REGISTER_MONTH);
    result.time.day_month = read_rtc(RTC_REGISTER_DAY_MONTH);
    result.time.day_week = read_rtc(RTC_REGISTER_DAY_WEEK);
    result.time.hour = read_rtc(RTC_REGISTER_HOUR);
    result.time.minute = read_rtc(RTC_REGISTER_MINUTE);
    result.time.second = read_rtc(RTC_REGISTER_SECOND);

    if(!binary_read)
    {
        result.ltime = (((result.ltime >> 4) & BCD_MASK) * 10) + (result.ltime & BCD_MASK);
    }

    return result.time;
}

void rtc_handler()
{
    boot_time = read_time();
    send_eoi(8);
}

void init_rtc()
{
    register_isr_handler(40, (isr_t)&rtc_handler);

    uint8_t rtc_data;
    rtc_data = read_rtc(RTC_REGISTER_B);
    rtc_data |= RTC_24H;
    rtc_data |= RTC_UPDATE_INT;
    rtc_data &= ~RTC_ALARM_INT;
    rtc_data &= ~RTC_PERIOD_INT;
    binary_read = RTC_BIN(rtc_data);

    write_rtc(RTC_REGISTER_B, rtc_data);
    read_rtc(RTC_REGISTER_C);

    // log("RTC clock initialised");
}

time_t read_rtc_time()
{
    return boot_time;
}