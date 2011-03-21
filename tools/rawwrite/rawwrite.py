#!
# \file
#
# \brief Write image files directly to a block device
#
# Copyright (C) 2010 Atmel Corporation. All rights reserved.
#
# \page License
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# 3. The name of Atmel may not be used to endorse or promote products derived
# from this software without specific prior written permission.
#
# 4. This software may only be redistributed and used in connection with an
# Atmel AVR product.
#
# THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
# DAMAGE.
import string
import sys
import win32con
import win32file
import wmi
from optparse import OptionParser
from optparse import OptionGroup

def main():
	def write_file_to_bdev(bdev, data):
		sector_size = int(bdev.BytesPerSector)

		print "Writing '%s' to block device '%s'... " % (image_file,
				bdev.Caption),

		# Make sure data length is a multiple of sector size
		if len(data) % sector_size != 0:
			missing_bytes = sector_size - (len(data) % sector_size)
			data = data.ljust(len(data) + missing_bytes, chr(0))

		file_handle = win32file.CreateFile(bdev.DeviceID,
				win32con.GENERIC_WRITE, 0, None,
				win32file.OPEN_EXISTING,
				win32file.FILE_ATTRIBUTE_NORMAL, None)

		win32file.WriteFile(file_handle, data, None)
		win32file.CloseHandle(file_handle)

		print "done"

	def verify_data_on_bdev(bdev, data):
		print "Verifying data on block device '%s'... " % \
			(bdev.Caption),

		file_handle     = open(bdev.DeviceID, 'rb')
		bdev_data       = file_handle.read(len(data))

		file_handle.close()

		print "done"
		print ""

		if data == bdev_data:
			print "Write operation completed successfully."
		else:
			print "Warning: verification of data on block " \
				"device does not match input file."

	parser = OptionParser(usage="%prog [options] [file]",
			description="rawwrite.py will write the FILE file to "
			"a block device. When executed the application will "
			"scan the system for possible block devices and "
			"present them in a list. The user must then select "
			"the appropriate block device in the list. A final "
			"confirmation will be requested before continuing "
			"with the direct write to the block device.")
	group = OptionGroup(parser, "Dangerous Options",
			"Caution: this application writes directly to the "
			"block device, potentially destroying data already "
			"present on the device. If the user wrongly selects "
			"the block device used by the operating system "
			"installation the user will destroy vital data and "
			"possibly make the system unusable.")
	group.add_option("-e", "--expert", dest="expert", default=False,
			action="store_true", help="Will list ALL the block "
			"devices on the system and let the user freely "
			"select the appropriate target and proceed with the "
			"write.")
	parser.add_option_group(group)

	(options, args) = parser.parse_args()

	list_of_devices = []
	valid_indexes   = []

	if len(args) != 1:
		parser.print_usage()
		sys.exit()

	image_file = args[0]

	# Read data from the input file
	try:
		input_file = open(image_file, 'rb')
		input_data = input_file.read()
		input_file.close()
	except IOError as (errno, strerror):
		print "I/O error({0}): {1}".format(errno, strerror)
		parser.print_usage()
		sys.exit(2)
	except:
		print "Unexpected error:", sys.exc_info()[0]
		parser.print_usage()
		sys.exit(2)

	print "Scanning system for block device targets:\n"

	wmi_connection = wmi.WMI()

	# Get list of all drives
	if options.expert:
		list_of_devices = wmi_connection.Win32_DiskDrive()
	else:
		list_of_devices = wmi_connection.Win32_DiskDrive(
				InterfaceType="USB")

	# List disks
	for bdev in list_of_devices:
		try:
			sector_size     = int(bdev.BytesPerSector)
			total_sectors   = int(bdev.TotalSectors)
		except:
			print "Error: failed getting block device details."
			print "Unexpected error:", sys.exc_info()[0]
			sys.exit(2)

		print "%i)" % bdev.Index, bdev.Caption, ("(%i KB)" %
			(int(bdev.Size) / 1024))

		valid_indexes.append(bdev.Index)

	# Check if any valid block devices was found
	if len(valid_indexes) == 0:
		print "No block devices found, exiting."
		sys.exit(2)

	try:
		bdev_num = int(raw_input("Please select block device target " \
					"[number]: "))

		# Check if the index is among the available block devices
		if (bdev_num in valid_indexes) == False:
			sys.exit(2)
	except:
		print "\nError: invalid disk index."
		sys.exit(2)

	# Get handle to block device
	bdev = wmi_connection.Win32_DiskDrive(Index = bdev_num)[0]

	print "\nAre you sure you want to write '%s' to block device '%s'?" % \
		(image_file, bdev.Caption)

	answer = raw_input("[yes/no]: ")

	print ""

	if answer.lower() != "yes":
		print "Aborted, no data written to block device '%s'" % \
			bdev.Caption
		sys.exit(2)

	try:
		write_file_to_bdev(bdev, input_data)
	except:
		print "Error: failed writing to block device '%s'" %    \
			bdev.Caption
		print "Unexpected error:", sys.exc_info()[0]
		sys.exit(2)

	print ""

	try:
		verify_data_on_bdev(bdev, input_data)
	except:
		print "Warning: failed verifying block device '%s'" %   \
			bdev.Caption
		sys.exit(2)

if __name__ == "__main__":
	main()
