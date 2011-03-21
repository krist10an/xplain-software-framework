#!
# \file
#
# \brief Library functions for Tiny Simple File System image packer
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
import os
import sys
import array
import struct

ZERO_BYTE                       = 0x00
BLOCK_SIZE                      = 512

TSFS_IDENTITY                   = 0x17C1
TSFS_HEADER_SIZE                = 16
TSFS_FILE_TABLE_ENTRY_SIZE      = 16

#! \brief TSFS header structure
#
# The TSFS header structure, for now predefine the identity to TSFS_IDENTITY
# and version to 1, since no other variants currently exists.
class header_info:
	identity        = TSFS_IDENTITY
	version         = 1
	volume_size     = 0
	number_of_files = 0

class file_info(object):
	def __init__(self, name_on_disk=None, name_on_volume=None, size=0):
		self.name_on_disk       = name_on_disk
		self.name_on_volume     = name_on_volume
		self.size               = size

class volume_creator(object):
	files   = []

	def create_header(self, volume_size, number_of_files):
		header                  = header_info()
		header.volume_size      = volume_size
		header.number_of_files  = number_of_files

		# TSFS header layout
		#
		# 2-byte identity
		# 1-byte version
		# 1-byte reserved for future use
		# 4-byte volume size
		# 4-byte number of files
		# 4-byte reserved for future use
		return struct.pack('>HBBIII', header.identity, header.version,
				ZERO_BYTE, header.volume_size,
				header.number_of_files, ZERO_BYTE)

	def add_files(self, filelist):
		self.files.extend(filelist)

	def prepare_file_list(self, list_of_file_names):
		all_files = []
		for file_name in list_of_file_names:
			# Strip directory path from file name
			short_name = os.path.basename(file_name)

			if os.path.isfile(file_name):
				file_size = os.path.getsize(file_name)
				all_files.append(file_info(file_name,
							short_name, file_size))
			else:
				print "File not found:", file_name
		return all_files

	def write_to_file(self, target_file):
		written_bytes   = 0
		file_handle     = open(target_file, 'wb')

		print "Generating Tiny Simple File System image..."

		written_bytes = self.write_to_volume(self.files, file_handle)
		size_of_file  = file_handle.tell()

		file_handle.close

		if written_bytes == size_of_file:
			print "Completed: image file '%s' done, wrote" %    \
					target_file, size_of_file, "bytes."
		else:
			print "Warning: mismatching volume size! Header "   \
					"indicates size", written_bytes,    \
					", file is", size_of_file

	def write_to_volume(self, list_of_file_names, file_handle):
		all_files = self.prepare_file_list(list_of_file_names)

		size_of_header           = TSFS_HEADER_SIZE
		size_of_file_table_entry = TSFS_FILE_TABLE_ENTRY_SIZE
		size_of_file_table       = (size_of_file_table_entry *
				len(all_files))
		volume_size              = size_of_header + size_of_file_table

		# Count bytes missing to move to next full block
		if (volume_size % BLOCK_SIZE) != 0:
			volume_size += BLOCK_SIZE - (volume_size % BLOCK_SIZE)

		for file in all_files:
			needed_blocks = int(file.size / BLOCK_SIZE) + 1

			if (file.size % BLOCK_SIZE) == 0:
				needed_blocks -= 1
			volume_size += needed_blocks * BLOCK_SIZE

		header = self.create_header(volume_size, len(all_files))

		file_handle.write(header)

		# Find offset of first file data
		offset_in_file = size_of_header + size_of_file_table

		# Jump forward to next block
		if (offset_in_file % BLOCK_SIZE) != 0:
			offset_in_file += BLOCK_SIZE - (offset_in_file %
					BLOCK_SIZE)

		# Write file table entries
		#
		# TSFS file table entry layout
		#
		# 4-byte address to offset in file system to file data
		# 4-byte file size
		# 8-byte file name
		for file in all_files:
			name_with_padding = file.name_on_volume.ljust(8, chr(0))

			# Write 4 byte start adress
			file_handle.write(struct.pack(">I", offset_in_file))

			# Write 4 byte file size
			file_handle.write(struct.pack(">I", file.size))

			# Write 8 byte filename
			file_handle.write(name_with_padding)

			needed_blocks = int(file.size / BLOCK_SIZE)
			if (file.size % BLOCK_SIZE) != 0:
				needed_blocks += 1

			offset_in_file += needed_blocks * BLOCK_SIZE

		# Pad with zeros for the remainder of the block
		file_length = file_handle.tell()
		missing_bytes = BLOCK_SIZE - (file_length % BLOCK_SIZE)
		padding = "".ljust(missing_bytes, chr(0))
		file_handle.write(padding)

		# Write raw file data
		for file in all_files:
			handle      = open(file.name_on_disk, 'rb')
			raw_data    = handle.read()

			# Pad raw_data to make it fit in flash blocks
			if len(raw_data) % BLOCK_SIZE != 0:
				missing_bytes = BLOCK_SIZE - (len(raw_data) %
						BLOCK_SIZE)

				raw_data = raw_data.ljust(len(raw_data) +
						missing_bytes, chr(0))

			file_handle.write(raw_data)
			handle.close

		return volume_size

if __name__ == "__main__":
	print "Do not execute me directly, use mkfs.tsfs.py wrapper."
