#include <map>

namespace module {
    struct s_vtable_candidate {
        uintptr_t address;
        std::vector<uintptr_t> functions;
        size_t function_count;
        bool is_valid;
    };

    class c_module {
    public:
        bool load_file( const std::string& file_path ) {
            std::ifstream fstr( file_path, std::ios::binary | std::ios::ate );
            if ( !fstr.is_open( ) ) {
                logging::print( encrypt( "Failed to open file: %s" ), file_path.c_str( ) );
                return false;
            }

            const auto file_size = fstr.tellg( );
            if ( file_size <= 0 ) {
                logging::print( encrypt( "File is empty: %s" ), file_path.c_str( ) );
                return false;
            }

            fstr.seekg( 0, std::ios::beg );

            m_target_image.resize( static_cast< size_t >( file_size ) );
            if ( !fstr.read( reinterpret_cast< char* >( m_target_image.data( ) ), file_size ) ) {
                logging::print( encrypt( "Failed to read file: %s" ), file_path.c_str( ) );
                m_target_image.clear( );
                return false;
            }

            if ( m_target_image.empty( ) ) {
                logging::print( encrypt( "File is empty or could not be read: %s" ), file_path.c_str( ) );
                return false;
            }

            m_dos_header = reinterpret_cast< PIMAGE_DOS_HEADER >( m_target_image.data( ) );
            if ( m_dos_header->e_magic != IMAGE_DOS_SIGNATURE ) {
                logging::print( encrypt( "Invalid DOS signature in: %s" ), file_path.c_str( ) );
                m_target_image.clear( );
                return false;
            }

            m_nt_headers = reinterpret_cast< PIMAGE_NT_HEADERS64 >(
                m_target_image.data( ) + m_dos_header->e_lfanew
                );
            if ( m_nt_headers->Signature != IMAGE_NT_SIGNATURE ) {
                logging::print( encrypt( "Invalid NT headers in: %s" ), file_path.c_str( ) );
                m_target_image.clear( );
                return false;
            }

            m_section_header = IMAGE_FIRST_SECTION( m_nt_headers );
            m_optional_header = m_nt_headers->OptionalHeader;

            logging::print( encrypt( "Successfully loaded file: %s (Size: 0x%llX)" ),
                file_path.c_str( ), m_target_image.size( ) );
            return true;
        }

        bool copy_headers( std::uintptr_t image_base ) {
            if ( !g_driver->write_memory(
                image_base,
                m_target_image.data( ),
                m_optional_header.SizeOfHeaders ) ) {
                logging::print( "Failed to write headers" );
                return false;
            }
            return true;
        }

