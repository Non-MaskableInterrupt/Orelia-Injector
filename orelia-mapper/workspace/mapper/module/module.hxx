
namespace modules
{
	[[nodiscard]]
	std::uintptr_t get_image( const char* module_name, char* full_path = nullptr ) {
		void* buffer = nullptr;
		unsigned long buffer_size = 0;

		auto status = NtQuerySystemInformation(
			static_cast< SYSTEM_INFORMATION_CLASS >( 11 ),
			buffer,
			buffer_size,
			&buffer_size
		);

		while ( status == 0xC0000004L ) {
			crt::free1( buffer );
			buffer = crt::malloc1( buffer_size );
			status = NtQuerySystemInformation( static_cast< SYSTEM_INFORMATION_CLASS >( 11 ), buffer, buffer_size, &buffer_size );
		}

		if ( !NT_SUCCESS( status ) ) {
			crt::free1( buffer );
			return 0;
		}

		std::uint64_t module_base = 0;
		const auto modules = static_cast< rtl_process_modules_t* >( buffer );
		for ( auto idx = 0u; idx < modules->m_count; ++idx ) {
			const auto current_module_name = std::string(
				reinterpret_cast< char* >( modules->m_modules[ idx ].m_full_path ) +
				modules->m_modules[ idx ].m_offset_to_file_name
			);

			if ( !_stricmp( current_module_name.c_str( ), module_name ) ) {
				if ( full_path ) {
					GetWindowsDirectoryA( full_path, 64 );
					crt::str_cat( full_path, encrypt( "\\" ).decrypt( ) );
					crt::str_cat( full_path, ( char* ) &modules->m_modules[ idx ].m_full_path[ 12 ] );
				}

				module_base = reinterpret_cast< std::uint64_t >( modules->m_modules[ idx ].m_image_base );
				break;
			}
		}

		crt::free1( buffer );
		return module_base;
	}

	[[nodiscard]]
	std::uint64_t find_pattern( const char* image_name, const char* pattern ) {
		char file_path[ 64 ]{ };
		auto image_base = get_image( image_name, file_path );
		if ( !image_base ) {
			return 0;
		}

		auto h_module = LoadLibraryExA( file_path, nullptr, DONT_RESOLVE_DLL_REFERENCES );
		if ( !h_module ) {
			return 0;
		}

		auto module_start = reinterpret_cast< std::uint8_t* >( h_module );
		if ( !module_start ) {
			return 0;
		}

		auto dos_header = reinterpret_cast< PIMAGE_DOS_HEADER >( module_start );
		auto nt_header = reinterpret_cast< PIMAGE_NT_HEADERS >( module_start + dos_header->e_lfanew );
		auto module_end = reinterpret_cast< std::uint8_t* >( module_start + nt_header->OptionalHeader.SizeOfImage - 0x1000 );

		std::uint8_t* result = 0;
		auto curr_pattern = pattern;
		for ( ; module_start < module_end; ++module_start ) {
			bool skip_byte = ( *curr_pattern == '\?' );
			if ( skip_byte || *module_start == get_byte( curr_pattern ) ) {
				if ( !result ) result = module_start;
				skip_byte ? curr_pattern += 2 : curr_pattern += 3;
				if ( curr_pattern[ -1 ] == 0 ) {
					break;
				}
			}
			else if ( result ) {
				module_start = result;
				result = nullptr;
				curr_pattern = pattern;
			}
		}

		const auto module_base = reinterpret_cast< std::uintptr_t >( h_module );
		auto address = image_base + (
			reinterpret_cast< std::uintptr_t >( result ) - module_base
			);

		FreeLibrary( h_module );
		return address;
	}

	[[nodiscard]]
	void* rva( const char* image_name, std::uint64_t instruction, int size ) {
		char file_path[ 64 ]{ };
		auto image_base = get_image( image_name, file_path );
		if ( !image_base ) {
			return nullptr;
		}

		auto h_module = LoadLibraryExA( file_path, nullptr, DONT_RESOLVE_DLL_REFERENCES );
		if ( !h_module ) {
			return nullptr;
		}

		const auto module_base = reinterpret_cast< std::uint64_t >( h_module );
		const auto local_instruction = module_base + ( instruction - image_base );

		const auto displacement_offset = size - sizeof( int32_t );
		const auto p_displacement = reinterpret_cast< int32_t* >(
			local_instruction + displacement_offset
			);

		const auto local_target = local_instruction + size + *p_displacement;
		const auto target_address = image_base + ( local_target - module_base );

		FreeLibrary( h_module );
		return reinterpret_cast< void* >( target_address );
	}

	[[nodiscard]]
	void* rva_lea( const char* image_name, std::uint64_t instruction, int size ) {
		char file_path[ 64 ]{};
		auto image_base = get_image( image_name, file_path );
		if ( !image_base ) {
			return nullptr;
		}

		auto h_module = LoadLibraryExA( file_path, nullptr, DONT_RESOLVE_DLL_REFERENCES );
		if ( !h_module ) {
			return nullptr;
		}

		const auto module_base = reinterpret_cast< std::uint64_t >( h_module );
		const auto local_instruction = module_base + ( instruction - image_base );

		const auto p_displacement = reinterpret_cast< int32_t* >( local_instruction + 3 );
		const auto local_target = ( local_instruction + 7 ) + *p_displacement;

		const auto target_address = image_base + ( local_target - module_base );

		FreeLibrary( h_module );
		return reinterpret_cast< void* >( target_address );
	}

	[[nodiscard]]
	std::uintptr_t get_export( const char* image_name, const char* module_name ) {
		char file_path[ 64 ]{ };
		auto image_base = get_image( image_name, file_path );
		if ( !image_base ) {
			logging::print( encrypt( "Failed to get image base for %s" ), image_name );
			return 0;
		}

		auto h_module = LoadLibraryExA( file_path, nullptr, DONT_RESOLVE_DLL_REFERENCES );
		if ( !h_module ) {
			logging::print( encrypt( "Failed to load module %s, error: %d" ), file_path, GetLastError( ) );
			return 0;
		}

		auto address = reinterpret_cast< std::uintptr_t >(
			GetProcAddress( h_module, module_name )
			);
		if ( !address ) {
			address = reinterpret_cast< std::uintptr_t >(
				pdb::get_symbol_address( file_path, module_name )
				);

			if ( !address ) {
				logging::print( encrypt( "Failed to Parse PDB for symbol: %s" ), module_name );
				FreeLibrary( h_module );
				return 0;
			}

			address += image_base;
			logging::print( encrypt( "Parsed PDB for %s at: 0x%llx" ), module_name, address );
		}
		else {
			address = image_base + ( address - reinterpret_cast< std::uintptr_t >( h_module ) );
		}

		FreeLibrary( h_module );
		return address;
	}
}