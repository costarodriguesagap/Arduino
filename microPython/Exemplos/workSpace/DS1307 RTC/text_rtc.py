# VCC-GND STM32F407VET6 on SPI3
>>> from machine import I2C, Pin
>>> i2c = I2C(3) # SCL=A8, SDA=C9
>>> i2c.scan()
[80, 104]

# 80 == 0x50 == AT24C32N EEPROM
# 104 == 0x68 == DS1307

>>> import ds1307
>>> ds = ds1307.DS1307(i2c)
ds.datetime()
ds.datetime(now)

# initial datetime value, oscillator disabled
>>> ds.datetime()
(2000, 1, 1, 0, 0, 0, 0, 0)

# read datetime after 1 second (no change)
>>> ds.datetime()
(2000, 1, 1, 0, 0, 0, 0, 0)

# enable oscillator
>>> ds.halt(False)

# read datetime after 1 second
>>> ds.datetime()
(2000, 1, 1, 0, 0, 0, 1, 0)

# read datetime after 1 more second
>>> ds.datetime()
(2000, 1, 1, 0, 0, 0, 2, 0)

# read datetime after 1 more second
>>> ds.datetime()
(2000, 1, 1, 0, 0, 0, 3, 0)

# set the datetime 24th March 2018 at 1:45:21 PM
>>> now = (2018, 3, 24, 6, 13, 45, 21, 0)
>>> ds.datetime(now)

# read datetime after 1 second
>>> ds.datetime()
(2018, 3, 24, 6, 13, 45, 22, 0)

# read datetime after 1 second
>>> ds.datetime()
(2018, 3, 24, 6, 13, 45, 23, 0)

# disable oscillator
>>> ds.halt(True)

# read datetime after 1 second
>>> ds.datetime()
(2018, 3, 24, 6, 13, 45, 24, 0)

# read datetime after 1 second (no change)
>>> ds.datetime()
(2018, 3, 24, 6, 13, 45, 24, 0)

# minute increment (00:00:59 -> 00:01:00)
>>> ds.datetime((2018, 3, 24, 6, 0, 0, 58, 0))
>>> ds.datetime()
(2018, 3, 24, 6, 0, 0, 59, 0)
>>> ds.datetime()
(2018, 3, 24, 6, 0, 1, 0, 0)

# hour increment (00:59:59 -> 01:00:00)
>>> ds.datetime((2018, 3, 24, 6, 0, 59, 58, 0))
>>> ds.datetime()
(2018, 3, 24, 6, 0, 59, 59, 0)
>>> ds.datetime()
(2018, 3, 24, 6, 1, 0, 0, 0)

# day + weekday increment (23:59:59 Sat -> 00:00:00 Sun)
>>> ds.datetime((2018, 3, 24, 6, 23, 59, 58, 0))
>>> ds.datetime()
(2018, 3, 24, 6, 23, 59, 59, 0)
>>> ds.datetime()
(2018, 3, 25, 0, 0, 0, 0, 0)

# month increment (Wed 28th Feb 2018 -> Thu 1st Mar 2018)
>>> ds.datetime((2018, 2, 28, 3, 23, 59, 58, 0))
>>> ds.datetime()
(2018, 2, 28, 3, 23, 59, 59, 0)
>>> ds.datetime()
(2018, 3, 1, 4, 0, 0, 0, 0)

# month increment on leap year Feb has 29 days (Sun 28th Feb 2016 -> Mon 29th Feb 2016)
>>> ds.datetime((2016, 2, 28, 0, 23, 59, 58, 0))
>>> ds.datetime()
(2016, 2, 28, 0, 23, 59, 59, 0)
>>> ds.datetime()
(2016, 2, 29, 1, 0, 0, 0, 0)

# month increment on leap year (Mon 29th Feb 2016 -> Tue 1st Mar 2016)
>>> ds.datetime((2016, 2, 29, 1, 23, 59, 58, 0))
>>> ds.datetime()
(2016, 2, 29, 1, 23, 59, 59, 0)
>>> ds.datetime()
(2016, 3, 1, 2, 0, 0, 0, 0)

# year increment (31st Dec 2018 -> 1st Jan 2019)
>>> ds.datetime((2018, 12, 31, 1, 23, 59, 58, 0))
>>> ds.datetime()
(2018, 12, 31, 1, 23, 59, 59, 0)
>>> ds.datetime()
(2019, 1, 1, 2, 0, 0, 0, 0)

# square wave output on pin SQ - 1Hz
>>> ds.square_wave(1, 0)

# square wave output on pin SQ - 4.096kHz
>>> ds.square_wave(4, 0)

# square wave output on pin SQ - 8.192kHz
>>> ds.square_wave(8, 0)

# square wave output on pin SQ - 32.768kHz
>>> ds.square_wave(32, 0)

# logic level output on pin SQ - high
>>> ds.square_wave(0, 1)

# logic level output on pin SQ - low
>>> ds.square_wave(0, 0)