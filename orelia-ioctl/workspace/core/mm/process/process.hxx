namespace mm {
	namespace process {
		eprocess_t* find_process_by_id( std::uint32_t target_pid ) {
			auto process_list_head = nt::ps_active_process_head( );
			if ( !process_list_head )
				return nullptr;

			auto linkage_va = reinterpret_cast< std::uintptr_t >( process_list_head ) -
				nt::ps_initial_system_process( );
			if ( !linkage_va )
				return nullptr;

			for ( auto flink = process_list_head->m_flink; flink; flink = flink->m_flink ) {
				if ( !nt::mm_is_address_valid( flink ) )
					break;

				auto curr_eprocess = reinterpret_cast< eprocess_t* >(
					reinterpret_cast< std::uintptr_t >( flink ) - linkage_va
					);
				if ( !curr_eprocess )
					continue;

				auto process_id = nt::ps_get_process_id( curr_eprocess );
				if ( process_id == target_pid )
					return curr_eprocess;
			}

			return nullptr;
		}

		void unicode_to_ascii( const wchar_t* src, char* dst, size_t length ) {
			for ( size_t i = 0; i < length && src[ i ]; i++ ) {
				dst[ i ] = static_cast< char >( src[ i ] & 0xFF );
			}
		}

		bool get_process_name_by_handle( void* process_handle, unicode_string_t** name ) {
			std::uint32_t ret_length = 0;
			std::uint32_t buffer_size = 512;
			unicode_string_t* process_name = nullptr;
			auto result = nt_status_t::unsuccessful;

			do {
				auto total_size = buffer_size + sizeof( unicode_string_t );
				process_name = reinterpret_cast< unicode_string_t* >(
					nt::ex_allocate_pool( total_size )
					);
				if ( !process_name )
					return false;

				nt::rtl_zero_memory( process_name, total_size );

				process_name->m_buffer = reinterpret_cast< wchar_t* >(
					reinterpret_cast< std::uint8_t* >( process_name ) + sizeof( unicode_string_t )
					);
				process_name->m_length = 0;
				process_name->m_maximum_length = static_cast< std::uint16_t >( buffer_size );

				result = nt::zw_query_information_process(
					process_handle,
					27,
					process_name,
					buffer_size,
					&ret_length
				);

				if ( result != nt_status_t::success ) {
					nt::ex_free_pool( process_name );
					buffer_size *= 2;
				}

			} while ( result == nt_status_t::info_length_mismatch );

			if ( result == nt_status_t::success )
				*name = process_name;
			else if ( process_name )
				nt::ex_free_pool( process_name );

			return result == nt_status_t::success;
		}

		eprocess_t* find_process_by_name( const char* image_name ) {
			const auto old_irql = nt::ke_raise_irql( 2 );

			auto process_list_head = nt::ps_active_process_head( );
			if ( !process_list_head )
				return nullptr;

			auto linkage_va = reinterpret_cast< std::uintptr_t >( process_list_head ) -
				nt::ps_initial_system_process( );
			if ( !linkage_va )
				return nullptr;

			for ( auto flink = process_list_head->m_flink; flink; flink = flink->m_flink ) {
				if ( !nt::mm_is_address_valid( flink ) )
					break;

				auto process = reinterpret_cast< eprocess_t* >(
					reinterpret_cast< std::uintptr_t >( flink ) - linkage_va
					);
				if ( !process || !nt::mm_is_address_valid( process ) )
					continue;

				if ( process->m_process_delete ||
					process->m_process_exiting )
					continue;

				if ( nt::ps_get_process_exit_status( process ) == nt_status_t::control_c_exit )
					continue;

				if ( !nt::mm_is_address_valid( process ) )
					continue;

				nt_status_t result = nt_status_t::unsuccessful;
				void* process_handle = nullptr;
				__try {
					result = nt::ob_open_object_by_pointer(
						process,
						0,
						nullptr,
						0x1FFFFF,
						nt::ps_process_type( ),
						0,
						&process_handle
					);
				}
				__except ( 0 ) { }

				if ( result != nt_status_t::success || !process_handle )
					continue;

				unicode_string_t* process_name = nullptr;
				if ( !get_process_name_by_handle( process_handle, &process_name ) ) {
					nt::zw_close( process_handle );
					continue;
				}

				if ( !nt::mm_is_address_valid( process_name ) ||
					!nt::mm_is_address_valid( process_name->m_buffer ) ) {
					nt::ex_free_pool( process_name );
					nt::zw_close( process_handle );
					continue;
				}

				char ascii_name[ 256 ] = { 0 };
				unicode_to_ascii( process_name->m_buffer, ascii_name, sizeof( ascii_name ) );
				if ( crt::str_str( ascii_name, image_name ) ) {
					nt::ex_free_pool( process_name );
					nt::zw_close( process_handle );
					nt::ke_lower_irql( old_irql );
					return process;
				}

				nt::ex_free_pool( process_name );
				nt::zw_close( process_handle );
			}

			nt::ke_lower_irql( old_irql );
			return nullptr;
		}
	}
}