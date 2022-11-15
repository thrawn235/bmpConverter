#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
using namespace std;

struct Header
{
	char signature[2];
	uint32_t size;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t offset;
}__attribute__((__packed__));

struct DIB
{
	uint32_t size;
	uint32_t width;
	uint32_t height;
	uint16_t colorPlanes;
	uint16_t bitsPerPixel;
	uint32_t compression;
	uint32_t imageSize;
	uint32_t hResolution;
	uint32_t vResolution;
	uint32_t numColors;
	uint32_t numImportantColors;
}__attribute__((__packed__));

struct BMP
{
	Header header;
	DIB dib;
	char* pixMap;
}__attribute__((__packed__));


int main(int argc, char *argv[])
{
	char* inputFilePath = NULL;
	char* outputFilePath =  NULL;
	bool x16 = false;

	for( int i = 0; i < argc; i++ )
	{
		if( strcmp(argv[i], "--help") == 0 || strcmp( argv[i], "-h" ) == 0 )
		{
			printf("--input | -i  - input file path \n");
			printf("--output | -o - output file path \n");
			return 0;
		}
		if( strcmp(argv[i], "--input") == 0 || strcmp( argv[i], "-i" ) == 0 )
		{
			inputFilePath = argv[i + 1];
		}
		if( strcmp(argv[i], "--output") == 0 || strcmp( argv[i], "-o" ) == 0 )
		{
			outputFilePath = argv[i + 1];
		}
		if( strcmp(argv[i], "--x16") == 0 || strcmp( argv[i], "-x" ) == 0 )
		{
			x16 = true;
		}
	}

	if( inputFilePath == NULL )
	{
		printf( "please specify a input File with --input. Abort\n" );
		return 0;
	}
	else
	{
		int len = strlen( inputFilePath );
		if( inputFilePath[len - 3] != 'b' || inputFilePath[len - 2] != 'm' || inputFilePath[len - 1] != 'p' )
		{
			printf( "please specify a .bmp file (with .bmp file extiension)\n" );
			return 0;
		}
	}
	if( outputFilePath == NULL )
	{	
		//remove .bmp
		outputFilePath = (char*)malloc(255);
		strncpy( outputFilePath, inputFilePath, strlen(inputFilePath) - 4 );
		outputFilePath[strlen(inputFilePath) - 3] = (char)0;
		//add suffix
		strcat( outputFilePath, "-palette.bin" );
		printf( "outputFilePath not specified. creating filename... %s\n", outputFilePath );
	}
	if( x16 )
	{
		printf( "x16 specify, will covert palette to x16 format)\n" );
	}

	printf( "opening file...\n" );
	FILE* file = fopen(inputFilePath, "rb" );
	if( file == NULL )
	{
		printf( "error reading file. Abort\n");
		return 0;
	}

	BMP bmp;

	fread( &bmp, sizeof(Header), 1, file );

	if( bmp.header.signature[0] != 'B' || bmp.header.signature[1] != 'M' )
	{
		printf( "file signature not BM Abort\n" );
		return 0;
	}

	fread( &bmp.dib, sizeof(DIB), 1, file);
	printf( "dib size (bytes) = %u\n", bmp.dib.size );
	printf( "pixelmap offset = %X\n", bmp.header.offset );
	printf( "width = %u, height = %u\n", bmp.dib.width, bmp.dib.height );
	printf( "bitsPerPixel = %u\n", bmp.dib.bitsPerPixel );
	printf( "imageSize = %u\n", bmp.dib.imageSize );
	printf( "numColors = %u\n", bmp.dib.numColors );

	fseek( file, bmp.dib.size - sizeof(DIB), SEEK_CUR );

	char* palette = (char*)malloc(bmp.dib.numColors * 4);
	fread( palette, bmp.dib.numColors * 4, 1, file );

	/*for(unsigned int i = 0; i < bmp.dib.numColors; i++)
	{
		printf( "color %3u: %3hhu,%3hhu,%3hhu,%3hhu\n", i, palette[i*4],palette[i*4+1],palette[i*4+2],palette[i*4+3] );
	}*/
	

	printf( "closing file...\n" );
	fclose( file );
	//Print(bmp.pixMap, bmp.dib.width, bmp.dib.height );

	printf( "opening file...\n" );
	file = fopen(outputFilePath, "wb" );

	for(unsigned int i = 0; i < bmp.dib.numColors; i++)
	{
		printf( "color %3u: %3hhu,%3hhu,%3hhu,%3hhu\n", i, (palette[i*4]&0b11110000)>>4,(palette[i*4+1]&0b11110000)>>4,(palette[i*4+2]&0b11110000)>>4,palette[i*4+3] );
		uint8_t r, g, b, gb;
		r = (palette[i*4]&0b11110000)>>4;
		g = (palette[i*4+1]&0b11110000);
		b = (palette[i*4+2]&0b11110000)>>4;
		gb = g + b;
		fwrite( &gb, 1, 1, file );
		fwrite( &r, 1, 1, file );
	}


	printf( "closing file...\n" );
	fclose( file );

	printf( "done\n" );
	return 0;
}
