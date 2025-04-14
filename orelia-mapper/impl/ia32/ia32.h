
typedef union _virt_addr_t
{
    std::uintptr_t value;
    struct
    {
        std::uint64_t offset : 12;        // 0:11
        std::uint64_t pte_index : 9;      // 12:20
        std::uint64_t pde_index : 9;      // 21:29
        std::uint64_t pdpte_index : 9;    // 30:38
        std::uint64_t pml4e_index : 9;    // 39:47
        std::uint64_t reserved : 16;      // 48:63
    };
    struct
    {
        std::uint64_t offset_4kb : 12;    // 4KB page offset
        std::uint64_t pt_offset : 9;
        std::uint64_t pd_offset : 9;
        std::uint64_t pdpt_offset : 9;
        std::uint64_t pml4_offset : 9;
        std::uint64_t reserved2 : 16;
    };
    struct
    {
        std::uint64_t offset_2mb : 21;    // 2MB page offset
        std::uint64_t pd_offset2 : 9;
        std::uint64_t pdpt_offset2 : 9;
        std::uint64_t pml4_offset2 : 9;
        std::uint64_t reserved3 : 16;
    };
    struct
    {
        std::uint64_t offset_1gb : 30;    // 1GB page offset
        std::uint64_t pdpt_offset3 : 9;
        std::uint64_t pml4_offset3 : 9;
        std::uint64_t reserved4 : 16;
    };
} virt_addr_t, *pvirt_addr_t;

typedef union _pml4e
{
    std::uint64_t value;
    struct
    {
        std::uint64_t present : 1;                   // Must be 1 if valid
        std::uint64_t read_write : 1;               // Write access control
        std::uint64_t user_supervisor : 1;           // User/supervisor access control
        std::uint64_t page_write_through : 1;        // Write-through caching
        std::uint64_t cached_disable : 1;            // Cache disable
        std::uint64_t accessed : 1;                  // Set when accessed
        std::uint64_t ignored0 : 1;                  // Ignored
        std::uint64_t large_page : 1;               // Reserved (must be 0)
        std::uint64_t ignored1 : 4;                 // Ignored
        std::uint64_t pfn : 36;                     // Physical frame number
        std::uint64_t reserved : 4;                 // Reserved for software
        std::uint64_t ignored2 : 11;                // Ignored
        std::uint64_t no_execute : 1;               // No-execute bit
    } hard;
} pml4e, * ppml4e;

typedef union _pdpte
{
    std::uint64_t value;
    struct
    {
        std::uint64_t present : 1;                   // Must be 1 if valid
        std::uint64_t read_write : 1;               // Write access control
        std::uint64_t user_supervisor : 1;           // User/supervisor access control
        std::uint64_t page_write_through : 1;        // Write-through caching
        std::uint64_t cached_disable : 1;            // Cache disable
        std::uint64_t accessed : 1;                  // Set when accessed
        std::uint64_t dirty : 1;                    // Set when written to (1GB pages)
        std::uint64_t page_size : 1;                // 1=1GB page, 0=points to page directory
        std::uint64_t ignored1 : 4;                 // Ignored
        std::uint64_t pfn : 36;                     // Physical frame number
        std::uint64_t reserved : 4;                 // Reserved for software
        std::uint64_t ignored2 : 11;                // Ignored
        std::uint64_t no_execute : 1;               // No-execute bit
    } hard;
} pdpte, * ppdpte;

typedef union _pde
{
    std::uint64_t value;
    struct
    {
        std::uint64_t present : 1;                   // Must be 1 if valid
        std::uint64_t read_write : 1;               // Write access control
        std::uint64_t user_supervisor : 1;           // User/supervisor access control
        std::uint64_t page_write_through : 1;        // Write-through caching
        std::uint64_t cached_disable : 1;            // Cache disable
        std::uint64_t accessed : 1;                  // Set when accessed
        std::uint64_t dirty : 1;                    // Set when written to (2MB pages)
        std::uint64_t page_size : 1;                // 1=2MB page, 0=points to page table
        std::uint64_t global : 1;                   // Global page (if CR4.PGE=1)
        std::uint64_t ignored1 : 3;                 // Ignored
        std::uint64_t pfn : 36;                     // Physical frame number
        std::uint64_t reserved : 4;                 // Reserved for software
        std::uint64_t ignored2 : 11;                // Ignored
        std::uint64_t no_execute : 1;               // No-execute bit
    } hard;
} pde, * ppde;

