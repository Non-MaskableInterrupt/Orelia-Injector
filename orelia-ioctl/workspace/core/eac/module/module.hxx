#pragma once

namespace eac {
    class c_module {
    public:
        bool setup( ) {
            m_eac_base = get_module_base( L"EasyAntiCheat_EOS.sys" );
            if ( !m_eac_base )
                return false;

            m_protect_memory = find_pattern(
                "\x48\x89\x5C\x24\x00\x55\x56\x57\x48\x83\xEC\x00\x33\xFF\x48\x8B\xF2\x48\x8B\xE9\x48\x85\xC9\x0F\x84\x00\x00\x00\x00\x48\x85\xD2\x0F\x84\x00\x00\x00\x00\x48\x8B\x52\x00\x48\x85\xD2\x0F\x84\x00\x00\x00\x00\x4C\x8B\x46",
                "xxxx?xxxxxx?xxxxxxxxxxxxx????xxxxx????xxx?xxxxx????xxx" );
            if ( !m_protect_memory )
                return false;

            m_allocate_memory = find_pattern(
                "\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x00\x48\x8D\x0D\x00\x00\x00\x00\x49\x8B\xD9\x49\x8B\xF8",
                "xxxx?xxxx?xxxx?xxx????xxxxxx" );
            if ( !m_allocate_memory )
                return false;

            m_zero_memory = find_pattern(
                "\x48\x8B\xC4\x48\x89\x58\x00\x48\x89\x70\x00\x48\x89\x78\x00\x41\x56\x48\x83\xEC\x00\x49\x8B\xD8",
                "xxxxxx?xxx?xxx?xxxxx?xxx" );

            if ( !m_zero_memory )
                return false;

            m_free_memory = find_pattern(
                "\x48\x89\x5C\x24\x00\x55\x56\x57\x48\x83\xEC\x00\x33\xFF\x48\x8B\xF2\x48\x8B\xE9\x48\x85\xC9\x0F\x84\x00\x00\x00\x00\x48\x85\xD2\x0F\x84\x00\x00\x00\x00\x48\x8B\x52\x00\x48\x85\xD2\x0F\x84\x00\x00\x00\x00\x8B\x46",
                "xxxx?xxxxxx?xxxxxxxxxxxxx????xxxxx????xxx?xxxxx????xx" );

            if ( !m_free_memory )
                return false;

            m_attach_process = find_pattern(
                "\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x00\x33\xDB\x48\x8B\xF2\x48\x8B\xF9\x48\x85\xC9\x0F\x84\x00\x00\x00\x00\x48\x85\xD2\x0F\x84\x00\x00\x00\x00\x48\x8D\x0D",
                "xxxx?xxxx?xxxx?xxxx?xxxxxxxxxxxxx????xxxxx????xxx" );

            if ( !m_attach_process )
                return false;

            if ( !m_detach_process ) {
                m_detach_process = find_pattern( "\xE8\x00\x00\x00\x00\x48\x8B\x9C\x24\x00\x00\x00\x00\x40\x8A\xC7", "x????xxxx????xxx" );

                if ( m_detach_process ) {
                    m_detach_process += *( int* )( m_detach_process + 1 ) + 5;

                    if ( *( char* )m_detach_process != 0x51 ) {
                        m_detach_process = NULL;
                    }
                }
            }

            if ( !m_detach_process )
                return false;

            return true;
        }

        char protect_memory( eprocess_t* process, protect_control_t* control ) const {
            if ( !m_protect_memory )
                return 0;

            return ( ( char( __fastcall* )( eprocess_t*, protect_control_t* ) )m_protect_memory )( process, control );
        }

        char attach_process( eprocess_t* process, kapc_state_t* state ) const {
            if ( !m_attach_process )
                return 0;

            return ( ( char( __fastcall* )( eprocess_t*, kapc_state_t* ) )m_attach_process )( process, state );
        }

        void detach_process( eprocess_t* process, kapc_state_t* state ) const {
            if ( !m_detach_process )
                return;

            ( ( void( __fastcall* )( eprocess_t*, kapc_state_t* ) )m_detach_process )( process, state );
        }

        nt_status_t allocate_memory( eprocess_t* process, void* baseAddress, std::size_t* size, std::uint32_t allocationType, std::uint32_t protect ) const {
            if ( !m_allocate_memory )
                return nt_status_t::insufficient_resources;

            return ( ( nt_status_t( __fastcall* )( __int64, void*, void*, std::size_t, std::size_t*, std::uint32_t, std::uint32_t ) )m_allocate_memory )(
                NULL, ( void* )0xFFFFFFFFFFFFFFFFi64, baseAddress, 0, size, allocationType, protect );
        }

        char zero_memory( std::uint32_t process_id, void* address, unsigned __int64 size ) const {
            if ( !m_zero_memory )
                return 0;

            return ( ( char( __fastcall* )( void*, void*, unsigned __int64 ) )m_zero_memory )( reinterpret_cast< void* >( process_id ), address, size );
        }

        char free_memory( eprocess_t* process, void* base_address, std::size_t size ) const {
            if ( !m_free_memory )
                return 0;

            free_control_t control_struct;
            control_struct.m_control_type = ( void* )( 5 );
            control_struct.m_address = base_address;
            control_struct.m_region_size = size;
            control_struct.m_free_type = 0x00008000;

            return ( ( char( __fastcall* )( eprocess_t*, free_control_t* ) )m_free_memory )( process, &control_struct );
        }

        std::uintptr_t find_pattern( const char* pattern, const char* mask ) const {
            auto dos_header{ reinterpret_cast< dos_header_t* > ( m_eac_base ) };
            auto nt_headers{ reinterpret_cast< nt_headers_t* > ( m_eac_base + dos_header->m_lfanew ) };
            if ( !dos_header->is_valid( )
                || !nt_headers->is_valid( ) )
                return false;

            auto size_of_image = nt_headers->m_size_of_image;
            return nt::g_resolver.find_pattern( m_eac_base, size_of_image, pattern, mask );
        }

        std::uint64_t get_module_base( const wchar_t* module_name ) {
            unicode_string_t module_name_string{ };
            nt::rtl_init_unicode_string( &module_name_string, module_name );

            auto ps_loaded_module_list = reinterpret_cast< list_entry_t* >(
                nt::g_resolver.get_system_routine( "PsLoadedModuleList" )
                );
            if ( !ps_loaded_module_list )
                return false;

            auto iter_ldr_entry = reinterpret_cast< kldr_data_table_entry_t* >(
                ps_loaded_module_list->m_flink
                );

            while ( reinterpret_cast< list_entry_t* >( iter_ldr_entry ) != ps_loaded_module_list ) {
                if ( !nt::rtl_compare_unicode_string( &iter_ldr_entry->m_base_dll_name, &module_name_string, true ) )
                    return reinterpret_cast< std::uintptr_t >( iter_ldr_entry->m_dll_base );

                iter_ldr_entry = reinterpret_cast< kldr_data_table_entry_t* >(
                    iter_ldr_entry->m_in_load_order_links.m_flink
                    );
            }

            return 0;
        }

    private:
        std::uintptr_t m_eac_base = 0;
        std::uintptr_t m_protect_memory = 0;
        std::uintptr_t m_allocate_memory = 0;
        std::uintptr_t m_zero_memory = 0;
        std::uintptr_t m_free_memory = 0;

        std::uintptr_t m_attach_process = 0;
        std::uintptr_t m_detach_process = 0;
    };
}