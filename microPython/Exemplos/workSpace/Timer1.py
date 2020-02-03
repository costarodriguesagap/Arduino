from machine import Timer
tim = Timer(-1)

def func(t):
  print (1)
  
tim.init(period=2000, mode=Timer.PERIODIC, callback=func)
