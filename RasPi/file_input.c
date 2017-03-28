
#include "file_input.h"


int main(int argc, char **argv)
{
	initCanInterface();
	if(argc < 2)
	{
		fprintf(stderr, "got no arg\n");
		// print help
		fprintf(stderr, "Please use the help\n");
	}
	else if(argc == 2)
	{
		fprintf(stderr, "Got one args\n");

		char* fileArray;
		long len = readFile(argv[1], fileArray);

		int i;
		for(i = 0; i < len; ++i)
		{
			printf(fileArray[i]);
		}
	}
}

long readFile(char* filename, char* buffer)
{
	FILE *fileptr;
	long filelen;

	fileptr = fopen(filename, "rb");  	// Open the file in binary mode
	fseek(fileptr, 0, SEEK_END);		// Jump to the end of the file
	filelen = ftell(fileptr);			// Get the current byte offset in the file
	rewind(fileptr);					// Jump back to the beginning of the file

	buffer = (char *)malloc((filelen+1)*sizeof(char)); // Enough memory for file + \0
	fread(buffer, filelen, 1, fileptr); // Read in the entire file
	fclose(fileptr); // Close the file

	return filelen;
}
