
namespace image
{
	class c_image {
		std::vector<uint8_t> m_image;
		PIMAGE_DOS_HEADER m_dos_header = nullptr;
		PIMAGE_NT_HEADERS64 m_nt_headers = nullptr;
		PIMAGE_SECTION_HEADER m_section_header = nullptr;
		IMAGE_OPTIONAL_HEADER64 m_optional_header{ };

	public:
		bool parse_headers( std::vector<uint8_t> image_bytes ) {
			m_image = std::move( image_bytes );
			if ( m_image.empty( ) )
				return false;

			m_dos_header = reinterpret_cast< PIMAGE_DOS_HEADER >(
				m_image.data( )
				);

			if ( m_dos_header->e_magic != IMAGE_DOS_SIGNATURE ) {
				return false;
			}

			m_nt_headers = reinterpret_cast< PIMAGE_NT_HEADERS >(
				m_image.data( ) + m_dos_header->e_lfanew
				);

			if ( m_nt_headers->Signature != IMAGE_NT_SIGNATURE || 
				m_nt_headers->OptionalHeader.Magic != 0x20B ) {
				return false;
			}

			m_section_header = reinterpret_cast< IMAGE_SECTION_HEADER* >(
				( std::uintptr_t ) ( &m_nt_headers->OptionalHeader ) + m_nt_headers->FileHeader.SizeOfOptionalHeader
				);

			m_optional_header = m_nt_headers->OptionalHeader;
			return true;
		}

		bool parse_headers( const uint8_t* bytes, size_t size ) {
			return parse_headers( std::vector<uint8_t>( bytes, bytes + size ) );
		}

		bool relocate( std::uint64_t new_image_base ) {
			auto delta_offset = new_image_base - m_optional_header.ImageBase;
			if ( m_nt_headers->FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED ) {
				logging::print( encrypt( "ERROR: Image has relocations stripped" ) );
				return false;
			}

			auto reloc_dir = get_directory( IMAGE_DIRECTORY_ENTRY_BASERELOC );
			if ( !reloc_dir || !reloc_dir->Size ) {
				logging::print( encrypt( "ERROR: No relocation directory" ) );
				return false;
			}

			logging::print( encrypt( "Starting image reallocation" ) );

			struct reloc_entry_t {
				std::uint32_t m_to_rva;
				std::uint32_t m_size;
				struct {
					std::uint16_t m_offset : 12;
					std::uint16_t m_type : 4;
				} m_item[ 1 ];
			};

			auto reloc_entry = reinterpret_cast< reloc_entry_t* >( rva_to_va( reloc_dir->VirtualAddress ) );
			auto reloc_end = reinterpret_cast< reloc_entry_t* >(
				reinterpret_cast< uint8_t* >( reloc_entry ) + reloc_dir->Size
				);

			std::uint32_t relocations = 0;
			while ( reloc_entry < reloc_end && reloc_entry->m_size ) {
				auto record_count = ( reloc_entry->m_size - 8 ) >> 1;
				logging::print( encrypt( "Processing block - RVA: 0x%x, Size: 0x%x, Records: %d" ),
								reloc_entry->m_to_rva, reloc_entry->m_size, record_count );

				for ( auto i = 0u; i < record_count; i++ ) {
					auto offset = reloc_entry->m_item[ i ].m_offset % 4096;
					auto type = reloc_entry->m_item[ i ].m_type;
					if ( type == IMAGE_REL_BASED_ABSOLUTE )
						continue;

					auto reloc_addr = reinterpret_cast< std::uintptr_t >(
						rva_to_va( reloc_entry->m_to_rva )
						);
					if ( !reloc_addr ) {
						logging::print( encrypt( "ERROR: Invalid relocation address at: 0x%x" ), reloc_addr );
						continue;
					}

					auto reloc_va = reinterpret_cast< std::uint64_t* >( reloc_addr + offset );
					auto original_va = *reloc_va;
					*reloc_va += delta_offset;
					relocations++;
				}

				reloc_entry = reinterpret_cast< reloc_entry_t* >(
					reinterpret_cast< uint8_t* >( reloc_entry ) + reloc_entry->m_size
					);
			}

			logging::print( encrypt( "Successfully completed %d relocations\n" ), relocations );
			return relocations > 0;
		}

