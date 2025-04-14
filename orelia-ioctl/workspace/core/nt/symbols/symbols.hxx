
extern "C" __declspec( dllimport ) void* IoDriverObjectType;

extern "C" nt_status_t __stdcall ObCreateObject(
    std::uint8_t object_type,
    void* object_type_address,
    object_attributes_t* object_attributes,
    std::uint8_t access_mode,
    void* parse_context,
    std::uint32_t object_size,
    std::uint32_t page_charge,
    std::uint32_t tag,
    void** object
);

enum class memory_caching_type : std::uint32_t {
    non_cached = 0,
    cached = 1,
    write_combined = 2,
    hardware_coherent_cached = 3,
    uncached_exported = 4,
    write_back = 5,
    write_protected = 6
};

namespace nt {
    constexpr std::uint32_t context_control = 0x00010001;
    constexpr std::uint32_t context_integer = 0x00010002;
    constexpr std::uint32_t context_segments = 0x00010004;
    constexpr std::uint32_t context_floating_point = 0x00010008;
    constexpr std::uint32_t context_debug_registers = 0x00010010;
    constexpr std::uint32_t context_all = ( context_control | context_integer | context_segments |
        context_floating_point | context_debug_registers );

    __forceinline nt_status_t ps_suspend_thread(
        kthread_t* thread,
        std::uint32_t* previous_count
    ) {
        using fn_t = nt_status_t( * )(
            kthread_t* thread,
            std::uint32_t* previous_count
            );
        static const auto fn = reinterpret_cast< fn_t >( g_resolver.get_system_routine( "PsSuspendThread" ) );
        if ( !fn )
            return nt_status_t::invalid_parameter;
        return fn( thread, previous_count );
    }

    __forceinline nt_status_t ps_resume_thread(
        kthread_t* thread,
        std::uint32_t* previous_count
    ) {
        using fn_t = nt_status_t( * )(
            kthread_t* thread,
            std::uint32_t* previous_count
            );
        static const auto fn = reinterpret_cast< fn_t >( g_resolver.get_system_routine( "PsResumeThread" ) );
        if ( !fn )
            return nt_status_t::invalid_parameter;
        return fn( thread, previous_count );
    }

    __forceinline nt_status_t ke_get_context_thread(
        kthread_t* thread,
        context_t* context
    ) {
        using fn_t = nt_status_t( * )(
            kthread_t* thread,
            context_t* context
            );
        static const auto fn = reinterpret_cast< fn_t >( g_resolver.get_system_routine( "KeGetContextThread" ) );
        if ( !fn )
            return nt_status_t::invalid_parameter;
        return fn( thread, context );
    }

    __forceinline nt_status_t ke_set_context_thread(
        kthread_t* thread,
        context_t* context
    ) {
        using fn_t = nt_status_t( * )(
            kthread_t* thread,
            context_t* context
            );
        static const auto fn = reinterpret_cast< fn_t >( g_resolver.get_system_routine( "KeSetContextThread" ) );
        if ( !fn )
            return nt_status_t::invalid_parameter;
        return fn( thread, context );
    }

    __forceinline nt_status_t ke_queue_user_apc(
        kthread_t* thread,
        void* user_apc_routine,
        void* user_apc_context,
        std::uint8_t priority_increment
    ) {
        using fn_t = nt_status_t( * )(
            kthread_t* thread,
            void* user_apc_routine,
            void* user_apc_context,
            std::uint8_t priority_increment
            );
        static const auto fn = reinterpret_cast< fn_t >( g_resolver.get_system_routine( "KeQueueUserApc" ) );
        if ( !fn )
            return nt_status_t::invalid_parameter;
        return fn( thread, user_apc_routine, user_apc_context, priority_increment );
    }

    __forceinline eprocess_t* ps_get_thread_process(
        kthread_t* thread
    ) {
        using fn_t = eprocess_t * ( * )(
            kthread_t* thread
            );
        static const auto fn = reinterpret_cast< fn_t >( g_resolver.get_system_routine( "PsGetThreadProcess" ) );
        if ( !fn )
            return nullptr;
        return fn( thread );
    }

    __forceinline bool ps_is_thread_terminating(
        kthread_t* thread
    ) {
        using fn_t = bool ( * )(
            kthread_t* thread
            );
        static const auto fn = reinterpret_cast< fn_t >( g_resolver.get_system_routine( "PsIsThreadTerminating" ) );
        if ( !fn )
            return false;
        return fn( thread );
    }

    __forceinline void obf_dereference_object(
        void* object
    ) {
        using fn_t = void ( * )(
            void* object
            );
        static const auto fn = reinterpret_cast< fn_t >( g_resolver.get_system_routine( "ObfDereferenceObject" ) );
        if ( fn )
            fn( object );
    }

    __forceinline void* get_ps_thread_type( ) {
        static void* thread_type = nullptr;
        if ( !thread_type ) {
            thread_type = reinterpret_cast< void* >( g_resolver.get_system_routine( "PsThreadType" ) );
        }
        return thread_type;
    }

    void* mm_map_io_space(
        std::uint64_t physical_address,
        std::size_t size,
        memory_caching_type caching_type = memory_caching_type::non_cached
    ) {
        using fn_t = void* ( * )(
            std::uint64_t physical_address,
            std::size_t size,
            memory_caching_type caching_type
            );

        static const auto fn = reinterpret_cast< fn_t >(
            g_resolver.get_system_routine( "MmMapIoSpace" )
            );

        if ( !fn )
            return nullptr;

        return fn( physical_address, size, caching_type );
    }

