from machine import Pin
from dht import DHT22
import time

dht22 = DHT22(Pin(27))

def read_DHT22():
	dht22.measure()
	return dht22.temperature(),dht22.humidity()

def web_page(t,h):
  html = """<html><head><title>Temperatura Divisoes</title></head><body><p>&nbsp;</p><h1 style="text-align: center;"><span style="color: #000080;">
  Temperatura Sala</span></h1><h3 style="text-align: left;"><span style="color: #ff0000;">Temperatura</span>:&nbsp;<strong>""" + t + """</strong></h3>
  <h3 style="text-align: left;"><span style="color: #0000ff;">Humidade</span> &nbsp; &nbsp;:&nbsp;<strong>""" + h + """</strong></h3></body></html>"""
  return html

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', 80))
s.listen(5)

while True:
  conn, addr = s.accept()
  print('Got a connection from %s' % str(addr))
  request = conn.recv(1024)
#  request = str(request)
#  print('Content = %s' % request)
  medida = read_DHT22()
  response = web_page(str(medida[0]),str(medida[1]))
  conn.send('HTTP/1.1 200 OK\n')
  conn.send('Content-Type: text/html\n')
  conn.send('Connection: close\n\n')
  conn.sendall(response)
  conn.close()
  time.sleep(3)
  print("----------------------------------------")