		bool map_delayed_imports( uint64_t base_address ) {
			auto delay_dir = get_directory( IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT );
			if ( !delay_dir || !delay_dir->VirtualAddress ) {
				return true;
			}

			logging::print( encrypt( "Starting delayed import mapping" ) );

			auto delay_desc = reinterpret_cast< PIMAGE_DELAYLOAD_DESCRIPTOR >(
				rva_to_va( delay_dir->VirtualAddress )
				);

			if ( !delay_desc ) {
				logging::print( encrypt( "Failed to read delay import descriptor" ) );
				return false;
			}

			while ( delay_desc->DllNameRVA ) {
				auto module_name = reinterpret_cast< LPSTR >(
					rva_to_va( delay_desc->DllNameRVA )
					);

				if ( !module_name ) {
					logging::print( encrypt( "Invalid module name" ) );
					return false;
				}

				auto import_address_table = reinterpret_cast< PIMAGE_THUNK_DATA >(
					base_address + delay_desc->ImportAddressTableRVA
					);

				auto import_name_table = reinterpret_cast< PIMAGE_THUNK_DATA >(
					base_address + delay_desc->ImportNameTableRVA
					);

				while ( import_name_table->u1.AddressOfData ) {
					if ( IMAGE_SNAP_BY_ORDINAL64( import_name_table->u1.Ordinal ) ) {
						auto ordinal = IMAGE_ORDINAL64( import_name_table->u1.Ordinal );
						import_address_table->u1.Function =
							modules::get_export(
								module_name,
								reinterpret_cast< LPSTR >( ordinal )
							);
					}
					else {
						auto ibn = reinterpret_cast< PIMAGE_IMPORT_BY_NAME >(
							base_address + import_name_table->u1.AddressOfData
							);

						import_address_table->u1.Function = modules::get_export( module_name, ibn->Name );
					}

					if ( !import_address_table->u1.Function ) {
						logging::print( encrypt( "Failed to resolve import" ) );
						return false;
					}

					import_name_table++;
					import_address_table++;
				}

				delay_desc++;
			}

			logging::print( encrypt( "Import mapping completed successfully\n" ) );
			return true;
		}

		bool map_imports( ) {
			auto import_desc = reinterpret_cast< PIMAGE_IMPORT_DESCRIPTOR >(
				rva_to_va( get_directory( IMAGE_DIRECTORY_ENTRY_IMPORT )->VirtualAddress )
				);

			if ( !import_desc || !get_directory( IMAGE_DIRECTORY_ENTRY_IMPORT )->Size )
				return true;

			logging::print( encrypt( "Starting import mapping" ) );

			while ( import_desc->Name != 0 ) {
				auto module_name = reinterpret_cast< LPSTR >(
					rva_to_va( import_desc->Name )
					);

				if ( !module_name ) {
					logging::print( encrypt( "Invalid module name" ) );
					break;
				}

				auto orig_first_thunk = reinterpret_cast< PIMAGE_THUNK_DATA >(
					rva_to_va( import_desc->OriginalFirstThunk )
					);

				auto first_thunk = reinterpret_cast< PIMAGE_THUNK_DATA >(
					rva_to_va( import_desc->FirstThunk )
					);

				if ( !first_thunk || !orig_first_thunk ) {
					logging::print( encrypt( "Invalid thunk data" ) );
					break;
				}

				while ( first_thunk->u1.AddressOfData ) {
					if ( IMAGE_SNAP_BY_ORDINAL64( orig_first_thunk->u1.Ordinal ) ) {
						auto ordinal = IMAGE_ORDINAL64( orig_first_thunk->u1.Ordinal );
						first_thunk->u1.Function =
							modules::get_export(
								module_name,
								reinterpret_cast< LPSTR >( ordinal ) );
						logging::print( encrypt( "Mapped ordinal import: %d" ), ordinal );
					}
					else {
						auto ibn = reinterpret_cast< PIMAGE_IMPORT_BY_NAME >(
							rva_to_va( orig_first_thunk->u1.AddressOfData )
							);

						if ( !ibn ) {
							logging::print( encrypt( "Invalid import by name" ) );
							return false;
						}

						first_thunk->u1.Function = modules::get_export( module_name, ibn->Name );
						logging::print( encrypt( "Mapped named import: %s" ), ibn->Name );
					}

					if ( !first_thunk->u1.Function ) {
						logging::print( encrypt( "Failed to resolve import" ) );
						return false;
					}

					orig_first_thunk++;
					first_thunk++;
				}

				crt::mem_zero( module_name, crt::str_len( module_name ) );
				import_desc++;
			}

			logging::print( encrypt( "Import mapping completed successfully\n" ) );
			return true;
		}

