
namespace mm {
    namespace phys {
        void* map_io_region( std::uint64_t address, std::size_t size ) {
            physical_address_t phys_addr{};
            phys_addr.m_quad_part = address;

            return nt::map_io_space_ex( phys_addr, size, 4 );
        }

        nt_status_t read_direct( std::uint64_t pa, void* buffer, std::size_t size ) {
            mm_copy_address_t src { };
            src.m_physical_address.m_quad_part = pa;

            std::size_t number_of_bytes = 0;
            return nt::mm_copy_memory( buffer, src, size, copy_physical, &number_of_bytes );
        }

        nt_status_t write_direct( std::uint64_t pa, void* buffer, std::size_t size ) {
            mm_copy_address_t dst { };
            dst.m_physical_address.m_quad_part = pa;

            std::size_t number_of_bytes = 0;
            return nt::mm_copy_memory( buffer, dst, size, copy_physical, &number_of_bytes );
        }

        nt_status_t copy_verified( std::uint64_t pa, void* user_buffer, std::size_t size ) {
            auto staging = nt::ex_allocate_pool( size );
            if ( !staging ) return nt_status_t::insufficient_resources;

            auto status = read_direct( pa, staging, size );
            if ( status == nt_status_t::success ) {
                __try {
                    nt::probe_for_write( user_buffer, size, 1 );
                    crt::memcpy( user_buffer, staging, size );
                }
                __except ( 1 ) {
                    status = nt_status_t::access_violation;
                }
            }

            nt::ex_free_pool( staging );
            return status;
        }

        bool write_region( std::uint64_t pa, void* data, std::size_t size ) {
            auto mapping = map_io_region( pa, size );
            if ( !mapping )
                return false;

            __try {
                crt::memcpy( mapping, data, size );
            }
            __finally {
                nt::mm_unmap_io_space( mapping, size );
            }
            return true;
        }
    }
}