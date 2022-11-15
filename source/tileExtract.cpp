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

struct Tile
{
	unsigned int width, height;
	char* data;
};

Tile Crop( char* source, unsigned int sourceWidth, unsigned int sourceHeight, unsigned int posX, unsigned int posY, unsigned int tileWidth, unsigned int tileHeight )
{
	Tile tile;
	tile.width = tileWidth;
	tile.height = tileHeight;
	tile.data = (char*)malloc( tileWidth * tileHeight );

	unsigned int startOffset = sourceWidth * posY + posX;
	//printf(" start = %i value = %hhu\n", startOffset, source[startOffset] );

	for(unsigned int y = 0; y < tileHeight; y++ )
	{
		for( unsigned int x = 0; x < tileWidth; x++ )
		{
			tile.data[tile.width * y + x] = source[startOffset + (y * sourceWidth + x) ];
		}
	}

	return tile;
}

void Print( char* source, unsigned int width, unsigned int height )
{
	for( unsigned int y = 0; y < height; y++ )
	{
		for( unsigned int x = 0; x < width; x++ )
		{
			printf("%2hhX", source[width * y + x] );
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	unsigned int tileHeight = 0, tileWidth = 0;
	unsigned int numTilesX = 0, numTilesY = 0;
	char* inputFilePath = NULL;
	char* outputFilePath =  NULL;

	for( int i = 0; i < argc; i++ )
	{
		if( strcmp(argv[i], "--help") == 0 || strcmp( argv[i], "-h" ) == 0 )
		{
			printf("--size | -s   - set tile size \n");
			printf("--width | -w  - set tile height \n");
			printf("--height | -h - set til width \n");
			printf("--numX | -x   - set number of horizontal tiles \n");
			printf("--numY | -y   - set number of vertical tiles \n");
			printf("--input | -i  - input file path \n");
			printf("--output | -o - output file path \n");
			return 0;
		}
		if( strcmp(argv[i], "--size") == 0 || strcmp( argv[i], "-s" ) == 0 )
		{
			tileWidth = tileHeight = atoi(argv[i + 1]);
		}
		if( strcmp(argv[i], "--width") == 0 || strcmp( argv[i], "-w" ) == 0 )
		{
			tileWidth = atoi(argv[i + 1]);
		}
		if( strcmp(argv[i], "--height") == 0 || strcmp( argv[i], "-h" ) == 0 )
		{
			tileWidth = atoi(argv[i + 1]);
		}
		if( strcmp(argv[i], "--numX") == 0 || strcmp( argv[i], "-x" ) == 0 )
		{
			numTilesX = atoi(argv[i + 1]);
		}
		if( strcmp(argv[i], "--numY") == 0 || strcmp( argv[i], "-y" ) == 0 )
		{
			numTilesY = atoi(argv[i + 1]);
		}
		if( strcmp(argv[i], "--input") == 0 || strcmp( argv[i], "-i" ) == 0 )
		{
			inputFilePath = argv[i + 1];
		}
		if( strcmp(argv[i], "--output") == 0 || strcmp( argv[i], "-o" ) == 0 )
		{
			outputFilePath = argv[i + 1];
		}
	}
	
	if( tileWidth == 0 || tileHeight == 0 )
	{
		printf( "please set either --tileWidth AND --tileHeight or --size. Abort\n");
		return 0;
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
		strcat( outputFilePath, "-tiles.bin" );
		printf( "outputFilePath not specified. creating filename... %s\n", outputFilePath );
	}
	printf("tileWidth = %i, tileHeight = %i\n", tileWidth, tileHeight );

	printf( "opening file...\n" );
	FILE* file = fopen(inputFilePath, "rb" );
	if( file == NULL )
	{
		printf( "error reading file. Abort\n");
		return 0;
	}

	BMP bmp;

	fread( &bmp, sizeof(Header) + sizeof(DIB), 1, file );

	if( bmp.header.signature[0] != 'B' || bmp.header.signature[1] != 'M' )
	{
		printf( "file signature not BM Abort\n" );
		return 0;
	}
	printf( "dib size = %u\n", bmp.header.size );
	printf( "pixelmap offset = %X\n", bmp.header.offset );
	printf( "width = %u, height = %u\n", bmp.dib.width, bmp.dib.height );
	printf( "bitsPerPixel = %u\n", bmp.dib.bitsPerPixel );
	printf( "imageSize = %u\n", bmp.dib.imageSize );
	printf( "numColors = %u\n", bmp.dib.numColors );

	if( numTilesX == 0 )
	{
		numTilesX = bmp.dib.width / tileWidth;
	}
	if( numTilesY == 0 )
	{
		numTilesY = bmp.dib.height / tileHeight;
	}
	printf( "numTilesX = %u, numTilesY = %u\n", numTilesX, numTilesY );

	printf( "create buffer to load pixelMap (%u bytes)...\n", bmp.dib.width * bmp.dib.height );
	bmp.pixMap = (char*)malloc( bmp.dib.width * bmp.dib.height );
	fseek( file, bmp.header.offset, SEEK_SET );
	fread( bmp.pixMap, 1, bmp.dib.width * bmp.dib.height, file );
	printf( "closing file...\n" );
	fclose( file );
	//Print(bmp.pixMap, bmp.dib.width, bmp.dib.height );
	printf( "flip image...\n" );
	char* pixMapFlipped = (char*)malloc( bmp.dib.width * bmp.dib.height );
	for( unsigned int y = 0; y < bmp.dib.height; y++ )
	{
		for( unsigned int x = 0; x < bmp.dib.width; x++ )
		{
			pixMapFlipped[ (bmp.dib.height - y - 1) * bmp.dib.width + x ] = bmp.pixMap[ y * bmp.dib.width + x ];
		}
	}
	//Print(pixMapFlipped, bmp.dib.width, bmp.dib.height );

	printf( "opening file...\n" );
	file = fopen(outputFilePath, "wb" );

	for( unsigned int y = 0; y < numTilesY; y++ )
	{
		for( unsigned int x = 0; x < numTilesX; x++ )
		{
			Tile test = Crop( pixMapFlipped, bmp.dib.width, bmp.dib.height, x * tileWidth, y * tileHeight, tileWidth, tileHeight );
			//Print(test.data, tileWidth, tileHeight);
			fwrite( test.data, 1, tileHeight * tileWidth, file );
			//int a;
			//scanf("%u", &a);
		}
	}	

	printf( "closing file...\n" );
	fclose( file );

	printf( "done\n" );
	return 0;
}