		bool map_sections( std::uint64_t new_image_base ) {
			logging::print( encrypt( "Starting section mapping" ) );

			for ( auto i = 0u; i < m_nt_headers->FileHeader.NumberOfSections; i++ ) {
				const auto& section = m_section_header[ i ];
				if ( !section.VirtualAddress || !section.SizeOfRawData ) {
					logging::print( encrypt( "Invalid section parameters detected" ) );
					continue;
				}

				auto section_va = new_image_base + section.VirtualAddress;
				if ( !section_va ) {
					logging::print( encrypt( "Invalid section found at: %s" ), section.Name );
					continue;
				}

				exports::memcpy(
					reinterpret_cast< void* >( section_va ),
					reinterpret_cast< void* >( m_image.data( ) + section.PointerToRawData ),
					section.SizeOfRawData
				);

				logging::print( encrypt( "Exported section %s at: 0x%llx" ), section.Name, new_image_base + section.VirtualAddress );
			}

			logging::print( encrypt( "Section mapping completed successfully\n" ) );
			return true;
		}

		bool zero_image( std::uint64_t new_image_base ) const {
			auto image_size = ( get_size( ) + 4095 ) & 0xFFFFF000;
			auto zero_buffer = crt::malloc1( image_size );
			if ( !zero_buffer )
				return false;

			crt::mem_zero( zero_buffer, image_size );
			exports::memcpy(
				reinterpret_cast< void* >( new_image_base ),
				zero_buffer,
				image_size
			);

			crt::free1( zero_buffer );
			return true;
		}

		void protect_sections( ) {

		}

		bool init_security_cookie( uint64_t base_address ) {
			auto load_config = get_directory( IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG );
			if ( !load_config->VirtualAddress )
				return true;

			auto config = reinterpret_cast< PIMAGE_LOAD_CONFIG_DIRECTORY64 >(
				rva_to_va( load_config->VirtualAddress )
				);

			if ( !config || !config->SecurityCookie )
				return true;

			uint64_t cookie = __rdtsc( );
			cookie ^= base_address;
			cookie |= 0x4000000000000000ull;

			*reinterpret_cast< uint64_t* >( base_address + config->SecurityCookie ) = cookie;
			return true;
		}

		std::uint64_t get_entry_point( ) const {
			auto entry_rva = m_nt_headers->OptionalHeader.AddressOfEntryPoint;
			for ( int i = 0; i < m_nt_headers->FileHeader.NumberOfSections; i++ ) {
				auto& section = m_section_header[ i ];
				if ( entry_rva >= section.VirtualAddress &&
					 entry_rva < ( section.VirtualAddress + section.Misc.VirtualSize ) ) {
					logging::print( encrypt( "Entry point found in section: %s" ), section.Name );
					break;
				}
			}

			return entry_rva;
		}

		std::uint32_t get_size( ) const {
			return m_nt_headers->OptionalHeader.SizeOfImage;
		}

		std::uint32_t get_timestamp( ) const {
			auto file_header = &m_nt_headers->FileHeader;
			return file_header->TimeDateStamp;
		}

	private:
		IMAGE_SECTION_HEADER* find_section(const char* name) {
			for (WORD i = 0; i < m_nt_headers->FileHeader.NumberOfSections; i++) {
				auto& section = m_section_header[i];

				if (memcmp(section.Name, name, strlen(name)) == 0) {
					logging::print(encrypt("Found section %s at index %d"), name, i);
					logging::print(encrypt("Section details - VA: 0x%x, Size: 0x%x"), 
									section.VirtualAddress, 
									section.SizeOfRawData);
					return &m_section_header[i];
				}
			}

			logging::print(encrypt("Section %s not found"), name);
			return nullptr;
		}

		void* rva_to_va( std::uintptr_t rva ) {
			auto first_section = m_section_header;
			for ( auto section = first_section; section < first_section + m_nt_headers->FileHeader.NumberOfSections; section++ ) {
				if ( rva >= section->VirtualAddress && rva < section->VirtualAddress + section->Misc.VirtualSize )
					return reinterpret_cast< void* >( m_image.data( ) + section->PointerToRawData + rva - section->VirtualAddress );
			}
			return nullptr;
		}

		IMAGE_DATA_DIRECTORY* get_directory( int directory_index ) {
			if ( directory_index >= IMAGE_NUMBEROF_DIRECTORY_ENTRIES )
				return nullptr;

			return &m_nt_headers->OptionalHeader.DataDirectory[ directory_index ];
		}
	};
}