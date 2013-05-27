Parses GNUSsim8085 assembler listings and outputs a binary
for burning to a ROM.

http://gnusim8085.org/

Instructions: Open up your assembly program in GNUSim85, make sure it runs. Set the "Load me at" value to the proper address offset. Then hit Ctrl + L to generate an assembler listing of your program and save it. Then run that assembler listing through gnuasm85 like so, "gnuasm85 foo.asm -o foo.bin".

Blog: http://www.homebrewtechnology.org/2011/02/parser-for-gnusim8085-assembler.html
