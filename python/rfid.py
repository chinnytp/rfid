#!/usr/bin/python

from twisted.protocols.basic import LineReceiver

from twisted.internet import reactor
from twisted.internet.serialport import SerialPort

from memberDb import *
import serial

class RfidReader(LineReceiver):
	def processData(self, data):
		subject, data = data[0], data[1:]
		if subject=='CARD':
			self.handle_card(data)
		elif subject=='STRIKER':
			self.handle_striker(data)

	def connectionMade(self):
		print 'Serial connection established.'

	def lineReceived(self, line):
		data = line.split()
		self.processData(data)

	def unlock(self):
		 # Send unlock sequence
                 self.transport.write("UNLOCK\r\n")

	def handle_card(self, data):
		subject, data = data[0], data[1:]
		if subject=='CODE':	
			code = data[0]
			print code
			# Check tag number
			db = MemberDb()
			if db.checkCard(code):
				print 'Access Granted - ', code
				self.unlock()
			else:
				print 'Access Denied - ', code
		elif subject=='REMOVED':
			print 'REMOVED'
	
	def handle_striker(self, data):
		print data[0]


if __name__=='__main__':
	
	try:
		SerialPort(RfidReader(), '/dev/ttyUSB0', reactor, 9600)
		reactor.run()

	except serial.serialutil.SerialException:
		print "Error: Serial port unavailable"