typedef union _pte
{
    std::uint64_t value;
    struct
    {
        std::uint64_t present : 1;                   // Must be 1 if valid
        std::uint64_t read_write : 1;               // Write access control
        std::uint64_t user_supervisor : 1;           // User/supervisor access control
        std::uint64_t page_write_through : 1;        // Write-through caching
        std::uint64_t cached_disable : 1;            // Cache disable
        std::uint64_t accessed : 1;                  // Set when accessed
        std::uint64_t dirty : 1;                    // Set when written to
        std::uint64_t pat : 1;                      // Page Attribute Table bit
        std::uint64_t global : 1;                   // Global page
        std::uint64_t ignored1 : 3;                 // Ignored
        std::uint64_t pfn : 36;                     // Physical frame number
        std::uint64_t reserved : 4;                 // Reserved for software
        std::uint64_t ignored2 : 7;                 // Ignored
        std::uint64_t protection_key : 4;           // Protection key
        std::uint64_t no_execute : 1;               // No-execute bit
    } hard;
} pte, * ppte;

typedef union _cr3 {
    std::uint64_t flags;

    struct {
        std::uint64_t pcid : 12;
        std::uint64_t page_frame_number : 36;
        std::uint64_t reserved_1 : 12;
        std::uint64_t reserved_2 : 3;
        std::uint64_t pcid_invalidate : 1;
    };
} cr3 , * pcr3;

enum pe_magic_t {
    dos_header = 0x5a4d,
    nt_headers = 0x4550,
    opt_header = 0x020b
};

struct dos_header_t {
    std::int16_t m_magic;
    std::int16_t m_cblp;
    std::int16_t m_cp;
    std::int16_t m_crlc;
    std::int16_t m_cparhdr;
    std::int16_t m_minalloc;
    std::int16_t m_maxalloc;
    std::int16_t m_ss;
    std::int16_t m_sp;
    std::int16_t m_csum;
    std::int16_t m_ip;
    std::int16_t m_cs;
    std::int16_t m_lfarlc;
    std::int16_t m_ovno;
    std::int16_t m_res0[ 0x4 ];
    std::int16_t m_oemid;
    std::int16_t m_oeminfo;
    std::int16_t m_res1[ 0xa ];
    std::int32_t m_lfanew;

    [[ nodiscard ]]
    constexpr bool is_valid( ) {
        return m_magic == pe_magic_t::dos_header;
    }
};

struct data_directory_t {
    std::int32_t m_virtual_address;
    std::int32_t m_size;

    template< class type_t >
    [[ nodiscard ]]
    type_t as_rva(
        std::uint64_t rva
    ) {
        return reinterpret_cast< type_t >( rva + m_virtual_address );
    }
};

struct import_descriptor_t {
    union {
        std::uint32_t m_characteristics;
        std::uint32_t m_original_first_thunk;
    };
    std::uint32_t m_time_date_stamp;
    std::uint32_t m_forwarder_chain;
    std::uint32_t m_name;
    std::uint32_t m_first_thunk;
};

struct nt_headers_t {
    std::int32_t m_signature;
    std::int16_t m_machine;
    std::int16_t m_number_of_sections;
    std::int32_t m_time_date_stamp;
    std::int32_t m_pointer_to_symbol_table;
    std::int32_t m_number_of_symbols;
    std::int16_t m_size_of_optional_header;
    std::int16_t m_characteristics;

