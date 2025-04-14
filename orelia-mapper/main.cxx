#include <impl/includes.h>

int __cdecl main( int argc, char** argv ) {
	if ( argc < 2 ) {
		logging::print( encrypt( "Error: Missing parameters" ) );
		logging::print( encrypt( "Press any key to exit..." ) );
		return std::getchar( );
	}

	SetConsoleTitleA( encrypt( "orelia-mapper" ) );
	SetUnhandledExceptionFilter( exception::exception_filter );

	std::vector<std::uint8_t> driver_image;
	utility::open_file( argv[ 1 ], driver_image );
	if ( driver_image.empty( ) ) {
		logging::print( encrypt( "Error: Failed to get driver image" ) );
		logging::print( encrypt( "Press any key to exit..." ) );
		return std::getchar( );
	}

	if ( !mapper::execute( driver_image ) ) {
		logging::print( encrypt( "Failed to execute payload.\n" ) );
		return std::getchar( );
	}

	return std::getchar( );
}