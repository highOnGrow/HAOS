#!/usr/bin/python 
import datetime
import serial
import RPi.GPIO as GPIO
#import plotly
 
#tokens = ['token1', 'token2']
#username = 'plotly_username_here'
#api_key = 'plotly_api_key_here'
 
#p = plotly.plotly(username, api_key)
#streams = [plotly.stream(token) for token in tokens]
 
# Initialize plot with stream tokens
#print p.plot([{
#    'x': [],
#    'y': [],
#    'type': 'scatter',
#    'stream': {
#        'token': token,
#        'maxpoints': 40
#        }
#    } for token in tokens],
#    filename='Atlas Streaming Multiplexer',
#    fileopt='overwrite')
 
# Set multiplexer to read 00 by setting S0 and S1 to low
GPIO.cleanup()
GPIO.setmode(GPIO.BOARD)
S0_pin = 11
S1_pin = 23
GPIO.setup(S0_pin, GPIO.OUT) # S0 
GPIO.setup(S1_pin, GPIO.OUT) # S1
GPIO.output(S0_pin, False)
GPIO.output(S1_pin, False)
 
# Serial code adapted from: https://www.atlas-scientific.com/_files/code/pi_sample_code.pdf
print "You are now Multiplexing with Atlas and Plotly!"
usbport = '/dev/ttyAMA0'
ser = serial.Serial(usbport, 38400)
print "00: start writing"
ser.write("C\r")  # Tell sensor 00 to start writing
print "00: LED ON"
ser.write("L,1\r") # Turn on LED
 
# Toggle and tell sensor 01 to start writing
GPIO.output(S0_pin, True)
GPIO.output(S1_pin, False)
print "01: start writing"
ser.write("C\r") 
print "01: LED ON"
ser.write("L,1\r") # Turn on 01's LED
 
line = ""
while True:
    data = ser.read()
    if(data == "\r"):
        print "Received from sensor:" + line
        # Parse the data
        try:
            line = float(line)
        except:
            print "Couldn't parse float, resetting buffer: ", line
            line = ""
            continue
        time_now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
        # Write the data to one of the two plotly streams
        if GPIO.input(S1_pin):
            streams[1].write({'x': time_now, 'y': line})
        else:
            streams[0].write({'x': time_now, 'y': line})
        line = ""
        GPIO.output(S0_pin,  not GPIO.input(S0_pin)) # toggle S0 value to switch channels
        
    else:
        line = line + data