    std::int16_t m_magic;
    std::int8_t m_major_linker_version;
    std::int8_t m_minor_linker_version;
    std::int32_t m_size_of_code;
    std::int32_t m_size_of_initialized_data;
    std::int32_t m_size_of_uninitialized_data;
    std::int32_t m_address_of_entry_point;
    std::int32_t m_base_of_code;
    std::uint64_t m_image_base;
    std::int32_t m_section_alignment;
    std::int32_t m_file_alignment;
    std::int16_t m_major_operating_system_version;
    std::int16_t m_minor_operating_system_version;
    std::int16_t m_major_image_version;
    std::int16_t m_minor_image_version;
    std::int16_t m_major_subsystem_version;
    std::int16_t m_minor_subsystem_version;
    std::int32_t m_win32_version_value;
    std::int32_t m_size_of_image;
    std::int32_t m_size_of_headers;
    std::int32_t m_check_sum;
    std::int16_t m_subsystem;
    std::int16_t m_dll_characteristics;
    std::uint64_t m_size_of_stack_reserve;
    std::uint64_t m_size_of_stack_commit;
    std::uint64_t m_size_of_heap_reserve;
    std::uint64_t m_size_of_heap_commit;
    std::int32_t m_loader_flags;
    std::int32_t m_number_of_rva_and_sizes;

    data_directory_t m_export_table;
    data_directory_t m_import_table;
    data_directory_t m_resource_table;
    data_directory_t m_exception_table;
    data_directory_t m_certificate_table;
    data_directory_t m_base_relocation_table;
    data_directory_t m_debug;
    data_directory_t m_architecture;
    data_directory_t m_global_ptr;
    data_directory_t m_tls_table;
    data_directory_t m_load_config_table;
    data_directory_t m_bound_import;
    data_directory_t m_iat;
    data_directory_t m_delay_import_descriptor;
    data_directory_t m_clr_runtime_header;
    data_directory_t m_reserved;

    [[ nodiscard ]]
    constexpr bool is_valid( ) {
        return m_signature == pe_magic_t::nt_headers
            && m_magic == pe_magic_t::opt_header;
    }
};

struct export_directory_t {
    std::int32_t m_characteristics;
    std::int32_t m_time_date_stamp;
    std::int16_t m_major_version;
    std::int16_t m_minor_version;
    std::int32_t m_name;
    std::int32_t m_base;
    std::int32_t m_number_of_functions;
    std::int32_t m_number_of_names;
    std::int32_t m_address_of_functions;
    std::int32_t m_address_of_names;
    std::int32_t m_address_of_names_ordinals;
};

struct section_header_t {
    char m_name[ 0x8 ];
    union {
        std::int32_t m_physical_address;
        std::int32_t m_virtual_size;
    };
    std::int32_t m_virtual_address;
    std::int32_t m_size_of_raw_data;
    std::int32_t m_pointer_to_raw_data;
    std::int32_t m_pointer_to_relocations;
    std::int32_t m_pointer_to_line_numbers;
    std::int16_t m_number_of_relocations;
    std::int16_t m_number_of_line_numbers;
    std::int32_t m_characteristics;
};

struct unicode_string_t {
    std::uint16_t m_length;
    std::uint16_t m_maximum_length;
    wchar_t* m_buffer;
};

struct rtl_process_module_information_t {
    HANDLE  m_section;
    void*   m_mapped_base;
    void*   m_image_base;
    uint32_t m_image_size;
    uint32_t m_flags;
    uint16_t m_load_order_index;
    uint16_t m_init_order_index;
    uint16_t m_load_count;
    uint16_t m_offset_to_file_name;
    uint8_t  m_full_path[256];
};

struct rtl_process_modules_t {
    uint32_t    m_count;
    rtl_process_module_information_t m_modules[1];
};

struct pool_tracker_big_pages_t {
    void* m_va;
    uint16_t m_key;
    uint16_t m_pool_type;
    uint16_t m_number_of_bytes;
};

struct mm_unloaded_drivers_t {
    UNICODE_STRING m_name;
    PVOID m_module_start;
    PVOID m_module_end;
    ULONG64 m_unload_time;
};

typedef struct _MM_UNLOADED_DRIVER {
    UNICODE_STRING 	Name;
    PVOID 			ModuleStart;
    PVOID 			ModuleEnd;
    ULONG64 		UnloadTime;
} MM_UNLOADED_DRIVER, * PMM_UNLOADED_DRIVER;