        bool verify_module_integrity( uintptr_t image_base ) {
            logging::print( encrypt( "Verifying mapped module integrity..." ) );

            IMAGE_DOS_HEADER dos_header;
            if ( !g_driver->read_memory( image_base, &dos_header, sizeof( dos_header ) ) ) {
                logging::print( encrypt( "Failed to read DOS header" ) );
                return false;
            }

            if ( dos_header.e_magic != IMAGE_DOS_SIGNATURE ) {
                logging::print( encrypt( "Invalid DOS header signature: 0x%x (expected 0x%x)" ),
                    dos_header.e_magic, IMAGE_DOS_SIGNATURE );
                return false;
            }

            IMAGE_NT_HEADERS64 nt_headers;
            if ( !g_driver->read_memory( image_base + dos_header.e_lfanew, &nt_headers, sizeof( nt_headers ) ) ) {
                logging::print( encrypt( "Failed to read NT headers" ) );
                return false;
            }

            if ( nt_headers.Signature != IMAGE_NT_SIGNATURE ) {
                logging::print( encrypt( "Invalid NT headers signature: 0x%x (expected 0x%x)" ),
                    nt_headers.Signature, IMAGE_NT_SIGNATURE );
                return false;
            }

            logging::print( encrypt( "PE headers verified successfully" ) );

            size_t section_headers_size = nt_headers.FileHeader.NumberOfSections * sizeof( IMAGE_SECTION_HEADER );
            std::vector<IMAGE_SECTION_HEADER> mapped_sections( nt_headers.FileHeader.NumberOfSections );

            if ( !g_driver->read_memory(
                image_base + dos_header.e_lfanew + sizeof( IMAGE_NT_HEADERS64 ),
                mapped_sections.data( ),
                section_headers_size ) ) {
                logging::print( encrypt( "Failed to read section headers" ) );
                return false;
            }

            for ( WORD i = 0; i < nt_headers.FileHeader.NumberOfSections; i++ ) {
                auto& mapped_section = mapped_sections[ i ];
                auto& original_section = m_section_header[ i ];

                char mapped_name[ IMAGE_SIZEOF_SHORT_NAME + 1 ] = { 0 };
                char original_name[ IMAGE_SIZEOF_SHORT_NAME + 1 ] = { 0 };

                memcpy( mapped_name, mapped_section.Name, IMAGE_SIZEOF_SHORT_NAME );
                memcpy( original_name, original_section.Name, IMAGE_SIZEOF_SHORT_NAME );

                logging::print( encrypt( "Section %d - Original: '%s', Mapped: '%s'" ),
                    i, original_name, mapped_name );

                if ( memcmp( mapped_section.Name, original_section.Name, IMAGE_SIZEOF_SHORT_NAME ) != 0 ) {
                    logging::print( encrypt( "Section name mismatch at index %d" ), i );

                    logging::print( encrypt( "Original name bytes:" ) );
                    for ( int j = 0; j < IMAGE_SIZEOF_SHORT_NAME; j++ ) {
                        logging::print( encrypt( "  %02X" ), ( uint8_t )original_section.Name[ j ] );
                    }

                    logging::print( encrypt( "Mapped name bytes:" ) );
                    for ( int j = 0; j < IMAGE_SIZEOF_SHORT_NAME; j++ ) {
                        logging::print( encrypt( "  %02X" ), ( uint8_t )mapped_section.Name[ j ] );
                    }

                    return false;
                }

                logging::print( encrypt( "Section %.8s verified successfully" ), original_section.Name );
            }

            return true;
        }

        std::uint64_t find_export( const char* export_name ) {
            auto export_dir = get_directory( IMAGE_DIRECTORY_ENTRY_EXPORT );
            if ( export_dir->VirtualAddress && export_dir->Size ) {
                auto export_directory = rva_to_va<PIMAGE_EXPORT_DIRECTORY>( export_dir->VirtualAddress );
                if ( export_directory ) {
                    auto functions = rva_to_va<DWORD*>( export_directory->AddressOfFunctions );
                    auto names = rva_to_va<DWORD*>( export_directory->AddressOfNames );
                    auto ordinals = rva_to_va<WORD*>( export_directory->AddressOfNameOrdinals );

                    if ( functions && names && ordinals ) {
                        for ( DWORD i = 0; i < export_directory->NumberOfNames; i++ ) {
                            auto name = rva_to_va<const char*>( names[ i ] );
                            if ( !name ) continue;

                            if ( !strcmp( name, export_name ) ) {
                                auto ordinal = ordinals[ i ];
                                return functions[ ordinal ];
                            }
                        }
                    }
                }
            }

            return 0;
        }

