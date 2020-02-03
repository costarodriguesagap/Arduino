import machine
import time
pin = machine.Pin(5,machine.Pin.OUT)

with open("inst.txt",'r') as fr_ln:
	for line in fr_ln.readlines():
		res = line.split(":")
		#print(res[0])
		if (res[0] == 'ON'):
			pin.value(True)
		else:
			pin.value(False)
		time.sleep(float(res[1]))