struct ex_push_lock_t {
    union {
        struct {
            std::uint64_t m_locked : 1;
            std::uint64_t m_waiting : 1;
            std::uint64_t m_waking : 1;
            std::uint64_t m_multiple_shared : 1;
            std::uint64_t m_shared : 60;
        };
        std::uint64_t m_value;
        void* m_ptr;
    };
}; // Size: 0x8

struct dispatcher_header_t {
    union {
        struct {
            std::uint8_t m_type;
            union {
                std::uint8_t m_absolute_timer : 1;
                std::uint8_t m_timer_resolution : 1;
                std::uint8_t m_timer_resolution_required : 1;
                std::uint8_t m_timer_resolution_set : 1;
            };
            union {
                std::uint8_t m_inserted : 1;
                std::uint8_t m_large_stack : 1;
                std::uint8_t m_priority_boost : 1;
                std::uint8_t m_thread_control_flags;
            };
            std::uint8_t m_signal_state;
        };
        std::uint32_t m_lock;
    };
    std::uint32_t m_size;
    union {
        std::uint64_t m_reserved1;
        struct {
            std::uint8_t m_hand_size;
            std::uint8_t m_inserted_2;
        };
    };
    union {
        std::uint64_t m_signal_state_2;
        struct {
            std::uint32_t m_signal_state_3;
            std::uint32_t m_thread_apc_disable;
        };
    };
}; // Size: 0x18

struct list_entry_t {
    list_entry_t* m_flink;
    list_entry_t* m_blink;
};

struct single_list_entry_t {
    single_list_entry_t* m_next;
};

struct kwait_status_register_t {
    union {
        std::uint8_t m_flags;
        struct {
            std::uint8_t m_state : 3;
            std::uint8_t m_affinity : 1;
            std::uint8_t m_priority : 1;
            std::uint8_t m_apc : 1;
            std::uint8_t m_user_apc : 1;
            std::uint8_t m_alert : 1;
        };
    };
}; // Size: 0x1

struct ktimer_t {
    dispatcher_header_t m_header;
    std::uint64_t m_due_time;
    list_entry_t m_timer_list_entry;
    struct kdpc_t* m_dpc;
    std::uint32_t m_period;
    std::uint32_t m_processor;
    std::uint32_t m_timer_type;
}; // Size: 0x40

struct group_affinity_t {
    std::uint64_t m_mask;
    std::uint16_t m_group;
    std::uint16_t m_reserved[ 3 ];
}; // Size: 0x10

struct kevent_t {
    dispatcher_header_t m_header;
}; // Size: 0x18

struct mmsupport_t {
    list_entry_t m_work_set_exp_head;                   // +0x000
    std::uint64_t m_flags;                              // +0x010
    std::uint64_t m_last_trim_time;                     // +0x018
    union {
        std::uint64_t m_page_fault_count;
        std::uint64_t m_peak_virtual_size;
        std::uint64_t m_virtual_size;
    };                                                  // +0x020
    std::uint64_t m_min_ws_size;                       // +0x028
    std::uint64_t m_max_ws_size;                       // +0x030
    std::uint64_t m_virtual_memory_threshold;          // +0x038
    std::uint64_t m_working_set_size;                  // +0x040
    std::uint64_t m_peak_working_set_size;            // +0x048
};

struct ex_fast_ref_t {
    union {
        void* m_object;
        std::uint64_t m_ref_cnt : 4;
        std::uint64_t m_value;
    };
}; // Size: 0x8

struct kprocess_t {
    dispatcher_header_t m_header;                       // +0x000
    list_entry_t m_profile_list_head;                  // +0x018
    std::uint64_t m_directory_table_base;              // +0x028
    std::uint64_t m_flags;                             // +0x030
    std::uint64_t m_flags2;                            // +0x038
    std::uint64_t m_session_id;                        // +0x040
    mmsupport_t m_mm;                                  // +0x048
    list_entry_t m_process_list_entry;                 // +0x0E0
    std::uint64_t m_total_cycle_time;                  // +0x0F0
    std::uint64_t m_create_time;                       // +0x0F8
    std::uint64_t m_user_time;                         // +0x100
    std::uint64_t m_kernel_time;                       // +0x108
    list_entry_t m_active_process_links;               // +0x110
    std::uint64_t m_process_quota_usage[ 2 ];            // +0x120
    std::uint64_t m_process_quota_peak[ 2 ];             // +0x130
    std::uint64_t m_commit_charge;                     // +0x140
    std::uint64_t m_peak_commit_charge;                // +0x148
    std::uint64_t m_peak_virtual_size;                 // +0x150
    std::uint64_t m_virtual_size;                      // +0x158
    std::uint32_t m_exit_status;                       // +0x160
    std::uint32_t m_address_policy;                    // +0x164
};

