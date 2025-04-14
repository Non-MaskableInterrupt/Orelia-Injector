
namespace nt {
    class c_resolver {
    public:
        template<typename type>
        type get( std::uint64_t offset ) const {
            return *reinterpret_cast< type* >( this->m_nt_base + offset );
        }

        bool setup( ) {
            this->m_nt_base = reinterpret_cast< std::addr_t >(
                get_nt_base( )
                );

            if ( !this->m_nt_base )
                return false;
            return true;
        }

        std::uintptr_t find_pattern( std::uintptr_t base, std::size_t size, const char* pattern, const char* mask ) const {
            const auto pattern_length = crt::strlen( mask );

            if ( pattern_length == 0 || size < pattern_length )
                return 0;

            for ( std::size_t i = 0; i <= size - pattern_length; ++i ) {
                bool found = true;

                for ( std::size_t j = 0; j < pattern_length; ++j ) {
                    if ( mask[ j ] == 'x' &&
                        *reinterpret_cast< const unsigned char* >( base + i + j ) != static_cast< unsigned char >( pattern[ j ] ) ) {
                        found = false;
                        break;
                    }
                }

                if ( found )
                    return base + i;
            }

            return 0;
        }

        std::uintptr_t find_signature( std::uintptr_t base, std::size_t size, const std::uint8_t* signature, const char* mask ) const {
            const auto sig_length = crt::strlen( mask );

            if ( sig_length == 0 || size < sig_length )
                return 0;

            for ( std::size_t i = 0; i <= size - sig_length; ++i ) {
                bool found = true;

                for ( std::size_t j = 0; j < sig_length; ++j ) {
                    if ( mask[ j ] == 'x' &&
                        *reinterpret_cast< const std::uint8_t* >( base + i + j ) != signature[ j ] ) {
                        found = false;
                        break;
                    }
                }

                if ( found )
                    return base + i;
            }

            return 0;
        }

        std::uintptr_t find_ida_pattern( std::uintptr_t base, std::size_t size, const char* ida_pattern ) const {
            std::uint8_t pattern[ 256 ];
            char mask[ 256 ];
            std::size_t pattern_size = 0;

            const char* ptr = ida_pattern;
            while ( *ptr ) {
                if ( *ptr == ' ' ) {
                    ptr++;
                    continue;
                }

                if ( *ptr == '?' ) {
                    mask[ pattern_size ] = '?';
                    pattern[ pattern_size++ ] = 0;
                    ptr++;

                    if ( *ptr == '?' ) ptr++;
                }
                else {
                    char byte_str[ 3 ] = { ptr[ 0 ], ptr[ 1 ], 0 };
                    pattern[ pattern_size ] = static_cast< std::uint8_t >( crt::strtoul( byte_str, nullptr, 16 ) );
                    mask[ pattern_size++ ] = 'x';
                    ptr += 2;
                }

                if ( *ptr == ' ' ) ptr++;
            }

            mask[ pattern_size ] = 0;

            for ( std::size_t i = 0; i < pattern_size; i++ ) {
                if ( mask[ i ] == '?' ) mask[ i ] = '?';
                else mask[ i ] = 'x';
            }

            return find_signature( base, size, pattern, mask );
        }

        std::uintptr_t scan_text_section( const char* pattern, const char* mask ) const {
            std::uint64_t text_base = 0;
            std::uint64_t text_size = 0;

            if ( !next_exec_section( &text_base, &text_size ) )
                return 0;

            return find_pattern( text_base, text_size, pattern, mask );
        }

        std::uintptr_t scan_ida_pattern( const char* ida_pattern ) const {
            std::uint64_t text_base = 0;
            std::uint64_t text_size = 0;

            if ( !next_exec_section( &text_base, &text_size ) )
                return 0;

            return find_ida_pattern( text_base, text_size, ida_pattern );
        }

