
import machine
import sys
import network
import utime
import urequests

# Pin definitions
repl_button = machine.Pin(0, machine.Pin.IN, machine.Pin.PULL_UP)
repl_led = machine.Pin(13, machine.Pin.OUT)
led_on = False

# Network settings
wifi_ssid = "BTXPS1_2.4G"
wifi_password = "#Pentagrama24968#"

# Web page (non-SSL) to get
url = "http://google.com"

# Create a station object to store our connection
station = network.WLAN(network.STA_IF)
station.active(True)

# Continually try to connect to WiFi access point
while not station.isconnected():

    # Try to connect to WiFi access point
    print("Connecting...")
    station.connect(wifi_ssid, wifi_password)

# Continually print out HTML from web page as long as we have a connection
while station.isconnected():
    # Display connection details
    print("Connected!")
    print("My IP Address:", station.ifconfig()[0])
    if not led_on:
      repl_led.value(1)
      len_on = True
    else:
      repl_led.value(1)
      len_on = True
    # Perform HTTP GET request on a non-SSL web
    utime.sleep_ms(10000)

# If we lose connection, repeat this main.py and retry for a connection
print("Connection lost. Trying again.")