struct eprocess_t {
    // Pcb and dispatch section (0x000 - 0x0B8)
    kprocess_t m_pcb;                                // +0x000
    ex_push_lock_t m_process_lock;                   // +0x0B0
    void* m_create_time;                            // +0x0B8

    // Handles and lists section (0x0C0 - 0x148)
    void* m_exit_time;                              // +0x0C0
    eprocess_t* m_reaper_link;                      // +0x0C8
    void* m_reaper_list_entry;                      // +0x0D0
    void* m_pcb_lock;                               // +0x0D8
    void* m_exit_status;                            // +0x0E0
    ex_push_lock_t m_address_creation_lock;          // +0x0E8
    void* m_active_threads;                         // +0x0F0
    void* m_next_page_color;                        // +0x0F8
    void* m_struct_count;                           // +0x100
    void* m_next_page_color2;                       // +0x108
    void* m_vad_root;                               // +0x110
    void* m_vad_bitmap;                             // +0x118
    void* m_vad_hint;                               // +0x120
    void* m_vad_count;                              // +0x128
    void* m_module_info;                            // +0x130
    list_entry_t m_module_list;                     // +0x138

    // Memory management section (0x148 - 0x1C8)
    void* m_page_directory;                         // +0x148
    void* m_image_file_name[ 15 ];                  // +0x150
    void* m_vad_free_hint;                          // +0x1C0
    void* m_vad_free_count;                         // +0x1C8

    // Process management section (0x1D0 - 0x2C8)
    void* m_flags;                                  // +0x1D0
    void* m_debug_port;                             // +0x1D8
    void* m_wow64_process;                          // +0x1E0
    void* m_exception_port;                         // +0x1E8
    void* m_object_table;                           // +0x1F0
    ex_fast_ref_t m_token;                          // +0x1F8
    void* m_working_set_page_count;                 // +0x200
    void* m_active_thread_count;                    // +0x208
    void* m_image_file;                             // +0x210
    void* m_job;                                    // +0x218
    void* m_section_object;                         // +0x220
    void* m_section_base_address;                   // +0x228
    void* m_peb;                                    // +0x230
    void* m_exception_port_data;                    // +0x238
    void* m_device_map;                             // +0x240
    void* m_session;                                // +0x248
    void* m_debug_data;                             // +0x250
    void* m_vdm_objects;                            // +0x258
    void* m_win32_window_station;                   // +0x260
    void* m_instrumentation_callback;                // +0x268
    void* m_win32_process;                          // +0x270
    void* m_inherited_from_unique_process_id;        // +0x278
    list_entry_t m_thread_list_head;                // +0x280
    void* m_app_compat_flags;                       // +0x290
    void* m_app_compat_flags_user;                  // +0x298
    void* m_ps_context_ptr;                         // +0x2A0
    void* m_image_version_low;                      // +0x2A8
    void* m_image_version_high;                     // +0x2B0
    void* m_flags3;                                 // +0x2B8
    void* m_vad_root3;                              // +0x2C0
    void* m_overflow_count;                         // +0x2C8

    // Security and protection section (0x2D0 - 0x358)
    void* m_protection;                             // +0x2D0
    std::uint64_t m_flags2;                         // +0x2D8
    void* m_security_port;                          // +0x2E0
    void* m_security_descriptor;                     // +0x2E8
    void* m_parent_security_descriptor;              // +0x2F0
    void* m_security_qos;                           // +0x2F8
    void* m_security_qos_flags;                     // +0x300
    void* m_image_file_mapping;                     // +0x308
    void* m_rotating_number;                        // +0x310
    void* m_fork_count;                             // +0x318
    void* m_hardware_count;                         // +0x320
    void* m_thread_fault_count;                     // +0x328
    void* m_mm_average_page_count;                  // +0x330
    void* m_process_runs;                           // +0x338
    void* m_last_switch_time;                       // +0x340
    void* m_total_switch_time;                      // +0x348
    void* m_page_fault_count;                       // +0x350
    void* m_clean_page_count;                       // +0x358