        std::uintptr_t find_reference( std::uintptr_t base, std::size_t size, std::uintptr_t target, std::size_t max_refs = 1 ) const {
            std::uintptr_t result[ 256 ];
            std::size_t found = 0;

            for ( std::size_t i = 0; i < size - 7; i++ ) {
                if ( *reinterpret_cast< const std::uint8_t* >( base + i ) == 0x48 &&
                    *reinterpret_cast< const std::uint8_t* >( base + i + 1 ) == 0x8B &&
                    *reinterpret_cast< const std::uint8_t* >( base + i + 2 ) == 0x05 ) {

                    std::int32_t offset = *reinterpret_cast< const std::int32_t* >( base + i + 3 );
                    std::uintptr_t address = base + i + 7 + offset;

                    if ( address == target ) {
                        result[ found++ ] = base + i;
                        if ( found >= max_refs )
                            break;
                    }
                }

                if ( *reinterpret_cast< const std::uint8_t* >( base + i ) == 0x48 &&
                    *reinterpret_cast< const std::uint8_t* >( base + i + 1 ) == 0x8D &&
                    *reinterpret_cast< const std::uint8_t* >( base + i + 2 ) == 0x05 ) {

                    std::int32_t offset = *reinterpret_cast< const std::int32_t* >( base + i + 3 );
                    std::uintptr_t address = base + i + 7 + offset;

                    if ( address == target ) {
                        result[ found++ ] = base + i;
                        if ( found >= max_refs )
                            break;
                    }
                }
            }

            return found > 0 ? result[ 0 ] : 0;
        }

        std::uintptr_t find_global_variable( const char* ida_pattern, std::size_t offset_to_rip = 3 ) const {
            std::uint64_t text_base = 0;
            std::uint64_t text_size = 0;

            if ( !next_exec_section( &text_base, &text_size ) )
                return 0;

            std::uintptr_t pattern_addr = find_ida_pattern( text_base, text_size, ida_pattern );
            if ( !pattern_addr )
                return 0;

            std::int32_t rip_offset = *reinterpret_cast< std::int32_t* >( pattern_addr + offset_to_rip );
            return pattern_addr + offset_to_rip + 4 + rip_offset;
        }

        bool next_exec_section( std::uint64_t* exec_base, std::uint64_t* exec_size ) const {
            auto dos_header{ reinterpret_cast< dos_header_t* > ( m_nt_base ) };
            auto nt_headers{ reinterpret_cast< nt_headers_t* > ( m_nt_base + dos_header->m_lfanew ) };
            if ( !dos_header->is_valid( )
                || !nt_headers->is_valid( ) )
                return false;

            auto section_header = reinterpret_cast< section_header_t* >(
                reinterpret_cast< std::uintptr_t >( nt_headers ) +
                nt_headers->m_size_of_optional_header + 0x18 );

            for ( int i = 0; i < nt_headers->m_number_of_sections; i++ ) {
                auto current_section_base = reinterpret_cast< std::uint64_t >( dos_header ) + section_header[ i ].m_virtual_address;
                if ( section_header[ i ].m_characteristics & 0x20000000 /*IMAGE_SCN_MEM_EXECUTE*/ ) {
                    *exec_base = current_section_base;
                    *exec_size = section_header[ i ].m_size_of_raw_data;
                    break;
                }
            }

            return *exec_base && *exec_size;
        }

        std::addr_t get_system_routine( const char* export_name ) const {
            auto dos_header{ reinterpret_cast< dos_header_t* > ( m_nt_base ) };
            auto nt_headers{ reinterpret_cast< nt_headers_t* > ( m_nt_base + dos_header->m_lfanew ) };
            if ( !dos_header->is_valid( )
                || !nt_headers->is_valid( ) )
                return {};

            auto exp_dir{ nt_headers->m_export_table.as_rva< export_directory_t* >( m_nt_base ) };
            if ( !exp_dir->m_address_of_functions
                || !exp_dir->m_address_of_names
                || !exp_dir->m_address_of_names_ordinals )
                return {};

            auto name{ reinterpret_cast< std::int32_t* > ( m_nt_base + exp_dir->m_address_of_names ) };
            auto func{ reinterpret_cast< std::int32_t* > ( m_nt_base + exp_dir->m_address_of_functions ) };
            auto ords{ reinterpret_cast< std::int16_t* > ( m_nt_base + exp_dir->m_address_of_names_ordinals ) };

            for ( std::int32_t i{}; i < exp_dir->m_number_of_names; i++ ) {
                auto cur_name{ m_nt_base + name[ i ] };
                auto cur_func{ m_nt_base + func[ ords[ i ] ] };
                if ( !cur_name
                    || !cur_func )
                    continue;

                if ( crt::strcmp( export_name, reinterpret_cast< char* > ( cur_name ) ) == 0 )
                    return reinterpret_cast< std::addr_t > ( cur_func );
            }
            return {};
        }

    private:
        std::addr_t m_nt_base = 0;

        bool check( std::uintptr_t ptr, const char* pat, const char* msk ) {
            for ( ; *msk; ++ptr, ++pat, ++msk )
                if ( *msk == 'x' && *reinterpret_cast< const char* >( ptr ) != *pat )
                    return false;
            return true;
        }
    };
}