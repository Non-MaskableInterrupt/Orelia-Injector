namespace pe
{
    std::uintptr_t g_module_base = 0;

    bool next_exec_section( std::uint64_t* exec_base, std::uint64_t* exec_size ) {
        auto dos_header = *( dos_header_t* )( g_module_base );
        if ( !dos_header.is_valid( ) )
            return false;

        auto nt_headers = *( nt_headers_t* )( g_module_base + dos_header.m_lfanew );
        if ( !nt_headers.is_valid( ) )
            return false;

        auto section_headers_offset = g_module_base + dos_header.m_lfanew +
            offsetof( nt_headers_t, m_magic );
        section_headers_offset += nt_headers.m_size_of_optional_header;

        for ( int i = 0; i < nt_headers.m_number_of_sections; i++ ) {
            auto section = *( section_header_t* )(
                section_headers_offset + i * sizeof( section_header_t ) );

            if ( section.m_characteristics & IMAGE_SCN_MEM_EXECUTE ) {
                *exec_base = g_module_base + section.m_virtual_address;
                *exec_size = section.m_size_of_raw_data;
                return true;
            }
        }

        return false;
    }

    std::uintptr_t search_exec_section( const char* pat, const char* msk ) {
        std::uint64_t exec_base;
        std::uint64_t exec_size;
        if ( !next_exec_section( &exec_base, &exec_size ) )
            return 0;

        const auto pattern_length = strlen( msk );
        const auto buffer_size = 4096;

        for ( auto offset = 0ull; offset < exec_size - pattern_length; offset += buffer_size - pattern_length ) {
            auto bytes_to_read = buffer_size;
            if ( offset + bytes_to_read > exec_size ) {
                bytes_to_read = exec_size - offset;
            }

            sdk::vector<char> buffer;
            buffer.resize( bytes_to_read );
            for ( size_t i = 0; i < bytes_to_read; i++ ) {
                buffer[ i ] = *( char* )( exec_base + offset + i );
            }

            for ( size_t i = 0; i < bytes_to_read - pattern_length; i++ ) {
                bool match = true;

                for ( size_t j = 0; j < pattern_length; j++ ) {
                    if ( msk[ j ] == 'x' && buffer[ i + j ] != pat[ j ] ) {
                        match = false;
                        break;
                    }
                }

                if ( match ) {
                    return exec_base + offset + i;
                }
            }
        }

        return 0;
    }

    std::uintptr_t find_jmp_rbx( ) {
        return search_exec_section( encrypt( "\xFF\xE3" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_jmp_rax( ) {
        return search_exec_section( encrypt( "\xFF\xE0" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_jmp_rcx( ) {
        return search_exec_section( encrypt( "\xFF\xE1" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_jmp_rdx( ) {
        return search_exec_section( encrypt( "\xFF\xE2" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_jmp_rsi( ) {
        return search_exec_section( encrypt( "\xFF\xE6" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_jmp_rdi( ) {
        return search_exec_section( encrypt( "\xFF\xE7" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_call_rbx( ) {
        return search_exec_section( encrypt( "\xFF\xD3" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_call_rax( ) {
        return search_exec_section( encrypt( "\xFF\xD0" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_mov_rbx_rcx( ) {
        return search_exec_section( encrypt( "\x48\x89\xCB\xC3" ), "xxxx" );
    }

    std::uintptr_t find_xor_rax_rax_jmp_rbx( ) {
        return search_exec_section( encrypt( "\x48\x31\xC0\xFF\xE3" ), encrypt( "xxxxx" ) );
    }

    std::uintptr_t find_push_rbx_ret( ) {
        return search_exec_section( encrypt( "\x53\xC3" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_push_rax_ret( ) {
        return search_exec_section( encrypt( "\x50\xC3" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_mov_rax_rbx_jmp_rax( ) {
        return search_exec_section( encrypt( "\x48\x8B\xC3\xFF\xE0" ), encrypt( "xxxxx" ) );
    }
}