        bool map_imports( ) {
            auto import_dir = get_directory( IMAGE_DIRECTORY_ENTRY_IMPORT );
            if ( !import_dir->VirtualAddress || !import_dir->Size ) {
                logging::print( encrypt( "No imports to resolve" ) );
                return true;
            }

            auto import_desc = rva_to_va<PIMAGE_IMPORT_DESCRIPTOR>( import_dir->VirtualAddress );
            if ( !import_desc ) {
                logging::print( encrypt( "Invalid import descriptor" ) );
                return false;
            }

            size_t imports = 0;
            size_t modules = 0;

            while ( import_desc->Name ) {
                modules++;
                auto module_name = rva_to_va<const char*>( import_desc->Name );
                if ( !module_name ) {
                    logging::print( encrypt( "Invalid module name" ) );
                    return false;
                }

                auto resolved_name = resolver::resolve_module_name( module_name );
                if ( resolved_name.empty( ) ) {
                    logging::print( encrypt( "Invalid module name" ) );
                    return false;
                }

                char module_path[ MAX_PATH ] = { 0 };
                GetSystemDirectoryA( module_path, MAX_PATH );
                char narrow_name[ MAX_PATH ] = { 0 };
                WideCharToMultiByte( CP_ACP, 0, resolved_name.c_str( ), -1, narrow_name, MAX_PATH, NULL, NULL );
                strcat_s( module_path, "\\" );
                strcat_s( module_path, narrow_name );

                auto module_base = g_driver->get_process_module( resolved_name.c_str( ) );
                if ( !module_base ) {
                    logging::print( encrypt( "Mapping depedency: %S" ), resolved_name.c_str( ) );

                    if ( !load_depedency( module_path ) ) {
                        logging::print( encrypt( "Failed to map depedency" ) );
                        return false;
                    }

                    auto module_base = g_driver->get_process_module( resolved_name.c_str( ) );
                    if ( !module_base ) {
                        logging::print( encrypt( "Depedency not found in process." ) );
                        return false;
                    }
                }

                auto thunk_data = rva_to_va<PIMAGE_THUNK_DATA64>( import_desc->FirstThunk );
                if ( !thunk_data ) {
                    logging::print( encrypt( "Invalid thunk data" ) );
                    return false;
                }

                auto original_thunk = rva_to_va<PIMAGE_THUNK_DATA64>(
                    import_desc->OriginalFirstThunk ? import_desc->OriginalFirstThunk : import_desc->FirstThunk );
                if ( !original_thunk ) {
                    logging::print( encrypt( "Invalid original thunk data" ) );
                    return false;
                }

                for ( size_t i = 0; original_thunk[ i ].u1.AddressOfData != 0; i++ ) {
                    imports++;

                    uintptr_t func_addr = 0;
                    if ( IMAGE_SNAP_BY_ORDINAL64( original_thunk[ i ].u1.Ordinal ) ) {
                        WORD ordinal = IMAGE_ORDINAL64( original_thunk[ i ].u1.Ordinal );

                        char ordinal_str[ 16 ] = { 0 };
                        sprintf_s( ordinal_str, "#%u", ordinal );
                        func_addr = resolver::resolve_export_pdb( module_base, ordinal_str, module_path );
                        if ( !func_addr ) {
                            logging::print( encrypt( "Failed to find export for ordinal %u" ), ordinal );
                            return false;
                        }
                    }
                    else {
                        auto import_by_name = rva_to_va<PIMAGE_IMPORT_BY_NAME>( original_thunk[ i ].u1.AddressOfData );
                        if ( !import_by_name ) {
                            logging::print( encrypt( "Invalid import by name" ) );
                            return false;
                        }

                        auto func_name = import_by_name->Name;
                        func_addr = resolver::resolve_export_pdb( module_base, func_name, module_path );
                        if ( !func_addr ) {
                            logging::print( encrypt( "Failed to find export for %s" ), func_name );
                            return false;
                        }
                    }

                    thunk_data[ i ].u1.Function = func_addr;
                }

                import_desc++;
            }

            logging::print( encrypt( "Processed %zu imports from %zu modules" ), imports, modules );
            return true;
        }

        bool map_sections( std::uint64_t new_image_base ) {
            for ( auto i = 0u; i < m_nt_headers->FileHeader.NumberOfSections; i++ ) {
                const auto& section = m_section_header[ i ];
                if ( section.VirtualAddress < m_optional_header.SizeOfHeaders ) {
                    logging::print( encrypt( "Section %s overlaps with headers (VA: 0x%X < SizeOfHeaders: 0x%X)" ),
                        section.Name, section.VirtualAddress, m_optional_header.SizeOfHeaders );
                    return false;
                }

                if ( !g_driver->write_memory(
                    new_image_base + section.VirtualAddress,
                    reinterpret_cast<void*>( m_target_image.data( ) + section.PointerToRawData ),
                    section.SizeOfRawData ) ) {
                    logging::print( encrypt( "Failed to copy section %-8s from 0x%llx" ),
                        section.Name, section.VirtualAddress );
                    return false;
                }

                logging::print( encrypt( "Copied section %-8s from 0x%llx to 0x%llx" ), section.Name,
                    ( uintptr_t )( m_target_image.data( ) + section.PointerToRawData ), 
                    new_image_base + section.VirtualAddress );
            }

            return true;
        }

