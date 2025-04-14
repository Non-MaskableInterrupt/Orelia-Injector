namespace handler {
    nt_status_t read_memory( user::s_command_data* input_data ) {
        auto find_min = [ input_data ]( std::size_t size ) {
            return ( size < ( input_data->m_size ) ? ( size ) : ( input_data->m_size ) );
            };

        auto physical_address = mm::g_paging.translate( input_data->m_address );
        if ( !physical_address ) {
            return nt_status_t::unsuccessful;
        }

        auto final_size = find_min( mm::paging::page_4kb_size - ( physical_address & mm::paging::page_4kb_mask ) );
        auto result = mm::phys::read_direct(
            physical_address,
            input_data->m_buffer,
            final_size
        );

        return result ? nt_status_t::success : nt_status_t::unsuccessful;
    }

    nt_status_t write_memory( user::s_command_data* input_data ) {
        auto find_min = [ input_data ]( std::size_t size ) {
            return ( size < ( input_data->m_size ) ? ( size ) : ( input_data->m_size ) );
            };

        auto physical_address = mm::g_paging.translate( input_data->m_address );
        if ( !physical_address ) {
            return nt_status_t::unsuccessful;
        }

        auto final_size = find_min( mm::paging::page_4kb_size - ( physical_address & mm::paging::page_4kb_mask ) );
        auto result = mm::phys::write_region(
            physical_address,
            input_data->m_buffer,
            final_size
        );
        
        return result ? nt_status_t::success : nt_status_t::access_violation;
    }

    nt_status_t translate_linear( user::s_command_data* input_data ) {
        std::uint32_t page_size = 0;
        input_data->m_address2 = mm::g_paging.translate( input_data->m_address, &page_size );
        input_data->m_size = static_cast< std::size_t >( page_size );
        return input_data->m_address2 ? nt_status_t::success : nt_status_t::unsuccessful;
    }

    nt_status_t map_process_pte( user::s_command_data* input_data ) {
        eac::c_module eac_module;
        if ( !eac_module.setup( ) ) {
            return nt_status_t::insufficient_resources;
        }

        kapc_state_t state{};
        if ( !eac_module.attach_process( input_data->m_process, &state ) ) {
            return nt_status_t::insufficient_resources;
        }

        if ( !mm::g_paging.map_process_pte( input_data->m_address ) ) {
            eac_module.detach_process( input_data->m_process, &state );
            return nt_status_t::access_violation;
        }

        eac_module.detach_process( input_data->m_process, &state );
        return input_data->m_address ? nt_status_t::success : nt_status_t::unsuccessful;
    }

    nt_status_t protect_virtual( user::s_command_data* input_data ) {
        eac::c_module eac_module;
        if ( !eac_module.setup( ) ) {
            return nt_status_t::insufficient_resources;
        }

        eac::protect_control_t protect_control{ };
        protect_control.m_control_type = reinterpret_cast< void* >( 4 );
        protect_control.m_address = reinterpret_cast< void* >( input_data->m_address );
        protect_control.m_size = input_data->m_size;
        protect_control.m_new_access_protection = input_data->m_new_protection;
        protect_control.m_old_access_protection = 0;

        if ( eac_module.protect_memory( input_data->m_process, &protect_control ) ) {
            return nt_status_t::insufficient_resources;
        }

        input_data->m_new_protection = protect_control.m_old_access_protection;
        return input_data->m_address ? nt_status_t::success : nt_status_t::unsuccessful;
    }

    nt_status_t allocate_virtual( user::s_command_data* input_data ) {
        eac::c_module eac_module;
        if ( !eac_module.setup( ) ) {
            return nt_status_t::insufficient_resources;
        }

        kapc_state_t state{};
        if ( !eac_module.attach_process( input_data->m_process, &state ) ) {
            return nt_status_t::insufficient_resources;
        }

        if ( eac_module.allocate_memory( input_data->m_process, &input_data->m_address, &input_data->m_size, 0x00001000 | 0x00002000, 0x40 ) ) {
            eac_module.detach_process( input_data->m_process, &state );
            return nt_status_t::insufficient_resources;
        }

        if ( !eac_module.zero_memory( input_data->m_pid, reinterpret_cast< void* >( input_data->m_address ), input_data->m_size ) ) {
            eac_module.detach_process( input_data->m_process, &state );
            return nt_status_t::insufficient_resources;
        }

        if ( !mm::g_paging.map_process_pte( input_data->m_address ) ) {
            eac_module.detach_process( input_data->m_process, &state );
            return nt_status_t::access_violation;
        }

        eac_module.detach_process( input_data->m_process, &state );
        return input_data->m_address ? nt_status_t::success : nt_status_t::unsuccessful;
    }

    nt_status_t free_virtual( user::s_command_data* input_data ) {
        eac::c_module eac_module;
        if ( !eac_module.setup( ) ) {
            return nt_status_t::insufficient_resources;
        }

        if ( eac_module.free_memory( input_data->m_process, reinterpret_cast< void* >( input_data->m_address ), input_data->m_size ) ) {
            return nt_status_t::unsuccessful;
        }
        
        return nt_status_t::success;
    }

    nt_status_t map_physical( user::s_command_data* input_data ) {
        if ( !input_data->m_address || !input_data->m_size ) {
            return nt_status_t::invalid_parameter;
        }

        auto aligned_phys = mm::g_paging.page_align( input_data->m_address );
        auto page_offset = input_data->m_address - aligned_phys;

        auto total_size = mm::g_paging.page_align( input_data->m_size + page_offset );
        if ( total_size == 0 ) {
            total_size = mm::paging::page_4kb_size;
        }

        auto mapped_va = reinterpret_cast< std::uintptr_t >(
            nt::mm_map_io_space( aligned_phys, total_size )
            );

        if ( !mapped_va ) {
            return nt_status_t::unsuccessful;
        }

        input_data->m_address2 = mapped_va + page_offset;
        input_data->m_size = total_size;

        return nt_status_t::success;
    }
}