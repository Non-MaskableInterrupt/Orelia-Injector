namespace kernel {
    class c_kernel {
        void* m_original_fn = nullptr;
        void* m_mapped_code = nullptr;
        std::uint8_t m_original_code[ 25 ]{ };
        std::uint64_t m_dtb = 0;

    public:
        bool install_hook( ) {
            logging::print( encrypt( "Initializing kernel execution" ) );

            m_dtb = get_dtb( );
            if ( !m_dtb )
                return false;

            logging::print( encrypt( "System DTB: 0x%llx" ), m_dtb );

            auto ke_query_auxiliary_counter_frequency = modules::get_export(
                encrypt( "ntoskrnl.exe" ),
                encrypt( "KeQueryAuxiliaryCounterFrequency" )
            );

            if ( !ke_query_auxiliary_counter_frequency )
                return false;

            auto kd_enumerate_debugging_devices = modules::rva(
                encrypt( "ntoskrnl.exe" ),
                ke_query_auxiliary_counter_frequency + 4,
                7
            );

            if ( !kd_enumerate_debugging_devices )
                return false;

            auto hook_phys = translate_linear( reinterpret_cast< std::uintptr_t >( kd_enumerate_debugging_devices ), 0 );
            if ( !hook_phys )
                return false;

            logging::print( encrypt( "Execution address: VA=0x%llx, PA=0x%llx" ), kd_enumerate_debugging_devices, hook_phys );

            if ( !g_driver->read_physical_memory( hook_phys, &this->m_original_fn, 8 ) )
                return false;

            logging::print( encrypt( "Original PA: 0x%llx" ), this->m_original_fn );

            auto code_ptr = modules::get_image( encrypt( "Beep.sys" ) ) + 0x1290;
            if ( !g_driver->map_physical_memory( translate_linear( code_ptr, 0 ), 25, &this->m_mapped_code ) )
                return false;

            logging::print( encrypt( "Found executable code at: 0x%llx" ), this->m_mapped_code );

            if ( !g_driver->write_physical_memory( hook_phys, &code_ptr, 8 ) ) {
                logging::print( encrypt( "Failed to write executable code" ) );
                return false;
            }

            logging::print( encrypt( "Successfully installed execution\n" ) );
            return true;
        }

        bool remove_hook( ) {
            auto ke_query_auxiliary_counter_frequency = modules::get_export(
                encrypt( "ntoskrnl.exe" ),
                encrypt( "KeQueryAuxiliaryCounterFrequency" )
            );

            if ( !ke_query_auxiliary_counter_frequency )
                return false;

            auto hook_ptr = modules::rva(
                encrypt( "ntoskrnl.exe" ),
                ke_query_auxiliary_counter_frequency + 4,
                7
            );

            if ( !hook_ptr )
                return false;

            write_va( hook_ptr, &m_original_fn );
            driver::syscall( 42, ( HANDLE )-1, this->m_mapped_code ); // NtUnmapViewOfSection
            return true;
        }

        template<typename ret_t = std::uint64_t, typename fn_t, typename a1_t = void*, typename a2_t = void*, typename a3_t = void*, typename a4_t = void*>
        ret_t invoke( fn_t func_ptr, a1_t arg1 = a1_t{}, a2_t arg2 = a2_t{}, a3_t arg3 = a3_t{}, a4_t arg4 = a4_t{} ) {
            std::uint8_t callback_door[ ] = {
                0x49, 0x8B, 0x01,       //mov rax, QWORD PTR [r9]
                0x49, 0x8B, 0x49, 0x08, //mov rcx, QWORD PTR [r9+0x8]
                0x49, 0x8B, 0x51, 0x10, //mov rdx, QWORD PTR [r9+0x10]
                0x4D, 0x8B, 0x41, 0x18, //mov r8,  QWORD PTR [r9+0x18]
                0x4D, 0x8B, 0x49, 0x20, //mov r9,  QWORD PTR [r9+0x20]
                0x48, 0x83, 0xC4, 0x30, //add rsp, 0x30
                0xFF, 0xE0              //jmp rax
            }, original_code[ sizeof( callback_door ) ];

            if ( !m_mapped_code ) {
                logging::print( encrypt( "Invalid mapped code pointer" ) );
                return { };
            }

            memcpy( &original_code[ 0 ], m_mapped_code, sizeof( callback_door ) );
            memcpy( m_mapped_code, &callback_door[ 0 ], sizeof( callback_door ) );

            struct call_struct { void* func; a1_t arg1; a2_t arg2; a3_t arg3; a4_t arg4; };
            call_struct call_context{ reinterpret_cast< void* >( func_ptr ), arg1, arg2, arg3, arg4 };
            using nt_query_counter_t = void* ( __fastcall* )( void*, std::uint64_t, std::uint64_t, call_struct* );

            auto* nt_query_counter = reinterpret_cast< nt_query_counter_t >(
                GetProcAddress(
                GetModuleHandleA( "ntdll.dll" ),
                "NtQueryAuxiliaryCounterFrequency" ) );
            if ( !nt_query_counter )
                return ret_t{};

            std::uint64_t dummy;
            auto result = reinterpret_cast< ret_t >(
                nt_query_counter( &dummy, 0, 0, &call_context )
                );

            memcpy( this->m_mapped_code, &original_code[ 0 ], sizeof( callback_door ) );
            return result;
        }

        std::uint64_t translate_linear( std::uint64_t address, std::uint32_t* page_size = 0 ) const {
            pml4e pml4_entry;
            std::uint16_t pml4 = ( uint16_t )( ( address >> 39 ) & 0x1FF );
            auto result = g_driver->read_physical_memory(
                this->m_dtb + ( pml4 * sizeof( pml4_entry ) ),
                &pml4_entry,
                sizeof( pml4_entry )
            );
            if ( !result || !pml4_entry.hard.present ) {
                logging::print( encrypt( "PML4 entry invalid - Present: %d" ), pml4_entry.hard.present );
                return 0;
            }

            pdpte pdpt_entry;
            std::uint16_t pdpt = ( uint16_t )( ( address >> 30 ) & 0x1FF );
            result = g_driver->read_physical_memory(
                ( pml4_entry.hard.pfn << 12 ) + ( pdpt * sizeof( pdpt_entry ) ),
                &pdpt_entry,
                sizeof( pdpt_entry )
            );
            if ( !result || !pdpt_entry.hard.present ) {
                logging::print( encrypt( "PDPT entry invalid - Present: %d" ), pdpt_entry.hard.present );
                return 0;
            }

            if ( pdpt_entry.hard.page_size ) {
                if ( page_size ) *page_size = 0x40000000;
                return ( pdpt_entry.hard.pfn << 12 ) + ( address & 0x3FFFFFFF );
            }

            pde pd_entry;
            std::uint16_t pd = ( uint16_t )( ( address >> 21 ) & 0x1FF );
            result = g_driver->read_physical_memory(
                ( pdpt_entry.hard.pfn << 12 ) + ( pd * sizeof( pd_entry ) ),
                &pd_entry,
                sizeof( pd_entry )
            );
            if ( !result || !pd_entry.hard.present ) {
                logging::print( encrypt( "PD entry invalid - Present: %d" ), pd_entry.hard.present );
                return 0;
            }

            if ( pd_entry.hard.page_size ) {
                if ( page_size ) *page_size = 0x200000;
                return ( pd_entry.hard.pfn << 12 ) + ( address & 0x1FFFFF );
            }

            pte pt_entry;
            std::uint16_t pt = ( uint16_t )( ( address >> 12 ) & 0x1FF );
            result = g_driver->read_physical_memory(
                ( pd_entry.hard.pfn << 12 ) + ( pt * sizeof( pt_entry ) ),
                &pt_entry,
                sizeof( pt_entry )
            );
            if ( !result || !pt_entry.hard.present ) {
                logging::print( encrypt( "PT entry invalid - Present: %d" ), pt_entry.hard.present );
                return 0;
            }

            if ( page_size ) *page_size = 0x1000;
            return ( pt_entry.hard.pfn << 12 ) + ( address & 0xFFF );
        }

        std::uint64_t get_dtb( ) {
            auto buffer = reinterpret_cast< uint8_t* >( crt::malloc1( 0x10000 ) );
            for ( int i = 0; i < 10; i++ ) {
                if ( !g_driver->read_physical_memory( i * 0x10000, buffer, 0x10000 ) )
                    continue;

                for ( int offset = 0; offset < 0x10000; offset += 0x1000 ) {
                    if ( ( 0x00000001000600E9 ^ ( 0xffffffffffff00ff & *( std::uint64_t* )( buffer + offset ) ) ) ||
                        ( 0xfffff80000000000 ^ ( 0xfffff80000000000 & *( std::uint64_t* )( buffer + offset + 0x70 ) ) ) ||
                        ( 0xffffff0000000fff & *( std::uint64_t* )( buffer + offset + 0xA0 ) ) )
                        continue;

                    auto curr_dtb = *( std::uint64_t* )( buffer + offset + 0xA0 );
                    if ( !curr_dtb )
                        continue;

                    crt::free1( buffer );
                    return curr_dtb;
                }
            }

            crt::free1( buffer );
            return 0;
        }

        template<typename ret_t = void, typename addr_t>
        ret_t read_va( addr_t address ) {
            ret_t data{};
            this->kmemcpy(
                &data,
                reinterpret_cast< void* >( address ),
                sizeof( ret_t )
            );
            return data;
        }

        template<typename data_t, typename addr_t>
        void write_va( addr_t address, data_t data ) {
            this->kmemcpy(
                reinterpret_cast< void* >( address ),
                &data,
                sizeof( data_t )
            );
        }

    private:
        void kmemcpy( void* dst, void* src, std::uint64_t size ) {
            static void* memcpy = nullptr;
            if ( !memcpy ) {
                memcpy = reinterpret_cast< void* >(
                    modules::get_export(
                    encrypt( "ntoskrnl.exe" ),
                    encrypt( "memcpy" )
                    ) );
            }

            this->invoke( memcpy, dst, src, size );
        }
    };
}