        bool relocate( std::uint64_t new_image_base ) {
            auto reloc_dir = get_directory( IMAGE_DIRECTORY_ENTRY_BASERELOC );
            if ( !reloc_dir->VirtualAddress || !reloc_dir->Size ) {
                logging::print( encrypt( "No relocations needed" ) );
                return true;
            }

            auto delta = new_image_base - m_optional_header.ImageBase;
            if ( delta == 0 ) {
                logging::print( encrypt( "No relocation needed - Image loaded at preferred base address" ) );
                return true;
            }

            if ( !( m_nt_headers->OptionalHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE ) ) {
                logging::print( encrypt( "Image does not support dynamic base relocation" ) );
                return false;
            }

            logging::print( encrypt( "Relocating image to delta: 0x%llX" ), delta );

            int block_count = 0;
            int reloc_count = 0;

            auto reloc_block = rva_to_va<PIMAGE_BASE_RELOCATION>( reloc_dir->VirtualAddress );
            auto reloc_end = reinterpret_cast< uintptr_t >( reloc_block ) + reloc_dir->Size;

            while ( reinterpret_cast< uintptr_t >( reloc_block ) < reloc_end && reloc_block->VirtualAddress && reloc_block->SizeOfBlock ) {
                auto reloc_entries = reinterpret_cast< PWORD >(
                    reinterpret_cast< uintptr_t >( reloc_block ) + sizeof( IMAGE_BASE_RELOCATION ) );
                auto num_entries = ( reloc_block->SizeOfBlock - sizeof( IMAGE_BASE_RELOCATION ) ) / sizeof( WORD );
                auto reloc_page = rva_to_va<uint8_t*>( reloc_block->VirtualAddress );

                for ( auto i = 0u; i < num_entries; i++ ) {
                    auto type = reloc_entries[ i ] >> 12;
                    auto offset = reloc_entries[ i ] & 0xFFF;

                    if ( type == IMAGE_REL_BASED_ABSOLUTE )
                        continue;

                    if ( type == IMAGE_REL_BASED_HIGHLOW || type == IMAGE_REL_BASED_DIR64 ) {
                        auto local_reloc_address = reinterpret_cast< uintptr_t* >( reloc_page + offset );
                        uintptr_t current_value = *local_reloc_address;
                        uintptr_t new_value = current_value + delta;
                        *local_reloc_address = new_value;
                        uintptr_t remote_reloc_address = new_image_base + reloc_block->VirtualAddress + offset;

                        if ( !g_driver->write_memory( remote_reloc_address, &new_value, sizeof( new_value ) ) ) {
                            logging::print( encrypt( "Failed to write relocation at 0x%llX" ), remote_reloc_address );
                            return false;
                        }

                        reloc_count++;
                    }
                }

                block_count++;
                reloc_block = reinterpret_cast< PIMAGE_BASE_RELOCATION >(
                    reinterpret_cast< uintptr_t >( reloc_block ) + reloc_block->SizeOfBlock );
            }

            logging::print( encrypt( "Processed %d relocations in %d blocks" ), reloc_count, block_count );
            return true;
        }

        std::uint32_t get_image_size( ) const {
            return m_optional_header.SizeOfImage;
        }

        std::uint64_t get_entry_point( ) const {
            return m_optional_header.AddressOfEntryPoint;
        }

        bool is_dll( ) const {
            return ( m_nt_headers->FileHeader.Characteristics & IMAGE_FILE_DLL ) != 0;
        }

    private:
        PIMAGE_DOS_HEADER m_dos_header = nullptr;
        PIMAGE_NT_HEADERS64 m_nt_headers = nullptr;
        PIMAGE_SECTION_HEADER m_section_header = nullptr;
        IMAGE_OPTIONAL_HEADER64 m_optional_header{ };
        std::vector<uint8_t> m_target_image;

