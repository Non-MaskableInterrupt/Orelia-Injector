#define image_scn_mem_execute 0x20000000

namespace std {
    using addr_t = unsigned char*;
}

enum nt_status_t
{
    success,
    alerted = 0x101,
    timeout = 0x102,
    pending = 0x103,
    length_mismatch = 0xc4
};

enum nt_build_t
{
    win11_23h2 = 0x589c,
    win11_22h2 = 0x585d,
    win11_21h2 = 0x55f0,
    win10_22h2 = 0x5a63,
    win10_21h1 = 0x4fc6,
    win10_20h2 = 0x4ec2,
    win10_20h1 = 0x4a61,
    win_server_2022 = 0x5900,
    win_server_2019 = 0x3c5a,
    win_server_2016 = 0x23f0,
    win8_1_update = 0x1db0,
    win8_1 = 0x1a2b,
    win7_sp1 = 0x1db1,
    win7_rtm = 0x1a28
};

enum pe_magic_t
{
    dos_header = 0x5a4d,
    nt_headers = 0x4550,
    opt_header = 0x020b
};

struct unicode_string_t
{
    std::uint16_t m_length;
    std::uint16_t m_maximum_length;
    wchar_t* m_buffer;
};

struct dos_header_t
{
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

    inline
    constexpr bool is_valid( )
    {
        return m_magic == pe_magic_t::dos_header;
    }
};

struct data_directory_t
{
    std::int32_t m_virtual_address;
    std::int32_t m_size;

    template< class type_t >
    inline
    type_t as_rva(
        std::addr_t rva
    )
    {
        return reinterpret_cast< type_t >( rva + m_virtual_address );
    }
};

struct nt_headers_t
{
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

    inline
    constexpr bool is_valid( )
    {
        return m_signature == pe_magic_t::nt_headers
            && m_magic == pe_magic_t::opt_header;
    }
};

struct export_directory_t
{
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

struct section_header_t
{
    char m_name[ 0x8 ];
    union
    {
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

struct list_entry_t
{
    list_entry_t* m_flink;
    list_entry_t* m_blink;
};

struct single_list_entry_t
{
    single_list_entry_t* m_next;
};

struct ldr_data_table_entry_t
{
    list_entry_t m_in_load_order_links;
    list_entry_t m_in_memory_order_links;
    list_entry_t m_in_initialization_order_links;
    void* m_dll_base;
    void* m_entry_point;
    std::uint32_t m_size_of_image;
    unicode_string_t m_full_dll_name;
    unicode_string_t m_base_dll_name;
    std::uint32_t m_flags;
    std::uint16_t m_load_count;
    std::uint16_t m_tls_index;
    list_entry_t m_hash_links;
    std::uint64_t m_time_date_stamp;
};

struct peb_ldr_data_t
{
    std::uint32_t m_length;
    std::uint8_t m_initialized;
    void* m_ss_handle;
    list_entry_t m_in_load_order_module_list;
    list_entry_t m_in_memory_order_module_list;
    list_entry_t m_in_initialization_order_module_list;
    void* m_entry_in_progress;
};

struct rtl_user_process_parameters_t
{
    std::uint32_t m_max_length;
    std::uint32_t m_length;
    std::uint32_t m_flags;
    std::uint32_t m_debug_flags;
    void* m_console_handle;
    std::uint32_t m_console_flags;
    void* m_standard_input;
    void* m_standard_output;
    void* m_standard_error;
    unicode_string_t m_current_directory_path;
    void* m_current_directory_handle;
    unicode_string_t m_dll_path;
    unicode_string_t m_image_path_name;
    unicode_string_t m_command_line;
    void* m_environment;
    std::uint32_t m_starting_x;
    std::uint32_t m_starting_y;
    std::uint32_t m_count_x;
    std::uint32_t m_count_y;
    std::uint32_t m_count_chars_x;
    std::uint32_t m_count_chars_y;
    std::uint32_t m_fill_attribute;
    std::uint32_t m_window_flags;
    std::uint32_t m_show_window_flags;
    unicode_string_t m_window_title;
    unicode_string_t m_desktop_info;
    unicode_string_t m_shell_info;
    unicode_string_t m_runtime_data;
};

struct peb_t
{
    std::uint8_t m_inherited_address_space;
    std::uint8_t m_read_image_file_exec_options;
    std::uint8_t m_being_debugged;
    std::uint8_t m_spare_bool;
    void* m_mutant;
    void* m_image_base_address;
    peb_ldr_data_t* m_ldr;
    rtl_user_process_parameters_t* m_process_parameters;
    void* m_sub_system_data;
    void* m_process_heap;
    void* m_fast_peb_lock;
    void* m_fast_peb_lock_routine;
    void* m_fast_peb_unlock_routine;
    std::uint32_t m_environment_update_count;
    void* m_kernel_callback_table;
    std::uint32_t m_system_reserved;
    std::uint32_t m_atl_thunk_s_list_ptr32;
    void* m_free_list;
    std::uint32_t m_tls_expansion_counter;
    void* m_tls_bitmap;
    std::uint32_t m_tls_bitmap_bits[ 2 ];
    void* m_read_only_shared_memory_base;
    void* m_read_only_shared_memory_heap;
    void** m_read_only_static_server_data;
    void* m_ansi_code_page_data;
    void* m_oem_code_page_data;
    void* m_unicode_case_table_data;
    std::uint32_t m_number_of_processors;
    std::uint32_t m_nt_global_flag;
    std::uint64_t m_critical_section_timeout;
    std::uint64_t m_heap_segment_reserve;
    std::uint64_t m_heap_segment_commit;
    std::uint64_t m_heap_decommit_total_free_threshold;
    std::uint64_t m_heap_decommit_free_block_threshold;
    std::uint32_t m_number_of_heaps;
    std::uint32_t m_maximum_number_of_heaps;
    void** m_process_heaps;
    void* m_gdi_shared_handle_table;
    void* m_process_starter_helper;
    std::uint32_t m_gdi_dc_attribute_list;
    void* m_loader_lock;
    std::uint32_t m_os_major_version;
    std::uint32_t m_os_minor_version;
    std::uint16_t m_os_build_number;
    std::uint16_t m_os_csd_version;
    std::uint32_t m_os_platform_id;
    std::uint32_t m_image_subsystem;
    std::uint32_t m_image_subsystem_major_version;
    std::uint32_t m_image_subsystem_minor_version;
    std::uint64_t m_active_process_affinity_mask;
    std::uint32_t m_gdi_handle_buffer[ 60 ];
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
    std::uint64_t m_minimum_stack_commit;
};