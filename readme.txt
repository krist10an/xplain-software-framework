Xplain Software Framework version 1.0

Directory contents:

 binaries/       -  Precompiled Intel HEX files of the Display Xplained demo
		    and training (with solution applied), the Xplain board
		    controller and the UART loopback applications.

 documentation/  -  source code documentation in HTML-format for all
                    applications.

 graphics/       -  bitmap images for the Display Xplained demo, in both raw
		    and original formats, and a pre-generated TSFS image.
		    Also contains barch files for creating and writing this
		    TSFS image to the Xplain board's Dataflash.

 source/         -  the source code for all four projects, in one framework.

 tools/          -  command line tools (as Python source code and precompiled
		    executables for Windows) for creating and writing TSFS
		    images to the Xplain board, and for converting image
		    files to the raw format suitable for the Display Xplained
		    LCD.

Known Issues:

1. The clock application has not been implemented.

2. The accuracy of the touch driver can be improved.

3. The length of slider widgets is limited to 255 pixels.

4. The slider widget will redraw the slider every time the slider value is
   changed. For sliders with high resolution (large maximum value relative to
   the length of the slider), inaccuracy in measurements of the touch
   position can be enough to trigger redrawing. The excessive redrawing does
   not look nice.

5. The xplain-bc application fails to compile on Microsoft Windows machines,
   since it fails to create the build directory. The user will have to
   manually create the build directoy, and can then return to using make as
   usual.

   When the build system fails to create the proper build directory, type in
   the following:
      mkdir ..\..\build\xplain-bc\xplain-bc\GNU

   The user can now call make again and the compilation will complete.
