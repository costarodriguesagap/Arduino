from machine import Pin
import time
led = Pin(13,Pin.OUT)
whileTrue:
  led.value(1)
  time.sleep(0.5)
  led.value(0)
  time.sleep(0.5)
