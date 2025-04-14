#pragma once

namespace shellcode {
	class c_shellcode {
	public:
		c_shellcode( std::uint64_t image_va, std::uint64_t func_rva ) :
			m_image_va( image_va ), m_dll_main( image_va + func_rva ) {
		}

		bool setup( ) {
			auto discord = std::make_unique<discord::c_discord>( );
			if ( !discord->setup( ) ) {
				logging::print( encrypt( "Please enable discord overlay and try again." ) );
				return false;
			}

			auto api_flag = discord->get_api_flag( );
			if ( api_flag == 1 ) {
				logging::print( encrypt( "Preparing execution for D3D11" ) );

				auto swap_chain = discord->get_swap_chain( );
				if ( !swap_chain ) {
					logging::print( encrypt( "Failed to find SwapChain." ) );
					return false;
				}

				auto vtable = g_driver->read( swap_chain );
				this->m_vtable = discord->get_vtable( vtable, 8 );
				this->m_original_fn = g_driver->read( m_vtable );
				if ( !m_original_fn ) {
					logging::print( encrypt( "Failed to get VTable." ) );
					return false;
				}

				return true;
			}
			else if ( api_flag == 2 ) {
				logging::print( encrypt( "Preparing execution for D3D12" ) );

				auto d3d12_context = discord->get_d3d12_context( );
				if ( !d3d12_context ) {
					logging::print( encrypt( "Failed to find D3D12 Context." ) );
					return false;
				}

				auto command_queue = discord->get_vtable( d3d12_context, 3 );
				auto command_vtable = g_driver->read( command_queue );
				if ( !command_vtable ) {
					logging::print( encrypt( "Failed to find CommandQueue." ) );
					return false;
				}

				auto vtable = g_driver->read( command_vtable );
				this->m_vtable = discord->get_vtable( vtable, 10 );
				this->m_original_fn = g_driver->read( m_vtable );
				if ( !m_original_fn ) {
					logging::print( encrypt( "Failed to get VTable." ) );
					return false;
				}

				return true;
			}

			logging::print( encrypt( "Unknown API flag: %d" ), api_flag );
			return false;
		}

		bool update( ) {
			auto compiler = std::make_unique<compiler::c_compiler>( );

			compiler->setup( );
			compiler->save_registers( );
			compiler->set_shadow_space( 40 );
			compiler->add_random_nops( );

			compiler->set_status( 1 );
			compiler->set_rcx( g_driver->m_base_address );
			compiler->set_rdx( 1 );
			compiler->set_r8( 0 );
			compiler->add_random_nops( );

			compiler->set_status( 2 );
			compiler->set_rax( m_dll_main );
			compiler->call_rax( );
			compiler->add_random_nops( );

			compiler->set_status( 3 );
			compiler->store_return( );
			compiler->add_random_nops( );

			compiler->restore_stack( 40 );
			compiler->restore_registers( );
			compiler->add_random_nops( );

			compiler->set_status( 4 );
			compiler->set_data_ref( &m_data_va );

			compiler->add_random_nops( );
			compiler->set_rax( m_original_fn );
			compiler->jmp_rax( );

			if ( !compiler->generate( &m_shellcode_va ) ) {
				logging::print( encrypt( "Failed to generate shellcode." ) );
				return false;
			}

			logging::print( encrypt( "Shellcode initialization complete:" ) );
			logging::print( encrypt( "  - DllMain: 0x%llX" ), m_dll_main );
			logging::print( encrypt( "  - Data address: 0x%llX" ), m_data_va );
			logging::print( encrypt( "  - Shellcode address: 0x%llX" ), m_shellcode_va );
			logging::print( encrypt( "Successfully initializated shellcode\n" ) );
			return true;
		}

		bool run( ) {
			logging::print( encrypt( "Waiting for module initialization" ) );

			g_driver->write( m_vtable, m_shellcode_va );

			while ( true ) {
				if ( !g_driver->get_process_id( target_process ) ) {
					debug_exception( );
					return false;
				}

				g_driver->read_memory( m_data_va, &m_remote_data, sizeof( m_remote_data ) );
				if ( m_remote_data.m_status == 3 )
					break;

				std::this_thread::sleep_for(
					std::chrono::milliseconds(
					25
				) );
			}

			logging::print( encrypt( "Module initialization complete:" ) );
			logging::print( encrypt( "  - Status: %i" ), m_remote_data.m_status );
			logging::print( encrypt( "  - Return: %i" ), m_remote_data.m_return );
			logging::print( encrypt( "Successfully initialized module\n" ) );
			return true;
		}

		void cleanup( ) const {
			if ( m_vtable && m_original_fn )
				g_driver->write( m_vtable, m_original_fn );

			if ( m_data_va ) g_driver->free_virtual( m_data_va );
			if ( m_shellcode_va ) g_driver->free_virtual( m_shellcode_va );
		}

	private:
		std::uint64_t m_image_va = 0;
		std::uint64_t m_dll_main = 0;
		std::uint64_t m_data_va = 0;
		std::uint64_t m_shellcode_va = 0;
		std::uint64_t m_vtable = 0;
		std::uint64_t m_original_fn = 0;
		shellcode::inject_data_t m_remote_data{};

		void debug_exception( ) {
			switch ( m_remote_data.m_status ) {
			case 1:
			{
				logging::print( encrypt( "Stored registers, allocating stack and settings up parameters" ) );
				logging::print( encrypt( "Target process terminated unexpectedly due to corrupted module code." ) );
			} break;
			case 2:
			{
				logging::print( encrypt( "Stored registers, derefrencing and executing module from RAX" ) );
				logging::print( encrypt( "Target process terminated unexpectedly due to corrupted module code." ) );
			} break;
			case 3:
			{
				logging::print( encrypt( "Executed module, restoring registers and calling original" ) );
				logging::print( encrypt( "Target process terminated unexpectedly due to runtime library." ) );
			} break;
			case 4:
			{
				logging::print( encrypt( "Successfully called original and restored registers" ) );
				logging::print( encrypt( "Target process terminated unexpectedly, corrupted module." ) );
			} break;
			default:
			{
				logging::print( encrypt( "Shellcode hasn't executed in target module" ) );
				logging::print( encrypt( "Target process terminated unexpectedly, unknown issue." ) );
			} break;
			}
		}
	};
}