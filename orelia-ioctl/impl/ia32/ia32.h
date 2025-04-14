#pragma once
constexpr auto ia32_gs_base = 0xC0000101;
constexpr auto ia32_kernel_gs_base = 0xC0000102;
constexpr auto ia32_fs_base = 0xC0000100;

enum class system_information_class : std::uint32_t {
    system_basic_information = 0,
    system_module_information = 11
};

enum class device_type_t : unsigned long {
    beep = 0x00000001,
    cd_rom = 0x00000002,
    cd_rom_file_system = 0x00000003,
    controller = 0x00000004,
    datalink = 0x00000005,
    dfs = 0x00000006,
    disk = 0x00000007,
    disk_file_system = 0x00000008,
    file_system = 0x00000009,
    inport_port = 0x0000000a,
    keyboard = 0x0000000b,
    mailslot = 0x0000000c,
    midi_in = 0x0000000d,
    midi_out = 0x0000000e,
    mouse = 0x0000000f,
    multi_unc_provider = 0x00000010,
    named_pipe = 0x00000011,
    network = 0x00000012,
    network_browser = 0x00000013,
    network_file_system = 0x00000014,
    null = 0x00000015,
    parallel_port = 0x00000016,
    physical_netcard = 0x00000017,
    printer = 0x00000018,
    scanner = 0x00000019,
    serial_mouse_port = 0x0000001a,
    serial_port = 0x0000001b,
    screen = 0x0000001c,
    sound = 0x0000001d,
    streams = 0x0000001e,
    tape = 0x0000001f,
    tape_file_system = 0x00000020,
    transport = 0x00000021,
    unknown = 0x00000022,
    video = 0x00000023,
    virtual_disk = 0x00000024,
    wave_in = 0x00000025,
    wave_out = 0x00000026,
    port_8042 = 0x00000027,
    network_redirector = 0x00000028,
    battery = 0x00000029,
    bus_extender = 0x0000002a,
    modem = 0x0000002b,
    vdm = 0x0000002c,
    mass_storage = 0x0000002d,
    smb = 0x0000002e,
    ks = 0x0000002f,
    changer = 0x00000030,
    smartcard = 0x00000031,
    acpi = 0x00000032,
    dvd = 0x00000033,
    full_text_index = 0x00000034,
    dfs_file_system = 0x00000035,
    dfs_volume = 0x00000036,
    serenum = 0x00000037,
    termsrv = 0x00000038,
    ksec = 0x00000039,
    fips = 0x0000003a,
    infiniband = 0x0000003b
};

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
} virt_addr_t, *pvirt_addr_t;

typedef union _pml4e
{
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
    std::uint64_t value;
} pml4e, * ppml4e;

typedef union _pdpte
{
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
    std::uint64_t value;
} pdpte, * ppdpte;

typedef union _pde
{
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
    std::uint64_t value;
} pde, * ppde;

typedef union _pte
{
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
    std::uint64_t value;
} pte, * ppte;

typedef union _cr3 {
    std::uint64_t flags;

    struct {
        std::uint64_t pcid : 12;
        std::uint64_t pfn : 36;
        std::uint64_t reserved_1 : 12;
        std::uint64_t reserved_2 : 3;
        std::uint64_t pcid_invalidate : 1;
    };
} cr3 , * pcr3;

struct slist_header_t {
    union {
        std::uint64_t m_alignment;
        struct {
            single_list_entry_t m_next;
            std::uint16_t m_depth;
            std::uint16_t m_sequence;
        };
    };
};

union ularge_integer_t
{
    struct
    {
        std::uint32_t  m_low_part;                                                      //0x0
        std::uint32_t  m_high_part;                                                     //0x4
    };
    struct
    {
        std::uint32_t  m_low_part;                                                      //0x0
        std::uint32_t  m_high_part;                                                     //0x4
    } u;                                                                    //0x0
    std::uint64_t m_quad_part;                                                     //0x0
};

struct machine_frame_t
{
    std::uint64_t m_rip;
    std::uint64_t m_cs;
    std::uint64_t m_eflags;
    std::uint64_t m_rsp;
    std::uint64_t m_ss;
};

// Task State Segment structure
struct ktss_t {
    std::uint32_t m_reserved0;
    std::uint64_t m_rsp0;
    std::uint64_t m_rsp1;
    std::uint64_t m_rsp2;
    std::uint64_t m_reserved1;
    std::uint64_t m_ist[8];
    std::uint64_t m_reserved2;
    std::uint16_t m_reserved3;
    std::uint16_t m_io_map_base;
};

// DPC structure
struct kdpc_t {
    std::uint16_t m_type;
    std::uint8_t m_importance;
    std::uint8_t m_number;
    list_entry_t m_dpc_list_entry;
    void* m_deferred_routine;
    void* m_deferred_context;
    void* m_system_argument1;
    void* m_system_argument2;
    void* m_dpc_data;
};

struct ldr_data_table_entry_t {
    list_entry_t m_in_load_order_module_list;
    list_entry_t m_in_memory_order_module_list;
    list_entry_t m_in_initialization_order_module_list;
    void* m_dll_base;
    void* m_entry_point;
    std::uint32_t m_size_of_image;
    unicode_string_t m_full_dll_name;
    unicode_string_t m_base_dll_name;
    std::uint32_t m_flags;
    std::uint16_t m_load_count;
    std::uint16_t m_tls_index;
    list_entry_t m_hash_links;
    void* m_section_pointer;
    std::uint32_t m_check_sum;
    std::uint32_t m_time_date_stamp;
};

struct rtl_critical_section_t {
    void* m_debug_info;
    std::int32_t m_lock_count;
    std::int32_t m_recursion_count;
    void* m_owning_thread;
    void* m_lock_semaphore;
    std::uint32_t m_spin_count;
};

struct peb_ldr_data_t {
    std::uint32_t m_length;
    bool m_initialized;
    void* m_ss_handle;
    list_entry_t m_module_list_load_order;
    list_entry_t m_module_list_memory_order;
    list_entry_t m_module_list_in_it_order;
};

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

struct ex_fast_ref_t {
    union {
        void* m_object;
        std::uint64_t m_ref_cnt : 4;
        std::uint64_t m_value;
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

struct object_header_t {
    std::int64_t m_pointer_count;
    union {
        std::int64_t m_handle_count;
        void* m_next_to_free;
    };
    std::uint8_t m_type_index;
    std::uint8_t m_flags;
    std::uint8_t m_name_info_offset;
    std::uint8_t m_handle_info_offset;
    std::uint8_t m_quota_info_offset;
    std::uint8_t m_process_info_offset;
};

struct ex_rundown_ref_t {
    union {
        std::uint64_t m_count;                    // Size=0x8
        void* m_ptr;                              // Size=0x8
    };
};

struct rtl_avl_tree_t {
    void* m_root;                                 // Size=0x8
};

struct se_audit_process_creation_info_t {
    unicode_string_t* m_image_file_name;    // Pointer to UNICODE_STRING
};

struct eprocess_t {
    kprocess_t m_pcb;                                                     // +0x000
    ex_push_lock_t m_process_lock;                                       // +0x438
    void* m_unique_process_id;                                           // +0x440
    list_entry_t m_active_process_links;                                // +0x448
    ex_rundown_ref_t m_rundown_protect;                                 // +0x458

    union {
        std::uint32_t m_flags2;                                         // +0x460
        struct {
            std::uint32_t m_job_not_really_active : 1;
            std::uint32_t m_accounting_folded : 1;
            std::uint32_t m_new_process_reported : 1;
            std::uint32_t m_exit_process_reported : 1;
            std::uint32_t m_report_commit_changes : 1;
            std::uint32_t m_last_report_memory : 1;
            std::uint32_t m_force_wake_charge : 1;
            std::uint32_t m_cross_session_create : 1;
            std::uint32_t m_needs_handle_rundown : 1;
            std::uint32_t m_ref_trace_enabled : 1;
            std::uint32_t m_force_ws_watch : 1;
            std::uint32_t m_create_reported : 1;
            std::uint32_t m_default_io_priority : 3;
            std::uint32_t m_spare_bits : 17;
        };
    };

    union {
        std::uint32_t m_flags;                                          // +0x464
        struct {
            std::uint32_t m_create_time_reported : 1;
            std::uint32_t m_image_not_loaded : 1;
            std::uint32_t m_process_exiting : 1;
            std::uint32_t m_process_delete : 1;
            std::uint32_t m_wow64_split_pages : 1;
            std::uint32_t m_vm_deleted : 1;
            std::uint32_t m_outswap_enabled : 1;
            std::uint32_t m_outswapped : 1;
            std::uint32_t m_fork_failed : 1;
            std::uint32_t m_has_address_space : 1;
            std::uint32_t m_address_space_initialized : 2;
            std::uint32_t m_set_timer_resolution : 1;
            std::uint32_t m_break_on_termination : 1;
            std::uint32_t m_dependent_on_session : 1;
            std::uint32_t m_auto_alignment : 1;
            std::uint32_t m_prefer_32bit : 1;
            std::uint32_t m_wow64_valid : 1;
            std::uint32_t m_cross_session_create : 1;
            std::uint32_t m_spare_flags0 : 13;
        };
    };

    std::int64_t m_create_time;                                         // +0x468
    std::uint64_t m_process_quota_usage[ 2 ];                            // +0x470
    std::uint64_t m_process_quota_peak[ 2 ];                             // +0x480
    std::uint64_t m_peak_virtual_size;                                 // +0x490
    std::uint64_t m_virtual_size;                                      // +0x498
    list_entry_t m_session_process_links;                              // +0x4A0
    union {
        void* m_exception_port;                                         // +0x4B0
        std::uint64_t m_exception_port_value;                          // +0x4B0
    };
    ex_fast_ref_t m_token;                                             // +0x4B8
    std::uint64_t m_working_set_page_count;                           // +0x4C0
    ex_push_lock_t m_address_creation_lock;                            // +0x4C8
    ex_push_lock_t m_page_table_commit_lock;                          // +0x4D0
    void* m_rotate_in_progress;                                        // +0x4D8
    void* m_fork_in_progress;                                          // +0x4E0
    std::uint64_t m_hardware_counters;                                // +0x4E8
    void* m_spare_ptr0;                                                // +0x4F0
    std::uint64_t m_spare_ulong0;                                     // +0x4F8
    std::uint64_t m_spare_ulong1;                                     // +0x500
    std::uint64_t m_spare_ulong2;                                     // +0x508
    std::uint64_t m_spare_ulong3;                                     // +0x510
    void* m_section_object;                                            // +0x518
    void* m_section_base_address;                                      // +0x520
    std::uint32_t m_cookie;                                           // +0x528
    std::uint32_t m_padding1;                                         // +0x52C
    void* m_working_set_watch;                                         // +0x530
    void* m_win32_window_station;                                      // +0x538
    void* m_inherited_from_unique_process_id;                          // +0x540
    void* m_peb;                                                       // +0x548
    void* m_session;                                                   // +0x550
    void* m_spare1;                                                    // +0x558
    void* m_quota_block;                                               // +0x560
    void* m_object_table;                                              // +0x568
    void* m_debug_port;                                                // +0x570
    void* m_wow64_process;                                             // +0x578
    ex_fast_ref_t m_device_map;                                        // +0x580
    void* m_etw_data_source;                                           // +0x588
    std::uint64_t m_page_directory_pte;                               // +0x590
    void* m_image_file_pointer;                                        // +0x598
    char m_image_file_name[ 15 ];                                        // +0x5A0
    std::uint8_t m_priority_class;                                     // +0x5AF
    void* m_security_port;                                             // +0x5B0
    se_audit_process_creation_info_t m_se_audit_process_creation_info; // +0x5B8
    list_entry_t m_job_links;                                         // +0x5C0
    void* m_spare2;                                                    // +0x5D0
    list_entry_t m_thread_list_head;                                  // +0x5D8
    std::uint32_t m_active_threads;                                   // +0x5E8
    std::uint32_t m_image_path_hash;                                  // +0x5EC
    std::uint32_t m_default_harderror_processing;                     // +0x5F0
    std::int32_t m_last_thread_exit_status;                          // +0x5F4
    void* m_pde_table;                                                // +0x5F8
};

struct kaffinity_ex_t {
    std::uint16_t m_count;          // +0x000 Count
    std::uint16_t m_size;           // +0x002 Size
    std::uint16_t m_reserved;       // +0x004 Reserved
    std::uint16_t m_maximum;        // +0x006 Maximum
    std::uint64_t m_bitmap[ 20 ];     // +0x008 Bitmap array
};  // Size: 0xA8 bytes

struct peb_t {
    std::uint8_t m_inherited_address_space;
    std::uint8_t m_read_image_file_exec_options;
    std::uint8_t m_being_debugged;
    std::uint8_t m_bit_field;

    struct {
        std::uint32_t m_image_uses_large_pages : 1;
        std::uint32_t m_is_protected_process : 1;
        std::uint32_t m_is_legacy_process : 1;
        std::uint32_t m_is_image_dynamically_relocated : 1;
        std::uint32_t m_spare_bits : 4;
    };

    void* m_mutant;
    void* m_image_base_address;
    peb_ldr_data_t m_ldr;
    void* m_process_parameters;
    void* m_subsystem_data;
    void* m_process_heap;
    rtl_critical_section_t* m_fast_peb_lock;
    void* m_atl_thunk_slist_ptr;
    void* m_ifeo_key;

    struct {
        std::uint32_t m_process_in_job : 1;
        std::uint32_t m_process_initializing : 1;
        std::uint32_t m_reserved_bits0 : 30;
    } m_cross_process_flags;

    union {
        void* m_kernel_callback_table;
        void* m_user_shared_info_ptr;
    };

