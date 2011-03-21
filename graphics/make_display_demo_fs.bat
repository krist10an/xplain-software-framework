@echo off

set P=xplain-formatted\
set files=
set files=%files% %P%i_pics
set files=%files% %P%i_clock
set files=%files% %P%i_games
set files=%files% %P%i_calc
set files=%files% %P%i_sett
set files=%files% %P%i_files
set files=%files% %P%i_fonts
set files=%files% %P%i_graph
set files=%files% %P%i_avr
set files=%files% %P%i_tank
set files=%files% %P%i_uc3
set files=%files% %P%i_xmega
set files=%files% %P%s_xmega
set files=%files% %P%s_avrman
set files=%files% %P%s_chip
set files=%files% %P%s_jtagic
set files=%files% %P%s_people
set files=%files% %P%avr1
set files=%files% %P%avr2
set files=%files% %P%avr3
set files=%files% %P%avr4
set files=%files% %P%dispxpl1
set files=%files% %P%dispxpl2
set files=%files% %P%uc31
set files=%files% %P%uc32
set files=%files% %P%uc33
set files=%files% %P%uc34
set files=%files% %P%uc35
set files=%files% %P%xmega1
set files=%files% %P%xmega2
set files=%files% %P%xmega3
set files=%files% %P%xmega4
set files=%files% %P%xmega5
set files=%files% %P%xmega6
set files=%files% %P%cardback

set files=%files% %P%p_clk0
set files=%files% %P%p_clk1
set files=%files% %P%p_clk2
set files=%files% %P%p_clk3
set files=%files% %P%p_clk4
set files=%files% %P%p_clk5
set files=%files% %P%p_clk6
set files=%files% %P%p_clk7
set files=%files% %P%p_clk8
set files=%files% %P%p_clk9
set files=%files% %P%p_clkbg

set files=%files% %P%p_lgtgrn
set files=%files% %P%p_lgtred
set files=%files% %P%p_tankbg

set files=%files% %P%f_allchr
set files=%files% %P%f_ericat
set files=%files% %P%f_fixrus
set files=%files% %P%f_larab
set files=%files% %P%f_monfur

rem python ..\tools\tsfs\mkfs.tsfs\mkfs.tsfs.py -o display_demo.raw %files%
..\tools\tsfs\mkfs.tsfs\dist\mkfs.tsfs.exe -o display_demo.raw %files%

pause
