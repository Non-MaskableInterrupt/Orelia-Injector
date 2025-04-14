namespace driver {
#pragma pack(push, 1)
    struct memory_request {
        HANDLE h_mem;
        size_t size;
        std::uint64_t phys_addr;
        void* p_mapped;
    };
#pragma pack(pop)

#pragma section(".text")
    __declspec( allocate( ".text" ) )
        const std::uint8_t syscall_stub[ ]{
        0x4C, 0x8B, 0x11, //mov    r10, QWORD PTR [rcx]
        0x8B, 0x41, 0x08, //mov    eax, DWORD PTR [rcx+0x8]
        0x0F, 0x05,       //syscall
        0xC3              //ret
    };

    template <
        typename ret_type = void,
        typename first = void*,
        typename... args
    >
    [[nodiscard]]
    ret_type syscall(
        DWORD index,
        first arg = first{},
        args... NextArgs
    ) {
        struct data_struct {
            first arg1;
            DWORD syscall_id;
        } data{ arg, index };

        using call_stub = ret_type( __fastcall* )( data_struct*, args... );
        return ( ( call_stub )&syscall_stub[ 0 ] )( &data, NextArgs... );
    }

    class c_driver {
        bool send_cmd( uint32_t ioctl_code, void* input_buffer, uint32_t input_size,
            void* output_buffer, uint32_t output_size ) const {
            if ( m_driver_handle == INVALID_HANDLE_VALUE ) {
                logging::print( encrypt( "Failed to open driver handle." ) );
                return false;
            }

            DWORD bytes_returned = 0;
            return DeviceIoControl(
                m_driver_handle,
                ioctl_code,
                input_buffer, input_size,
                output_buffer, output_size,
                &bytes_returned,
                nullptr
            ) != 0;
        }

    public:
        bool initialize( ) {
            m_driver_handle = CreateFileW(
                L"\\\\.\\inpoutx64",
                0xC0000000, 3u, 0i64, 3u, 0, 0i64
            );

            if ( !m_driver_handle || m_driver_handle == INVALID_HANDLE_VALUE ) {
                logging::print( encrypt( "Failed to open driver handle." ) );
                return false;
            }

            logging::print( encrypt( "Driver handle: 0x%llx" ), m_driver_handle );
            return true;
        }

        bool map_physical_memory( uint64_t physical_address, uint64_t size, void** mapped_address ) {
            *mapped_address = nullptr;

            memory_request request = {};
            request.phys_addr = physical_address;
            request.size = size;

            if ( !send_cmd( 0x9C40201C, &request, sizeof( request ), &request, sizeof( request ) ) ) {
                return false;
            }

            syscall( 15, request.h_mem );
            *mapped_address = request.p_mapped;
            return true;
        }

        bool read_physical_memory( uint64_t physical_address, void* buffer, uint32_t size ) {
            void* mapped_va = nullptr;
            if ( !map_physical_memory( physical_address, size, &mapped_va ) )
                return false;

            crt::memcpy( buffer, mapped_va, size );

            syscall( 42, ( HANDLE )-1, mapped_va );
            return true;
        }

        bool write_physical_memory( uint64_t physical_address, void* buffer, uint32_t size ) {
            void* mapped_va = nullptr;
            if ( !map_physical_memory( physical_address, size, &mapped_va ) )
                return false;

            crt::memcpy( mapped_va, buffer, size );

            syscall( 42, ( HANDLE )-1, mapped_va );
            return true;
        }

    private:
        HANDLE m_driver_handle = INVALID_HANDLE_VALUE;
    };
}