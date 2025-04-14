#include <impl/includes.h>

int __cdecl main( int argc, char** argv ) {
	auto dll_path = argv[ 1 ];
	if ( argc < 2 ) {
		logging::print( encrypt( "Error: Missing parameters" ) );
		logging::print( encrypt( "Press any key to exit..." ) );
		return std::getchar( );
	}

	SetConsoleTitleA( encrypt( "orelia-injector" ) );

	if ( !g_driver->setup( ) )
		return std::getchar( );

	if ( !g_driver->attach( target_process ) ) {
		logging::print( encrypt( "Failed to attach process.\n" ) );
		g_driver->unload( );
		return std::getchar( );
	}

	if ( !injector::inject_dll( dll_path ) ) {
		g_driver->unload( );
		return std::getchar( );
	}

	g_driver->unload( );
	return std::getchar( );
}