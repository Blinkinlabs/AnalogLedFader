import serial
import time
import optparse
import array

class Redirector:
	"""
	Takes frames of UDP data and bashes them out over the serial port. Used to avoid Java serial.
	"""

	frameCount = 0
	outputTimeTotal = 0

#	def __init__(self):
#		"""
#		Initialize a UDP packet listener
#		"""

	def connect(self, port):
		"""
		Connect to the specified serial port
		"""
		self.ser = serial.Serial(port, 115200, timeout=0)

	def sendFrame(self, data):
		"""
		Load the next frame into the strips, but don't actually clock it out.
		@param data Image data, as a 1D, 8bit RGB array.
		"""

		# Send the data out in 64-byte chunks
		outputStartTime = time.time()
		for x in range(0, len(data)/64):
			t = data[64 * x : (64 * x) + 64]
			self.ser.write(t)
		outputTime = time.time() - outputStartTime

		self.outputTimeTotal += outputTime
		self.frameCount += 1

		if self.frameCount > 30:
			average_time = self.outputTimeTotal/self.frameCount
			self.frameCount = 0
			self.outputTimeTotal = 0
			print average_time

if __name__ == "__main__":
	parser = optparse.OptionParser()
	parser.add_option("-p", "--serialport", dest="serial_port",
		help="serial port (ex: /dev/ttyUSB0)", default="/dev/tty.usbmodel20001")

	(options, args) = parser.parse_args()

	redirector = Redirector()
        redirector.connect(options.serial_port)

	channels = 13*6
	value = 0;
	while True:
		data = ''
                data += chr(0xde)
                data += chr(0xad)
                data += chr(channels*2)
		for channel in range (0, channels):
			data += chr(value      & 0xFF)
			data += chr((value>>8) & 0xFF)

	        redirector.sendFrame(data)
		value = value + 2000