        IMAGE_SECTION_HEADER* find_section( const char* name ) {
            for ( WORD i = 0; i < m_nt_headers->FileHeader.NumberOfSections; i++ ) {
                auto& section = m_section_header[ i ];

                if ( memcmp( section.Name, name, strlen( name ) ) == 0 ) {
                    logging::print( encrypt( "Found section %s at index %d" ), name, i );
                    logging::print( encrypt( "Section details - VA: 0x%x, Size: 0x%x" ),
                        section.VirtualAddress,
                        section.SizeOfRawData );
                    return &m_section_header[ i ];
                }
            }

            logging::print( encrypt( "Section %s not found" ), name );
            return nullptr;
        }

        template <typename result, typename type>
        result rva_to_va( type rva ) {
            auto first_section = m_section_header;
            for ( auto section = first_section; section < first_section + m_nt_headers->FileHeader.NumberOfSections; section++ ) {
                if ( rva >= section->VirtualAddress && rva < section->VirtualAddress + section->Misc.VirtualSize )
                    return reinterpret_cast< result >( m_target_image.data( ) + section->PointerToRawData + rva - section->VirtualAddress );
            }
            return reinterpret_cast< result >( nullptr );
        }

        IMAGE_DATA_DIRECTORY* get_directory( int directory_index ) {
            if ( directory_index >= IMAGE_NUMBEROF_DIRECTORY_ENTRIES )
                return nullptr;

            return &m_optional_header.DataDirectory[ directory_index ];
        }

        bool load_depedency( const char* module_path ) {
            logging::print( encrypt( "Starting payload mapping process" ) );

            auto pe_image = std::make_unique<module::c_module>( );
            if ( !pe_image->load_file( module_path ) ) {
                logging::print( encrypt( "Failed to parse pe headers." ) );
                return false;
            }

            if ( !pe_image->is_dll( ) ) {
                logging::print( encrypt( "Image is not a dll or corrupted PE Headers." ) );
                return false;
            }

            auto image_size = pe_image->get_image_size( );
            auto image_va = g_driver->allocate_virtual( image_size );
            if ( !image_va ) {
                logging::print( encrypt( "Failed to allocate virtual memory." ) );
                g_driver->free_virtual( image_va );
                return false;
            }

            auto [image_pa, page_size] = g_driver->translate_linear( image_va );

            logging::print( encrypt( "PE allocated at: 0x%llx, Size=0x%x" ), image_va, image_size );
            logging::print( encrypt( "PE Page at: 0x%llx, Size=0x%x" ), image_pa, page_size );

            if ( !pe_image->map_sections( image_va ) ) {
                logging::print( encrypt( "Failed to write sections" ) );
                g_driver->free_virtual( image_va );
                return false;
            }

            if ( !pe_image->copy_headers( image_va ) ) {
                logging::print( encrypt( "Failed to copy headers" ) );
                g_driver->free_virtual( image_va );
                return false;
            }

            if ( !pe_image->relocate( image_va ) ) {
                logging::print( encrypt( "Failed to relocate image" ) );
                g_driver->free_virtual( image_va );
                return false;
            }

            if ( !pe_image->map_imports( ) ) {
                logging::print( encrypt( "Failed to map imports" ) );
                g_driver->free_virtual( image_va );
                return false;
            }

            if ( !pe_image->verify_module_integrity( image_va ) ) {
                logging::print( encrypt( "Target image headers or code corrupted" ) );
                g_driver->free_virtual( image_va );
                return false;
            }

            auto export_rva = pe_image->get_entry_point( );
            if ( !export_rva ) {
                logging::print( encrypt( "Failed to DllMain RVA" ) );
                g_driver->free_virtual( image_va );
                return false;
            }

            logging::print( encrypt( "Module preparation completed successfully\n" ) );

            auto shellcode = std::make_unique<shellcode::c_shellcode>( image_va, export_rva );
            if ( !shellcode->setup( ) ) {
                logging::print( encrypt( "Failed to setup shellcode." ) );
                g_driver->free_virtual( image_va );
                return false;
            }

            if ( !shellcode->update( ) ) {
                logging::print( encrypt( "Failed to compile shellcode." ) );
                g_driver->free_virtual( image_va );
                return false;
            }

            if ( !shellcode->run( ) ) {
                logging::print( encrypt( "Failed to run shellcode." ) );
                g_driver->free_virtual( image_va );
                return false;
            }

            shellcode->cleanup( );
            return true;
        }
    };
}