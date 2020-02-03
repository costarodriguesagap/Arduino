import network
import time

def authmode_str(authmode_num):
	if authmode_num == 0:
		return 'OPEN'
	elif authmode_num == 1:
		return 'WEP'
	elif authmode_num == 2:
		return 'WPA-PSK'
	elif authmode_num == 3:
		return 'WPA2-PSK'
	elif authmode_num == 4:
		return 'WPA/WPA2-PSK'
	else:
		return 'N/A'

def main():
	station = network.WLAN(network.STA_IF)
	station.active(True)
	while True:
		station.scan()
		print('------------------------------------------------------------------------------------')
		for net in station.scan():
			print("ssid - "+str(net[0])+" | rssi - ("+str(net[3])+")dBm | auth_mode - "+authmode_str(net[4]))
		print('------------------------------------------------------------------------------------')
		time.sleep(5)

main()
