namespace gadgets {
    bool next_exec_section( const char* image_name, std::uint64_t* exec_base, std::uint64_t* exec_size ) {
        char file_path[ 64 ]{};
        auto image_base = modules::get_image( image_name, file_path );
        if ( !image_base ) {
            logging::print( encrypt( "Failed to get image base for %s" ), image_name );
            return false;
        }

        auto dos_header = g_kernel->read_va<dos_header_t>( image_base );
        if ( !dos_header.is_valid( ) ) {
            logging::print( encrypt( "Invalid DOS header" ) );
            return false;
        }

        auto nt_headers = g_kernel->read_va<nt_headers_t>( image_base + dos_header.m_lfanew );
        if ( !nt_headers.is_valid( ) ) {
            logging::print( encrypt( "Invalid NT headers" ) );
            return false;
        }

        auto section_headers_offset = image_base + dos_header.m_lfanew +
            offsetof( nt_headers_t, m_magic );
        section_headers_offset += nt_headers.m_size_of_optional_header;

        for ( int i = 0; i < nt_headers.m_number_of_sections; i++ ) {
            auto section = g_kernel->read_va<section_header_t>(
                section_headers_offset + i * sizeof( section_header_t ) );

            if ( section.m_characteristics & IMAGE_SCN_MEM_EXECUTE ) {
                *exec_base = image_base + section.m_virtual_address;
                *exec_size = section.m_size_of_raw_data;
                return true;
            }
        }

        logging::print( encrypt( "No executable section found" ) );
        return false;
    }

    std::uintptr_t search_exec_section( const char* image_name, const char* pat, const char* msk ) {
        std::uint64_t exec_base;
        std::uint64_t exec_size;
        if ( !next_exec_section( image_name, &exec_base, &exec_size ) )
            return 0;

        const auto pattern_length = strlen( msk );
        const auto buffer_size = 4096;

        for ( auto offset = 0ull; offset < exec_size - pattern_length; offset += buffer_size - pattern_length ) {
            auto bytes_to_read = buffer_size;
            if ( offset + bytes_to_read > exec_size ) {
                bytes_to_read = exec_size - offset;
            }

            std::vector<char> buffer( bytes_to_read );
            for ( size_t i = 0; i < bytes_to_read; i++ ) {
                buffer[ i ] = g_kernel->read_va<char>( exec_base + offset + i );
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
        return search_exec_section( encrypt( "ntoskrnl.exe" ), encrypt( "\xFF\xE3" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_jmp_rax( ) {
        return search_exec_section( encrypt( "ntoskrnl.exe" ), encrypt( "\xFF\xE0" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_jmp_rcx( ) {
        return search_exec_section( encrypt( "ntoskrnl.exe" ), encrypt( "\xFF\xE1" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_jmp_rdx( ) {
        return search_exec_section( encrypt( "ntoskrnl.exe" ), encrypt( "\xFF\xE2" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_jmp_rsi( ) {
        return search_exec_section( encrypt( "ntoskrnl.exe" ), encrypt( "\xFF\xE6" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_jmp_rdi( ) {
        return search_exec_section( encrypt( "ntoskrnl.exe" ), encrypt( "\xFF\xE7" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_call_rbx( ) {
        return search_exec_section( encrypt( "ntoskrnl.exe" ), encrypt( "\xFF\xD3" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_call_rax( ) {
        return search_exec_section( encrypt( "ntoskrnl.exe" ), encrypt( "\xFF\xD0" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_mov_rbx_rcx( ) {
        return search_exec_section( encrypt( "ntoskrnl.exe" ), encrypt( "\x48\x89\xCB\xC3" ), "xxxx" );
    }

    std::uintptr_t find_xor_rax_rax_jmp_rbx( ) {
        return search_exec_section( encrypt( "ntoskrnl.exe" ), encrypt( "\x48\x31\xC0\xFF\xE3" ), encrypt( "xxxxx" ) );
    }

    std::uintptr_t find_push_rbx_ret( ) {
        return search_exec_section( encrypt( "ntoskrnl.exe" ), encrypt( "\x53\xC3" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_push_rax_ret( ) {
        return search_exec_section( encrypt( "ntoskrnl.exe" ), encrypt( "\x50\xC3" ), encrypt( "xx" ) );
    }

    std::uintptr_t find_mov_rax_rbx_jmp_rax( ) {
        return search_exec_section( encrypt( "ntoskrnl.exe" ), encrypt( "\x48\x8B\xC3\xFF\xE0" ), encrypt( "xxxxx" ) );
    }

    std::uintptr_t find_mov_rax_current_process( ) {
        const char pattern[ ] = "\x65\x48\x8B\x04\x25\x88\x01\x00\x00\x48\x8B\x80\xB8\x00\x00\x00";
        const char mask[ ] = "xxxxxxxxxxxxxxxx";
        auto result = search_exec_section( encrypt( "ntoskrnl.exe" ), pattern, mask );

        if ( !result ) {
            const char alt_pattern[ ] = "\x65\x48\x8B\x04\x25\x88\x01\x00\x00\x48\x8B\x88";
            const char alt_mask[ ] = "xxxxxxxxxxxx";
            result = search_exec_section( encrypt( "ntoskrnl.exe" ), alt_pattern, alt_mask );
        }

        return result;
    }

    std::uintptr_t find_restore_execution( ) {
        const char pattern1[ ] = "\x5F\x5E\x5B\x5D\xC3";
        const char mask1[ ] = "xxxxx";
        auto result = search_exec_section( encrypt( "ntoskrnl.exe" ), pattern1, mask1 );

        if ( !result ) {
            const char pattern2[ ] = "\x5F\x5E\x5D\xC3";
            const char mask2[ ] = "xxxx";
            result = search_exec_section( encrypt( "ntoskrnl.exe" ), pattern2, mask2 );
        }

        if ( !result ) {
            const char pattern3[ ] = "\xC3";
            const char mask3[ ] = "x";
            result = search_exec_section( encrypt( "ntoskrnl.exe" ), pattern3, mask3 );
        }

        return result;
    }
}