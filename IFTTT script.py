import serial
import requests
ser = serial.Serial('/dev/ttyACM0', 9600)
while True:
	line = ser.readline()
	if "Alarm" in line:
		requests.post('https://maker.ifttt.com/trigger/counter_change/with/key/GaQ4lphSAx27GdOr18ycj')
		print('sending')
		line = ""
