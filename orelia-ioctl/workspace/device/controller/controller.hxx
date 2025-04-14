namespace controller {
    iop_irp_stack_profiler_t* io_get_current_irp_stack_location( iop_irp_t* irp ) {
        if ( !irp )
            return nullptr;

        if ( irp->m_current_location >
            irp->m_stack_count + 1 ) {
            nt::dbg_print( "[orelia] Invalid IRP current location\n" );
            return nullptr;
        }

        return irp->m_tail.m_overlay.m_list_stack_struct.m_stack_location_union.m_current_stack_location;
    }

    void cancel_irp( iop_irp_t* irp ) {
        std::uint8_t irql = static_cast< std::uint8_t >( irp->m_cancel_irql );
        nt::io_release_cancel_spin_lock( irql );
        nt::io_set_irp_info( irp, nt_status_t::cancelled, 0 );
        nt::iof_complete_request( irp, 0 );
    }

    nt_status_t dispatch( device_object_t* device_object, iop_irp_t* irp ) {
        if ( !device_object || !irp ) {
            nt::dbg_print( "[orelia] Invalid parameters to dispatch\n" );
            return nt_status_t::invalid_parameter;
        }

        auto stack = io_get_current_irp_stack_location( irp );
        if ( !stack ) {
            nt::dbg_print( "[orelia] Failed to process IRP\n" );
            nt::io_set_irp_info( irp, nt_status_t::invalid_parameter, 0 );
            nt::iof_complete_request( irp, 0 );
            return nt_status_t::invalid_parameter;
        }

        switch ( stack->m_major_function ) {
        case irp_mj_create:
            nt::io_set_irp_info( irp, nt_status_t::success, 0 );
            nt::iof_complete_request( irp, 0 );
            return nt_status_t::success;

        case irp_mj_close:
            nt::io_set_irp_info( irp, nt_status_t::success, 0 );
            nt::iof_complete_request( irp, 0 );
            return nt_status_t::success;

        case irp_mj_cleanup: {
            auto file_obj = stack->m_file_object;
            nt::io_cancel_irps_for_file_object( file_obj, nt_status_t::cancelled );

            mm::g_paging.cleanup( );
            nt::io_set_irp_info( irp, nt_status_t::success, 0 );
            nt::iof_complete_request( irp, 0 );
            return nt_status_t::success;
        }

        case irp_mj_device_control: {
            std::uint8_t irql = 0;
            nt::io_acquire_cancel_spin_lock( &irql );
            nt::io_set_cancel_routine( irp, cancel_irp );
            nt::io_release_cancel_spin_lock( irql );

            auto device_io_control = &stack->m_parameters.m_device_io_control;
            auto input_length = device_io_control->m_input_buffer_length;
            auto output_length = device_io_control->m_output_buffer_length;

            auto input_buffer = reinterpret_cast< user::c_command* >(
                irp->m_associated_irp.m_system_buffer
                );
            auto input_type = input_buffer->get_type( );
            auto input_data = input_buffer->get_data( );

            auto status = nt_status_t::invalid_parameter;

            switch ( input_type ) {
            case user::e_command_type::read_memory: {
                status = handler::read_memory( &input_data );
            } break;

            case user::e_command_type::write_memory: {
                status = handler::write_memory( &input_data );
            } break;

            case user::e_command_type::open_process: {
                status = handler::open_process( &input_data );
            } break;

            case user::e_command_type::attach_process: {
                status = handler::attach_process( &input_data );
            } break;

            case user::e_command_type::detach_process: {
                status = handler::detach_process( &input_data );
            } break;

            case user::e_command_type::get_eprocess: {
                status = handler::get_eprocess( &input_data );
            } break;

            case user::e_command_type::get_process_peb: {
                status = handler::get_peb( &input_data );
            } break;

            case user::e_command_type::get_base_address: {
                status = handler::get_base_address( &input_data );
            } break;

            case user::e_command_type::translate_linear: {
                status = handler::translate_linear( &input_data );
            } break;

            case user::e_command_type::protect_virtual: {
                status = handler::protect_virtual( &input_data );
            } break;

            case user::e_command_type::allocate_virtual: {
                status = handler::allocate_virtual( &input_data );
            } break;

            case user::e_command_type::free_virtual: {
                status = handler::free_virtual( &input_data );
            } break;

            case user::e_command_type::get_dtb: {
                status = handler::get_dtb( &input_data );
            } break;

            case user::e_command_type::map_process_pte: {
                status = handler::map_process_pte( &input_data );
            } break;

            default: {
                nt::dbg_print( "[orelia] Invalid ioctl code\n" );
                status = nt_status_t::invalid_parameter;
            } break;
            }

            user::c_command output_command( input_type, input_data, true );
            output_command.set_status( status );

            *reinterpret_cast< user::c_command* >( irp->m_associated_irp.m_system_buffer ) = output_command;

            nt::io_acquire_cancel_spin_lock( &irql );
            nt::io_set_cancel_routine( irp, nullptr );
            nt::io_release_cancel_spin_lock( irql );

            nt::io_set_irp_info( irp, status, sizeof( output_command ) );
            nt::iof_complete_request( irp, 0 );
            return status;
        }

        default:
            nt::dbg_print( "[orelia] Unsupported major function: %d\n", stack->m_major_function );
            nt::io_set_irp_info( irp, nt_status_t::not_supported, 0 );
            nt::iof_complete_request( irp, 0 );
            return nt_status_t::not_supported;
        }
    }
}