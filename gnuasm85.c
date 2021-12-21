/*
gnuasm85

Chris Handwerker (2011) <chris.handwerker@gmail.com>
http://homebrewtechnology.org

Parser to convert GNUSim8085 assembler listings to binary OP codes for the Intel 8085 CPU
*/
#include <stdio.h>

int shex(char *buffer){
	int value = 0;		// 4 bit value being worked on
	int result = 0;	// Final result
	int x = 0;			// Buffer counter
	int y = 0;			// General purpose counter

	while(buffer[x] != '\0'){
		switch (buffer[x]){
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
		if (y > 0){
			result = result << 4;
			result = result ^ value;
		}
		else{
			result = value ^ result;
			y++;
		}
		x++;
	}
	return result;
}

int main(int argc, char *argv[]){
	if (argc != 2 && argc != 4){
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
	if (!file){
		printf("Could not open file\n");
		return 0;
	}
	// Load file into allocated array
	//--------------------------------------->
	int file_size;					//	sizeof file array
	int fileIndex;					//	current array position
	int hexBufIndex = 0;			//	hex buffer counter
	int addressSkip = 0;			//	hex address skip counter
	int toMuchSpace = 0;            //  too many spaces preventer
	int rawDataOverride = 0;        //  overrides the address skip counter if '0x' is detected.

	fseek(file, 0, SEEK_END); 		// seek to end of file
	file_size = ftell(file); 		// get current file pointer
	fseek(file, 0, SEEK_SET); 		// seek back to beginning of file
	int input[file_size];			// source file array

	// Read source file into source array
	for (fileIndex = 0; fileIndex <= file_size; fileIndex++)
		input[fileIndex] = fgetc(file);

	fclose(file);

	char buffer[15];				// array holds current hex value being worked on
	int output[file_size];		// array holds stripped out hex data
	int dataIndex = 0;				// holds array position for output data

	//Strips out hex data
    // Keeps going until end of file
	for(fileIndex=0; fileIndex < file_size; fileIndex++){
	    toMuchSpace = 0;
        //Skip white space and go to first real character on the line
		while((input[fileIndex] == ' ') || (input[fileIndex] == '\t'))
            fileIndex++;
		addressSkip = 0;	//Reset hex status counter (used skip first hex value A.K.A. the address)
		//Keep looping until new line character reached
		while(input[fileIndex] != '\n'){
		    if( (input[fileIndex] == ' ') || (input[fileIndex] == '\t') ){
                toMuchSpace++;
		    }
		    //If comment character, or if there are too many spaces, seek to end of line.
			if( (input[fileIndex] == ';') || (input[fileIndex] == '#') || (toMuchSpace >= 3) ){
				while(input[fileIndex] != '\n')
					fileIndex++;
				break;  //Finished with this line, break out of main loop to process next line of code
			}
			// Skips '0x' prefix for hex data if it's there.
			rawDataOverride = 0;
            if (input[fileIndex] == '0'){
                fileIndex++;
                if (input[fileIndex] == 'x'){
                    fileIndex++;
                    rawDataOverride = 1;
                }
                else
                    fileIndex--;
            }
			//Start reading in hex data here
			//Read in hex numbers until space char reached
			while( (input[fileIndex] != ' ') && (input[fileIndex] != '\t')){
			    toMuchSpace = 0;    //reset toMuchSpace any time we get any other char.
                //Read actual HEX data and store it in a buffer.
				if( (input[fileIndex] >= '0' && input[fileIndex] <= '9') || (input[fileIndex] >= 'a' && input[fileIndex] <= 'f') || (input[fileIndex] >= 'A' && input[fileIndex] <= 'F') ){
					buffer[hexBufIndex] = input[fileIndex];	//Store in temporary buffer.
					hexBufIndex++;
				}
				else{
					hexBufIndex = -1; 			 //Set if non-hex char found
					break;
				}
				fileIndex++;
			}
			//Null terminate hex string buffer unless we came across a non-hex char. (Don't write to -1)
            if(hexBufIndex > -1)
                buffer[hexBufIndex] = '\0';
            //Convert 8 bit hex char found. Skip address data unless override.
			if ((hexBufIndex >= 1) && ((addressSkip > 0) || (rawDataOverride == 1))){
				output[dataIndex] = shex(buffer);
				dataIndex++;
			}
			//Ran into a non-hex character, skip to end of line.
			else if (hexBufIndex == -1){
				while(input[fileIndex] != '\n')		//Done with line, seek to end
					fileIndex++;
			}
			addressSkip++;			// First hex data (address) found
			hexBufIndex = 0; 			// Reset hex buffer index
            // In case new line char reached inside hex reading while loop
			if(input[fileIndex] != '\n')
				fileIndex++;
		} //end hex reading while loop
	}

	//Output parsed hex data to file
	if(dataIndex > 1){
		if (argc == 2)
			file = fopen("output.bin", "wb");
		else
			file = fopen(argv[3], "wb");
		for(fileIndex=0; fileIndex<dataIndex; fileIndex++)
			fputc(output[fileIndex], file);
		printf("Successfully parsed %d bytes of data. Cheers!\n", dataIndex-1);
	}
	else{
		printf
		(
			"No hex data was found!\nMake sure the input file is an assembler listing.\n\n"
			"To get an assembler listing of your program,\nopen up GNUSim8085 and hit Ctrl + L and save it to a file.\n\n"
			"Also make sure you set the \"Load me at\" value to the proper address offset.\n"
		);
	}
	return 0;
}