    std::uint32_t m_system_reserved[ 1 ];
    std::uint32_t m_spare_ulong;
    void* m_free_list;
    std::uint32_t m_tls_expansion_counter;
    void* m_tls_bitmap;
    std::uint32_t m_tls_bitmap_bits[ 2 ];
    void* m_read_only_shared_memory_base;
    void* m_hotpatch_information;
    void** m_read_only_static_server_data;
    void* m_ansi_code_page_data;
    void* m_oem_code_page_data;
    void* m_unicode_case_table_data;
    std::uint32_t m_number_of_processors;
    std::uint32_t m_nt_global_flag;
    std::int64_t m_critical_section_timeout;
    std::uint32_t m_heap_segment_reserve;
    std::uint32_t m_heap_segment_commit;
    std::uint32_t m_heap_decomit_total_free_threshold;
    std::uint32_t m_heap_decomit_free_block_threshold;
    std::uint32_t m_number_of_heaps;
    std::uint32_t m_maximum_number_of_heaps;
    void** m_process_heaps;
    void* m_gdi_shared_handle_table;
    void* m_process_starter_helper;
    std::uint32_t m_gdi_dc_attribute_list;
    rtl_critical_section_t* m_loader_lock;
    std::uint32_t m_os_major_version;
    std::uint32_t m_os_minor_version;
    std::uint16_t m_os_build_number;
    std::uint16_t m_os_csd_version;
    std::uint32_t m_os_platform_id;
    std::uint32_t m_image_subsystem;
    std::uint32_t m_image_subsystem_major_version;
    std::uint32_t m_image_subsystem_minor_version;
    std::uint32_t m_image_process_affinity_mask;
    std::uint32_t m_gdi_handle_buffer[ 34 ];
    void* m_post_process_init_routine;
    void* m_tls_expansion_bitmap;
    std::uint32_t m_tls_expansion_bitmap_bits[ 32 ];
    std::uint32_t m_session_id;
    std::uint64_t m_app_compat_flags;
    std::uint64_t m_app_compat_flags_user;
    void* m_p_shim_data;
    void* m_app_compat_info;
    unicode_string_t m_csd_version;
    void* m_activation_context_data;
    void* m_process_assembly_storage_map;
    void* m_system_default_activation_context_data;
    void* m_system_assembly_storage_map;
    std::uint32_t m_minimum_stack_commit;
    void* m_fls_callback;
    list_entry_t m_fls_list_head;
    void* m_fls_bitmap;
    std::uint32_t m_fls_bitmap_bits[ 4 ];
    std::uint32_t m_fls_high_index;
    void* m_wer_registration_data;
    void* m_wer_ship_assert_ptr;
};

//0x30 bytes (sizeof)
struct kwait_block_t
{
    struct list_entry_t WaitListEntry;                                       //0x0
    std::uint8_t WaitType;                                                         //0x10
    volatile std::uint8_t BlockState;                                              //0x11
    std::uint16_t WaitKey;                                                         //0x12
    std::int32_t SpareLong;                                                         //0x14
    union
    {
        struct kthread* Thread;                                            //0x18
        struct _KQUEUE* NotificationQueue;                                  //0x18
    };
    void* Object;                                                           //0x20
    void* SparePtr;                                                         //0x28
};

struct large_integer_t {
    union {
        struct {
            std::uint32_t m_low_part;
            std::int32_t m_high_part;
        };
        struct {
            std::uint32_t m_low_part;
            std::int32_t m_high_part;
        } u;
        std::int64_t m_quad_part;
    };
};

enum class pool_type_t : std::uint32_t {
    NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed,
    DontUseThisType,
    NonPagedPoolCacheAligned,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS,
    MaxPoolType,
    NonPagedPoolBase = 0,
    NonPagedPoolBaseMustSucceed = 2,
    NonPagedPoolBaseCacheAligned = 4,
    NonPagedPoolBaseCacheAlignedMustS = 6
};

enum class processor_cache_type_t : std::uint32_t {
    Unified = 0,
    Instruction = 1,
    Data = 2,
    Trace = 3
};

struct kdpc_data_t {
    list_entry_t m_dpc_list_head;
    std::uint32_t m_dpc_list_lock;
    std::uint32_t m_dpc_queue_depth;
    std::uint32_t m_dpc_count;
};

struct kgate_t {
    dispatcher_header_t m_header;
};

struct cache_descriptor_t {
    std::uint8_t m_level;
    std::uint8_t m_associativity;
    std::uint16_t m_line_size;
    std::uint32_t m_size;
    processor_cache_type_t m_type;
};

struct rtl_rb_tree_t {
    void* m_root;
    void* m_min;
};

struct processor_power_state_t {
    void* m_idle_function;
    std::uint32_t m_idle_state_max;
    std::uint32_t m_last_idle_check;
    std::uint32_t m_last_thermal_interval;
    struct {
        std::uint32_t m_idle_check : 1;
        std::uint32_t m_thermal_check : 1;
        std::uint32_t m_thermal_active : 1;
        std::uint32_t m_reserved : 29;
    } m_flags;
    std::uint32_t m_last_idle_duration;
    std::uint32_t m_idle_sum;
    std::uint32_t m_idle_count;
    std::uint32_t m_idle_average;
    std::uint32_t m_thermal_sum;
    std::uint32_t m_thermal_count;
    std::uint32_t m_thermal_average;
    std::uint32_t m_thermal_interval;
    std::uint32_t m_reserved[ 8 ];
};

struct kprocessor_state_t {
    struct {
        std::uint64_t m_rax;
        std::uint64_t m_rbx;
        std::uint64_t m_rcx;
        std::uint64_t m_rdx;
        std::uint64_t m_rsi;
        std::uint64_t m_rdi;
        std::uint64_t m_rbp;
        std::uint64_t m_rsp;
        std::uint64_t m_r8;
        std::uint64_t m_r9;
        std::uint64_t m_r10;
        std::uint64_t m_r11;
        std::uint64_t m_r12;
        std::uint64_t m_r13;
        std::uint64_t m_r14;
        std::uint64_t m_r15;
        std::uint64_t m_rip;
        std::uint64_t m_rflags;
        std::uint64_t m_cs;
        std::uint64_t m_ss;
        std::uint64_t m_ds;
        std::uint64_t m_es;
        std::uint64_t m_fs;
        std::uint64_t m_gs;
    } m_context_frame;
    std::uint16_t m_segment_registers[ 6 ];
    std::uint32_t m_reserved[ 6 ];
};

struct kspin_lock_queue_t {
    void* m_next;
    void* m_lock;
};

struct ktimer_table_t {
    std::uint64_t m_timer_expiry;
    kdpc_t m_timer_dpc;
    std::uint64_t m_timer_entries[ 256 ];
};

struct ktimer_expiration_trace_t {
    std::uint64_t m_time;
    void* m_thread;
};

struct kstatic_affinity_block_t {
    std::uint64_t m_bitmap[ 64 ];
};

struct kshared_ready_queue_t {
    std::uint32_t m_lock;
    std::uint32_t m_owner;
    std::uint32_t m_current_size;
    std::uint32_t m_maximum_size;
    list_entry_t m_list_head;
};

struct ksecure_fault_information_t {
    std::uint32_t m_fault_type;
    std::uint32_t m_reserved;
    std::uint64_t m_virtual_address;
};

struct klock_queue_handle_t {
    kspin_lock_queue_t m_lock_queue;
    std::uint8_t m_old_irql;
};

struct kentropy_timing_state_t {
    std::uint64_t m_enter_time;
    std::uint64_t m_enter_cycles;
    std::uint64_t m_reserved[ 2 ];
};

struct pp_lookaside_list_t {
    struct {
        slist_header_t m_list_head;
        std::uint16_t m_depth;
        std::uint16_t m_maximum_depth;
        std::uint32_t m_total_allocates;
        union {
            std::uint32_t m_allocate_misses;
            std::uint32_t m_allocate_hits;
        };
        std::uint32_t m_total_frees;
        union {
            std::uint32_t m_free_misses;
            std::uint32_t m_free_hits;
        };
        pool_type_t m_pool_type;
        std::uint32_t m_tag;
        std::uint32_t m_size;
        void* m_allocate_ex;
        void* m_free_ex;
        list_entry_t m_list_entry;
    } m_p;
    struct {
        std::uint32_t m_total_allocates;
        union {
            std::uint32_t m_allocate_misses;
            std::uint32_t m_allocate_hits;
        };
        std::uint32_t m_total_frees;
        union {
            std::uint32_t m_free_misses;
            std::uint32_t m_free_hits;
        };
    } m_l;
};

struct general_lookaside_pool_t {
    slist_header_t m_list_head;
    std::uint16_t m_depth;
    std::uint16_t m_maximum_depth;
    std::uint32_t m_total_allocates;
    union {
        std::uint32_t m_allocate_misses;
        std::uint32_t m_allocate_hits;
    };
    std::uint32_t m_total_frees;
    union {
        std::uint32_t m_free_misses;
        std::uint32_t m_free_hits;
    };
    pool_type_t m_pool_type;
    std::uint32_t m_tag;
    std::uint32_t m_size;
    void* m_allocate_ex;
    void* m_free_ex;
    list_entry_t m_list_entry;
};

struct filesystem_disk_counters_t {
    std::uint32_t m_fs_read_operations;
    std::uint32_t m_fs_write_operations;
    std::uint32_t m_fs_other_operations;
    std::uint32_t m_fs_read_bytes;
    std::uint32_t m_fs_write_bytes;
    std::uint32_t m_fs_other_bytes;
};

#pragma pack(push, 8)  // Use 8-byte alignment for x64 Windows

struct iop_irp_stack_profiler_t {
    std::uint8_t m_major_function;
    std::uint8_t m_minor_function;
    std::uint8_t m_flags;
    std::uint8_t m_control;

    union {
        // For NtCreateFile
        struct {
            void* m_security_context;
            std::uint32_t m_options;
            std::uint16_t m_file_attributes;
            std::uint16_t m_share_access;
            std::uint32_t m_ea_length;
        } m_create;

        // For NtCreateNamedPipeFile
        struct {
            void* m_security_context;
            std::uint32_t m_options;
            std::uint16_t m_reserved;
            std::uint16_t m_share_access;
            void* m_parameters;
        } m_create_pipe;

        // For NtCreateMailslotFile
        struct {
            void* m_security_context;
            std::uint32_t m_options;
            std::uint16_t m_reserved;
            std::uint16_t m_share_access;
            void* m_parameters;
        } m_create_mailslot;

        // For NtReadFile
        struct {
            std::uint32_t m_length;
            std::uint32_t m_key;
#if defined(_WIN64)
            std::uint32_t m_flags;
#endif
            std::int64_t m_byte_offset;
        } m_read;

        // For NtWriteFile
        struct {
            std::uint32_t m_length;
            std::uint32_t m_key;
#if defined(_WIN64)
            std::uint32_t m_flags;
#endif
            std::int64_t m_byte_offset;
        } m_write;

        // For NtQueryDirectoryFile
        struct {
            std::uint32_t m_length;
            void* m_file_name;
            std::uint32_t m_file_information_class;
            std::uint32_t m_file_index;
        } m_query_directory;

        // For NtNotifyChangeDirectoryFile
        struct {
            std::uint32_t m_length;
            std::uint32_t m_completion_filter;
        } m_notify_directory;

        // For NtNotifyChangeDirectoryFileEx
        struct {
            std::uint32_t m_length;
            std::uint32_t m_completion_filter;
            std::uint32_t m_directory_notify_information_class;
        } m_notify_directory_ex;

        // For NtQueryInformationFile
        struct {
            std::uint32_t m_length;
            std::uint32_t m_file_information_class;
        } m_query_file;

        // For NtSetInformationFile
        struct {
            std::uint32_t m_length;
            std::uint32_t m_file_information_class;
            void* m_file_object;
            union {
                struct {
                    std::uint8_t m_replace_if_exists;
                    std::uint8_t m_advance_only;
                };
                std::uint32_t m_cluster_count;
                void* m_delete_handle;
            };
        } m_set_file;

        // For NtQueryEaFile
        struct {
            std::uint32_t m_length;
            void* m_ea_list;
            std::uint32_t m_ea_list_length;
            std::uint32_t m_ea_index;
        } m_query_ea;

        // For NtSetEaFile
        struct {
            std::uint32_t m_length;
        } m_set_ea;

        // For NtQueryVolumeInformationFile
        struct {
            std::uint32_t m_length;
            std::uint32_t m_fs_information_class;
        } m_query_volume;

        // For NtSetVolumeInformationFile
        struct {
            std::uint32_t m_length;
            std::uint32_t m_fs_information_class;
        } m_set_volume;

        // For NtFsControlFile
        struct {
            std::uint32_t m_output_buffer_length;
            std::uint32_t m_input_buffer_length;
            std::uint32_t m_fs_control_code;
            void* m_type3_input_buffer;
        } m_file_system_control;

        // For NtLockFile/NtUnlockFile
        struct {
            void* m_length;
            std::uint32_t m_key;
            std::int64_t m_byte_offset;
        } m_lock_control;

        // For NtDeviceIoControlFile
        struct {
            std::uint32_t m_output_buffer_length;
            std::uint32_t m_input_buffer_length;
            std::uint32_t m_io_control_code;
            void* m_type3_input_buffer;
        } m_device_io_control;

        // For NtQuerySecurityObject
        struct {
            std::uint32_t m_security_information;
            std::uint32_t m_length;
        } m_query_security;

        // For NtSetSecurityObject
        struct {
            std::uint32_t m_security_information;
            void* m_security_descriptor;
        } m_set_security;

        // For MountVolume
        struct {
            void* m_vpb;
            void* m_device_object;
            std::uint32_t m_output_buffer_length;
        } m_mount_volume;

        // For VerifyVolume
        struct {
            void* m_vpb;
            void* m_device_object;
        } m_verify_volume;

        // For Scsi with internal device control
        struct {
            void* m_srb;
        } m_scsi;

        // For NtQueryQuotaInformationFile
        struct {
            std::uint32_t m_length;
            void* m_start_sid;
            void* m_sid_list;
            std::uint32_t m_sid_list_length;
        } m_query_quota;

        // For NtSetQuotaInformationFile
        struct {
            std::uint32_t m_length;
        } m_set_quota;

        // For IRP_MN_QUERY_DEVICE_RELATIONS
        struct {
            std::uint32_t m_type;
        } m_query_device_relations;

        // For IRP_MN_QUERY_INTERFACE
        struct {
            const void* m_interface_type;
            std::uint16_t m_size;
            std::uint16_t m_version;
            void* m_interface;
            void* m_interface_specific_data;
        } m_query_interface;

        // For IRP_MN_QUERY_CAPABILITIES
        struct {
            void* m_capabilities;
        } m_device_capabilities;

        // For IRP_MN_FILTER_RESOURCE_REQUIREMENTS
        struct {
            void* m_io_resource_requirement_list;
        } m_filter_resource_requirements;

        // For IRP_MN_READ_CONFIG and IRP_MN_WRITE_CONFIG
        struct {
            std::uint32_t m_which_space;
            void* m_buffer;
            std::uint32_t m_offset;
            std::uint32_t m_length;
        } m_read_write_config;

        // For IRP_MN_SET_LOCK
        struct {
            std::uint8_t m_lock;
        } m_set_lock;

        // For IRP_MN_QUERY_ID
        struct {
            std::uint32_t m_id_type;
        } m_query_id;

        // For IRP_MN_QUERY_DEVICE_TEXT
        struct {
            std::uint32_t m_device_text_type;
            std::uint32_t m_locale_id;
        } m_query_device_text;

        // For IRP_MN_DEVICE_USAGE_NOTIFICATION
        struct {
            std::uint8_t m_in_path;
            std::uint8_t m_reserved[ 3 ];
            std::uint32_t m_type;
        } m_usage_notification;

        // For IRP_MN_WAIT_WAKE
        struct {
            std::uint32_t m_power_state;
        } m_wait_wake;

        // For IRP_MN_POWER_SEQUENCE
        struct {
            void* m_power_sequence;
        } m_power_sequence;

        // For IRP_MN_SET_POWER and IRP_MN_QUERY_POWER
#if (NTDDI_VERSION >= NTDDI_VISTA)
        struct {
            union {
                std::uint32_t m_system_context;
                std::uint64_t m_system_power_state_context;
            };
            std::uint32_t m_type;
            std::uint32_t m_state;
            std::uint32_t m_shutdown_type;
        } m_power;
#else
        struct {
            std::uint32_t m_system_context;
            std::uint32_t m_type;
            std::uint32_t m_state;
            std::uint32_t m_shutdown_type;
        } m_power;
#endif

        // For StartDevice
        struct {
            void* m_allocated_resources;
            void* m_allocated_resources_translated;
        } m_start_device;

        // For WMI Irps
        struct {
            std::uint64_t m_provider_id;
            void* m_data_path;
            std::uint32_t m_buffer_size;
            void* m_buffer;
        } m_wmi;

        // Others - driver-specific
        struct {
            void* m_arg1;
            void* m_arg2;
            void* m_arg3;
            void* m_arg4;
        } m_others;

    } m_parameters;

    void* m_device_object;
    void* m_file_object;
    void* m_completion_routine;
    void* m_context;
};

#pragma pack(pop)

struct iop_irp_t {
    std::uint16_t m_type;                  // Type
    std::uint16_t m_size;                  // Size
    void* m_mdl;                           // MdlAddress
    std::uint32_t m_flags;                 // Flags

    // AssociatedIrp union
    union {
        struct iop_irp_t* m_master_irp;    // MasterIrp
        volatile std::int32_t m_irp_count; // IrpCount
        void* m_system_buffer;             // SystemBuffer
    } m_associated_irp;

    // ThreadListEntry
    struct {
        void* m_flink;
        void* m_blink;
    } m_thread_list_entry;

    // IoStatus
    struct {
        nt_status_t m_status;             // Status
        std::uint64_t m_information;       // Information
    } m_io_status;

    std::uint8_t m_requestor_mode;         // RequestorMode
    std::uint8_t m_pending_returned;       // PendingReturned
    std::int8_t m_stack_count;             // StackCount
    std::int8_t m_current_location;        // CurrentLocation
    std::uint8_t m_cancel;                 // Cancel
    std::uint8_t m_cancel_irql;            // CancelIrql
    std::int8_t m_apc_environment;         // ApcEnvironment
    std::uint8_t m_allocation_flags;       // AllocationFlags

    // User parameters
    union {
        void* m_user_iosb;                 // UserIosb
        void* m_io_ring_context;           // IoRingContext
    } m_user_iosb_union;

    void* m_user_event;                    // UserEvent

    // Overlay union
    union {
        struct {
            union {
                void* m_user_apc_routine;   // UserApcRoutine
                void* m_issuing_process;    // IssuingProcess
            } m_apc_routine_union;

            union {
                void* m_user_apc_context;   // UserApcContext
                void* m_io_ring;            // IoRing
            } m_apc_context_union;
        } m_asynchronous_parameters;

        std::uint64_t m_allocation_size;    // AllocationSize
    } m_overlay;

    volatile void* m_cancel_routine;        // CancelRoutine
    void* m_user_buffer;                    // UserBuffer

    // Tail union
    union {
        struct {
            union {
                // DeviceQueueEntry
                struct {
                    void* m_device_list_entry;
                    void* m_sorting_key;
                    std::uint8_t m_inserted;
                } m_device_queue_entry;

                // DriverContext
                struct {
                    void* m_driver_context[ 4 ];
                };
            } m_device_context_union;

            void* m_thread;                 // Thread
            void* m_auxiliary_buffer;       // AuxiliaryBuffer

            struct {
                // ListEntry
                struct {
                    void* m_flink;
                    void* m_blink;
                } m_list_entry;