    // Process identification section (0x360 - 0x440)
    std::uint64_t m_unique_process_id;               // +0x360
    std::uint64_t m_active_process_links[ 2 ];       // +0x368
    std::uint64_t m_token_reference_count;           // +0x378
    std::uint64_t m_user_reference_count;            // +0x380
    std::uint64_t m_subsystem_type;                  // +0x388
    void* m_page_directory_base;                    // +0x390
    void* m_unique_thread_id;                       // +0x398
    void* m_session_id;                             // +0x3A0
    void* m_image_path_name[ 16 ];                  // +0x3A8
    void* m_image_name[ 16 ];                       // +0x408
    void* m_subsystem_data;                         // +0x440

    // Process environment section (0x448 - 0x4F0)
    void* m_win32k_thread;                          // +0x448
    void* m_win32k_process;                         // +0x450
    void* m_win32k_window;                          // +0x458
    void* m_win32k_parent_window;                   // +0x460
    void* m_app_compat_info;                        // +0x468
    void* m_spare_bytes[ 24 ];                      // +0x470
    void* m_pde_table;                              // +0x4D0
    void* m_shared_key;                             // +0x4D8
    void* m_shared_key_flags;                       // +0x4E0
    void* m_shared_key_value;                       // +0x4E8
    void* m_shared_key_index;                       // +0x4F0
}; // Size: 0x4F8

struct balanced_links_t {
    void* m_parent;             // 0x00
    void* m_left;               // 0x08
    void* m_right;              // 0x10
    std::uint8_t m_balance;     // 0x18
    std::uint8_t m_reserved[ 3 ]; // 0x19-0x1B
    std::uint32_t m_pad;        // 0x1C
}; // size: 0x20

struct avl_table_t {
    balanced_links_t m_balanced_root;    // 0x00-0x20
    void* m_ordered_pointer;             // 0x20
    std::uint32_t m_which_ordered_element; // 0x28
    std::uint32_t m_number_generic_table_elements; // 0x2C
    std::uint32_t m_depth_of_tree;       // 0x30
    std::uint32_t m_pad1;                // 0x34
    void* m_restart_key;                 // 0x38
    std::uint32_t m_delete_count;        // 0x40
    std::uint32_t m_pad2;                // 0x44
    void* m_compare_routine;             // 0x48
    void* m_allocate_routine;            // 0x50
    void* m_free_routine;                // 0x58
    void* m_table_context;               // 0x60
}; // size: 0x68

struct kldr_data_table_entry_t {
    list_entry_t m_load_order_links;                  // 0x00-0x10

    void* m_exception_table;               // 0x10
    std::uint32_t m_exception_table_size;  // 0x18
    std::uint32_t m_pad0;                  // 0x1C

    void* m_gp_value;                      // 0x20
    void* m_non_paged_debug_info;          // 0x28
    void* m_dll_base;                      // 0x30
    void* m_entry_point;                   // 0x38
    std::uint32_t m_size_of_image;         // 0x40
    std::uint32_t m_pad1;                  // 0x44

    struct {
        std::uint16_t m_length;            // 0x48
        std::uint16_t m_maximum_length;     // 0x4A
        wchar_t* m_buffer;                 // 0x50
    } m_full_dll_name;                     // 0x48-0x58

    struct {
        std::uint16_t m_length;            // 0x58
        std::uint16_t m_maximum_length;     // 0x5A
        wchar_t* m_buffer;                 // 0x60
    } m_base_dll_name;                     // 0x58-0x68

    std::uint32_t m_flags;                 // 0x68
    std::uint16_t m_load_count;            // 0x6C

