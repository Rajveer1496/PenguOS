// RTC (Real Time Clock) - reading time from CMOS

#include <stdint.h>
#include "debug.h"

#define CMOS_ADDRESS_PORT 0x70
#define CMOS_DATA_PORT 0x71

//CMOS registers
#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x02
#define RTC_HOURS 0x04
#define RTC_STATUS_A 0x0A
#define RTC_STATUS_B 0x0B

#define TIMEZONE_OFFSET_MIN 330 //IST = UTC + 5:30 (qemu gives UTC time)

extern uint8_t inb(uint16_t port); //to read data from port (in io.asm)
extern void outb(uint16_t port, uint8_t value); // to write data to a port

uint8_t rtc_read_register(uint8_t reg){
    outb(CMOS_ADDRESS_PORT,reg);
    return inb(CMOS_DATA_PORT);
}

int rtc_update_in_progress(){ //bit 7 of status A is set while RTC is updating time
    return (rtc_read_register(RTC_STATUS_A) & 0x80);
}

#define BCD_TO_BINARY(x) (((x) & 0x0F) + (((x) >> 4) * 10))

void rtc_get_time(int *hour,int *minute,int *second){
    while(rtc_update_in_progress()); //wait so we dont read half updated time

    uint8_t sec = rtc_read_register(RTC_SECONDS);
    uint8_t min = rtc_read_register(RTC_MINUTES);
    uint8_t hr = rtc_read_register(RTC_HOURS);
    uint8_t status_b = rtc_read_register(RTC_STATUS_B);

    //bit 2 of status B = 0 means values are in BCD
    if(!(status_b & 0x04)){
        sec = BCD_TO_BINARY(sec);
        min = BCD_TO_BINARY(min);
        hr = ((hr & 0x0F) + (((hr & 0x70) >> 4) * 10)) | (hr & 0x80); //keep PM bit
    }

    //bit 1 of status B = 0 means 12 hour format
    if(!(status_b & 0x02) && (hr & 0x80)){
        hr = ((hr & 0x7F) + 12) % 24;
    }

    //timezone
    int total_min = (hr*60) + min + TIMEZONE_OFFSET_MIN;
    total_min = total_min % (24*60);

    *hour = total_min/60;
    *minute = total_min%60;
    *second = sec;
}