                union {
                    // CurrentStackLocation
                    struct iop_irp_stack_profiler_t* m_current_stack_location;
                    std::uint32_t m_packet_type;  // PacketType
                } m_stack_location_union;
            } m_list_stack_struct;

            void* m_original_file_object;   // OriginalFileObject
        } m_overlay;

        // Apc
        struct {
            // KAPC structure fields
            std::uint16_t m_type;
            std::uint16_t m_size;
            std::uint32_t m_spare0;
            void* m_thread;
            void* m_apc_list_entry;
            void* m_kernel_routine;
            void* m_rundown_routine;
            void* m_normal_routine;
            void* m_normal_context;
            void* m_system_argument1;
            void* m_system_argument2;
            std::uint8_t m_apc_state_index;
            std::uint8_t m_apc_mode;
            std::uint8_t m_inserted;
            std::uint8_t m_padding;
        } m_apc;

        void* m_completion_key;             // CompletionKey
    } m_tail;

    // The stack array is at the end (variable length)
    iop_irp_stack_profiler_t m_stack[ 1 ];
};

struct machine_check_context_t {
    std::uint32_t m_version_id;
    std::uint32_t m_check_type;
    std::uint32_t m_processor_number;
    std::uint32_t m_bank_number;
    std::uint64_t m_address;
    std::uint64_t m_misc;
};

struct synch_counters_t {
    std::uint32_t m_spinlock_acquire;
    std::uint32_t m_spinlock_content;
    std::uint32_t m_spinlock_spin;
    std::uint32_t m_kevent;
    std::uint32_t m_kevent_level;
    std::uint32_t m_kevent_spinlock_spin;
    std::uint32_t m_kmutex_acquire;
    std::uint32_t m_kmutex_content;
    std::uint32_t m_kmutex_spin;
    std::uint32_t m_fast_mutex_acquire;
    std::uint32_t m_fast_mutex_content;
    std::uint32_t m_fast_mutex_spin;
    std::uint32_t m_guarded_mutex_acquire;
    std::uint32_t m_guarded_mutex_content;
    std::uint32_t m_guarded_mutex_spin;
};

struct request_mailbox_t {
    std::uint64_t m_next;
    std::uint32_t m_request_type;
    std::uint32_t m_request_flags;
    list_entry_t m_request_list_entry;
    std::uint64_t m_request_context;
    union {
        struct {
            std::uint32_t m_processor_number;
            std::uint32_t m_node_number;
        };
        std::uint64_t m_target_object;
    };
    std::uint64_t m_reserved[ 2 ];
};

struct m128a {
    std::uint64_t low;
    std::uint64_t high;
};

struct ktrap_frame_t {
    std::uint64_t m_p1_home;                    // +0x000
    std::uint64_t m_p2_home;                    // +0x008
    std::uint64_t m_p3_home;                    // +0x010
    std::uint64_t m_p4_home;                    // +0x018
    std::uint64_t m_p5;                         // +0x020

    std::uint8_t m_previous_mode;               // +0x028
    std::uint8_t m_previous_irql;               // +0x029
    std::uint8_t m_fault_indicator;             // +0x02A
    std::uint8_t m_exception_active;            // +0x02B
    std::uint32_t m_mxcsr;                      // +0x02C

    std::uint64_t m_rax;                        // +0x030
    std::uint64_t m_rcx;                        // +0x038
    std::uint64_t m_rdx;                        // +0x040
    std::uint64_t m_r8;                         // +0x048
    std::uint64_t m_r9;                         // +0x050
    std::uint64_t m_r10;                        // +0x058
    std::uint64_t m_r11;                        // +0x060

    union {
        std::uint64_t m_gs_base;                // +0x068
        std::uint64_t m_gs_swap;
    };

    m128a m_xmm0;                               // +0x070
    m128a m_xmm1;                               // +0x080
    m128a m_xmm2;                               // +0x090
    m128a m_xmm3;                               // +0x0A0
    m128a m_xmm4;                               // +0x0B0
    m128a m_xmm5;                               // +0x0C0

    union {
        std::uint64_t m_fault_address;          // +0x0D0
        std::uint64_t m_context_record;
    };

    std::uint64_t m_dr0;                        // +0x0D8
    std::uint64_t m_dr1;                        // +0x0E0
    std::uint64_t m_dr2;                        // +0x0E8
    std::uint64_t m_dr3;                        // +0x0F0
    std::uint64_t m_dr6;                        // +0x0F8
    std::uint64_t m_dr7;                        // +0x100

    // Debug registers block
    std::uint64_t m_debug_control;              // +0x108
    std::uint64_t m_last_branch_to_rip;         // +0x110
    std::uint64_t m_last_branch_from_rip;       // +0x118
    std::uint64_t m_last_exception_to_rip;      // +0x120
    std::uint64_t m_last_exception_from_rip;    // +0x128

    std::uint16_t m_seg_ds;                     // +0x130
    std::uint16_t m_seg_es;                     // +0x132
    std::uint16_t m_seg_fs;                     // +0x134
    std::uint16_t m_seg_gs;                     // +0x136

    std::uint64_t m_nested_trap_frame;                 // +0x138
    std::uint64_t m_rbx;                        // +0x140
    std::uint64_t m_rdi;                        // +0x148
    std::uint64_t m_rsi;                        // +0x150
    std::uint64_t m_rbp;                        // +0x158

    union {
        std::uint64_t m_error_code;             // +0x160
        std::uint64_t m_exception_frame;
    };

    std::uint64_t m_rip;                        // +0x168
    std::uint16_t m_seg_cs;                     // +0x170
    std::uint8_t m_fill0;                       // +0x172
    std::uint8_t m_logging;                     // +0x173
    std::uint16_t m_fill1[ 2 ];                   // +0x174
    std::uint32_t m_eflags;                     // +0x178
    std::uint32_t m_fill2;                      // +0x17C
    std::uint64_t m_rsp;                        // +0x180
    std::uint16_t m_seg_ss;                     // +0x188
    std::uint16_t m_fill3;                      // +0x18A
    std::uint32_t m_fill4;                      // +0x18C
}; // Size = 0x190

struct activation_context_stack_t
{
    std::addr_t active_frame;                    // 0x000
    list_entry_t frame_list_cache;               // 0x008
    std::uint32_t flags;                         // 0x018
    std::uint32_t next_cookie;                   // 0x01C
    std::uint32_t frame_count;                   // 0x020
    std::uint32_t padding;                       // 0x024
};

struct client_id_t {
    void* m_unique_process;
    void* m_unique_thread;
};

struct _exception_registration_record {
    struct _exception_registration_record* m_next;
    void* m_handler;
};

struct _vectored_handler_entry {
    list_entry_t m_link;
    std::uint32_t m_reference_count;
    void* m_vectored_handler;
};

struct _vectored_handler_list {
    std::uint32_t m_count;
    list_entry_t m_head;
};

struct teb_t;
struct nt_tib_t 
{
    struct _exception_registration_record* m_exception_list;  // 0x000
    std::addr_t m_stack_base;                                // 0x008
    std::addr_t m_stack_limit;                              // 0x010
    std::addr_t m_sub_system_tib;                           // 0x018
    union {
        std::addr_t m_fiber_data;                           // 0x020
        std::uint32_t m_version;                            // 0x020
    };
    std::addr_t m_arbitrary_user_pointer;                    // 0x028
    teb_t* m_self;                                          // 0x030
};

struct teb_t
{
    nt_tib_t m_nt_tib;                          // 0x000 Contains exception_list, stack_base, stack_limit, etc.
    std::addr_t m_environment_pointer;           // 0x038
    client_id_t m_client_id;                    // 0x040
    std::addr_t m_active_rpc_handle;            // 0x050
    std::addr_t m_thread_local_storage_pointer; // 0x058
    peb_t* m_process_environment_block;         // 0x060
    std::uint32_t m_last_error_value;           // 0x068
    std::uint32_t m_count_of_owned_critical_sections; // 0x06C
    std::addr_t m_csr_client_thread;            // 0x070
    std::addr_t m_win32_thread_info;            // 0x078
    std::uint32_t m_user32_reserved[26];        // 0x080
    std::addr_t m_user_reserved[5];             // 0x0E8
    std::addr_t m_wow32_reserved;               // 0x100
    std::uint32_t m_current_locale;             // 0x108
    std::uint32_t m_fp_software_status_register; // 0x10C
    std::addr_t m_system_reserved1[54];         // 0x110
    std::int32_t m_exception_code;              // 0x2C0

    activation_context_stack_t* m_activation_context_stack_pointer; // 0x2C8
    std::uint8_t m_spare_bytes[24];             // 0x2D0
    std::uint32_t m_tls_slots[64];              // 0x2E8
    list_entry_t m_tls_links;                   // 0x4E8
};

struct kapc_state_t {
    list_entry_t m_apc_list_head[2];
    eprocess_t* m_process;
    std::uint8_t m_kernel_apc_in_progress;
    std::uint8_t m_kernel_apc_pending;
    std::uint8_t m_user_apc_pending;
    std::uint8_t m_pad;
}; // Size: 0x40

struct kapc_t {
    std::uint8_t type;
    std::uint8_t spare_byte0;
    std::uint8_t size;
    std::uint8_t spare_byte1;
    std::uint32_t spare_long0;
    void* thread;
    list_entry_t apc_list_entry;
    void* kernel_routine;
    void* rundown_routine;
    void* normal_routine;
    void* normal_context;
    void* system_argument1;
    void* system_argument2;
    std::uint8_t apc_state_index;
    std::uint8_t apc_mode;
    std::uint8_t inserted;
    std::uint8_t pad;
}; // Size: 0x58

struct kthread_t {
    // Base section (0x000 - 0x090)
    dispatcher_header_t m_header;                      // +0x000
    void* m_slist_fault_address;                      // +0x018
    std::uint64_t m_quantum_target;                   // +0x020
    void* m_initial_stack;                            // +0x028
    void* m_stack_limit;                              // +0x030
    void* m_stack_base;                              // +0x038
    std::uint64_t m_thread_lock;                     // +0x040
    std::uint64_t m_cycle_time;                      // +0x048
    std::uint32_t m_current_run_time;                // +0x050
    std::uint32_t m_expected_run_time;               // +0x054
    void* m_kernel_stack;                            // +0x058
    void* m_state_save_area;                         // +0x060
    void* m_scheduling_group;                        // +0x068
    kwait_status_register_t m_wait_register;         // +0x070
    std::uint8_t m_running;                          // +0x071
    std::uint8_t m_alerted[ 2 ];                      // +0x072
    std::uint32_t m_auto_alignment;                  // +0x074
    std::uint8_t m_tag;                              // +0x07C
    std::uint8_t m_system_hetero_cpu_policy;         // +0x07D
    std::uint8_t m_spare_byte;                       // +0x07E
    std::uint32_t m_system_call_number;              // +0x080
    std::uint32_t m_ready_time;                      // +0x084
    void* m_first_argument;                          // +0x088
    ktrap_frame_t* m_trap_frame;                     // +0x090

    // Callback and system section (0x098 - 0x0F8)
    kapc_state_t m_apc_state;                          // +0x098
    std::uint8_t m_apc_queueable;                    // +0x0A0
    std::uint8_t m_apc_queueable_padding[7];         // +0x0A1
    void* m_service_table;                          // +0x0A8
    std::uint32_t m_kernel_reserve_apc;             // +0x0B0
    std::uint32_t m_kernel_reserve_apc_padding;     // +0x0B4
    void* m_win32_thread;                           // +0x0B8
    void* m_trap_frame_base;                        // +0x0C0
    std::uint64_t m_wait_status;                     // +0x0C8
    void* m_wait_block_list;                         // +0x0D0
    void* m_queue;                                   // +0x0E8
    teb_t* m_teb;                                     // +0x0F0
    std::uint64_t m_relative_timer_bias;             // +0x0F8

    // Timer and flags section (0x100 - 0x177)
    ktimer_t m_timer;                                // +0x100
    std::uint32_t m_misc_flags;                      // +0x170
    std::uint8_t m_kernel_apc_disable;               // +0x174
    std::uint8_t m_kernel_apc_pending;               // +0x175
    std::uint8_t m_io_pending;                       // +0x176
    std::uint8_t m_io_pending_high;                  // +0x177

    // Wait info section (0x178 - 0x1FC)
    std::int64_t m_entropy_count;                    // +0x178
    std::uint32_t m_permission_key;                  // +0x180
    std::uint32_t m_permission_key_non_paged;        // +0x184
    void* m_wait_prcb;                              // +0x188
    void* m_wait_next;                              // +0x190
    void* m_wait_value;                             // +0x198
    void* m_wait_reason;                            // +0x1A0
    std::uint32_t m_wait_irql;                      // +0x1A8
    std::uint8_t m_wait_mode;                       // +0x1AC
    std::uint8_t m_wait_next_flag;                  // +0x1AD
    std::uint8_t m_wait_reason_flag;                // +0x1AE
    std::uint8_t m_wait_response;                   // +0x1AF
    void* m_wait_pointer;                          // +0x1B0
    std::uint32_t m_thread_flags;                    // +0x1B8
    std::uint32_t m_spare0;                         // +0x1BC
    void* m_wait_block_list2;                       // +0x1C0
    std::uint32_t m_wait_block_count;               // +0x1C8
    std::uint32_t m_wait_block_offset;              // +0x1CC
    void* m_wait_blocks;                            // +0x1D0
    // Queue and process section (0x1D8 - 0x250)
    list_entry_t m_wait_list_entry;                 // +0x1D8
    std::uint32_t m_wait_status2;                   // +0x1E8
    std::uint32_t m_wait_state_sequence;            // +0x1EC
    std::uint32_t m_wait_irql_old;                  // +0x1F0
    std::uint32_t m_wait_mode_old;                  // +0x1F4
    std::uint32_t m_wait_queue_timeout;             // +0x1F8
    std::uint32_t m_wait_block_multiple;            // +0x1FC
    void* m_thread_queue_list_entry;                // +0x200
    list_entry_t m_queue_list_entry;                // +0x208
    std::uint16_t m_queue_index;                    // +0x218
    std::uint32_t m_queue_priority;                 // +0x21C
    kprocess_t* m_process;                          // +0x220
    group_affinity_t m_affinity;                    // +0x228
    std::uint64_t m_affinity_version;               // +0x238
    void* m_npx_state;                             // +0x250

    // Performance and kernel stack section (0x258 - 0x2E0)
    void* m_performance_counters;                   // +0x258
    void* m_context_switch_count;                   // +0x260
    void* m_scheduler_assist_thread;                // +0x268
    void* m_kernel_stack_control;                   // +0x270
    void* m_kernel_stack_limit;                     // +0x278
    void* m_kernel_stack_base;                      // +0x280
    void* m_thread_lock_owner;                      // +0x288
    void* m_kernel_wait_always;                     // +0x290
    void* m_user_wait_always;                       // +0x298
    void* m_win32k_thread;                         // +0x2A0
    void* m_worker_routine;                        // +0x2A8
    void* m_worker_context;                        // +0x2B0
    void* m_win32_start_address;                   // +0x2B8
    void* m_lpaccel;                              // +0x2C0
    void* m_lpfnwndproc;                          // +0x2C8
    void* m_win32k_callback;                      // +0x2D0
    void* m_win32k_callback_context;               // +0x2D8

