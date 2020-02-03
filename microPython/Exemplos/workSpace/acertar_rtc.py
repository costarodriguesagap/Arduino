# ESP8266 MicroPython Web Clock
# by Alan Wang

import network, urequests, utime, machine
from machine import RTC, I2C, Pin
import DS3231
#from ssd1306 import SSD1306_I2C

# user data
ssid = "BTXPS1_2.4G" # wifi router name
pw = "#Pentagrama24968#" # wifi router password
url = "http://worldtimeapi.org/api/timezone/Europe/Lisbon" # see http://worldtimeapi.org/timezones
web_query_delay = 60000 # interval time of web JSON query
retry_delay = 5000 # interval time of retry after a failed Web query

# Pin(5) no esp32 => D5
# Pin(4) no esp32 => D4
i2c = I2C(sda = Pin(5), scl=Pin(4)) 
ds = DS3231.DS3231(i2c)

# initialization

# SSD1306 OLED display
print("Connecting to wifi...")
#oled = SSD1306_I2C(128, 64, I2C(scl=Pin(5), sda=Pin(4)))
#oled.fill(0)
#oled.text("Connecting", 0, 5)
#oled.text(" to wifi...", 0, 15)
#oled.show()

# internal real time clock
rtc = RTC()

# wifi connection
wifi = network.WLAN(network.STA_IF) # station mode
wifi.active(True)
wifi.connect(ssid, pw)

# wait for connection
while not wifi.isconnected():
    pass

# wifi connected
print("IP:", wifi.ifconfig()[0], "\n")
#oled.text("Connected. IP: ", 0, 35)
#oled.text(" " + str(wifi.ifconfig()[0]), 0, 45)
#oled.show()

# set timer
update_time = utime.ticks_ms() - web_query_delay

# main loop
while True:
    
    # if lose wifi connection, reboot ESP8266
    if not wifi.isconnected():
        machine.reset()
    
    # query and get web JSON every web_query_delay ms
    if utime.ticks_ms() - update_time >= web_query_delay:
    
        # HTTP GET data
        response = urequests.get(url)
    
        if response.status_code == 200: # query success
        
            print("JSON response:\n", response.text)
            
            # parse JSON
            parsed = response.json()
            datetime_str = str(parsed["datetime"])
            year = int(datetime_str[0:4])
            month = int(datetime_str[5:7])
            day = int(datetime_str[8:10])
            hour = int(datetime_str[11:13])
            minute = int(datetime_str[14:16])
            second = int(datetime_str[17:19])
            subsecond = int(round(int(datetime_str[20:26]) / 10000))
        
            # update internal RTC
            rtc.datetime((year, month, day, 0, hour, minute, second, subsecond))
            update_time = utime.ticks_ms()
            print("RTC updated\n")
            #ds.DateTime([year, month, day, hour, minute, second, 0])
            #print("Year - ",str(year))
            #print("Month - ",month)
            #print("Day - ",day)
            #print("Hour - ",hour)
            #print("Minute - ",minute)
            #print("Secound - ",second)
   
        else: # query failed, retry retry_delay ms later
            update_time = utime.ticks_ms() - web_query_delay + retry_delay
    
    # generate formated date/time strings from internal RTC
    date_str = "Date: {1:02d}/{2:02d}/{0:4d}".format(*rtc.datetime())
    time_str = "Time: {4:02d}:{5:02d}:{6:02d}".format(*rtc.datetime())
    #print("date_str - ",date_str)
    #print("time_str - ",time_str)
    

    # update SSD1306 OLED display
    #oled.fill(0)
    #oled.text("ESP8266 WebClock", 0, 5)
    #oled.text(date_str, 0, 25)
    #oled.text(time_str, 0, 45)
    #oled.show()
    
    utime.sleep(0.1)

