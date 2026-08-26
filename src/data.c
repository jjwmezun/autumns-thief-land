#include <brotli/decode.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"

static void get_bits_from_byte( unsigned char byte, uint8_t * bits );
static unsigned char * data_get_gfx_data( uint32_t pointer, size_t width, size_t height );
static uint16_t get_uint16_from_bytes( const unsigned char * data, size_t i );
static uint32_t get_uint32_from_bytes( const unsigned char * data, size_t i );
static data_t open_file( const char * filename );

static data_t data;

unsigned int data_load()
{
	data = open_file( "data.had" );

	if ( data.size < 4 )
	{
		fprintf( stderr, "Data file is too small.\n" );
		return 1;
	}

	uint32_t checksum = 0;
	uint32_t expected_checksum = get_uint32_from_bytes( data.data, data.size - 4 );
	for ( size_t i = 0; i < data.size - 4; ++i )
	{
		checksum += data.data[ i ];
	}
	if ( checksum != expected_checksum )
	{
		fprintf( stderr, "Data file checksum mismatch: expected %u, got %u.\n", expected_checksum, checksum );
		return 1;
	}

	return 0;
}

unsigned char * data_get_block_gfx_data()
{
	const size_t after_palette_pointers = 2
		+ ( data_get_main_palette_count() * 4 )
		+ ( data_get_overworld_palette_count() * 4 )
		+ 8;
	const uint32_t pointer = get_uint32_from_bytes( data.data, after_palette_pointers );
	return data_get_gfx_data( pointer, 512, 512 );
}

unsigned char * data_get_sprite_gfx_data()
{
	const size_t after_palette_pointers = 2
		+ ( data_get_main_palette_count() * 4 )
		+ ( data_get_overworld_palette_count() * 4 )
		+ 12;
	const uint32_t pointer = get_uint32_from_bytes( data.data, after_palette_pointers );
	return data_get_gfx_data( pointer, 512, 512 );
}

size_t data_get_main_palette_count()
{
	return data.data[ 0 ];
}

unsigned char * data_get_main_palette_data()
{
	const size_t palette_count = data_get_main_palette_count();
	const size_t after_palette_name_pointers = 2 + palette_count * 4 + data_get_overworld_palette_count() * 4;
	const size_t palette_data_size = palette_count * 8;
	const size_t palette_length = 14;
	uint16_t * palette_data = calloc( palette_data_size, sizeof( uint16_t ) );
	uint32_t pointer = get_uint32_from_bytes( data.data, after_palette_name_pointers );
	for ( size_t i = 0; i < palette_count; ++i )
	{
		for ( size_t j = 0; j < 7; ++j )
		{
			uint16_t color = get_uint16_from_bytes( data.data, pointer + palette_length * i + j * 2 );
			palette_data[ 8 * i + j + 1 ] = color;
		}
	}
	return ( unsigned char * )( palette_data );
}

size_t data_get_overworld_palette_count()
{
	return data.data[ 1 ];
}

static void get_bits_from_byte( unsigned char byte, uint8_t * bits )
{
	for ( int i = 0; i < 8; i++ ) {
		bits[ i ] = ( byte >> ( 7-i ) ) & 1;
	}
}

static unsigned char * data_get_gfx_data( uint32_t pointer, size_t width, size_t height )
{
	const uint32_t block_gfx_compressed_len = get_uint32_from_bytes( data.data, pointer );
	size_t block_gfx_decompressed_len = ( size_t )( ceil( width * height * ( 3.0 / 8.0 ) ) );

	BrotliDecoderState * state = BrotliDecoderCreateInstance( NULL, NULL, NULL );
	unsigned char * decoded = ( unsigned char * )( malloc( block_gfx_decompressed_len) );
	BrotliDecoderResult result = BrotliDecoderDecompress( block_gfx_compressed_len, &data.data[ pointer + 4 ], &block_gfx_decompressed_len, decoded );
	if ( result != BROTLI_DECODER_RESULT_SUCCESS ) {
		printf( "Brotli decompression failed\n" );
		free( decoded );
		return NULL;
	}
	BrotliDecoderDestroyInstance( state );

	uint8_t total_bits[ width * height * 3 ];
	memset( total_bits, 0, block_gfx_decompressed_len );
	size_t total_bits_used = 0;
	unsigned char * pixels = ( unsigned char * )( malloc( width * height ) );
	memset( pixels, 0, width * height );
	uint8_t bits[ 10 ];
	memset( bits, 0, 10 );
	size_t pixels_used = 0;
	size_t bits_used = 0;
	size_t bits_index = 0;

	for ( size_t i = 0; i < block_gfx_decompressed_len; i++ ) {
		get_bits_from_byte( decoded[ i ], &bits[ bits_used ] );
		bits_used += 8;
		while ( bits_used >= 3 ) {
			total_bits[ total_bits_used++ ] = bits[ bits_index ];
			total_bits[ total_bits_used++ ] = bits[ bits_index+1 ];
			total_bits[ total_bits_used++ ] = bits[ bits_index+2 ];
			pixels[ pixels_used++ ] = ( bits[ bits_index++ ] << 2 ) | ( bits[ bits_index++ ] << 1 ) | bits[ bits_index++ ];
			bits_used -= 3;
		}
		for ( size_t j = 0; j < bits_used; j++ ) {
			bits[ j ] = bits[ bits_index++ ];
		}
		bits_index = 0;
	}

	if ( bits_used > 0 ) {
		printf( "Warning: %zu unused bits remaining after decoding\n", bits_used );
	}

	printf( "Decoded %zu pixels / %zu\n", pixels_used, width * height );

	free( decoded );

	return pixels;
}

static uint16_t get_uint16_from_bytes( const unsigned char * data, size_t i )
{
	return data[ i + 1 ] | ( data[ i ] << 8 );
}

static uint32_t get_uint32_from_bytes( const unsigned char * data, size_t i )
{
	return data[ i + 3 ] | ( data[ i + 2 ] << 8 ) | ( data[ i + 1 ] << 16 ) | ( data[ i ] << 24 );
}

static data_t open_file( const char * filename )
{
	data_t had = { 0, 0 };
	FILE * file = fopen( filename, "rb" );
	if ( ! file ) {
		perror( "Failed to open file" );
		return had;
	}
	fseek( file, 0, SEEK_END );
	long size = ftell( file );
	if ( size < 0 ) {
		perror( "Failed to determine file size" );
		fclose( file );
		return had;
	}
	char * buffer = malloc( size );
	if ( ! buffer ) {
		perror( "Failed to allocate memory" );
		fclose( file );
		return had;
	}
	fseek( file, 0, SEEK_SET );
	size_t bytesread = fread( buffer, 1, size, file );
	if ( bytesread < size ) {
		perror( "Failed to read file" );
		free( buffer );
		fclose( file );
		return had;
	}
	fclose( file );
	had.data = buffer;
	had.size = size;
	return had;
}