    // Final section (0x2E0 - 0x430)
    kevent_t m_suspend_event;                       // +0x2E0
    list_entry_t m_thread_list_entry;               // +0x2F8
    list_entry_t m_mutant_list_head;                // +0x308
    std::uint8_t m_ab_entry_summary;                // +0x318
    std::uint8_t m_ab_wait_entry_count;             // +0x319
    std::uint8_t m_ab_allocation_region_count;      // +0x31A
    std::uint8_t m_system_priority;                 // +0x31B
    std::uint32_t m_secure_thread_cookie;           // +0x31C
    void* m_lock_entries;                          // +0x320
    single_list_entry_t m_propagate_boosts_entry;   // +0x328
    single_list_entry_t m_io_self_boosts_entry;     // +0x330
    std::uint8_t m_priority_floor_counts[ 16 ];       // +0x338
    std::uint8_t m_priority_floor_counts_reserved[ 16 ]; // +0x348
    std::uint32_t m_priority_floor_summary;         // +0x358
    std::uint32_t m_ab_completed_io_boost_count;    // +0x35C
    std::uint32_t m_ab_completed_io_qos_boost_count; // +0x360
    std::uint16_t m_ke_reference_count;             // +0x364
    std::uint8_t m_ab_orphaned_entry_summary;       // +0x366
    std::uint8_t m_ab_owned_entry_count;            // +0x367
    std::uint32_t m_foreground_loss_time;           // +0x368
    std::uint64_t m_read_operation_count;           // +0x380
    std::uint64_t m_write_operation_count;          // +0x388
    std::uint64_t m_other_operation_count;          // +0x390
    std::uint64_t m_read_transfer_count;            // +0x398
    std::uint64_t m_write_transfer_count;           // +0x3A0
    std::uint64_t m_other_transfer_count;           // +0x3A8
    void* m_queued_scb;                            // +0x3B0
    std::uint32_t m_thread_timer_delay;             // +0x3B8
    void* m_tracing_private;                        // +0x3C0
    void* m_scheduler_assist;                       // +0x3C8
    void* m_ab_wait_object;                        // +0x3D0
    std::uint32_t m_reserved_previous_ready_time_value; // +0x3D8
    std::uint64_t m_kernel_wait_time;               // +0x3E0
    std::uint64_t m_user_wait_time;                 // +0x3E8
    void* m_explicit_scheduling;                    // +0x3F0
    void* m_debug_active;                          // +0x3F8
    std::uint32_t m_scheduler_assist_priority_floor; // +0x400
    std::uint32_t m_spare28;                       // +0x404
    std::uint8_t m_resource_index;                  // +0x408
    std::uint8_t m_spare31[ 3 ];                     // +0x409
    std::uint64_t m_end_padding[ 4 ];                // +0x410
}; // Size: 0x430

struct ethread_t {
    struct kthread_t m_kthread;              // +0x000 - KTHREAD portion
    list_entry_t m_thread_list_entry;                // +0x430
    void* m_create_time;                             // +0x440
    union {
        void* m_exit_time;                           // +0x448
        list_entry_t m_active_execution_list;
    };
    union {
        void* m_exit_status;                         // +0x450
        void* m_post_block_old;
    };
    union {
        void* m_terminate_apc;                       // +0x458
        void* m_terminate_pending;
    };
    union {
        void* m_thread_flags;                        // +0x460
        union {
            std::uint32_t m_thread_flags_value;
            struct {
                std::uint32_t m_terminate_requested : 1;
                std::uint32_t m_dead_thread : 1;
                std::uint32_t m_hide_from_debugger : 1;
                std::uint32_t m_active_impersonation_info : 1;
                std::uint32_t m_system_thread : 1;
                std::uint32_t m_hard_errors_are_disabled : 1;
                std::uint32_t m_break_on_termination : 1;
                std::uint32_t m_skip_creation_msg : 1;
                std::uint32_t m_skip_termination_msg : 1;
            };
        };
        std::uint32_t m_padding;
    };
    union {
        void* m_create_info;                         // +0x468
        void* m_win32_start_address;
    };
    void* m_teb;                                     // +0x470
    client_id_t m_client_id;                       // +0x478
    void* m_security_port;                           // +0x480
    void* m_previous_mode;                           // +0x488
    void* m_resource_index;                          // +0x490
    void* m_large_stack;                             // +0x498
    void* m_fx_save_area;                           // +0x4A0
    void* m_priority_class;                          // +0x4A8
    void* m_desktop;                                 // +0x4B0
    void* m_suspend_semaphore;                       // +0x4B8
    union {
        void* m_win32_thread;                        // +0x4C0
        struct {
            std::uint32_t m_io_priority : 3;
            std::uint32_t m_memory_priority : 3;
            std::uint32_t m_absolute_cpu_priority : 1;
        };
    };
    void* m_working_on_behalf_ticket;                // +0x4C8
    void* m_impersonation_info;                      // +0x4D0
    void* m_io_pending_mr;                           // +0x4D8
    void* m_io_suppress_thread;                      // +0x4E0
    void* m_memory_attribute;                         // +0x4E8
    union {
        void* m_win32_thread_event;                  // +0x4F0
        void* m_running_down;
    };
    void* m_thread_lock;                             // +0x4F8
    std::uint32_t m_read_operation_count;            // +0x500
    std::uint32_t m_write_operation_count;           // +0x504
    std::uint32_t m_other_operation_count;           // +0x508
    std::uint32_t m_io_priority_boost;               // +0x50C
    void* m_io_client_pointer;                       // +0x510
    void* m_file_object;                             // +0x518
    void* m_word_list_head;                          // +0x520
    void* m_process_context;                         // +0x528
    void* m_granted_access;                          // +0x530
    void* m_cross_thread_flags;                      // +0x538
    union {
        std::uint32_t m_cross_thread_flags_uint;     // +0x540
        struct {
            std::uint32_t m_terminated : 1;
            std::uint32_t m_debug_active : 1;
            std::uint32_t m_system_process : 1;
            std::uint32_t m_impersonating : 1;
            std::uint32_t m_break_on_termination : 1;
            std::uint32_t m_reserved : 27;
        };
    };
    std::uint32_t m_cross_thread_flags_padding;      // +0x544
    void* m_start_address;                           // +0x548
    void* m_win32_thread_info;                       // +0x550
    void* m_lpaccel;                                 // +0x558
    void* m_lpfnwndproc;                            // +0x560
    void* m_win32k;                                  // +0x568
}; // Size = 0x570

struct xmm_save_area32_t {
    std::uint16_t m_control_word;
    std::uint16_t m_status_word;
    std::uint8_t m_tag_word;
    std::uint8_t m_reserved1;
    std::uint16_t m_error_opcode;
    std::uint32_t m_error_offset;
    std::uint16_t m_error_selector;
    std::uint16_t m_reserved2;
    std::uint32_t m_data_offset;
    std::uint16_t m_data_selector;
    std::uint16_t m_reserved3;
    std::uint32_t m_mx_csr;
    std::uint32_t m_mx_csr_mask;
    std::m128a_t m_float_registers[ 8 ];
    std::m128a_t m_xmm_registers[ 16 ];
    std::uint8_t m_reserved4[ 96 ];
};

struct context_t {
    std::uint64_t m_p1_home;                    // +0x000
    std::uint64_t m_p2_home;                    // +0x008
    std::uint64_t m_p3_home;                    // +0x010
    std::uint64_t m_p4_home;                    // +0x018
    std::uint64_t m_p5_home;                    // +0x020
    std::uint64_t m_p6_home;                    // +0x028
    std::uint32_t m_context_flags;              // +0x030
    std::uint32_t m_mx_csr;                     // +0x034
    std::uint16_t m_seg_cs;                     // +0x038
    std::uint16_t m_seg_ds;                     // +0x03A
    std::uint16_t m_seg_es;                     // +0x03C
    std::uint16_t m_seg_fs;                     // +0x03E
    std::uint16_t m_seg_gs;                     // +0x040
    std::uint16_t m_seg_ss;                     // +0x042
    std::uint32_t m_eflags;                     // +0x044
    std::uint64_t m_dr0;                        // +0x048
    std::uint64_t m_dr1;                        // +0x050
    std::uint64_t m_dr2;                        // +0x058
    std::uint64_t m_dr3;                        // +0x060
    std::uint64_t m_dr6;                        // +0x068
    std::uint64_t m_dr7;                        // +0x070
    std::uint64_t m_rax;                        // +0x078
    std::uint64_t m_rcx;                        // +0x080
    std::uint64_t m_rdx;                        // +0x088
    std::uint64_t m_rbx;                        // +0x090
    std::uint64_t m_rsp;                        // +0x098
    std::uint64_t m_rbp;                        // +0x0A0
    std::uint64_t m_rsi;                        // +0x0A8
    std::uint64_t m_rdi;                        // +0x0B0
    std::uint64_t m_r8;                         // +0x0B8
    std::uint64_t m_r9;                         // +0x0C0
    std::uint64_t m_r10;                        // +0x0C8
    std::uint64_t m_r11;                        // +0x0D0
    std::uint64_t m_r12;                        // +0x0D8
    std::uint64_t m_r13;                        // +0x0E0
    std::uint64_t m_r14;                        // +0x0E8
    std::uint64_t m_r15;                        // +0x0F0
    std::uint64_t m_rip;                        // +0x0F8
    union {
        xmm_save_area32_t m_flt_save;          // +0x100
        struct {
            std::m128a_t m_header[ 2 ];
            std::m128a_t m_legacy[ 8 ];
            std::m128a_t m_xmm0;
            std::m128a_t m_xmm1;
            std::m128a_t m_xmm2;
            std::m128a_t m_xmm3;
            std::m128a_t m_xmm4;
            std::m128a_t m_xmm5;
            std::m128a_t m_xmm6;
            std::m128a_t m_xmm7;
            std::m128a_t m_xmm8;
            std::m128a_t m_xmm9;
            std::m128a_t m_xmm10;
            std::m128a_t m_xmm11;
            std::m128a_t m_xmm12;
            std::m128a_t m_xmm13;
            std::m128a_t m_xmm14;
            std::m128a_t m_xmm15;
        };
    };
    std::m128a_t m_vector_register[ 26 ];             // +0x300
    std::uint64_t m_vector_control;            // +0x4A0
    std::uint64_t m_debug_control;             // +0x4A8
    std::uint64_t m_last_branch_to_rip;        // +0x4B0
    std::uint64_t m_last_branch_from_rip;      // +0x4B8
    std::uint64_t m_last_exception_to_rip;     // +0x4C0
    std::uint64_t m_last_exception_from_rip;   // +0x4C8
};

struct kirql_t {
    static constexpr std::uint8_t m_passive_level = 0x00;
    static constexpr std::uint8_t m_apc_level = 0x01;
    static constexpr std::uint8_t m_dispatch_level = 0x02;
    static constexpr std::uint8_t m_cmci_level = 0x03;
    static constexpr std::uint8_t m_clock_level = 0x0D;
    static constexpr std::uint8_t m_ipi_level = 0x0E;
    static constexpr std::uint8_t m_dpc_level = 0x02;
    static constexpr std::uint8_t m_power_level = 0x0D;
    static constexpr std::uint8_t m_profile_level = 0x0F;
    static constexpr std::uint8_t m_device0_level = 0x03;
    static constexpr std::uint8_t m_device1_level = 0x04;
    static constexpr std::uint8_t m_device2_level = 0x05;
    static constexpr std::uint8_t m_device3_level = 0x06;
    static constexpr std::uint8_t m_device4_level = 0x07;
    static constexpr std::uint8_t m_device5_level = 0x08;
    static constexpr std::uint8_t m_device6_level = 0x09;
    static constexpr std::uint8_t m_device7_level = 0x0A;
    static constexpr std::uint8_t m_device8_level = 0x0B;
    static constexpr std::uint8_t m_device9_level = 0x0C;
    static constexpr std::uint8_t m_device10_level = 0x0D;
    static constexpr std::uint8_t m_device11_level = 0x0E;
    static constexpr std::uint8_t m_device12_level = 0x0F;
    static constexpr std::uint8_t m_device13_level = 0x10;
    static constexpr std::uint8_t m_device14_level = 0x11;
    static constexpr std::uint8_t m_device15_level = 0x12;
    static constexpr std::uint8_t m_high_level = 0x1F;
}; // Size: 0x020

struct kprcb_t {
    std::uint32_t m_mxcsr;                              // +0x000
    std::uint8_t m_legacy_number;                       // +0x004
    std::uint8_t m_reserved_must_be_zero;               // +0x005
    std::uint8_t m_interrupt_request;                   // +0x006
    std::uint8_t m_idle_halt;                          // +0x007
    kthread_t* m_current_thread;                             // +0x008
    void* m_next_thread;                               // +0x010
    void* m_idle_thread;                               // +0x018
    std::uint8_t m_nesting_level;                      // +0x020
    std::uint8_t m_clock_owner;                        // +0x021
    std::uint8_t m_pad_0x22;                          // +0x022
    std::uint8_t m_idle_state;                         // +0x023
    std::uint32_t m_number;                            // +0x024
    std::uint64_t m_rsp_base;                         // +0x028
    std::uint64_t m_prcb_lock;                        // +0x030
    void* m_priority_state;                           // +0x038
    std::uint8_t m_cpu_type;                          // +0x040
    std::uint8_t m_cpu_id;                            // +0x041
    std::uint16_t m_pad_0x42;                         // +0x042
    std::uint32_t m_mhz;                              // +0x044
    std::uint64_t m_hal_reserved[ 8 ];                  // +0x048
    std::uint16_t m_minor_version;                    // +0x088
    std::uint16_t m_major_version;                    // +0x08A
    std::uint8_t m_build_type;                        // +0x08C
    std::uint8_t m_cpu_vendor;                        // +0x08D
    std::uint8_t m_cores_per_physical_processor;      // +0x08E
    std::uint8_t m_logical_processors_per_core;       // +0x08F
    std::uint64_t m_tsc_frequency;                    // +0x090
    std::uint32_t m_cores_per_physical_processor32;   // +0x098
    std::uint32_t m_logical_processors_per_core32;    // +0x09C
    std::uint64_t m_prcb_pad04[ 4 ];                   // +0x0A0
    void* m_parent_node;                             // +0x0C0
    std::uint64_t m_group_set_member;                // +0x0C8
    std::uint8_t m_group;                            // +0x0D0
    std::uint8_t m_group_index;                      // +0x0D1
    std::uint8_t m_pad_0xD2[ 2 ];                      // +0x0D2
    std::uint32_t m_initial_apic_id;                 // +0x0D4
    std::uint32_t m_scb_offset;                      // +0x0D8
    std::uint32_t m_apic_mask;                       // +0x0DC
    void* m_acpi_reserved;                           // +0x0E0
    std::uint32_t m_cflush_size;                     // +0x0E8
    std::uint32_t m_prcb_flags;                       // +0x0EC
    std::uint64_t m_prcb_pad11[ 2 ];                   // +0x0F0
    kprocessor_state_t m_processor_state;             // +0x100
    void* m_extended_supervisor_state;                // +0x6C0
    std::uint32_t m_processor_signature;              // +0x6C8
    std::uint32_t m_processor_flags;                  // +0x6CC
    std::uint64_t m_pad_0x6D0;                       // +0x6D0
    std::uint64_t m_pad_0x6D8;                       // +0x6D8
    kspin_lock_queue_t m_lock_queue[ 17 ];             // +0x6F0
    pp_lookaside_list_t m_pp_lookaside_list[ 16 ];     // +0x800
    general_lookaside_pool_t m_pp_nx_paged_lookaside_list[ 32 ];  // +0x900
    general_lookaside_pool_t m_pp_n_paged_lookaside_list[ 32 ];   // +0x1500
    general_lookaside_pool_t m_pp_paged_lookaside_list[ 32 ];     // +0x2100
    std::uint64_t m_msr_ia32_tsx_ctrl;               // +0x2D00
    single_list_entry_t m_deferred_ready_list_head;  // +0x2D08
    std::uint32_t m_mm_page_fault_count;             // +0x2D10
    std::uint32_t m_mm_copy_on_write_count;          // +0x2D14
    std::uint32_t m_mm_transition_count;             // +0x2D18
    std::uint32_t m_mm_demand_zero_count;            // +0x2D1C
    std::uint32_t m_mm_page_read_count;              // +0x2D20
    std::uint32_t m_mm_page_read_io_count;           // +0x2D24
    std::uint32_t m_mm_dirty_pages_write_count;      // +0x2D28
    std::uint32_t m_mm_dirty_write_io_count;         // +0x2D2C
    std::uint32_t m_mm_mapped_pages_write_count;     // +0x2D30
    std::uint32_t m_mm_mapped_write_io_count;        // +0x2D34
    std::uint32_t m_ke_system_calls;                 // +0x2D38
    std::uint32_t m_ke_context_switches;             // +0x2D3C
    std::uint32_t m_prcb_pad40;                      // +0x2D40
    std::uint32_t m_cc_fast_read_no_wait;            // +0x2D44
    std::uint32_t m_cc_fast_read_wait;               // +0x2D48
    std::uint32_t m_cc_fast_read_not_possible;       // +0x2D4C
    std::uint32_t m_cc_copy_read_no_wait;            // +0x2D50
    std::uint32_t m_cc_copy_read_wait;               // +0x2D54
    std::uint32_t m_cc_copy_read_no_wait_miss;       // +0x2D58
    std::uint32_t m_io_read_operation_count;         // +0x2D5C
    std::uint32_t m_io_write_operation_count;        // +0x2D60
    std::uint32_t m_io_other_operation_count;        // +0x2D64
    large_integer_t m_io_read_transfer_count;        // +0x2D68
    large_integer_t m_io_write_transfer_count;       // +0x2D70
    large_integer_t m_io_other_transfer_count;       // +0x2D78
    std::uint32_t m_packet_barrier;                  // +0x2D80
    std::uint32_t m_target_count;                    // +0x2D84
    std::uint32_t m_ipi_frozen;                      // +0x2D88
    std::uint32_t m_prcb_pad30;                      // +0x2D8C
    void* m_isr_dpc_stats;                          // +0x2D90
    std::uint32_t m_device_interrupts;               // +0x2D98
    std::uint32_t m_lookaside_irp_float;            // +0x2D9C
    std::uint32_t m_interrupt_last_count;            // +0x2DA0
    std::uint32_t m_interrupt_rate;                  // +0x2DA4
    std::uint64_t m_prcb_pad31;                     // +0x2DA8
    void* m_pair_prcb;                              // +0x2DB0
    kstatic_affinity_block_t m_static_affinity;     // +0x2DB8
    std::uint64_t m_prcb_pad35[ 5 ];                  // +0x3058
    slist_header_t m_interrupt_object_pool;          // +0x3080
    void* m_dpc_runtime_history_hash_table;         // +0x3090
    void* m_dpc_runtime_history_hash_table_cleanup_dpc; // +0x3098
    void* m_current_dpc_routine;                    // +0x30A0
    std::uint64_t m_current_dpc_runtime_history_cached; // +0x30A8
    std::uint64_t m_current_dpc_start_time;         // +0x30B0
    std::uint64_t m_prcb_pad41;                     // +0x30B8
    kdpc_data_t m_dpc_data[ 2 ];                      // +0x30C0
    void* m_dpc_stack;                              // +0x3110
    std::uint32_t m_maximum_dpc_queue_depth;        // +0x3118
    std::uint32_t m_dpc_request_rate;               // +0x311C
    std::uint32_t m_minimum_dpc_rate;               // +0x3120
    std::uint32_t m_dpc_last_count;                 // +0x3124
    std::uint8_t m_thread_dpc_enable;               // +0x3128
    std::uint8_t m_quantum_end;                     // +0x3129
    std::uint8_t m_dpc_routine_active;              // +0x312A
    std::uint8_t m_idle_schedule;                   // +0x312B
    std::uint32_t m_pad_0x312C;                    // +0x312C
    std::uint32_t m_prcb_pad93;                    // +0x3130
    std::uint32_t m_last_tick;                      // +0x3134
    std::uint32_t m_clock_interrupts;               // +0x3138
    std::uint32_t m_ready_scan_tick;                // +0x313C
    void* m_interrupt_object[ 256 ];                  // +0x3140
    ktimer_table_t m_timer_table;                   // +0x3940
    std::uint32_t m_prcb_pad92[ 10 ];                // +0x7B58
    kgate_t m_dpc_gate;                            // +0x7B80
    void* m_prcb_pad52;                            // +0x7B98
    kdpc_t m_call_dpc;                             // +0x7BA0
    std::uint32_t m_clock_keep_alive;               // +0x7BE0
    std::uint8_t m_prcb_pad60[ 2 ];                  // +0x7BE2
    std::uint16_t m_pad_0x7BE6;                    // +0x7BE6
    std::uint32_t m_dpc_watchdog_period;            // +0x7BE8
    std::uint32_t m_dpc_watchdog_count;             // +0x7BEC
    std::uint32_t m_ke_spinlock_ordering;           // +0x7BF0
    std::uint32_t m_dpc_watchdog_profile_cumulative_dpc_threshold; // +0x7BF4
    void* m_cached_ptes;                           // +0x7BF8
    list_entry_t m_wait_list_head;                 // +0x7C00
    std::uint64_t m_wait_lock;                     // +0x7C10
    std::uint32_t m_ready_summary;                  // +0x7C18
    std::uint32_t m_affinitized_selection_mask;     // +0x7C1C
    std::uint32_t m_queue_index;                    // +0x7C20
    std::uint32_t m_prcb_pad75[ 2 ];                 // +0x7C24
    std::uint32_t m_dpc_watchdog_sequence_number;   // +0x7C2C
    kdpc_t m_timer_expiration_dpc;                 // +0x7C30
    rtl_rb_tree_t m_scb_queue;                     // +0x7C70
    list_entry_t m_dispatcher_ready_list_head[ 32 ];  // +0x7C80
    std::uint32_t m_interrupt_count;                // +0x7E80
    std::uint32_t m_kernel_time;                    // +0x7E84
    std::uint32_t m_user_time;                      // +0x7E88
    std::uint32_t m_dpc_time;                      // +0x7E8C
    std::uint32_t m_interrupt_time;                 // +0x7E90
    std::uint32_t m_adjust_dpc_threshold;           // +0x7E94
    std::uint8_t m_debugger_saved_irql;            // +0x7E98
    std::uint8_t m_group_scheduling_over_quota;     // +0x7E99
    std::uint8_t m_deep_sleep;                     // +0x7E9A
    std::uint8_t m_prcb_pad80;                     // +0x7E9B
    std::uint32_t m_dpc_time_count;                // +0x7E9C
    std::uint32_t m_dpc_time_limit;                // +0x7EA0
    std::uint32_t m_periodic_count;                 // +0x7EA4
    std::uint32_t m_periodic_bias;                  // +0x7EA8
    std::uint32_t m_available_time;                 // +0x7EAC
    std::uint32_t m_ke_exception_dispatch_count;    // +0x7EB0
    std::uint32_t m_ready_thread_count;             // +0x7EB4
    std::uint64_t m_ready_queue_expected_run_time;  // +0x7EB8
    std::uint64_t m_start_cycles;                   // +0x7EC0
    std::uint64_t m_tagged_cycles_start;            // +0x7EC8
    std::uint64_t m_tagged_cycles[ 3 ];               // +0x7ED0
    std::uint64_t m_affinitized_cycles;             // +0x7EE8
    std::uint64_t m_important_cycles;               // +0x7EF0
    std::uint64_t m_unimportant_cycles;             // +0x7EF8ye
    std::uint32_t m_dpc_watchdog_profile_single_dpc_threshold; // +0x7F00
    std::uint32_t m_mm_spinlock_ordering;           // +0x7F04
    void* m_cached_stack;                          // +0x7F08
    std::uint32_t m_page_color;                    // +0x7F10
    std::uint32_t m_node_color;                    // +0x7F14
    std::uint32_t m_node_shifted_color;            // +0x7F18
    std::uint32_t m_secondary_color_mask;          // +0x7F1C
    std::uint8_t m_prcb_pad81[ 6 ];                 // +0x7F20
    std::uint8_t m_exception_stack_active;         // +0x7F26
    std::uint8_t m_tb_flush_list_active;          // +0x7F27
    void* m_exception_stack;                       // +0x7F28
    std::uint64_t m_prcb_pad82;                   // +0x7F30
    std::uint64_t m_cycle_time;                    // +0x7F38
    std::uint64_t m_cycles[ 8 ];                     // +0x7F40
    std::uint32_t m_cc_fast_mdl_read_no_wait;      // +0x7F80
    std::uint32_t m_cc_fast_mdl_read_wait;         // +0x7F84
    std::uint32_t m_cc_fast_mdl_read_not_possible; // +0x7F88
    std::uint32_t m_cc_map_data_no_wait;           // +0x7F8C
    std::uint32_t m_cc_map_data_wait;              // +0x7F90
    std::uint32_t m_cc_pin_mapped_data_count;      // +0x7F94
    std::uint32_t m_cc_pin_read_no_wait;           // +0x7F98
    std::uint32_t m_cc_pin_read_wait;              // +0x7F9C
    std::uint32_t m_cc_mdl_read_no_wait;           // +0x7FA0
    std::uint32_t m_cc_mdl_read_wait;              // +0x7FA4
    std::uint32_t m_cc_lazy_write_hot_spots;       // +0x7FA8
    std::uint32_t m_cc_lazy_write_ios;             // +0x7FAC
    std::uint32_t m_cc_lazy_write_pages;           // +0x7FB0
    std::uint32_t m_cc_data_flushes;               // +0x7FB4
    std::uint32_t m_cc_data_pages;                 // +0x7FB8
    std::uint32_t m_cc_lost_delayed_writes;        // +0x7FBC
    std::uint32_t m_cc_fast_read_resource_miss;     // +0x7FC0
    std::uint32_t m_cc_copy_read_wait_miss;         // +0x7FC4
    std::uint32_t m_cc_fast_mdl_read_resource_miss; // +0x7FC8
    std::uint32_t m_cc_map_data_no_wait_miss;       // +0x7FCC
    std::uint32_t m_cc_map_data_wait_miss;          // +0x7FD0
    std::uint32_t m_cc_pin_read_no_wait_miss;       // +0x7FD4
    std::uint32_t m_cc_pin_read_wait_miss;          // +0x7FD8
    std::uint32_t m_cc_mdl_read_no_wait_miss;       // +0x7FDC
    std::uint32_t m_cc_mdl_read_wait_miss;          // +0x7FE0
    std::uint32_t m_cc_read_ahead_ios;              // +0x7FE4
    std::uint32_t m_mm_cache_transition_count;       // +0x7FE8
    std::uint32_t m_mm_cache_read_count;            // +0x7FEC
    std::uint32_t m_mm_cache_io_count;              // +0x7FF0
    std::uint32_t m_prcb_pad91;                     // +0x7FF4
    void* m_mm_internal;                            // +0x7FF8
    processor_power_state_t m_power_state;          // +0x8000
    void* m_hyper_pte;                             // +0x8200
    list_entry_t m_scb_list;                       // +0x8208
    kdpc_t m_force_idle_dpc;                       // +0x8218
    kdpc_t m_dpc_watchdog_dpc;                     // +0x8258
    ktimer_t m_dpc_watchdog_timer;                 // +0x8298
    cache_descriptor_t m_cache[ 5 ];                  // +0x82D8
    std::uint32_t m_cache_count;                    // +0x8314
    std::uint32_t m_cached_commit;                  // +0x8318
    std::uint32_t m_cached_resident_available;      // +0x831C
    void* m_whea_info;                             // +0x8320
    void* m_etw_support;                           // +0x8328
    void* m_ex_sa_page_array;                      // +0x8330
    std::uint32_t m_ke_alignment_fixup_count;       // +0x8338
    std::uint32_t m_prcb_pad95;                     // +0x833C
    slist_header_t m_hypercall_page_list;           // +0x8340
    void* m_statistics_page;                        // +0x8350
    std::uint64_t m_generation_target;              // +0x8358
    std::uint64_t m_prcb_pad85[ 4 ];                 // +0x8360
    void* m_hypercall_cached_pages;                // +0x8380
    void* m_virtual_apic_assist;                   // +0x8388
    kaffinity_ex_t m_package_processor_set;        // +0x8390
    std::uint32_t m_package_id;                    // +0x8438
    std::uint32_t m_prcb_pad86;                    // +0x843C
    std::uint64_t m_shared_ready_queue_mask;        // +0x8440
    void* m_shared_ready_queue;                    // +0x8448
    std::uint32_t m_shared_queue_scan_owner;        // +0x8450
    std::uint32_t m_scan_sibling_index;            // +0x8454
    std::uint64_t m_core_processor_set;            // +0x8458
    std::uint64_t m_scan_sibling_mask;             // +0x8460
    std::uint64_t m_llc_mask;                      // +0x8468
    std::uint64_t m_cache_processor_mask[ 5 ];       // +0x8470
    void* m_processor_profile_control_area;        // +0x8498
    void* m_profile_event_index_address;           // +0x84A0
    void* m_dpc_watchdog_profile;                 // +0x84A8
    void* m_dpc_watchdog_profile_current_empty_capture; // +0x84B0
    void* m_scheduler_assist;                      // +0x84B8
    synch_counters_t m_synch_counters;            // +0x84C0
    std::uint64_t m_prcb_pad94;                   // +0x8578
    filesystem_disk_counters_t m_fs_counters;      // +0x8580
    std::uint8_t m_vendor_string[ 13 ];              // +0x8590
    std::uint8_t m_prcb_pad100[ 3 ];                // +0x859D
    std::uint64_t m_feature_bits;                   // +0x85A0
    large_integer_t m_update_signature;             // +0x85A8
    std::uint64_t m_pte_bit_cache;                 // +0x85B0
    std::uint32_t m_pte_bit_offset;                // +0x85B8
    std::uint32_t m_prcb_pad105;                   // +0x85BC
    context_t* m_context;                          // +0x85C0
    std::uint32_t m_context_flags_init;            // +0x85C8
    std::uint32_t m_prcb_pad115;                   // +0x85CC
    void* m_extended_state;                        // +0x85D0
    void* m_isr_stack;                            // +0x85D8
    kentropy_timing_state_t m_entropy_timing_state; // +0x85E0
    std::uint64_t m_prcb_pad110;                   // +0x8730
    void* m_stibp_pairing_trace;                   // +0x8738
    single_list_entry_t m_ab_self_io_boosts_list;  // +0x8770
    single_list_entry_t m_ab_propagate_boosts_list; // +0x8778
    kdpc_t m_ab_dpc;                              // +0x8780
    iop_irp_stack_profiler_t m_io_irp_stack_profiler_current;  // +0x87C0
    iop_irp_stack_profiler_t m_io_irp_stack_profiler_previous; // +0x8814
    ksecure_fault_information_t m_secure_fault;    // +0x8868
    std::uint64_t m_prcb_pad120;                   // +0x8878
    kshared_ready_queue_t m_local_shared_ready_queue; // +0x8880
    std::uint64_t m_prcb_pad125[ 2 ];                // +0x8AF0
    std::uint32_t m_timer_expiration_trace_count;   // +0x8B00
    std::uint32_t m_prcb_pad127;                   // +0x8B04
    ktimer_expiration_trace_t m_timer_expiration_trace[ 16 ]; // +0x8B08
    std::uint64_t m_prcb_pad128[ 7 ];                // +0x8C08
    void* m_mailbox;                              // +0x8C40
    std::uint64_t m_prcb_pad130[ 7 ];                // +0x8C48
    machine_check_context_t m_mcheck_context[ 2 ];    // +0x8C80
    std::uint64_t m_prcb_pad134[ 4 ];                // +0x8D20
    klock_queue_handle_t m_selfmap_lock_handle[ 4 ];  // +0x8D40
    std::uint64_t m_prcb_pad134a[ 4 ];               // +0x8DA0
    std::uint8_t m_prcb_pad138[ 128 ];               // +0x8DC0
    std::uint8_t m_prcb_pad138a[ 64 ];               // +0x8E40
    std::uint64_t m_kernel_directory_table_base;    // +0x8E80
    std::uint64_t m_rsp_base_shadow;               // +0x8E88
    std::uint64_t m_user_rsp_shadow;               // +0x8E90
    std::uint32_t m_shadow_flags;                  // +0x8E98
    std::uint32_t m_prcb_pad138b;                  // +0x8E9C
    std::uint64_t m_prcb_pad138c;                  // +0x8EA0
    std::uint16_t m_prcb_pad138d;                  // +0x8EA8
    std::uint16_t m_verw_selector;                 // +0x8EAA
    std::uint32_t m_dbg_mce_nesting_level;         // +0x8EAC
    std::uint32_t m_dbg_mce_flags;                 // +0x8EB0
    std::uint32_t m_prcb_pad139b;                  // +0x8EB4
    std::uint64_t m_prcb_pad140[ 505 ];              // +0x8EB8
    std::uint64_t m_prcb_pad140a[ 8 ];               // +0x9E80
    std::uint64_t m_prcb_pad141[ 504 ];              // +0x9EC0
    std::uint8_t m_prcb_pad141a[ 64 ];               // +0xAE80
    request_mailbox_t m_request_mailbox;           // +0xAEC0
}; // Size: 0xAF00

struct kpcr_t {
    union {
        struct {
            std::uint8_t m_nmi_in_progress;              // +0x000
            std::uint8_t m_pad[3];
            std::uint32_t m_number;                      // +0x004
        };
        std::uint64_t m_gdt_base;                       // +0x000
    };
    ktss_t* m_tss_base;                                 // +0x008
    kthread_t* m_current_thread;                        // +0x010
    kprcb_t* m_current_prcb;                           // +0x018
    std::uint64_t m_irr;                               // +0x020
    std::uint64_t m_irr_active;                        // +0x028
    std::uint64_t m_spare0;                            // +0x030
    void* m_ist[8];                                    // +0x038 (Interrupt Stack Table)
    ktss_t* m_tss_base2;                               // +0x078
    std::uint64_t m_spare1;                            // +0x080
    std::uint32_t m_major_version;                     // +0x088
    std::uint32_t m_minor_version;                     // +0x08C
    std::uint32_t m_build_type;                        // +0x090
    std::uint32_t m_set_member;                        // +0x094
    std::uint8_t m_cpu_step;                           // +0x098
    std::uint8_t m_cpu_stepping;                       // +0x099
    std::uint16_t m_cpu_number;                        // +0x09A
    std::uint32_t m_spare2;                            // +0x09C
    kdpc_t* m_vdm_alert;                               // +0x0A0
    std::uint64_t m_kernel_reserved[14];               // +0x0A8
    std::uint64_t m_spare3;                            // +0x120
    kprcb_t m_prcb;                                    // +0x128
};

typedef enum _MEMORY_INFORMATION_CLASS {
    MemoryBasicInformation
} MEMORY_INFORMATION_CLASS;

enum class system_information_class_t : std::uint32_t {
    basic_information = 0,
    performance_information = 2,
    time_of_day_information = 3,
    process_information = 5,
    processor_performance_information = 8,
    interrupt_information = 23,
    exception_information = 33,
    registry_quota_information = 37,
    lookaside_information = 45
};

struct system_basic_information_t {
    std::uint32_t m_reserved;
    std::uint32_t m_timer_resolution;
    std::uint32_t m_page_size;
    std::uint32_t m_number_of_physical_pages;
    std::uint32_t m_lowest_physical_page_number;
    std::uint32_t m_highest_physical_page_number;
    std::uint32_t m_allocation_granularity;
    std::uintptr_t m_minimum_user_mode_address;
    std::uintptr_t m_maximum_user_mode_address;
    std::uintptr_t m_active_processors_affinity_mask;
    std::int8_t m_number_of_processors;
};

struct rtl_balanced_node_t {
    union {
        rtl_balanced_node_t* m_children[ 2 ];
        struct {
            rtl_balanced_node_t* m_left;
            rtl_balanced_node_t* m_right;
        };
    };
    union {
        std::uint8_t m_red : 1;
        rtl_balanced_node_t* m_parent;
        std::uint64_t m_value;
    };
};

struct mi_active_pfn_t {
    std::uint64_t m_page_frame : 40;
    std::uint64_t m_priority : 8;
    std::uint64_t m_color : 16;
};

struct mmpte_hardware_t {
    std::uint64_t m_valid : 1;
    std::uint64_t m_write : 1;
    std::uint64_t m_owner : 1;
    std::uint64_t m_write_through : 1;
    std::uint64_t m_cache_disable : 1;
    std::uint64_t m_accessed : 1;
    std::uint64_t m_dirty : 1;
    std::uint64_t m_large_page : 1;
    std::uint64_t m_global : 1;
    std::uint64_t m_copy_on_write : 1;
    std::uint64_t m_prototype : 1;
    std::uint64_t m_reserved0 : 1;
    std::uint64_t m_page_frame_number : 36;
    std::uint64_t m_reserved1 : 4;
    std::uint64_t m_software_ws_index : 11;
    std::uint64_t m_no_execute : 1;
};

struct mmpte_t {
    union {
        mmpte_hardware_t m_hard;
        std::uint64_t m_value;
    };
};

struct mipfnblink_t {
    union {
        std::uint64_t m_blink : 40;
        std::uint64_t m_type_size : 24;
    };
};

struct rtl_process_module_info_t {
    void* m_section;
    void* m_mapped_base;
    void* m_image_base;
    std::uint32_t   m_image_size;
    std::uint32_t   m_flags;
    std::uint16_t   m_load_order_index;
    std::uint16_t   m_init_order_index;
    std::uint16_t   m_load_count;
    std::uint16_t   m_offset_to_filename;
    char    m_full_path_name[ 256 ];
};

struct rtl_process_modules_t {
    std::uint32_t               m_number_of_modules;
    rtl_process_module_info_t   m_modules[ 1 ];
};

struct mmpfnentry1_t {
    std::uint8_t m_page_color : 6;
    std::uint8_t m_modified : 1;
    std::uint8_t m_read_in_progress : 1;
};

struct mmpfnentry3_t {
    std::uint8_t m_write_in_progress : 1;
    std::uint8_t m_protection_code : 5;
    std::uint8_t m_modified_write : 1;
    std::uint8_t m_read_in_progress : 1;
};

struct mi_pfn_ulong5_t {
    union {
        struct {
            std::uint32_t m_modified_write_count : 16;
            std::uint32_t m_shared_count : 16;
        };
        std::uint32_t m_entire_field;
    };
};

struct mmpfn_t {
    union {
        list_entry_t m_list_entry;
        rtl_balanced_node_t m_tree_node;
        struct {
            union {
                union {
                    single_list_entry_t m_next_slist_pfn;
                    void* m_next;
                    struct {
                        std::uint64_t m_flink : 40;
                        std::uint64_t m_node_flink_low : 24;
                    };
                    mi_active_pfn_t m_active;
                } m_u1;