    int rtl_compare_unicode_string(
        unicode_string_t* string1,
        unicode_string_t* string2,
        bool case_insensitive
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "RtlCompareUnicodeString" );
            if ( !function_address ) return 0;
        }

        using function_t = int( * )(
            unicode_string_t* string1,
            unicode_string_t* string2,
            bool case_insensitive
            );

        return reinterpret_cast< function_t >( function_address )(
            string1,
            string2,
            case_insensitive
            );
    }

    void init_object_attributes(
        object_attributes_t* object_attributes,
        unicode_string_t* object_name,
        std::uint32_t attributes,
        void* root_directory,
        security_descriptor_t* security_descriptor
    ) {
        if ( !object_attributes )
            return;

        object_attributes->m_length = sizeof( object_attributes_t );
        object_attributes->m_root_directory = root_directory;
        object_attributes->m_attributes = attributes;
        object_attributes->m_object_name = object_name;
        object_attributes->m_security_descriptor = security_descriptor;
        object_attributes->m_security_quality_of_service = nullptr;
    }

    nt_status_t zw_open_process(
        void** process_handle,
        std::uint32_t desired_access,
        object_attributes_t* object_attributes,
        client_id_t* client_id
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ZwOpenProcess" );
            if ( !function_address ) return nt_status_t::unsuccessful;
        }

        using function_t = nt_status_t( * )(
            void**,
            std::uint32_t,
            object_attributes_t*,
            client_id_t*
            );

        return reinterpret_cast< function_t >( function_address )(
            process_handle,
            desired_access,
            object_attributes,
            client_id
            );
    }

    void mm_free_non_cached_memory( void* base_address, std::size_t size ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "MmFreeNonCachedMemory" );
        }
        if ( !function_address ) return;

        using function_t = void( void*, std::size_t );
        reinterpret_cast< function_t* >( function_address )( base_address, size );
    }

    void* mm_allocate_non_cached_memory( std::size_t size ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "MmAllocateNonCachedMemory" );
        }
        if ( !function_address ) return nullptr;

        using function_t = void* ( std::size_t );
        return reinterpret_cast< function_t* >( function_address )( size );
    }

    std::uintptr_t get_physical_address( std::uintptr_t virtual_address ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "MmGetPhysicalAddress" );
        }
        if ( !function_address ) return {};

        using function_t = physical_address_t( void* virtual_address );
        return reinterpret_cast< function_t* >( function_address )(
            reinterpret_cast< void* >( virtual_address ) ).m_quad_part;
    }

    void io_cancel_irps_for_file_object( void* file_object, nt_status_t cancel_status ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "IoCancelIrpsForThread" );
            if ( !function_address ) return;
        }

        using function_t = void( * )( void*, nt_status_t );
        reinterpret_cast< function_t >( function_address )( file_object, cancel_status );
    }

    void* mm_get_virtual_for_physical( std::uintptr_t physical_address ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "MmGetVirtualForPhysical" );
            if ( !function_address ) return nullptr;
        }

        physical_address_t phys_addr{ };
        phys_addr.m_quad_part = physical_address;

        using function_t = void* ( * )( physical_address_t physical_address );
        return reinterpret_cast< function_t >( function_address )( phys_addr );
    }

    pte mi_make_valid_pte( std::uint64_t pfn, std::uint32_t protection, bool large_page ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = nullptr;
        }
        if ( !function_address ) return {};

        using function_t = pte( std::uint64_t, std::uint32_t, bool );
        return pte{ reinterpret_cast< function_t* >( function_address )( pfn, protection, large_page ) };
    }

        void io_acquire_cancel_spin_lock( std::uint8_t* cancel_irql ) {
            static std::addr_t function_address = nullptr;
            if ( !function_address ) {
                function_address = g_resolver.get_system_routine( "IoAcquireCancelSpinLock" );
                if ( !function_address ) return;
            }

            using function_t = void( * )( std::uint8_t* cancel_irql );

            reinterpret_cast< function_t >( function_address )( cancel_irql );
        }

        void io_release_cancel_spin_lock( std::uint8_t cancel_irql ) {
            static std::addr_t function_address = nullptr;
            if ( !function_address ) {
                function_address = g_resolver.get_system_routine( "IoReleaseCancelSpinLock" );
                if ( !function_address ) return;
            }

            using function_t = void( * )( std::uint8_t cancel_irql );

            reinterpret_cast< function_t >( function_address )( cancel_irql );
        }

        void io_set_cancel_routine( iop_irp_t* irp, void ( *cancel_routine )( iop_irp_t* ) ) {
            static std::addr_t function_address = nullptr;
            if ( !function_address ) {
                function_address = g_resolver.get_system_routine( "IoSetCancelRoutine" );
                if ( !function_address ) return;
            }

            using function_t = void( * )( iop_irp_t* irp, void ( *cancel_routine )( iop_irp_t* ) );

            reinterpret_cast< function_t >( function_address )( irp, cancel_routine );
        }

        void rtl_init_unicode_string( unicode_string_t* destination_string, const wchar_t* source_string ) {
            static std::addr_t function_address = nullptr;
            if ( !function_address ) {
                function_address = g_resolver.get_system_routine( "RtlInitUnicodeString" );
                if ( !function_address ) return;
            }

            using function_t = void( * )( unicode_string_t* destination_string, const wchar_t* source_string );

            reinterpret_cast< function_t >( function_address )( destination_string, source_string );
        }

        nt_status_t rtl_convert_string_to_security_descriptor(
            unicode_string_t* string_security_descriptor,
            std::uint32_t revision,
            security_descriptor_t** security_descriptor,
            std::uint32_t* security_descriptor_size ) {

            static std::addr_t function_address = nullptr;
            if ( !function_address ) {
                function_address = g_resolver.get_system_routine( "RtlConvertStringSecurityDescriptorToSecurityDescriptor" );
                if ( !function_address ) return nt_status_t::access_violation;
            }

            using function_t = nt_status_t( * )(
                unicode_string_t* string_security_descriptor,
                std::uint32_t revision,
                security_descriptor_t** security_descriptor,
                std::uint32_t* security_descriptor_size );

            return reinterpret_cast< function_t >( function_address )(
                string_security_descriptor,
                revision,
                security_descriptor,
                security_descriptor_size );
        }

    iop_irp_stack_profiler_t* io_get_current_irp_stack_location( iop_irp_t* irp ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "IoGetCurrentIrpStackLocation" );
            if ( !function_address ) return nullptr;
        }

        using function_t = iop_irp_stack_profiler_t * ( * )(
            iop_irp_t* irp
            );

        return reinterpret_cast< function_t >( function_address )(
            irp
            );
    }

    void iof_complete_request( iop_irp_t* irp, char priority_boost ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "IofCompleteRequest" );
            if ( !function_address ) return;
        }

        using function_t = void( * )(
            iop_irp_t* irp,
            char priority_boost
            );

        reinterpret_cast< function_t >( function_address )(
            irp,
            priority_boost
            );
    }

    void* io_get_irp_input_buffer( iop_irp_t* irp, size_t* length ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "IoGetIrpInputBuffer" );
            if ( !function_address ) return nullptr;
        }

        using function_t = void* ( * )(
            iop_irp_t* irp,
            size_t* length
            );

        return reinterpret_cast< function_t >( function_address )(
            irp,
            length
            );
    }

    void* io_get_irp_output_buffer( iop_irp_t* irp, size_t* length ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "IoGetIrpOutputBuffer" );
            if ( !function_address ) return nullptr;
        }

        using function_t = void* ( * )(
            iop_irp_t* irp,
            size_t* length
            );

        return reinterpret_cast< function_t >( function_address )(
            irp,
            length
            );
    }

    void io_set_irp_info( iop_irp_t* irp, nt_status_t status, size_t information ) {
        irp->m_io_status.m_status = status;
        irp->m_io_status.m_information = information;
    }

    nt_status_t io_delete_symbolic_link( unicode_string_t* symbolic_link_name ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "IoDeleteSymbolicLink" );
            if ( !function_address ) return nt_status_t::access_violation;
        }

        using function_t = nt_status_t( * )(
            unicode_string_t* symbolic_link_name
            );

        return reinterpret_cast< function_t >( function_address )(
            symbolic_link_name
            );
    }

    nt_status_t io_delete_device( device_object_t* device_object ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "IoDeleteDevice" );
            if ( !function_address ) return nt_status_t::access_violation;
        }

        using function_t = nt_status_t( * )(
            device_object_t* device_object
            );

        return reinterpret_cast< function_t >( function_address )(
            device_object
            );
    }

    nt_status_t io_create_device(
        driver_object_t* driver_object,
        unsigned long device_extension_size,
        unicode_string_t* device_name,
        device_type_t device_type,
        unsigned long device_characteristics,
        bool exclusive,
        device_object_t** device_object
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "IoCreateDevice" );
            if ( !function_address ) return nt_status_t::access_violation;
        }

        using function_t = nt_status_t( * )(
            driver_object_t* driver_object,
            unsigned long device_extension_size,
            unicode_string_t* device_name,
            device_type_t device_type,
            unsigned long device_characteristics,
            bool exclusive,
            device_object_t** device_object
            );

        return reinterpret_cast< function_t >( function_address )(
            driver_object,
            device_extension_size,
            device_name,
            device_type,
            device_characteristics,
            exclusive,
            device_object
            );
    }

    nt_status_t io_create_symbolic_link(
        unicode_string_t* symbolic_link_name,
        unicode_string_t* device_name
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "IoCreateSymbolicLink" );
            if ( !function_address ) return nt_status_t::access_violation;
        }

        using function_t = nt_status_t( * )(
            unicode_string_t* symbolic_link_name,
            unicode_string_t* device_name
            );

        return reinterpret_cast< function_t >( function_address )(
            symbolic_link_name,
            device_name
            );
    }

    void ob_make_temporary_object( void* object ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ObMakeTemporaryObject" );
            if ( !function_address ) return;
        }

        using function_t = void( * )( void* object );
        reinterpret_cast< function_t >( function_address )( object );
    }

    std::uint64_t ke_query_unbiased_interrupt_time( ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "KeQueryUnbiasedInterruptTime" );
            if ( !function_address ) return 0;
        }

        using function_t = std::uint64_t( * )( );
        return reinterpret_cast< function_t >( function_address )( );
    }

    nt_status_t ob_reference_object_by_name(
        unicode_string_t* object_name,
        std::uint32_t attributes,
        void* parsed_object,
        std::uint32_t access_mode,
        void* object_type,
        std::uint32_t access_mode2,
        void* context,
        void** object
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ObReferenceObjectByName" );
            if ( !function_address ) return {};
        }

        using function_t = nt_status_t( * )(
            unicode_string_t* object_name,
            std::uint32_t attributes,
            void* parsed_object,
            std::uint32_t access_mode,
            void* object_type,
            std::uint32_t access_mode2,
            void* context,
            void** object
            );

        return reinterpret_cast< function_t >( function_address )(
            object_name,
            attributes,
            parsed_object,
            access_mode,
            object_type,
            access_mode2,
            context,
            object
            );
    }

    nt_status_t ke_query_system_information(
        std::uint32_t information_class,
        void* system_information,
        std::uint32_t length,
        std::uint32_t* return_length
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "KeQuerySystemInformation" );
            if ( !function_address ) return {};
        }

        using function_t = nt_status_t( * )(
            std::uint32_t information_class,
            void* system_information,
            std::uint32_t length,
            std::uint32_t* return_length
            );

        return reinterpret_cast< function_t >( function_address )(
            information_class,
            system_information,
            length,
            return_length
            );
    }

    void rtl_copy_memory(
        void* dest,
        const void* src,
        size_t length
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "RtlCopyMemory" );
            if ( !function_address ) return;
        }

        using function_t = void( * )(
            void* dest,
            const void* src,
            size_t length
            );

        reinterpret_cast< function_t >( function_address )(
            dest,
            src,
            length
            );
    }

    void probe_for_read(
        void* address,
        std::size_t length,
        std::uint8_t alignment
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ProbeForRead" );
            if ( !function_address ) return;
        }

        using function_t = void( * )(
            void*,
            std::size_t,
            std::uint8_t
            );

        return reinterpret_cast< function_t >( function_address )(
            address,
            length,
            alignment
            );
    }

    eprocess_t* zw_current_process( ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ZwCurrentProcess" );
            if ( !function_address ) return nullptr;
        }

        using function_t = eprocess_t * ( * )( );

        return reinterpret_cast< function_t >( function_address )( );
    }

    void ke_initialize_spin_lock( kspin_lock_t* lock ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "KeInitializeSpinLock" );
            if ( !function_address ) return;
        }

        using function_t = void ( * )( kspin_lock_t* );
        reinterpret_cast< function_t >( function_address )( lock );
    }

    void ke_acquire_spin_lock( kspin_lock_t* lock, kirql_t* old_irql ) {
        //static std::addr_t function_address = nullptr;
        //if ( !function_address ) {
        //    function_address = g_resolver.get_system_routine( "KeAcquireSpinLock" );
        //    if ( !function_address ) return;
        //}

        //using function_t = void ( * )( kspin_lock_t*, kirql_t* );
        //reinterpret_cast< function_t >( function_address )( lock, old_irql );
    }

    void ke_release_spin_lock( kspin_lock_t* lock, kirql_t old_irql ) {
        //if ( !lock ) return;

        //static std::addr_t function_address = nullptr;
        //if ( !function_address ) {
        //    function_address = g_resolver.get_system_routine( "KeReleaseSpinLock" );
        //    if ( !function_address ) return;
        //}

        //using function_t = void ( * )( kspin_lock_t*, kirql_t );
        //reinterpret_cast< function_t >( function_address )( lock, old_irql );
    }

    void rtl_zero_memory( void* dest, size_t length ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "RtlZeroMemory" );
            if ( !function_address ) return;
        }

        using function_t = void ( * )( void*, size_t );
        reinterpret_cast< function_t >( function_address )( dest, length );
    }

    void rtl_move_memory( void* dest, const void* src, size_t length ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "RtlMoveMemory" );
            if ( !function_address ) return;
        }

        using function_t = void ( * )( void*, const void*, size_t );
        reinterpret_cast< function_t >( function_address )( dest, src, length );
    }

    nt_status_t zw_map_view_of_section(
        void* section_handle,
        void* process_handle,
        void** base_address,
        std::uint64_t zero_bits,
        std::size_t commit_size,
        large_integer_t* section_offset,
        std::size_t* view_size,
        std::uint32_t inherit_disposition,
        std::uint64_t allocation_type,
        std::uint32_t win32_protect
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ZwMapViewOfSection" );
            if ( !function_address ) return nt_status_t::unsuccessful;
        }

        using function_t = nt_status_t( * )(
            void*,
            void*,
            void**,
            std::uint64_t,
            std::size_t,
            large_integer_t*,
            std::size_t*,
            std::uint32_t,
            std::uint64_t,
            std::uint32_t
            );

        return reinterpret_cast< function_t >( function_address )(
            section_handle,
            process_handle,
            base_address,
            zero_bits,
            commit_size,
            section_offset,
            view_size,
            inherit_disposition,
            allocation_type,
            win32_protect
            );
    }

    std::uintptr_t ps_get_thread_teb(
        void* thread
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsGetThreadTeb" );
            if ( !function_address ) return 0;
        }

        using function_t = std::uintptr_t( * )(
            void*
            );

        return reinterpret_cast< function_t >( function_address )( thread );
    }

    object_type_t* ps_process_type( ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsProcessType" );
            if ( !function_address ) return nullptr;
        }

        return *reinterpret_cast< object_type_t** >( function_address );
    }

    nt_status_t zw_get_next_thread(
        void* process_handle,
        void* thread_handle,
        std::uint32_t desired_access,
        std::uint32_t handle_attributes,
        std::uint32_t flags,
        void** new_thread_handle
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ZwGetNextThread" );
            if ( !function_address ) return { };
        }

        using function_t = nt_status_t( * )(
            void*,
            void*,
            std::uint32_t,
            std::uint32_t,
            std::uint32_t,
            void**
            );

        return reinterpret_cast< function_t >( function_address )(
            process_handle,
            thread_handle,
            desired_access,
            handle_attributes,
            flags,
            new_thread_handle
            );
    }

    nt_status_t ob_open_object_by_pointer(
        void* object,
        std::uint32_t handle_attributes,
        void* access_state,
        std::uint32_t desired_access,
        void* object_type,
        std::uint32_t access_mode,
        void** handle
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ObOpenObjectByPointer" );
            if ( !function_address ) return {};
        }

        using function_t = nt_status_t( * )(
            void*,              // Object
            std::uint32_t,      // HandleAttributes
            void*,              // PassedAccessState
            std::uint32_t,      // DesiredAccess
            void*,              // ObjectType
            std::uint32_t,      // AccessMode
            void**              // Handle
            );

        return reinterpret_cast< function_t >( function_address )(
            object,
            handle_attributes,
            access_state,
            desired_access,
            object_type,
            access_mode,
            handle
            );
    }

    nt_status_t ob_reference_object_by_handle(
        void* handle,
        std::uint32_t desired_access,
        void* object_type,
        std::uint8_t access_mode,
        void** object,
        void* handle_information
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ObReferenceObjectByHandle" );
            if ( !function_address ) return nt_status_t::unsuccessful;
        }

        using function_t = nt_status_t( * )(
            void*,
            std::uint32_t,
            void*,
            std::uint8_t,
            void**,
            void*
            );

        return reinterpret_cast< function_t >( function_address )(
            handle,
            desired_access,
            object_type,
            access_mode,
            object,
            handle_information
            );
    }

    __forceinline nt_status_t reference_thread_by_handle(
        void* thread_handle,
        kthread_t** thread
    ) {
        return ob_reference_object_by_handle(
            thread_handle,
            0,
            get_ps_thread_type( ),
            0, // KernelMode
            reinterpret_cast< void** >( thread ),
            nullptr
        );
    }

    void ob_dereference_object(
        void* object
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ObDereferenceObject" );
            if ( !function_address ) return;
        }

        using function_t = void( * )(
            void*
            );

        reinterpret_cast< function_t >( function_address )( object );
    }

    nt_status_t zw_create_section(
        void** section_handle,
        std::uint32_t desired_access,
        object_attributes_t* object_attributes,
        large_integer_t* maximum_size,
        std::uint32_t page_protection,
        std::uint32_t allocation_attributes,
        void* file_handle
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ZwCreateSection" );
            if ( !function_address ) return nt_status_t::unsuccessful;
        }

        using function_t = nt_status_t( * )(
            void**,
            std::uint32_t,
            object_attributes_t*,
            large_integer_t*,
            std::uint32_t,
            std::uint32_t,
            void*
            );

        return reinterpret_cast< function_t >( function_address )(
            section_handle,
            desired_access,
            object_attributes,
            maximum_size,
            page_protection,
            allocation_attributes,
            file_handle
            );
    }

    nt_status_t zw_create_user_process(
        void** process_handle,
        void** thread_handle,
        std::uint32_t process_access,
        std::uint32_t thread_access,
        object_attributes_t* process_attributes,
        object_attributes_t* thread_attributes,
        std::uint32_t process_flags,
        std::uint32_t thread_flags,
        void* process_parameters,
        ps_create_info_t* create_info,
        ps_attribute_list_t* attribute_list
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ZwCreateUserProcess" );
            if ( !function_address ) return nt_status_t::unsuccessful;
        }

        using function_t = nt_status_t( * )(
            void**,
            void**,
            std::uint32_t,
            std::uint32_t,
            object_attributes_t*,
            object_attributes_t*,
            std::uint32_t,
            std::uint32_t,
            void*,
            ps_create_info_t*,
            ps_attribute_list_t*
            );

        return reinterpret_cast< function_t >( function_address )(
            process_handle,
            thread_handle,
            process_access,
            thread_access,
            process_attributes,
            thread_attributes,
            process_flags,
            thread_flags,
            process_parameters,
            create_info,
            attribute_list
            );
    }

    nt_status_t zw_close(
        void* handle
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ZwClose" );
            if ( !function_address ) return nt_status_t::unsuccessful;
        }

        using function_t = nt_status_t( * )( void* );
        return reinterpret_cast< function_t >( function_address )( handle );
    }

    std::uint16_t* get_ke_active_processors( ) {
        static std::uint16_t* ke_active_processors = nullptr;
        if ( !ke_active_processors ) {
            std::addr_t function_address = g_resolver.get_system_routine( "KeQueryGroupAffinity" );
            while ( function_address[ 0x0 ] != 0x66 ||
                function_address[ 0x1 ] != 0x3B ||
                function_address[ 0x2 ] != 0x0D )
                function_address++;

            ke_active_processors = reinterpret_cast< std::uint16_t* >(
                &function_address[ 0x7 ] + *reinterpret_cast< std::int32_t* >( &function_address[ 0x3 ] ) );
        }

        return ke_active_processors;
    }

    std::uint64_t hal_send_nmi(
        std::uint64_t* affinity
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "HalSendNMI" );
            if ( !function_address ) return {};
        }

        using function_t = std::uint64_t(
            std::uint64_t* affinity
        );

        return reinterpret_cast< function_t* >( function_address )( affinity );
    }

    std::int16_t ke_remove_processor_affinity_ex(
        std::uint64_t affinity,
        std::uint32_t number
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "KeRemoveProcessorAffinityEx" );
            if ( !function_address ) return {};
        }

        using function_t = std::int16_t(
            std::uint64_t affinity,
            std::uint32_t number
        );

        return reinterpret_cast< function_t* >( function_address )( affinity, number );
    }

    std::int16_t ke_copy_affinity_ex(
        std::uint64_t affinity,
        std::uint16_t* processors
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "KeCopyAffinityEx" );
            if ( !function_address ) return {};
        }

        using function_t = std::int16_t(
            std::uint64_t affinity,
            std::uint16_t* processors
        );

        return reinterpret_cast< function_t* >( function_address )( affinity, processors );
    }

    void* ke_register_nmi_callback(
        p_nmi_callback callback_routine,
        void* context
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "KeRegisterNmiCallback" );
            if ( !function_address ) return {};
        }

        using function_t = void* (
            p_nmi_callback,
            void*
            );

        return reinterpret_cast< function_t* >( function_address )( callback_routine, context );
    }

    bool ke_deregister_nmi_callback( void* handle ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "KeDeregisterNmiCallback" );
            if ( !function_address ) return false;
        }

        using function_t = bool( void* Handle );
        return reinterpret_cast< function_t* >( function_address )( handle );
    }

    void probe_for_write(
        void* address,
        std::size_t length,
        std::uint8_t alignment
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ProbeForWrite" );
            if ( !function_address ) return;
        }

        using function_t = void(
            void* address,
            std::size_t length,
            std::uint8_t alignment
            );

        reinterpret_cast< function_t* >( function_address )(
            address,
            length,
            alignment
            );
    }

    void mm_unmap_io_space(
        void* base_address,
        std::size_t number_of_bytes
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "MmUnmapIoSpace" );
            if ( !function_address ) return;
        }

        using function_t = void(
            void* base_address,
            std::size_t number_of_bytes
            );

        reinterpret_cast< function_t* >( function_address ) (
            base_address,
            number_of_bytes );
    }

    void* map_io_space_ex(
        physical_address_t physical_address,
        std::size_t number_of_bytes,
        std::uint32_t protect
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "MmMapIoSpaceEx" );
            if ( !function_address ) return { };
        }

        using function_t = void* (
            physical_address_t physical_address,
            std::size_t number_of_bytes,
            std::uint32_t protect
            );

        return reinterpret_cast< function_t* >( function_address ) (
            physical_address,
            number_of_bytes,
            protect );
    }

    nt_status_t mm_copy_memory(
        void* target_address,
        mm_copy_address_t source_address,
        std::size_t number_of_bytes,
        std::uint32_t flags,
        std::size_t* number_of_bytes_transferred
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "MmCopyMemory" );
            if ( !function_address ) return { };
        }

        using function_t = nt_status_t(
            void* target_address,
            mm_copy_address_t source_address,
            std::size_t number_of_bytes,
            std::uint32_t flags,
            std::size_t* number_of_bytes_transferred
        );

        return reinterpret_cast< function_t* >( function_address )(
            target_address,
            source_address,
            number_of_bytes,
            flags,
            number_of_bytes_transferred
            );
    }

    nt_status_t zw_query_system_information(
        system_information_class information_class,
        void* system_information,
        std::uint32_t system_information_length,
        std::uint32_t* return_length
    ) {
        static void* function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ZwQuerySystemInformation" );
            if ( !function_address ) return nt_status_t::unsuccessful;
        }

        using function_t = nt_status_t(
            system_information_class information_class,
            void* system_information,
            std::uint32_t system_information_length,
            std::uint32_t* return_length
            );

        return reinterpret_cast< function_t* >( function_address )(
            information_class,
            system_information,
            system_information_length,
            return_length
            );
    }

    mmpfn_t* get_mm_pfn_database( ) {
        static mmpfn_t* mm_pfn_database = nullptr;
        if ( !mm_pfn_database ) {
            static std::addr_t function_address = nullptr;
            if ( !function_address ) {
                function_address = g_resolver.get_system_routine( "KeCapturePersistentThreadState" );
                if ( !function_address ) return { };
            }

            while ( function_address[ 0x0 ] != 0x48
                || function_address[ 0x1 ] != 0x8B
                || function_address[ 0x2 ] != 0x05 )
                function_address++;

            mm_pfn_database = *reinterpret_cast< mmpfn_t** >(
                &function_address[ 0x7 ] + *reinterpret_cast< std::int32_t* >( &function_address[ 0x3 ] ) );
        }

        return mm_pfn_database;
    }

    std::uint8_t ke_raise_irql(
        std::uint8_t new_irql
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "KeRaiseIrql" );
            if ( !function_address ) return {};
        }

        using function_t = void( * )(
            std::uint8_t new_irql,
            std::uint8_t* old_irql
            );

        std::uint8_t old_irql;
        reinterpret_cast< function_t >( function_address )(
            new_irql,
            &old_irql
            );

        return old_irql;
    }

    void ke_lower_irql(
        std::uint8_t new_irql
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "KeLowerIrql" );
            if ( !function_address ) return;
        }

        using function_t = void( * )(
            std::uint8_t new_irql
            );

        reinterpret_cast< function_t >( function_address )(
            new_irql
            );
    }

    void ke_sleep(
        std::int64_t ms
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "KeDelayExecutionThread" );
            if ( !function_address ) return;
        }

        using function_t = void( * )(
            std::uint8_t wait_mode,
            std::uint8_t alertable,
            std::int64_t* interval
            );

        std::int64_t interval = -10000LL * ms;
        reinterpret_cast< function_t >( function_address )(
            0,              // KernelMode
            false,          // Non-alertable
            &interval
            );
    }

    nt_status_t zw_free_virtual_memory(
        void* process_handle,
        void** base_address,
        std::size_t* region_size,
        std::uint32_t free_type
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ZwFreeVirtualMemory" );
            if ( !function_address ) return {};
        }

        using function_t = nt_status_t( * )(
            void* process_handle,
            void** base_address,
            std::size_t* region_size,
            std::uint32_t free_type
            );

        return reinterpret_cast< function_t >( function_address )(
            process_handle,
            base_address,
            region_size,
            free_type
            );
    }

    physical_memory_range_t* mm_get_physical_memory_ranges( ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "MmGetPhysicalMemoryRanges" );
            if ( !function_address ) return { };
        }

        using function_t = physical_memory_range_t * ( void );

        return reinterpret_cast< function_t* >( function_address )( );
    }

    nt_status_t zw_query_information_process(
        void* process_handle,
        std::uint32_t process_information_class,
        void* process_information,
        std::uint32_t process_information_length,
        std::uint32_t* return_length
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ZwQueryInformationProcess" );
            if ( !function_address ) return {};
        }

        using fn_t = nt_status_t( * )( void*, std::uint32_t, void*, std::uint32_t, std::uint32_t* );
        return reinterpret_cast< fn_t >( function_address )(
            process_handle,
            process_information_class,
            process_information,
            process_information_length,
            return_length
            );
    }

    nt_status_t ps_query_full_process_image_name(
        void* process,
        unicode_string_t** image_name
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsQueryFullProcessImageName" );
            if ( !function_address ) return { };
        }

        using function_t = nt_status_t( * )(
            void*,
            unicode_string_t**
            );

        return reinterpret_cast< function_t >( function_address )(
            process,
            image_name
            );
    }

    std::uintptr_t ps_initial_system_process( ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsInitialSystemProcess" );
            if ( !function_address ) return {};
        }

        return *reinterpret_cast< std::uintptr_t* >( function_address );
    }

    std::uint32_t ps_get_process_id(
        eprocess_t* process
    ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsGetProcessId" );
            if ( !function_address ) return {};
        }

        using function_t = void* (
            eprocess_t* Process
            );

        return reinterpret_cast< std::uint32_t >(
            reinterpret_cast< function_t* >( function_address )( process )
            );
    }

    list_entry_t* ps_active_process_head( ) {
        static list_entry_t* ps_active_process_head = nullptr;
        if ( !ps_active_process_head ) {
            static std::addr_t function_address = 0;
            if ( !function_address ) {
                function_address = g_resolver.get_system_routine( "KeCapturePersistentThreadState" );
                if ( !function_address ) return {};
            }

            while ( function_address[ 0x0 ] != 0x20
                || function_address[ 0x1 ] != 0x48
                || function_address[ 0x2 ] != 0x8d )
                function_address++;

            ps_active_process_head = *reinterpret_cast< list_entry_t** >
                ( &function_address[ 0x8 ] + *reinterpret_cast< std::int32_t* >( &function_address[ 0x4 ] ) );
        }

        return ps_active_process_head;
    }

    std::uintptr_t get_cr3( eprocess_t* process ) {
        if ( !process ) return 0;

        auto directory_table_base = *reinterpret_cast< std::uintptr_t* >(
            reinterpret_cast< std::uintptr_t >( process ) + 0xB8
            );

        return *reinterpret_cast< std::uintptr_t* >( directory_table_base + 0x28 ) & 0xFFFFFFFFFFFFF000;
    }

    void memcpy( void* destination, const void* source, size_t length ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "RtlCopyMemory" );
        }
        if ( !function_address ) return;

        using function_t = void( void* destination, const void* source, size_t length );
        reinterpret_cast< function_t* >( function_address )( destination, source, length );
    }

    void ex_free_pool( void* base_address ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ExFreePool" );
        }
        if ( !function_address ) return;

        using function_t = void( void* );
        reinterpret_cast< function_t* >( function_address )( base_address );
    }

    void* ex_allocate_pool( std::size_t number_of_bytes, pool_type used_pool_type = pool_type::nonpaged_pool ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ExAllocatePool" );
        }
        if ( !function_address ) return nullptr;

        using function_t = void* ( pool_type, std::size_t );
        return reinterpret_cast< function_t* >( function_address )( used_pool_type, number_of_bytes );
    }

    void* ex_allocate_pool_with_tag( std::size_t number_of_bytes, std::uint32_t tag, pool_type used_pool_type = pool_type::nonpaged_pool ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ExAllocatePoolWithTag" );
        }
        if ( !function_address ) return nullptr;

        using function_t = void* ( pool_type, std::size_t, std::uint32_t );
        return reinterpret_cast< function_t* >( function_address )( used_pool_type, number_of_bytes, tag );
    }

    nt_status_t zw_unload_driver( unicode_string_t* driver_service_name ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ZwUnloadDriver" );
            if ( !function_address ) return nt_status_t::unsuccessful;
        }

        using function_t = nt_status_t( * )( unicode_string_t* driver_service_name );
        return reinterpret_cast< function_t >( function_address )( driver_service_name );
    }

    void ex_init_work_item(
        void* work_item,
        void ( *worker_routine )( void* parameter ),
        void* parameter
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ExInitializeWorkItem" );
            if ( !function_address ) return;
        }

        using function_t = void( * )(
            void* work_item,
            void ( *worker_routine )( void* parameter ),
            void* parameter
            );

        reinterpret_cast< function_t >( function_address )(
            work_item,
            worker_routine,
            parameter
            );
    }

    void ex_queue_work_item(
        void* work_item,
        std::uint32_t queue_type
    ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "ExQueueWorkItem" );
            if ( !function_address ) return;
        }

        using function_t = void( * )(
            void* work_item,
            std::uint32_t queue_type
            );

        reinterpret_cast< function_t >( function_address )(
            work_item,
            queue_type
            );
    }

    nt_status_t ps_get_process_exit_status( eprocess_t* process ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsGetProcessExitStatus" );
        }
        if ( !function_address ) return {};

        using function_t = nt_status_t( eprocess_t* );
        return reinterpret_cast< function_t* >( function_address )( process );
    }

    nt_status_t mm_copy_virtual_memory(
        eprocess_t* source_process,
        void* source_address,
        eprocess_t* target_process,
        void* target_address,
        std::size_t buffer_size,
        std::int8_t previous_mode
    ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "MmCopyVirtualMemory" );
        }
        if ( !function_address ) return {};

        std::size_t bytes_read = 0;
        using function_t = nt_status_t( eprocess_t*, void*, eprocess_t*, void*, std::size_t, std::int8_t, std::size_t* );
        return reinterpret_cast< function_t* >( function_address )( source_process, source_address, target_process, target_address, buffer_size, previous_mode, &bytes_read );
    }

    eprocess_t* io_get_current_process( ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "IoGetCurrentProcess" );
        }
        if ( !function_address ) return nullptr;

        using function_t = eprocess_t * ( );
        return reinterpret_cast< function_t* >( function_address )( );
    }

    eprocess_t* ps_get_current_process( ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsGetCurrentProcess" );
        }
        if ( !function_address ) return nullptr;

        using function_t = eprocess_t * ( );
        return reinterpret_cast< function_t* >( function_address )( );
    }

    void* ps_get_thread_teb( ethread_t* Thread ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsGetThreadTeb" );
        }
        if ( !function_address ) return nullptr;

        using function_t = void* ( ethread_t* );
        return reinterpret_cast< function_t* >( function_address )( Thread );
    }

    void* ps_get_process_section_base_address( eprocess_t* Process ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsGetProcessSectionBaseAddress" );
        }
        if ( !function_address ) return nullptr;

        using function_t = void* (
            eprocess_t* Process
            );

        return reinterpret_cast< function_t* >( function_address )( Process );
    }

    eprocess_t* ps_lookup_process_by_process_id( std::uint32_t process_id ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsLookupProcessByProcessId" );
        }
        if ( !function_address ) return nullptr;

        eprocess_t* process;
        using function_t = nt_status_t( void*, eprocess_t** );
        auto result = reinterpret_cast< function_t* >( function_address )(
            reinterpret_cast< void* >( process_id ),
            &process );
        if ( result == 0 )
            return process;

        return nullptr;
    }

    ethread_t* ps_get_next_process_thread( eprocess_t* Process, ethread_t* Thread ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsGetNextProcessThread" );
        }
        if ( !function_address ) return nullptr;

        using function_t = ethread_t * ( eprocess_t*, ethread_t* );
        return reinterpret_cast< function_t* >( function_address )(
            Process, Thread );
    }

    nt_status_t zw_terminate_thread( void* ThreadHandle, nt_status_t ExitStatus ) {
        auto function_address = g_resolver.get_system_routine( "ZwTerminateThread" );
        if ( !function_address ) {
            return {};
        }

        using function_t = nt_status_t( void*, nt_status_t );
        return reinterpret_cast< function_t* >( function_address )(
            ThreadHandle,
            ExitStatus
            );
    }

    nt_status_t ex_destory_handle(
        handle_table_t* table,
        void* handle,
        handle_table_entry_t* handle_table_entry
    ) {
        static std::addr_t ex_destory_handle = 0;
        if ( !ex_destory_handle ) {
            static auto function_address = g_resolver.get_system_routine( "RtlEmptyAtomTable" );
            if ( !function_address )
                return {};

            while ( function_address[ 0x0 ] != 0x48 ||
                function_address[ 0x1 ] != 0x83 ||
                function_address[ 0x2 ] != 0x23 ||
                function_address[ 0x3 ] != 0x00 ||
                function_address[ 0x4 ] != 0xE8 )
                function_address++;

            auto relative_offset = *reinterpret_cast< std::int32_t* >( &function_address[ 0x5 ] );
            auto rtlp_free_handle_for_atom = reinterpret_cast< std::addr_t >( function_address + 9 + relative_offset );
            if ( !rtlp_free_handle_for_atom )
                return {};

            while ( rtlp_free_handle_for_atom[ 0x0 ] != 0x4C ||
                rtlp_free_handle_for_atom[ 0x1 ] != 0x8B ||
                rtlp_free_handle_for_atom[ 0x2 ] != 0xC0 ||
                rtlp_free_handle_for_atom[ 0x3 ] != 0x48 ||
                rtlp_free_handle_for_atom[ 0x4 ] != 0x8B ||
                rtlp_free_handle_for_atom[ 0x5 ] != 0x4B ||
                rtlp_free_handle_for_atom[ 0x6 ] != 0x10 ||
                rtlp_free_handle_for_atom[ 0x7 ] != 0xE8 )
                rtlp_free_handle_for_atom++;

            relative_offset = *reinterpret_cast< std::int32_t* >( &rtlp_free_handle_for_atom[ 0x8 ] );
            ex_destory_handle = rtlp_free_handle_for_atom + 0xC + relative_offset;
        }

        using function_t = nt_status_t( handle_table_t* table,
            void* handle,
            handle_table_entry_t* handle_table_entry );

        return reinterpret_cast< function_t* >( ex_destory_handle )( table, handle, handle_table_entry );
    }

    handle_table_entry_t* exp_lookup_handle_table_entry( std::uint64_t* handle_table, long long handle ) {
        std::uint64_t v2; // rdx
        long long v3; // r8

        v2 = handle & 0xFFFFFFFFFFFFFFFC;
        if ( v2 >= *handle_table )
            return 0;
        v3 = *( handle_table + 1 );
        if ( ( v3 & 3 ) == 1 )
            return reinterpret_cast< handle_table_entry_t* >( *reinterpret_cast< std::uint64_t* >( v3 + 8 * ( v2 >> 10 ) - 1 ) + 4 * ( v2 & 0x3FF ) );
        if ( ( v3 & 3 ) != 0 )
            return reinterpret_cast< handle_table_entry_t* >( *reinterpret_cast< std::uint64_t* >( *reinterpret_cast< std::uint64_t* >( v3 + 8 * ( v2 >> 19 ) - 2 ) + 8 * ( ( v2 >> 10 ) & 0x1FF ) ) + 4 * ( v2 & 0x3FF ) );
        return reinterpret_cast< handle_table_entry_t* >( v3 + 4 * v2 );
    }

    handle_table_t* get_psp_cid_table( ) {
        static handle_table_t* psp_cid_table = nullptr;
        if ( !psp_cid_table ) {
            auto function_address = g_resolver.get_system_routine( "PsLookupThreadByThreadId" );
            if ( !function_address )
                return {};

            while ( function_address[ 0x0 ] != 0xB2 ||
                function_address[ 0x1 ] != 0x06 ||
                function_address[ 0x2 ] != 0xE8 )
                function_address++;

            auto relative_offset = *reinterpret_cast< std::int32_t* >( &function_address[ 0x3 ] );
            auto psp_reference_cid_table_entry = function_address + 7 + relative_offset;
            if ( !psp_reference_cid_table_entry )
                return {};

            while ( psp_reference_cid_table_entry[ 0x0 ] != 0x48 ||
                psp_reference_cid_table_entry[ 0x1 ] != 0x8B ||
                psp_reference_cid_table_entry[ 0x2 ] != 0x05 )
                psp_reference_cid_table_entry++;

            auto psp_cid_table_ptr = reinterpret_cast< handle_table_t** >(
                psp_reference_cid_table_entry + 7 +
                *reinterpret_cast< std::int32_t* >( &psp_reference_cid_table_entry[ 0x3 ] ) );

            psp_cid_table = *psp_cid_table_ptr;
        }

        return psp_cid_table;
    }

    void* ps_get_thread_id( ethread_t* Thread ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsGetThreadId" );
            if ( !function_address ) return {};
        }

        using function_t = void* (
            ethread_t* Thread
            );

        return reinterpret_cast< function_t* >( function_address )( Thread );
    }

    ethread_t* ps_get_current_thread( ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsGetCurrentThread" );
            if ( !function_address ) return {};
        }

        using function_t = ethread_t * ( );

        return reinterpret_cast< function_t* >( function_address )( );
    }

    bool ps_is_system_thread( ethread_t* Thread ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsIsSystemThread" );
            if ( !function_address ) return {};
        }

        using function_t = bool(
            ethread_t* Thread
            );

        return reinterpret_cast< function_t* >( function_address )( Thread );
    }

    nt_status_t ps_lookup_thread_by_thread_id( std::uint32_t ThreadId, ethread_t** Thread ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsLookupThreadByThreadId" );
            if ( !function_address ) return {};
        }

        using function_t = nt_status_t(
            void* ThreadId,
            ethread_t** Thread
        );

        return reinterpret_cast< function_t* >( function_address )(
            reinterpret_cast< void* >( ThreadId ),
            Thread
            );
    }

    bool mm_is_address_valid( void* VirtualAddress ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "MmIsAddressValid" );
            if ( !function_address ) return {};
        }

        using function_t = bool(
            void* VirtualAddress
            );

        return reinterpret_cast< function_t* >( function_address )( VirtualAddress );
    }

    kthread_t* ke_get_current_thread( void ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "KeGetCurrentThread" );
            if ( !function_address ) return {};
        }

        using function_t = kthread_t * ( void );
        return reinterpret_cast< function_t* >( function_address )( );
    }

    peb_t* get_process_peb( eprocess_t* process ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsGetProcessPeb" );
            if ( !function_address ) return {};
        }

        using function_t = peb_t * ( eprocess_t* process );
        return reinterpret_cast< function_t* >( function_address )( process );
    }


    nt_status_t ps_create_system_thread(
        void** thread_handle,
        std::uint32_t desired_access,
        void* object_attributes,
        void* process_handle,
        void* client_id,
        void* start_routine,
        void* start_context ) {
        static std::addr_t function_address = nullptr;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "PsCreateSystemThread" );
            if ( !function_address ) return {};
        }

        using function_t = nt_status_t(
            void** thread_handle,
            std::uint32_t desired_access,
            void* object_attributes,
            void* process_handle,
            void* client_id,
            void* start_routine,
            void* start_context
        );

        return reinterpret_cast< function_t* >( function_address )(
            thread_handle,
            desired_access,
            object_attributes,
            process_handle,
            client_id,
            start_routine,
            start_context
            );
    }

    template<class... args_t>
    std::int8_t dbg_print( const char* format, args_t... va_args ) {
        static std::addr_t function_address = 0;
        if ( !function_address ) {
            function_address = g_resolver.get_system_routine( "DbgPrintEx" );
        }
        if ( !function_address ) return 0;

        using function_t = std::int32_t( std::uint32_t flag, std::uint32_t level,
            const char* format, args_t... va_args );
        return reinterpret_cast< function_t* >( function_address )( 0, 0, format, va_args... ) ==
            nt_status_t::success;
    }

    nt_status_t create_driver( nt_status_t( *entry_point )( driver_object_t*, unicode_string_t* ) ) {
        driver_object_t* driver_object = nullptr;
        wchar_t name_buffer[ 100 ] = { 0 };
        unicode_string_t driver_name;
        object_attributes_t obj_attributes;
        nt_status_t status = nt_status_t::unsuccessful;

        auto timestamp = ke_query_unbiased_interrupt_time( );
        int pos = 0;
        name_buffer[ pos++ ] = L'\\';
        name_buffer[ pos++ ] = L'D';
        name_buffer[ pos++ ] = L'r';
        name_buffer[ pos++ ] = L'i';
        name_buffer[ pos++ ] = L'v';
        name_buffer[ pos++ ] = L'e';
        name_buffer[ pos++ ] = L'r';
        name_buffer[ pos++ ] = L'\\';

        for ( int i = 0; i < 8; i++ ) {
            int digit = ( timestamp >> ( 28 - i * 4 ) ) & 0xF;
            name_buffer[ pos++ ] = digit < 10 ? ( L'0' + digit ) : ( L'A' + digit - 10 );
        }

        auto name_length = static_cast< std::uint16_t >( pos );
        if ( name_length == 0 )
            return nt_status_t::invalid_parameter;

        driver_name.m_length = name_length * sizeof( wchar_t );
        driver_name.m_maximum_length = driver_name.m_length + sizeof( wchar_t );
        driver_name.m_buffer = name_buffer;

        init_object_attributes(
            &obj_attributes,
            &driver_name,
            0x00000240,
            nullptr,
            nullptr
        );

        auto obj_size = sizeof( driver_object_t ) + sizeof( void* ) * 10;

        void* driver_obj_ptr = nullptr;
        status = ObCreateObject(
            0,
            IoDriverObjectType,
            &obj_attributes,
            0,
            nullptr,
            obj_size,
            0,
            0,
            &driver_obj_ptr
        );
        if ( status != nt_status_t::success || !driver_obj_ptr )
            return status;

        driver_object = static_cast< driver_object_t* >( driver_obj_ptr );

        rtl_zero_memory( driver_object, obj_size );
        driver_object->type = 4;
        driver_object->size = sizeof( driver_object_t );
        driver_object->flags = 2;

        driver_object->driver_extension = reinterpret_cast< void* >(
            reinterpret_cast< std::uint8_t* >( driver_object ) + sizeof( driver_object_t )
            );

        if ( !driver_object->driver_extension ) {
            ob_make_temporary_object( driver_object );
            ob_dereference_object( driver_object );
            return nt_status_t::insufficient_resources;
        }

        for ( int i = 0; i <= 0x1B; i++ )
            driver_object->major_function[ i ] = nullptr;

        status = entry_point( driver_object, nullptr );
        ob_make_temporary_object( driver_object );
        ob_dereference_object( driver_object );
        return status;
    }

    std::uintptr_t get_numa_node_memory( eprocess_t* process ) {
        auto node_index = *reinterpret_cast< std::uint16_t* >(
            reinterpret_cast< std::addr_t >( process ) + 0x72E );

        nt::dbg_print( "node index: %i\n", node_index );

        auto mm_physical_memory_block = g_resolver.find_global_variable(
            "48 8B 15 ? ? ? ? 25", 3 );

        nt::dbg_print( "mm physical memory block: %llx\n", mm_physical_memory_block );

        return *reinterpret_cast< std::uintptr_t* >( mm_physical_memory_block + sizeof( std::uintptr_t ) * node_index );
    }

}