import machine
import time
pin = machine.Pin(5,machine.Pin.OUT)
while True:
	pin.value(True)
	time.sleep(1)
	pin.value(False)
	time.sleep(1)