namespace dispatch {
	void unload_driver( driver_object_t* driver_object ) {
		nt::io_delete_symbolic_link( &device::dos_device_name );

		if ( driver_object->device_object ) {
			nt::io_delete_device( driver_object->device_object );
		}
	}

	nt_status_t create_secure_device(
		driver_object_t* driver_object,
		unicode_string_t* device_name,
		device_type_t device_type,
		device_object_t** device_object
	) {
		auto status = nt::io_create_device(
			driver_object,
			0,
			device_name,
			device_type,
			file_device_secure_open,
			false,
			device_object
		);

		if ( status != nt_status_t::success ) {
			nt::dbg_print( "[orelia] failed to create device with %x\n" , status );
			return status;
		}

		//unicode_string_t sddl;
		//nt::rtl_init_unicode_string( &sddl, L"D:P(A;;GA;;;SY)(A;;GA;;;BA)" );

		//security_descriptor_t* sd = nullptr;
		//status = nt::rtl_convert_string_to_security_descriptor(
		//	&sddl,
		//	1,
		//	&sd,
		//	nullptr
		//);

		//if ( status == nt_status_t::success ) {
		//	( *device_object )->security_descriptor = sd;
		//}
		//else {
		//	nt::dbg_print( "[orelia] failed to create security descriptor with %x\n", status );
		//	nt::io_delete_device( *device_object );
		//}

		return status;
	}

	nt_status_t intialize_driver( driver_object_t* driver_object , unicode_string_t* device_path ) {
		nt::io_delete_symbolic_link( &device::dos_device_name );

		device_object_t* device_object;
		auto result = create_secure_device(
			driver_object,
			&device::device_name,
			device_type_t::network,
			&device_object
		);
		if ( result ) {
			nt::dbg_print( "[orelia] failed to create device\n" );
			return result;
		}

		result = nt::io_create_symbolic_link( 
			&device::dos_device_name,
			&device::device_name
		);
		if ( result ) {
			nt::dbg_print( "[orelia] failed to create symbolic link with %x\n" , result );
			nt::io_delete_device( driver_object->device_object );
			return result;
		}

		for ( auto i = 0; i < irp_mj_maximum_function; i++ )
			driver_object->major_function[ i ] = &controller::dispatch;

		device_object->flags |= do_buffered_io;

		driver_object->major_function[ irp_mj_close ] = &controller::dispatch;
		driver_object->major_function[ irp_mj_create ] = &controller::dispatch;
		driver_object->major_function[ irp_mj_cleanup ] = &controller::dispatch;
		driver_object->major_function[ irp_mj_device_control ] = &controller::dispatch;
		driver_object->driver_unload = &unload_driver;

		device_object->flags &= ~do_device_initializing;
		return nt_status_t::success;
	}
}