                union {
                    mmpte_t* m_pte_address;
                    std::uint64_t m_pte_long;
                };

                mmpte_t m_original_pte;
            };
        };
    };

    mipfnblink_t m_u2;

    union {
        union {
            struct {
                std::uint16_t m_reference_count;
                mmpfnentry1_t m_e1;
                mmpfnentry3_t m_e3;
            };
            struct {
                std::uint16_t m_reference_count2;
            } m_e2;
            struct {
                std::uint32_t m_entire_field;
            } m_e4;
        };
    } m_u3;

    mi_pfn_ulong5_t m_u5;

    union {
        union {
            struct {
                std::uint64_t m_pte_frame : 40;
                std::uint64_t m_resident_page : 1;
                std::uint64_t m_unused1 : 1;
                std::uint64_t m_unused2 : 1;
                std::uint64_t m_partition : 10;
                std::uint64_t m_file_only : 1;
                std::uint64_t m_pfn_exists : 1;
                std::uint64_t m_node_flink_high : 5;
                std::uint64_t m_page_identity : 3;
                std::uint64_t m_prototype_pte : 1;
            };
            std::uint64_t m_entire_field;
        };
    } m_u4;
};

struct handle_table_entry_info_t {
    std::uint32_t m_audit_mask;                     // +0x000
    std::uint32_t m_max_relative_access_mask;       // +0x004
}; // Size: 0x008

struct handle_table_entry_t {
    union {
        std::uint64_t m_volatile_low_value;         // +0x000
        std::uint64_t m_low_value;                  // +0x000
        std::uint64_t m_ref_count_field;            // +0x000
        handle_table_entry_info_t* m_info_table;    // +0x000
        struct {
            std::uint64_t m_unlocked : 1;           // Bit  0
            std::uint64_t m_ref_cnt : 16;           // Bits 1-16
            std::uint64_t m_attributes : 3;         // Bits 17-19
            std::uint64_t m_object_pointer : 44;    // Bits 20-63
        };
    };
    union {
        std::uint64_t m_high_value;                 // +0x008
        handle_table_entry_t* m_next_free_entry;    // +0x008
    };
}; // Size: 0x010

struct handle_table_t {
    std::uint32_t m_next_handle_needing_pool;       // +0x000
    std::int32_t m_extra_info_pages;                // +0x004
    std::uint64_t m_table_code;                     // +0x008
    eprocess_t* m_quota_process;                    // +0x010
    list_entry_t m_handle_table_list;               // +0x018
    std::uint32_t m_unique_process_id;              // +0x028
}; // Size: 0x02C

struct nmi_handler_callback_t {
    nmi_handler_callback_t* m_next;     // +0x000 Points to next entry in list
    void*                 m_callback;  // +0x008 NMI callback routine
    void*                 m_context;   // +0x010 Context passed to callback
    void*                 m_handle;    // +0x018 Registration handle (points to self)
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

struct physical_memory_range_t {
    large_integer_t m_base_page;
    large_integer_t m_page_count;
}; // Size: 0x010

// Physical memory descriptor structure
struct mm_physical_memory_descriptor_t {
    std::uint32_t m_number_of_runs;                  // +0x000 Number of physical memory runs
    std::uint32_t m_number_of_pages;                 // +0x004 Total number of physical pages
    physical_memory_range_t m_runs[ 1 ];               // +0x008 Array of physical memory ranges
}; // Size: variable

// NUMA node physical memory information
// NUMA node physical memory information
struct mm_numa_node_memory_t {
    std::uint64_t m_node_number;                     // +0x0000
    std::uint64_t m_color_count;                     // +0x0008
    std::uint64_t m_color_count_max;                 // +0x0010
    std::uint64_t m_page_counts[ 32 ];                 // +0x0018
    std::uint64_t m_zero_counts[ 32 ];                 // +0x0118
    std::uint64_t m_free_counts[ 32 ];                 // +0x0218
    std::uint64_t m_modified_page_count_total;       // +0x0318
    std::uint64_t m_modified_page_count_by_color[ 32 ]; // +0x0320
    std::uint64_t m_free_page_count;                 // +0x0420
    std::uint64_t m_zero_page_count;                 // +0x0428
    std::uint64_t m_modified_page_count;             // +0x0430
    std::uint64_t m_modified_no_write_page_count;    // +0x0438
    std::uint64_t m_modified_page_list_head;         // +0x0440
    std::uint64_t m_modified_page_list_tail;         // +0x0448
    std::uint64_t m_modified_no_write_list_head;     // +0x0450
    std::uint64_t m_modified_no_write_list_tail;     // +0x0458
    std::uint64_t m_standby_page_count;              // +0x0460
    std::uint64_t m_standby_page_count_by_priority[ 8 ]; // +0x0468
    std::uint64_t m_standby_list_head[ 8 ];            // +0x04A8
    std::uint64_t m_standby_list_tail[ 8 ];            // +0x04E8
    std::uint64_t m_priority_page_count;             // +0x0528
    std::uint64_t m_page_file_page_count;            // +0x0530
    std::uint64_t m_transition_page_count;           // +0x0538
    std::uint64_t m_invalid_page_count;              // +0x0540
    std::uint64_t m_page_file_space_used;            // +0x0548
    std::uint64_t m_page_file_space_max;             // +0x0550
    std::uint64_t m_available_pages;                 // +0x0558
    std::uint64_t m_resavail_pages;                  // +0x0560
    std::uint64_t m_zero_page_count_target;          // +0x0568
    std::uint64_t m_running_zero_page_count;         // +0x0570
    std::uint64_t m_running_demand_zero_count;       // +0x0578
    std::uint64_t m_zero_page_thread_active;         // +0x0580
    std::uint64_t m_flags;                           // +0x0588
    std::uint64_t m_page_file_high_water_mark;       // +0x0590
    std::uint64_t m_cache_page_count;                // +0x0598
    std::uint64_t m_cache_page_peak;                 // +0x05A0
    std::uint64_t m_physical_memory_block;           // +0x05A8
    std::uint64_t m_process_list_head;               // +0x05B0
    std::uint64_t m_process_list_tail;               // +0x05B8
    std::uint64_t m_session_list_head;               // +0x05C0
    std::uint64_t m_session_list_tail;               // +0x05C8
    std::uint64_t m_trim_context;                    // +0x05D0
    std::uint64_t m_zero_thread_handle;              // +0x05D8
    std::uint64_t m_free_page_list_head;             // +0x05E0
    std::uint64_t m_free_page_list_tail;             // +0x05E8
    std::uint64_t m_total_pages_detailed[ 4 ];         // +0x05F0
    std::uint64_t m_valid_page_count;                // +0x0610
    std::uint64_t m_vapor_page_count;                // +0x0618
    std::uint64_t m_vapor_page_threshold;            // +0x0620
    std::uint64_t m_vapor_free_threshold;            // +0x0628
    std::uint64_t m_vapor_free_action;               // +0x0630
    std::uint64_t m_system_page_table_pages;         // +0x0638
    std::uint64_t m_total_committed_pages;           // +0x0640
    std::uint64_t m_commit_limit;                    // +0x0648
    std::uint64_t m_commit_limit_max;                // +0x0650
    std::uint64_t m_commit_exceeded;                 // +0x0658
    std::uint64_t m_system_cache_pages;              // +0x0660
    std::uint64_t m_page_hash_pages;                 // +0x0668
    std::uint64_t m_page_hash_pages_peak;            // +0x0670
    std::uint64_t m_resident_available_pages;        // +0x0678
    std::uint64_t m_resident_available_pages_target; // +0x0680
    std::uint64_t m_modified_write_count;            // +0x0688
    std::uint64_t m_modified_page_writer_active;     // +0x0690
    std::uint64_t m_modified_page_writer_no_write;   // +0x0698
    std::uint64_t m_modified_page_writer_triggers;   // +0x06A0
    std::uint64_t m_modified_page_writer_high;       // +0x06A8
    std::uint64_t m_modified_page_writer_max;        // +0x06B0
    std::uint64_t m_mapped_page_writer_active;       // +0x06B8
    std::uint64_t m_mapped_page_writer_triggers;     // +0x06C0
    std::uint64_t m_mapped_page_writer_high;         // +0x06C8
    std::uint64_t m_mapped_page_writer_max;          // +0x06D0
    std::uint64_t m_system_page_list_head;           // +0x06D8
    std::uint64_t m_system_page_list_tail;           // +0x06E0
    std::uint64_t m_system_page_count;               // +0x06E8
    std::uint64_t m_process_commit;                  // +0x06F0
    std::uint64_t m_last_trim_time;                  // +0x06F8
    std::uint64_t m_page_fault_count;                // +0x0700
    std::uint64_t m_copy_on_write_count;             // +0x0708
    std::uint64_t m_transition_page_fault_count;     // +0x0710
    std::uint64_t m_demand_zero_fault_count;         // +0x0718
    std::uint64_t m_page_read_count;                 // +0x0720
    std::uint64_t m_page_io_count;                   // +0x0728
    std::uint64_t m_mapped_page_count;               // +0x0730
    std::uint64_t m_mapped_page_count_peak;          // +0x0738
    std::uint64_t m_mapped_write_count;              // +0x0740
    std::uint64_t m_mapped_page_list_head;           // +0x0748
    std::uint64_t m_mapped_page_list_tail;           // +0x0750
    std::uint64_t m_page_write_count;                // +0x0758
    std::uint64_t m_modified_page_write_count;       // +0x0760
    std::uint64_t m_mapped_page_write_count;         // +0x0768
    std::uint64_t m_io_space_commit;                 // +0x0770
    std::uint64_t m_rotate_physical_for_standby_reuse; // +0x0778
    std::uint64_t m_file_backed_pages;               // +0x0780
    std::uint64_t m_file_backed_pages_peak;          // +0x0788
    std::uint64_t m_mft_bitmap_commit;               // +0x0790
    std::uint64_t m_mft_bitmap_commit_peak;          // +0x0798
    std::uint64_t m_mft_bitmap_commit_exceeds_max;   // +0x07A0
    std::uint64_t m_cow_page_count;                  // +0x07A8
    std::uint64_t m_modified_page_life;              // +0x07B0
    std::uint64_t m_modified_page_life_cycles;       // +0x07B8
    std::uint64_t m_io_page_read_count;              // +0x07C0
    std::uint64_t m_io_page_read_bytes;              // +0x07C8
    std::uint64_t m_standby_repurposed_by_priority[ 8 ]; // +0x07D0
    std::uint64_t m_repurposed_by_priority_pages;    // +0x0810
    std::uint64_t m_last_standby_repurpose_time;     // +0x0818
    std::uint64_t m_repurposed_standby_page_count;   // +0x0820
    std::uint64_t m_available_page_count_target;     // +0x0828
    std::uint64_t m_dirty_page_threshold;            // +0x0830
    std::uint64_t m_dirty_page_target;               // +0x0838
    std::uint64_t m_modified_page_threshold;         // +0x0840
    std::uint64_t m_modified_page_target;            // +0x0848
    std::uint64_t m_available_page_threshold;        // +0x0850
    std::uint64_t m_mapped_page_threshold;           // +0x0858
    std::uint64_t m_mapped_page_target;              // +0x0860
    std::uint64_t m_system_cache_threshold;          // +0x0868
    std::uint64_t m_system_cache_target;             // +0x0870
    std::uint64_t m_system_pte_threshold;            // +0x0878
    std::uint64_t m_system_pte_target;               // +0x0880
    std::uint64_t m_system_pte_minimum;              // +0x0888
    std::uint64_t m_non_page_table_large_page_count; // +0x0890
    std::uint64_t m_system_large_page_table_count;   // +0x0898
    std::uint64_t m_speculative_read_count;          // +0x08A0
    std::uint64_t m_speculative_read_override_count; // +0x08A8
    std::uint64_t m_fs_cache_pages;                  // +0x08B0
    std::uint64_t m_fs_cache_pages_peak;             // +0x08B8
    std::uint64_t m_shared_commit;                   // +0x08C0
    std::uint64_t m_shared_commit_peak;              // +0x08C8
    std::uint64_t m_page_by_color_use;               // +0x08D0
    std::uint64_t m_system_working_set_target;       // +0x08D8
    std::uint64_t m_system_working_set_minimum;      // +0x08E0
    std::uint64_t m_write_combined_failures;         // +0x08E8
    std::uint64_t m_write_combined_bytes;            // +0x08F0
    std::uint64_t m_system_available_page_count;     // +0x08F8
    std::uint64_t m_zero_and_free_page_count;        // +0x0900
    std::uint64_t m_extended_commit_limit;           // +0x0908
    std::uint64_t m_per_process_paged_limit;         // +0x0910
    std::uint64_t m_virtual_size_limit;              // +0x0918
    std::uint64_t m_session_commit;                  // +0x0920
    std::uint64_t m_session_commit_peak;             // +0x0928
    std::uint64_t m_driver_commit;                   // +0x0930
    std::uint64_t m_driver_commit_peak;              // +0x0938
    std::uint64_t m_driver_locked_commit;            // +0x0940
    std::uint64_t m_driver_locked_commit_peak;       // +0x0948
    std::uint64_t m_driver_non_paged_commit;         // +0x0950
    std::uint64_t m_driver_paged_commit;             // +0x0958
    std::uint64_t m_driver_ws_peak;                  // +0x0960
    std::uint64_t m_driver_commit_by_type[ 10 ];       // +0x0968
    std::uint64_t m_driver_commit_peak_by_type[ 10 ];  // +0x09B8
    std::uint64_t m_unused_segment_pages;            // +0x0A08
    std::uint64_t m_unused_segment_pages_peak;       // +0x0A10
    std::uint64_t m_unused_segment_threshold;        // +0x0A18
    std::uint64_t m_unused_segment_target;           // +0x0A20
    std::uint64_t m_bad_pages;                       // +0x0A28
    std::uint64_t m_zero_page_count_active;          // +0x0A30
    std::uint64_t m_repurposed_page_count_by_color[ 32 ]; // +0x0A38
    std::uint64_t m_repurposed_page_count;           // +0x0B38
    std::uint64_t m_process_count;                   // +0x0B40
    std::uint64_t m_trim_count;                      // +0x0B48
    std::uint64_t m_unused_segments;                 // +0x0B50
    std::uint64_t m_total_unused_segments;           // +0x0B58
    std::uint64_t m_segment_created;                 // +0x0B60
    std::uint64_t m_segment_deleted;                 // +0x0B68
    std::uint64_t m_total_used_segments;             // +0x0B70
    std::uint64_t m_segments_no_write;               // +0x0B78
    std::uint64_t m_segments_with_write;             // +0x0B80
    std::uint64_t m_page_table_pages;                // +0x0B88
    std::uint64_t m_page_table_page_peak;            // +0x0B90
    std::uint64_t m_vad_bitmap_pages;                // +0x0B98
    std::uint64_t m_vad_bitmap_page_peak;            // +0x0BA0
    std::uint64_t m_shared_commit_charges;           // +0x0BA8
    std::uint64_t m_shared_commit_charges_peak;      // +0x0BB0
    std::uint64_t m_hyper_space_hash_pages;          // +0x0BB8
    std::uint64_t m_hyper_space_hash_pages_peak;     // +0x0BC0
    std::uint64_t m_session_view_pages;              // +0x0BC8
    std::uint64_t m_session_view_pages_peak;         // +0x0BD0
    std::uint64_t m_wsle_hash_pages;                 // +0x0BD8
    std::uint64_t m_wsle_hash_pages_peak;            // +0x0BE0
    std::uint64_t m_modified_no_write_page_list_sum; // +0x0BE8
    std::uint64_t m_modified_page_list_sum;          // +0x0BF0
    std::uint64_t m_locked_pages;                    // +0x0BF8
    std::uint64_t m_locked_pages_peak;               // +0x0C00
    std::uint64_t m_free_page_list_head_backup;      // +0x0C08
    std::uint64_t m_modified_page_list_head_backup;  // +0x0C10
    std::uint64_t m_modified_no_write_page_list_head_backup; // +0x0C18
    std::uint64_t m_numa_node_count;                 // +0x0C20
    std::uint64_t m_hardware_page_count;             // +0x0C28
    std::uint64_t m_additional_counts[ 64 ];           // +0x0C30
    std::uint64_t m_colored_page_info_base;          // +0x0E30
    std::uint64_t m_colored_page_info_flags;         // +0x0E38
    std::uint64_t m_colored_page_info_pages;         // +0x0E40
    std::uint64_t m_colored_page_info_page_counts[ 32 ]; // +0x0E48
    std::uint64_t m_standby_list_by_priority_base;   // +0x0F48
    std::uint64_t m_standby_list_by_priority_flags;  // +0x0F50
    std::uint64_t m_standby_list_by_priority_pages;  // +0x0F58
    std::uint64_t m_standby_list_by_priority_counts[ 8 ]; // +0x0F60
    std::uint64_t m_repurposed_page_info_base;       // +0x0FA0
    std::uint64_t m_repurposed_page_info_flags;      // +0x0FA8
    std::uint64_t m_repurposed_page_info_pages;      // +0x0FB0
    std::uint64_t m_repurposed_page_info_counts[ 32 ]; // +0x0FB8
    std::uint64_t m_numa_available_pages;            // +0x10B8
    std::uint64_t m_system_pte_info_base;            // +0x10C0
    std::uint64_t m_system_pte_info_flags;           // +0x10C8
    std::uint64_t m_system_pte_info_pages;           // +0x10D0
    std::uint64_t m_system_pte_info_counts[ 4 ];       // +0x10D8
    std::uint64_t m_kernel_stack_pages;              // +0x10F8
    std::uint64_t m_kernel_stack_pages_peak;         // +0x1100
    std::uint64_t m_energy_values[ 10 ];               // +0x1108
    std::uint64_t m_virtual_memory_thresholds[ 10 ];   // +0x1158
    std::uint64_t m_virtual_memory_counters[ 10 ];     // +0x11A8
    std::uint64_t m_system_image_info_base;          // +0x11F8
    std::uint64_t m_system_image_info_flags;         // +0x1200
    std::uint64_t m_system_image_info_pages;         // +0x1208
    std::uint64_t m_system_image_info_page_counts[ 4 ]; // +0x1210
    std::uint64_t m_system_pte_counts[ 4 ];            // +0x1230
    std::uint64_t m_kernel_page_counts[ 4 ];           // +0x1250
    std::uint64_t m_pfn_database_pages;              // +0x1270
    std::uint64_t m_non_paged_pool_pages;            // +0x1278
    std::uint64_t m_non_paged_pool_pages_peak;       // +0x1280
    std::uint64_t m_non_paged_pool_allocation;       // +0x1288
    std::uint64_t m_non_paged_pool_allocation_peak;  // +0x1290
    std::uint64_t m_non_paged_pool_space;            // +0x1298
    std::uint64_t m_non_paged_pool_space_peak;       // +0x12A0
    std::uint64_t m_non_paged_pool_pages_free;       // +0x12A8
    std::uint64_t m_paged_pool_pages;                // +0x12B0
    std::uint64_t m_paged_pool_pages_peak;           // +0x12B8
    std::uint64_t m_paged_pool_allocation;           // +0x12C0
    std::uint64_t m_paged_pool_allocation_peak;      // +0x12C8
    std::uint64_t m_paged_pool_space;                // +0x12D0
    std::uint64_t m_paged_pool_space_peak;           // +0x12D8
    std::uint64_t m_paged_pool_pages_free;           // +0x12E0
    std::uint64_t m_nv_large_page_count;             // +0x12E8
    std::uint64_t m_system_ptes_large;               // +0x12F0
    std::uint64_t m_system_ptes_large_peak;          // +0x12F8
    std::uint64_t m_system_ptes_small;               // +0x1300
    std::uint64_t m_system_ptes_small_peak;          // +0x1308
    std::uint64_t m_caching_and_mapping_count;       // +0x1310
    std::uint64_t m_caching_and_mapping_count_peak;  // +0x1318
    std::uint64_t m_resident_available_pages_initial; // +0x1320
    std::uint64_t m_resident_available_pages_peak;   // +0x1328
    std::uint64_t m_hyper_space_pages;               // +0x1330
    std::uint64_t m_system_code_pages;               // +0x1338
    std::uint64_t m_system_driver_pages;             // +0x1340
    std::uint64_t m_system_registry_pages;           // +0x1348
    std::uint64_t m_system_page_table_pages_peak;    // +0x1350
    std::uint64_t m_counter_array_size;              // +0x1358
    std::uint64_t m_system_cache_page_peak;          // +0x1360
    std::uint64_t m_page_lookup_count;               // +0x1368
    std::uint64_t m_page_lookup_cycles;              // +0x1370
    std::uint64_t m_average_idle_time_cycles;        // +0x1378
    std::uint64_t m_memory_management_count;         // +0x1380
    std::uint64_t m_memory_management_cycles;        // +0x1388
    std::uint64_t m_standby_cache_reserve_pages;     // +0x1390
    std::uint64_t m_standby_cache_normal_priority_pages; // +0x1398
    std::uint64_t m_standby_cache_core_pages;        // +0x13A0
    std::uint64_t m_standby_cache_reserve_pages_target; // +0x13A8
    std::uint64_t m_standby_cache_normal_priority_pages_target; // +0x13B0
    std::uint64_t m_standby_cache_core_pages_target; // +0x13B8
    std::uint64_t m_hot_add_memory_pages;            // +0x13C0
    std::uint64_t m_removed_memory_pages;            // +0x13C8
    std::uint64_t m_hot_remove_memory_pages;         // +0x13D0
    std::uint64_t m_removed_memory_one_time;         // +0x13D8
    std::uint64_t m_reprogram_virtual_address_bit;   // +0x13E0
    std::uint64_t m_reprogram_virtual_address_flag;  // +0x13E8
    std::uint64_t m_total_system_driver_pages;       // +0x13F0
    std::uint64_t m_total_system_code_pages;         // +0x13F8
    std::uint64_t m_page_file_io_alignment;          // +0x1400
    std::uint64_t m_kernel_total_page_count;         // +0x1408
    std::uint64_t m_pfn_list_corrupt;                // +0x1410
    std::uint64_t m_virtual_address_space_number;    // +0x1418
    std::uint64_t m_virtual_address_space_number_win8; // +0x1420
    std::uint64_t m_second_level_cache_errors;       // +0x1428
    std::uint64_t m_third_level_cache_errors;        // +0x1430
    std::uint64_t m_soft_reserved_pages;             // +0x1438
    std::uint64_t m_soft_committed_pages;            // +0x1440
    std::uint64_t m_vsl_process_memory;              // +0x1448
    std::uint64_t m_core_driver_pages;               // +0x1450
    std::uint64_t m_core_kernel_stack_pages;         // +0x1458
    std::uint64_t m_page_file_bytes;                 // +0x1460
    std::uint64_t m_virtual_memory_bytes;            // +0x1468
    std::uint64_t m_page_file_and_virtual_memory_bytes; // +0x1470
    std::uint64_t m_session_driver_pages;            // +0x1478
    std::uint64_t m_session_mapped_pages;            // +0x1480
    std::uint64_t m_session_dirty_pages;             // +0x1488
    std::uint64_t m_reserved_pages;                  // +0x1490
    std::uint64_t m_shared_image_pages;              // +0x1498
    std::uint64_t m_shared_system_pages;             // +0x14A0
    std::uint64_t m_system_driver_resident_pages;    // +0x14A8
    std::uint64_t m_system_code_resident_pages;      // +0x14B0
    std::uint64_t m_write_combined_bytes_requested;  // +0x14B8
    std::uint64_t m_write_combined_bytes_completed;  // +0x14C0
    std::uint64_t m_shared_commit_charges_success;   // +0x14C8
    std::uint64_t m_hybrid_sleep_pages;              // +0x14D0
    std::uint64_t m_hybrid_sleep_pages_peak;         // +0x14D8
    std::uint64_t m_image_load_backed_pages;         // +0x14E0
    std::uint64_t m_image_load_backed_pages_peak;    // +0x14E8
    std::uint64_t m_post_crash_memory_dump_count;    // +0x14F0
    std::uint64_t m_dump_heap_pages;                 // +0x14F8
    std::uint64_t m_dump_heap_pages_peak;            // +0x1500
    std::uint64_t m_virtual_address_space_expansions; // +0x1508
    std::uint64_t m_virtual_address_space_contractions; // +0x1510
    std::uint64_t m_page_file_space_used_peak;       // +0x1518
    std::uint64_t m_page_file_backing_store_io;      // +0x1520
    std::uint64_t m_page_file_backing_store_io_peak; // +0x1528
    std::uint64_t m_unused_system_ptes;              // +0x1530
    std::uint64_t m_system_ptes_used_peak;           // +0x1538
    std::uint64_t m_process_reflector_pages;         // +0x1540
    std::uint64_t m_process_reflector_pages_peak;    // +0x1548
    std::uint64_t m_process_reflector_2_pages;       // +0x1550
    std::uint64_t m_process_reflector_2_pages_peak;  // +0x1558
    std::uint64_t m_process_reflector_3_pages;       // +0x1560
    std::uint64_t m_process_reflector_3_pages_peak;  // +0x1568
    std::uint64_t m_process_reflector_non_paged_pool_usage; // +0x1570
    std::uint64_t m_process_reflector_non_paged_pool_usage_peak; // +0x1578
    std::uint64_t m_process_reflector_paged_pool_usage; // +0x1580
    std::uint64_t m_process_reflector_paged_pool_usage_peak; // +0x1588
    std::uint64_t m_process_reflector_session_mapped_pages; // +0x1590
    std::uint64_t m_process_reflector_session_mapped_pages_peak; // +0x1598
    std::uint64_t m_unused_segment_free_pages;       // +0x15A0
    std::uint64_t m_unused_segment_free_pages_peak;  // +0x15A8
    std::uint64_t m_prefetch_pages;                  // +0x15B0
    std::uint64_t m_prefetch_pages_peak;             // +0x15B8
    std::uint64_t m_zero_access_pages;               // +0x15C0
    std::uint64_t m_zero_access_pages_peak;          // +0x15C8
    std::uint64_t m_compressed_pages;                // +0x15D0
    std::uint64_t m_compressed_pages_peak;           // +0x15D8
    std::uint64_t m_contiguous_memory_failures;      // +0x15E0
    std::uint64_t m_throttled_pages;                 // +0x15E8
    std::uint64_t m_cfg_failure_pages;               // +0x15F0
    std::uint64_t m_available_pages_for_priority_0;  // +0x15F8
    std::uint64_t m_low_page_table_pages;            // +0x1600
    std::uint64_t m_window_manager_pool_pages;       // +0x1608
    std::uint64_t m_system_available_charge;         // +0x1610
    std::uint64_t m_target_pages_for_priority_0;     // +0x1618
    std::uint64_t m_demand_zero_faults_in_page_file; // +0x1620
    std::uint64_t m_demand_zero_metadata_faults;     // +0x1628
    std::uint64_t m_cow_metadata_faults;             // +0x1630
    std::uint64_t m_demand_zero_faults_with_prefetch; // +0x1638
    std::uint64_t m_demand_zero_page_file_with_prefetch; // +0x1640
    std::uint64_t m_demand_zero_metadata_faults_with_prefetch; // +0x1648
    std::uint64_t m_cow_metadata_faults_with_prefetch; // +0x1650
    std::uint64_t m_abort_prefetch_for_no_access;    // +0x1658
    std::uint64_t m_abort_prefetch_for_paging_io;    // +0x1660
    std::uint64_t m_abort_prefetch_for_locked_pages; // +0x1668
    std::uint64_t m_abort_prefetch_for_no_memory;    // +0x1670
    std::uint64_t m_abort_prefetch_for_address_range; // +0x1678
    std::uint64_t m_abort_prefetch_for_address_space_delete; // +0x1680
    std::uint64_t m_abort_prefetch_for_image_mismatch; // +0x1688
    std::uint64_t m_prefetch_pages_added_by_mm;      // +0x1690
    std::uint64_t m_prefetch_pages_added_by_fs;      // +0x1698
    std::uint64_t m_prefetch_pages_added_by_first_fault; // +0x16A0
    std::uint64_t m_transition_pages_added_by_prefetch; // +0x16A8
    std::uint64_t m_transition_pages_mapped_by_prefetch; // +0x16B0
    std::uint64_t m_standby_pages_added_by_prefetch; // +0x16B8
    std::uint64_t m_standby_pages_mapped_by_prefetch; // +0x16C0
    std::uint64_t m_prefetch_pages_added_by_mm_without_standby; // +0x16C8
    std::uint64_t m_prefetch_pages_added_by_fs_without_standby; // +0x16D0
    std::uint64_t m_mft_records_in_use;              // +0x16D8
    std::uint64_t m_mft_records_in_use_peak;         // +0x16E0
    std::uint64_t m_mft_records_allocated;           // +0x16E8
    std::uint64_t m_mft_records_allocated_peak;      // +0x16F0
    std::uint64_t m_mft_lost_records;                // +0x16F8
    std::uint64_t m_mft_all_records;                 // +0x1700
    std::uint64_t m_mft_record_segments;             // +0x1708
    std::uint64_t m_mft_bytes_allocated;             // +0x1710
    std::uint64_t m_mft_bytes_in_use;                // +0x1718
    std::uint64_t m_mft_bytes_in_use_peak;           // +0x1720
    std::uint64_t m_root_system_page_table_entries;  // +0x1728
    std::uint64_t m_root_system_page_table_entries_peak; // +0x1730
    std::uint64_t m_non_page_table_pool_usage;       // +0x1738
    std::uint64_t m_non_page_table_pool_usage_peak;  // +0x1740
    std::uint64_t m_io_space_page_table_entries;     // +0x1748
    std::uint64_t m_io_space_page_table_entries_peak; // +0x1750
    std::uint64_t m_system_page_table_entries_in_use; // +0x1758
    std::uint64_t m_system_large_page_table_entries_in_use; // +0x1760
    std::uint64_t m_spare_page_table_entries;        // +0x1768
    std::uint64_t m_demand_zero_system_cache_views;  // +0x1770
    std::uint64_t m_page_fault_io_system_cache_views; // +0x1778
    std::uint64_t m_demand_zero_metadata_faults_system_cache_views; // +0x1780
    std::uint64_t m_cow_metadata_faults_system_cache_views; // +0x1788
    std::uint64_t m_mft_bitmap_allocated;            // +0x1790
    std::uint64_t m_mft_bitmap_allocated_peak;       // +0x1798
    std::uint64_t m_mft_bitmap_bytes;                // +0x17A0
    std::uint64_t m_mft_bitmap_bytes_peak;           // +0x17A8
    std::uint64_t m_mft_bitmap_pages;                // +0x17B0
    std::uint64_t m_mft_bitmap_pages_peak;           // +0x17B8
    std::uint64_t m_page_table_pages_for_core_kernel; // +0x17C0
    std::uint64_t m_resident_pages_for_core_kernel;  // +0x17C8
    std::uint64_t m_resident_pages_for_core_drivers; // +0x17D0
    std::uint64_t m_resident_pages_for_driver_stack; // +0x17D8
    std::uint64_t m_resident_pages_for_mapping_tables; // +0x17E0
    std::uint64_t m_resident_pages_for_mirroring;    // +0x17E8
    std::uint64_t m_resident_pages_for_nonpaged_pool; // +0x17F0
    std::uint64_t m_resident_pages_for_platform_data; // +0x17F8
    std::uint64_t m_resident_pages_for_reserved;     // +0x1800
    std::uint64_t m_bytes_active;                    // +0x1808
    std::uint64_t m_bytes_standby;                   // +0x1810
    std::uint64_t m_bytes_transition;                // +0x1818
    std::uint64_t m_bytes_modified;                  // +0x1820
    std::uint64_t m_bytes_modified_no_write;         // +0x1828
    std::uint64_t m_bytes_modified_page_file_backed; // +0x1830
    std::uint64_t m_bytes_standby_core;              // +0x1838
    std::uint64_t m_bytes_standby_normal_priority;   // +0x1840
    std::uint64_t m_bytes_standby_reserve;           // +0x1848
    std::uint64_t m_bytes_standby_repurposed_page_file_backed; // +0x1850
    std::uint64_t m_bytes_standby_repurposed_file_backed; // +0x1858
    std::uint64_t m_available_bytes;                 // +0x1860
    std::uint64_t m_available_bytes_for_overcommit;  // +0x1868
    std::uint64_t m_standby_bytes_for_priority_0;    // +0x1870
    std::uint64_t m_standby_bytes_for_priority_1;    // +0x1878
    std::uint64_t m_standby_bytes_for_priority_2;    // +0x1880
    std::uint64_t m_standby_bytes_for_priority_3;    // +0x1888
    std::uint64_t m_standby_bytes_for_priority_4;    // +0x1890
    std::uint64_t m_standby_bytes_for_priority_5;    // +0x1898
    std::uint64_t m_standby_bytes_for_priority_6;    // +0x18A0
    std::uint64_t m_standby_bytes_for_priority_7;    // +0x18A8
    std::uint64_t m_available_bytes_for_priority_0;  // +0x18B0
    std::uint64_t m_resident_bytes;                  // +0x18B8
    std::uint64_t m_shared_bytes;                    // +0x18C0
    std::uint64_t m_available_without_standby_bytes; // +0x18C8
    std::uint64_t m_commit_bytes;                    // +0x18D0
    std::uint64_t m_commit_limit_bytes;              // +0x18D8
    std::uint64_t m_commit_peak_bytes;               // +0x18E0
    std::uint64_t m_kernel_bytes;                    // +0x18E8
    std::uint64_t m_kernel_total_bytes;              // +0x18F0
    std::uint64_t m_kernel_paged_bytes;              // +0x18F8
    std::uint64_t m_kernel_nonpaged_bytes;           // +0x1900
    std::uint64_t m_page_file_bytes_peak;            // +0x1908
    std::uint64_t m_system_cache_bytes;              // +0x1910
    std::uint64_t m_page_size_bytes;                 // +0x1918
    std::uint64_t m_system_driver_bytes;             // +0x1920
    std::uint64_t m_system_code_bytes;               // +0x1928
    std::uint64_t m_pool_paged_bytes;                // +0x1930
    std::uint64_t m_pool_nonpaged_bytes;             // +0x1938
    std::uint64_t m_pool_paged_bytes_resident;       // +0x1940
    std::uint64_t m_system_driver_bytes_resident;    // +0x1948
    std::uint64_t m_system_code_bytes_resident;      // +0x1950
    std::uint64_t m_pool_paged_bytes_peak;           // +0x1958
    std::uint64_t m_pool_nonpaged_bytes_peak;        // +0x1960
    std::uint64_t m_pool_nonpaged_bytes_resident;    // +0x1968
    std::uint64_t m_cache_bytes;                     // +0x1970
    std::uint64_t m_cache_bytes_peak;                // +0x1978
    std::uint64_t m_pool_nonpaged_allocation;        // +0x1980
    std::uint64_t m_pool_paged_allocation;           // +0x1988
    std::uint64_t m_system_bytes;                    // +0x1990
    std::uint64_t m_system_cache_resident_bytes;     // +0x1998
    std::uint64_t m_system_driver_total_bytes;       // +0x19A0
    std::uint64_t m_system_code_total_bytes;         // +0x19A8
    std::uint64_t m_pool_paged_resident_bytes;       // +0x19B0
    std::uint64_t m_pool_nonpaged_resident_bytes;    // +0x19B8
    std::uint64_t m_lowest_page;                     // +0x1B00 Lowest physical page in this node
    std::uint64_t m_highest_page;                    // +0x1B08 Highest physical page in this node
    std::uint64_t m_total_pages;                     // +0x1B10 Total pages in this node
}; // Size: 0x1B18

// NUMA information structure
struct mm_numa_info_t {
    std::uint32_t m_number_of_nodes;                 // Number of NUMA nodes
    std::uint32_t m_max_node_count;                  // Maximum possible nodes
    mm_numa_node_memory_t* m_node_memory[ 1 ];         // Array of pointers to node memory info
};

// Physical memory block structure
struct mm_physical_memory_block_t {
    std::uint64_t m_number_of_physical_pages;        // Total number of physical pages
    mm_physical_memory_descriptor_t* m_physical_memory_descriptor; // Pointer to physical memory descriptor
    mm_numa_info_t* m_numa_information;              // Pointer to NUMA information
};

struct mm_copy_address_t {
    union {
        std::uint64_t m_virtual_address;    // +0x000
        physical_address_t m_physical_address;   // +0x000
    };
}; // Size: 0x008

struct object_type_initializer_t {
    std::uint16_t m_length;
    std::uint8_t m_unused;
    std::uint8_t m_object_type_flags;
    std::uint32_t m_object_type_code;
    std::uint32_t m_invalid_attributes;
    std::uint32_t m_generic_mapping;
    std::uint32_t m_valid_access;
    std::uint8_t m_retain_access;
    std::uint8_t m_pool_type;
    std::uint32_t m_default_page_charge;
    std::uint32_t m_security_required;
    void* m_security_procedure;
    void* m_delete_procedure;
    void* m_close_procedure;
    void* m_parse_procedure;
    void* m_security_procedure2;
    void* m_query_name_procedure;
    void* m_okayto_close_procedure;
};

struct object_type_t {
    list_entry_t m_type_list;                          // 0x00
    unicode_string_t m_name;                           // 0x10
    void* m_default_object;                            // 0x20
    std::uint8_t m_index;                             // 0x28
    std::uint8_t m_pad0[ 3 ];                          // 0x29
    std::uint32_t m_total_number_of_objects;          // 0x2C
    std::uint32_t m_total_number_of_handles;          // 0x30
    std::uint32_t m_high_water_number_of_objects;     // 0x34
    std::uint32_t m_high_water_number_of_handles;     // 0x38
    std::uint8_t m_pad1[ 4 ];                          // 0x3C
    object_type_initializer_t m_type_info;            // 0x40
    std::uint64_t m_type_lock;                        // 0xB8
    std::uint32_t m_key;                              // 0xC0
    std::uint8_t m_pad2[ 4 ];                          // 0xC4
    list_entry_t m_callback_list;                     // 0xC8
};

using p_nmi_callback = void (*)(
    void* context,                   // Context passed during registration
    void* register_area              // CPU register state during NMI
    );

enum class ps_create_state_t : std::uint32_t {
    ps_create_initial = 0,
    ps_create_failed = 1,
    ps_create_success = 2,
    // Add any additional states if needed
};

struct ps_create_initial_t {
    std::uint32_t m_output_flags;                    // +0x000
    std::uint32_t m_file_handle;                     // +0x004
    std::uint32_t m_section_handle;                  // +0x008
    std::uint64_t m_user_process_parameters;         // +0x010
    std::uint32_t m_create_flags;                    // +0x018
    std::uint32_t m_reserved;                        // +0x01C
}; // Size: 0x020

struct ps_create_success_t {
    void* m_kernel_handle;                        // +0x000
    void* m_user_process_parameters;              // +0x008
    void* m_kernel_thread;                        // +0x010
    void* m_kernel_process;                       // +0x018
    void* m_thread_info;                          // +0x020
    void* m_process_info;                         // +0x028
}; // Size: 0x030

struct ps_create_failure_t {
    nt_status_t m_error_code;                          // +0x000
    std::uint32_t m_padding;                        // +0x004
    void* m_failed_handle;                       // +0x008
}; // Size: 0x010

struct ps_create_info_t {
    std::uint32_t m_size;                          // +0x000
    ps_create_state_t m_state;                     // +0x004
    union {
        ps_create_success_t m_success;              // +0x008
        ps_create_failure_t m_failure;              // +0x008
        ps_create_initial_t m_initial;              // +0x008
    };
}; // Size: 0x038

struct ps_attribute_t {
    std::uint32_t m_attribute;              // +0x000
    std::size_t m_size;                          // +0x004
    union {
        std::addr_t m_value;                     // +0x008
        void* m_value_ptr;                  // +0x008
    };
    std::size_t* m_return_length;                // +0x010
}; // Size: 0x018

struct ps_attribute_list_t {
    std::size_t m_total_length;                  // +0x000
    ps_attribute_t m_attributes[ 2 ];         // +0x008
}; // Size: 0x038

struct section_image_information_t {
    std::addr_t m_entry_point;                   // +0x000
    std::uint32_t m_stack_zero_bits;        // +0x008
    std::uint32_t m_stack_commit_size;      // +0x00C
    std::uint32_t m_stack_reserve_size;     // +0x010
    std::uint64_t m_subsystem_version;             // +0x018
    std::uint32_t m_dll_characteristics;    // +0x020
    std::uint16_t m_machine;                // +0x024
    std::uint8_t m_image_contains_code;     // +0x026
    std::uint8_t m_image_flags;             // +0x027
    std::uint16_t m_system_dll;             // +0x028
    std::uint16_t m_dll_count;              // +0x02A
    std::uint32_t m_lock_prefix_table;      // +0x02C
    std::uint32_t m_lock_prefix_count;      // +0x030
}; // Size: 0x034

struct object_attributes_t {
    std::uint32_t m_length;
    void* m_root_directory;
    unicode_string_t* m_object_name;
    std::uint32_t m_attributes;
    security_descriptor_t* m_security_descriptor;
    void* m_security_quality_of_service;
};

struct device_object_t;

struct driver_object_t {
    short type;
    short size;
    device_object_t* device_object;
    unsigned long flags;
    void* driver_start;
    unsigned long driver_size;
    void* driver_section;
    void* driver_extension;
    unicode_string_t driver_name;
    unicode_string_t* hardware_database;
    void* fast_io_dispatch;
    void* driver_init;
    void* driver_start_io;
    void* driver_unload;
    void* major_function[ 28 ];
};

struct device_object_t {
    short type;
    unsigned short size;
    long reference_count;
    driver_object_t* driver_object;
    device_object_t* next_device;
    device_object_t* attached_device;
    void* current_irp;
    void* timer_queue;
    unsigned long flags;
    unsigned long characteristics;
    void* vpb;
    void* device_extension;
    device_type_t device_type;
    unsigned char stack_size;
    union {
        struct {
            unsigned short pending_returned;
            unsigned short padding;
        } list_entry;
        void* wait_list_entry;
    } queue;
    unsigned long align_requirement;
    void* device_queue;
    void* dpc;
    unsigned long active_threads;
    void* security_descriptor;
    void* device_lock;
    unsigned short sector_size;
    unsigned short spare1;
    void* device_object_extension;
    void* reserved;
};

enum pool_type
{
    nonpaged_pool,
    nonpaged_pool_execute = nonpaged_pool,
    paged_pool,
    nonpaged_pool_must_succeed = nonpaged_pool + 2,
    dont_use_this_type,
    nonpaged_pool_cache_aligned = nonpaged_pool + 4,
    paged_pool_cache_aligned,
    nonpaged_pool_cache_aligned_must_s = nonpaged_pool + 6,
    max_pool_type,

