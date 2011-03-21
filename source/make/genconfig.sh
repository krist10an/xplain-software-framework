#! /bin/sh
#
# Copyright (C) 2009 Atmel Corporation. All rights reserved.

echo '/* Automatically generated; edit at your own risk */'
echo

# Generate a configuration header to stdout from the configuration info read
# from stdin using the following command pipeline:
#
# 1. Reverse the order of the lines. This is necessary because sort -u later
#    picks the first entry among identical ones, and we want to give precedence
#    to the last.
# 2. Do a stable sort based on everything before the first '=' character and
#    delete all entries with identical sort key except the first one.
# 3. Filter out anything that is disabled or not config-related
# 4. Turn what's left into cpp defines. Symbols set to 'y' are defined as 1,
#    everything else is defined to the literal value of the symbol.

tac | sort -s -t= -k1,1 -u | grep '^CONFIG_.*=[^n]'	\
	| sed -e 's/\(.*\)=y/#define \1 1/'		\
		-e 's/\(.*\)=\(.*\)$/#define \1 \2/'
