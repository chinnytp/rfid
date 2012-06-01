#!/usr/bin/python

from twisted.protocols.basic import LineReceiver

from twisted.internet import reactor
from twisted.internet.serialport import SerialPort

from memberDb import *
import serial

class Echo(LineReceiver):
	def processData(self, data):
		if data[0]=='CARD':
			self.handleCard(data[1:])
		if data[0]=='STRIKER':
			print data[1]

	def connectionMade(self):
		print 'Serial connection established.'

	def lineReceived(self, line):
		print line
		data = line.split()
		self.processData(data)

	def unlock(self):
		 # Send unlock sequence
                 self.transport.write("UNLOCK\r\n")

	def handleCard(self, data):
		if data[0]=='CODE':	
			print data[1]
			# Check tag number
			db = MemberDb()
			if db.checkCard(data[1]):
				self.unlock()
			else:
				print 'Card Denied'
			if data[0]=='REMOVED':
				print 'REMOVED'





if __name__=='__main__':
	
	try:
		SerialPort(Echo(), '/dev/ttyUSB0', reactor, 9600)
		reactor.run()

	except serial.serialutil.SerialException:
		print "Error: Serial port unavailable"