    nonpaged_pool_base = 0,
    nonpaged_pool_base_must_succeed = nonpaged_pool_base + 2,
    nonpaged_pool_base_cache_aligned = nonpaged_pool_base + 4,
    nonpaged_pool_base_cache_aligned_must_s = nonpaged_pool_base + 6,

    nonpaged_pool_session = 32,
    paged_pool_session = nonpaged_pool_session + 1,
    nonpaged_pool_must_succeed_session = paged_pool_session + 1,
    dont_use_this_type_session = nonpaged_pool_must_succeed_session + 1,
    nonpaged_pool_cache_aligned_session = dont_use_this_type_session + 1,
    paged_pool_cache_aligned_session = nonpaged_pool_cache_aligned_session + 1,
    nonpaged_pool_cache_aligned_must_s_session = paged_pool_cache_aligned_session + 1,

    nonpaged_pool_nx = 512,
    nonpaged_pool_nx_cache_aligned = nonpaged_pool_nx + 4,
    nonpaged_pool_session_nx = nonpaged_pool_nx + 32,
};

struct kldr_data_table_entry_t {
    list_entry_t m_in_load_order_links;                  // +0x00
    void* m_exception_table;                            // +0x10
    std::uint32_t m_exception_table_size;               // +0x18
    void* m_gp_value;                                   // +0x20
    void* m_non_paged_debug_info;                       // +0x28
    void* m_dll_base;                                   // +0x30
    void* m_entry_point;                                // +0x38
    std::uint32_t m_size_of_image;                      // +0x40
    unicode_string_t m_full_dll_name;                   // +0x48
    unicode_string_t m_base_dll_name;                   // +0x58
    std::uint32_t m_flags;                              // +0x68
    std::uint16_t m_load_count;                         // +0x6C
    union {
        struct {
            std::uint16_t m_signature_level : 4;        // +0x6E
            std::uint16_t m_signature_type : 3;         // +0x6E
            std::uint16_t m_frozen : 2;                 // +0x6E
            std::uint16_t m_hot_patch : 1;              // +0x6E
            std::uint16_t m_unused : 6;                 // +0x6E
        };
        std::uint16_t m_entire_field;                   // +0x6E
    } u1;                                               // +0x6E
    void* m_section_pointer;                            // +0x70
    std::uint32_t m_check_sum;                          // +0x78
    std::uint32_t m_coverage_section_size;              // +0x7C
    void* m_coverage_section;                           // +0x80
    void* m_loaded_imports;                             // +0x88
    union {
        void* m_spare;                                  // +0x90
        kldr_data_table_entry_t* m_nt_data_table_entry; // +0x90
    };
    std::uint32_t m_size_of_image_not_rounded;          // +0x98
    std::uint32_t m_time_date_stamp;                    // +0x9C
}; // Size: 0xA0

struct object_type_two {
    std::uint32_t m_length;
    std::uint32_t m_object_count;
    void* m_object_creator;
};