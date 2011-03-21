#!
# \file
#
# \brief Main function for Tiny Simple File System image packer
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
import sys
from optparse import OptionParser
from tsfscreator import volume_creator

def main():
	parser = OptionParser(usage="%prog [options] [files]",
			description="mkfs.tsfs.py will combine all the files "
			"listed in FILES into a Tiny Simple File System "
			"(TSFS)	image file with a TSFS structure. By default "
			"mkfs.tsfs.py will write to 'out.raw', unless a file "
			"is specified by the -o option.")
	parser.add_option("-o", "--output", dest="output",
			help="write TSFS image to FILE. Default is raw.out.",
			metavar="FILE", default="raw.out")

	(options, args) = parser.parse_args()

	list_of_file_names = args
	if len(list_of_file_names) == 0:
		parser.print_usage()
		sys.exit()

	vc = volume_creator()

	try:
		vc.add_files(list_of_file_names)
		vc.write_to_file(options.output)
	except:
		print "Unexpected error:", sys.exc_info()[0]
		parser.print_usage()
		sys.exit(2)

if __name__ == "__main__":
	main()
