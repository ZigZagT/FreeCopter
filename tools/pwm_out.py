#!/usr/bin/env python
import RPi.GPIO as GPIO
import time, signal, sys, _thread

# ============== User Defines ==============
pinOut = 22			# GPIO pin number for PWM output
PWMBaseFreq = 400.0	# in Hz
PWMWidthMin = 1.0	# in ms
PWMWidthMax = 2.0	# in ms
# ============ End User Defines ============

# ============= State Variable =============
PWMLevel = 0.0 # Describe moter's speed in percentage.
LastInput = "";
# =========== End State Variable ===========

# ======= Environment Initialization =======
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(pinOut, GPIO.OUT)

def signal_handler(signal, frame):
	print ('\nPWM Terminated.');
	GPIO.cleanup()
	print ('Goodbye')
	sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)
# ===== End Environment Initialization =====


LastInput = input('Enter PWM frequency: (400)')
if LastInput:
	try:
		PWMBaseFreq = float(LastInput)
	except ValueError:
		print ('Invalid input.')
print ('PWM Base Frequency is set to ' + str(PWMBaseFreq) + 'Hz')

print ('Output pin number is BCM' + str(pinOut))

LastInput = input('Enter minimum pluse width in millisecond: (1.0) ')
if LastInput:
	try:
		PWMWidthMin = float(LastInput)
	except ValueError:
		print ('Invalid input.')
print ('Minimum pluse width is set to ' + str(PWMWidthMin) + 'ms')

LastInput = input('Enter Maximum pluse width in millisecond: (2.0) ')
if LastInput:
	try:
		PWMWidthMax = float(LastInput)
	except ValueError:
		print ('Invalid input.')
print ('Maximum pluse width is set to ' + str(PWMWidthMax) + 'ms')

LastInput = input('Enter starting PWM speed in percentage: (0) ')
if LastInput:
	try:
		PWMLevel = float(LastInput)
	except ValueError:
		print ('Invalid input.')
print ('PWM Level is set to ' + str(PWMLevel) + '%')

print ('Starting PWM output. Press Ctrl+C to exit.')
print ('=======================================================')

def PWMOut():
	CurrentLevel = 0.0
	activeTime = 0.0
	sleepTime = 0.0
	while 1:
		CurrentLevel = PWMLevel
		activeTime = (PWMWidthMin + (PWMWidthMax - PWMWidthMin) * (CurrentLevel / 100)) / 1000
		sleepTime = 1.0 / PWMBaseFreq - activeTime
		GPIO.output(pinOut, 1)
		time.sleep(activeTime)
		GPIO.output(pinOut, 0)
		time.sleep(sleepTime)

_thread.start_new_thread(PWMOut, ())

while 1:
	LastInput = input('Enter PWM speed in percentage: ')
	if LastInput and 1 :
		lastLevel = PWMLevel
		try:
			PWMLevel = float(LastInput)
			if PWMLevel < 0 or PWMLevel > 100:
				print('Value out of range, no change occurs.')
				PWMLevel = lastLevel
				continue
			print ('PWM Level is set to ' + str(PWMLevel) + '%')
		except ValueError:
			print ('ValueError, no change occurs.')
			PWMLevel = lastLevel;
	else :
		print ('Invalid value, no change occurs.')