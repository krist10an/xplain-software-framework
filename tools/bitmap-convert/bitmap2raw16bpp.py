import os, sys
from PIL import Image

def conv(pixel):
	red   = pixel[0] >> 3;
	green = pixel[1] >> 2;
	blue  = pixel[2] >> 3;

	color = red << 11 | green << 5 | blue
	return (color >> 8, color & 0xff)

program = os.path.basename(sys.argv[0])

if len(sys.argv) <= 1:
	print "usage: %s [image file]" % program
	sys.exit(0)

input_file = sys.argv[1]

try:
	fsocks = open(input_file)
except IOError:
	print "The file '%s' does not exist, exiting." % input_file
	sys.exit(2)

fsocks.close()
print "Converting '%s' to 16bpp raw image data" % input_file

image_file = Image.open(input_file)
image_data = image_file.getdata()

# remove file extension
outname = input_file.split('.')[0]
output_file = open(outname, "wb")

for pixel in image_data:
	color = conv(pixel);
	output_file.write(chr(color[0]))
	output_file.write(chr(color[1]))

output_file.close()

print "Done (-: Output file is '%s'." % (outname)