    union {
        struct {
            std::uint16_t m_signature_level : 4;  // 0x6E
            std::uint16_t m_signature_type : 3;   // 0x6E
            std::uint16_t m_unused : 9;           // 0x6E
        };
        std::uint16_t m_entire_field;      // 0x6E
    } m_u1;                                // 0x6E

    void* m_section_pointer;               // 0x70
    std::uint32_t m_checksum;             // 0x78
    std::uint32_t m_coverage_section_size; // 0x7C
    void* m_coverage_section;              // 0x80
    void* m_loaded_imports;                // 0x88
    void* m_spare;                         // 0x90
    std::uint32_t m_size_of_image_not_rounded; // 0x98
    std::uint32_t m_time_date_stamp;      // 0x9C
}; // size: 0xA0

struct apic_icr {
    union {
        uint64_t m_raw; // Full 64-bit ICR value
        struct {
            uint32_t m_vector : 8;            // Interrupt vector (0x00 to 0xFF)
            uint32_t m_delivery_mode : 3;     // Delivery mode (e.g., fixed = 0)
            uint32_t m_destination_mode : 1;  // Destination mode (0 = physical, 1 = logical)
            uint32_t m_delivery_status : 1;   // Delivery status (0 = idle, 1 = send pending)
            uint32_t m_reserved1 : 1;         // Reserved (set to 0)
            uint32_t m_level : 1;             // Level (0 = deassert, 1 = assert)
            uint32_t m_trigger_mode : 1;      // Trigger mode (0 = edge, 1 = level)
            uint32_t m_reserved2 : 2;         // Reserved (set to 0)
            uint32_t m_destination_shorthand : 2; // Destination shorthand (e.g., self = 1)
            uint32_t m_reserved3 : 12;        // Reserved (set to 0)

            uint32_t m_reserved4 : 24;        // Reserved (set to 0)
            uint32_t m_destination : 8;       // Destination processor (physical mode)
        } m_fields;
    };
};

struct idt_entry_t {
    uint16_t m_offset_low;
    uint16_t m_selector;
    uint8_t m_ist;
    uint8_t m_attributes;
    uint16_t m_offset_middle;
    uint32_t m_offset_high;
    uint32_t m_reserved;
};

struct physical_address_t {
    union {
        struct {
            std::uint32_t m_low_part;      // +0x000
            std::int32_t m_high_part;      // +0x004
        };
        struct {
            std::uint64_t m_quad_part;     // +0x000
        };
    };
}; // Size: 0x008

struct eresource_t {
    struct {
        void* m_flink;             // 0x00
        void* m_blink;             // 0x08
    } m_system_resource_list;      // 0x00-0x10

    void* m_owner_table;           // 0x10
    std::uint16_t m_active_count;  // 0x18
    union {
        std::uint16_t m_flag;      // 0x1A
        struct {
            std::uint8_t m_shared_wait_count;   // 0x1A
            std::uint8_t m_exclusive_wait_count; // 0x1B
        };
    };
    std::uint32_t m_pad1;          // 0x1C

    void* m_shared_waiters;        // 0x20
    void* m_exclusive_waiters;     // 0x28

    struct {
        void* m_owner_thread;      // 0x30
        void* m_owner_count;       // 0x38
    } m_owner_entry;               // 0x30-0x40

    std::uint32_t m_active_entries;     // 0x40
    std::uint32_t m_contention_count;   // 0x44
    std::uint32_t m_shared_waiter_count;    // 0x48
    std::uint32_t m_exclusive_waiter_count; // 0x4C

    std::uint8_t m_misc_flags;          // 0x50
    std::uint8_t m_reserved[ 3 ];         // 0x51-0x53
    std::uint32_t m_timeout_count;      // 0x54

    union {
        void* m_descriptor;             // 0x58
        std::uint32_t m_converted_type; // 0x58
    };

    std::uint64_t m_spin_lock;         // 0x60
}; // size: 0x68

struct piddb_cache_entry_t {
    list_entry_t m_list;           // 0x00-0x10
    UNICODE_STRING m_driver_name;  // 0x10-0x1C
    std::uint32_t m_timestamp;     // 0x20
    std::int32_t m_load_status;    // 0x24
    std::uint8_t m_shim_data[ 16 ];// 0x28-0x38
}; // size: 0x38