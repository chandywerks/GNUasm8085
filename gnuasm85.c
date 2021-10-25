/*
gnuasm85

Chris Handwerker (2011) <chris.handwerker@gmail.com>
http://homebrewtechnology.org

Parser to convert GNUSim8085 assembler listings to binary OP codes for the Intel 8085 CPU
*/
#include <stdio.h>

int shex(char *buffer)
{
	int value = 0;		// 4 bit value being worked on
	int result = 0;	// Final result
	int x = 0;			// Buffer counter
	int y = 0;			// General purpose counter

	while(buffer[x] != '\0')
	{
		switch (buffer[x])
		{
			case '0':
				value = 0;
			break;
			case '1':
				value = 1;
			break;
			case '2':
				value = 2;
	  		break;
			case '3':
				value = 3;
			break;
			case '4':
				value = 4;
			break;
			case '5':
				value = 5;
			break;
			case '6':
				value = 6;
			break;
			case '7':
				value = 7;
			break;
			case '8':
				value = 8;
			break;
			case '9':
				value = 9;
			break;
			case 'a' : case 'A':
				value = 10;
			break;
			case 'b' : case 'B':
				value = 11;
			break;
			case 'c' : case 'C':
				value = 12;
			break;
			case 'd' : case 'D':
				value = 13;
			break;
			case 'e' : case 'E':
				value = 14;
			break;
			case 'f' : case 'F':
				value = 15;
			break;
			default:
			return -1;
	  		break;
		}
		if (y > 0)
		{
			result = result << 4;
			result = result ^ value;
		}
		else
		{
			result = value ^ result;
			y++;
		}
		x++;
	}
	return result;
}

int main(int argc, char *argv[])
{

	if (argc != 2 && argc != 4)
	{
		printf("Usage: %s foo.asm -o foo.bin\n", argv[0]);
		printf
		(
			"\nWhere foo.asm is an assembler listing from GNUSim8085.\n"
			"To get an assembler listing of your program,\nopen up GNUSim8085 and hit Ctrl + L and save it to a file.\n\n"
			"Also make sure you set the \"Load me at\" value to the proper address offset.\n"
		);
		return 0;
	}

	FILE *file = fopen(argv[1], "rb");	//gets file pointer of file given in argument

	if (!file)
	{
		printf("Could not open file\n");
		return 0;
	}


	// Load file into allocated array
	//--------------------------------------->

	int file_size;					//	sizeof file array
	int x;							//	current array position
	int y = 0;						//	hex buffer counter
	int z = 0;						//	general purpose status counter
	int toMuchSpace = 0;            //  too many spaces preventer

	fseek(file, 0, SEEK_END); 		// seek to end of file
	file_size = ftell(file); 		// get current file pointer
	fseek(file, 0, SEEK_SET); 		// seek back to beginning of file
	int input[file_size];			// source file array

	// Read source file into source array
	for (x = 0; x <= file_size; x++)
		input[x] = fgetc(file);

	fclose(file);

	char buffer[15];				// array holds current hex value being worked on
	int output[file_size];		// array holds stripped out hex data
	int offset = 0;				// holds array position for output data

	//Strips out hex data

	for(x=0; x < file_size; x++)	// Keeps going until end of file
	{
	    toMuchSpace = 0;
	    		//Skip white space and go to first real character on the line
		while( (input[x] == ' ') || (input[x] == '\t') )
            x++;

		z = 0;	//Reset hex status counter (used skip first hex value A.K.A. the address)
		while(input[x] != '\n')			//Keep looping until new line character reached
		{
		    if( (input[x] == ' ') || (input[x] == '\t') ){
                toMuchSpace++;
		    }
			if( (input[x] == ';') || (toMuchSpace >= 3) )//If comment character, or if there are too many spaces, seek to end of line.
			{
				while(input[x] != '\n')
					x++;

				break;						//Finished with this line, break out of main loop to process next line of code
			}

			//Start reading in hex data here
			while( (input[x] != ' ') && (input[x] != '\t'))  //Read in hex numbers until space char reached
			{
			    toMuchSpace = 0;    //reset toMuchSpace any time we get any other char.
				if( (input[x] >= '0' && input[x] <= '9') || (input[x] >= 'a' && input[x] <= 'f') || (input[x] >= 'A' && input[x] <= 'F') )
				{
					buffer[y] = input[x];	//Store in temporary buffer
					y++;
				}
				else
				{
					y = -1; 			 //Set if non-hex char found
					break;
				}
				x++;
			}

			buffer[y] = '\0';		//Null terminate hex string buffer

			if ((y >= 1) && (z > 0))			//8 bit hex char found passed the address offset value
			{
				output[offset] = shex(buffer);
				offset++;
			}
			else if (y == -1)		//Ran into a non-hex character
			{
				while(input[x] != '\n')		//Done with line, seek to end
					x++;
			}

			z++;			// First hex (address) found
			y=0; 			// Reset hex buffer counter

			if(input[x] != '\n')		// In case new line char reached inside hex reading while loop
				x++;
		} //end hex reading while loop
	}

	//Output parsed hex data to file
	if(offset > 1)
	{
		if (argc == 2)
			file = fopen("output.bin", "wb");
		else
			file = fopen(argv[3], "wb");

		for(x=0; x<offset; x++)
			fputc(output[x], file);

		printf("Successfully parsed %d bytes of data. Cheers!\n", offset-1);
	}
	else
	{
		printf
		(
			"No hex data was found!\nMake sure the input file is an assembler listing.\n\n"
			"To get an assembler listing of your program,\nopen up GNUSim8085 and hit Ctrl + L and save it to a file.\n\n"
			"Also make sure you set the \"Load me at\" value to the proper address offset.\n"
		);
	}
	return 0;
}

