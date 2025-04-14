namespace mm {
	namespace paging {
		constexpr auto page_4kb_size = 0x1000ull;
		constexpr auto page_2mb_size = 0x200000ull;
		constexpr auto page_1gb_size = 0x40000000ull;

		constexpr auto page_shift = 12ull;
		constexpr auto page_2mb_shift = 21ull;
		constexpr auto page_1gb_shift = 30ull;

		constexpr auto page_4kb_mask = 0xFFFull;
		constexpr auto page_2mb_mask = 0x1FFFFFull;
		constexpr auto page_1gb_mask = 0x3FFFFFFFull;

		enum class page_table_level {
			pml4,
			pdpt,
			pd,
			pt
		};

		class c_paging {
			cr3 m_dtb;
			mmpfn_t* m_pfn_database;

			c_vector< pml4e, 512 > m_pml4_table;
			c_vector< pdpte, 512 > m_pdpt_table;
			c_vector< pde, 512 > m_pd_table;
			c_vector< pte, 512 > m_pt_table;

			std::uint16_t m_cached_pml4e;
			std::uint16_t m_cached_pdpte;
			std::uint16_t m_cached_pde;

		public:
			void cleanup( ) {
				m_pml4_table.clear( );
				m_pdpt_table.clear( );
				m_pd_table.clear( );
				m_pt_table.clear( );

				m_cached_pml4e = 0;
				m_cached_pdpte = 0;
				m_cached_pde = 0;
				m_dtb.flags = 0;
			}

			bool setup( ) {
				if ( !m_pml4_table.empty( ) )
					return false;

				this->m_pfn_database = nt::get_mm_pfn_database( );
				if ( !m_pfn_database )
					return false;

				return true;
			}

			bool scan_pages( std::uint64_t base_address ) {
				if ( !base_address )
					return false;

				const auto ranges = nt::mm_get_physical_memory_ranges( );
				for ( auto i = 0; ; i++ ) {
					auto memory_range = ranges[ i ];
					if ( !memory_range.m_base_page.m_quad_part ||
						!memory_range.m_page_count.m_quad_part )
						break;

					std::uint64_t current_pa = memory_range.m_base_page.m_quad_part;
					for ( auto current_page = 0;
						current_page < memory_range.m_page_count.m_quad_part;
						current_page += 0x1000, current_pa += 0x1000 ) {
						cr3 current_dtb{ .pfn = current_pa >> page_shift };
						if ( !current_dtb.flags )
							continue;

						auto pfn_entry = &m_pfn_database[ current_dtb.pfn ];
						if ( !pfn_entry ) continue;
						if ( pfn_entry->m_u4.m_pte_frame != current_dtb.pfn ) continue;

						if ( this->cache_pt( current_pa, base_address ) ) {
							this->m_dtb = current_dtb;
							return true;
						}
					}
				}

				return false;
			}

			bool cache_pt( std::uint64_t physical_address, std::uint64_t base_address ) {
				auto va = virt_addr_t{ base_address };

				pml4e pml4_table[ 512 ]{};
				if ( phys::read_direct( physical_address, pml4_table, 4096Ui64 ) != nt_status_t::success )
					return false;

				auto pml4_entry = pml4_table[ va.pml4e_index ];
				if ( !pml4_entry.hard.present )
					return false;

				pdpte pdpt_entry{};
				if ( phys::read_direct( ( pml4_entry.hard.pfn << page_shift ) + ( va.pdpte_index * sizeof( pdpte ) ),
					&pdpt_entry, sizeof( pdpte ) ) != nt_status_t::success )
					return false;

				if ( !pdpt_entry.hard.present )
					return false;

				pde pd_entry{};
				if ( phys::read_direct( ( pdpt_entry.hard.pfn << page_shift ) + ( va.pde_index * sizeof( pde ) ),
					&pd_entry, sizeof( pde ) ) != nt_status_t::success )
					return false;

				if ( !pd_entry.hard.present )
					return false;

				pte pt_entry{};
				if ( phys::read_direct( ( pd_entry.hard.pfn << page_shift ) + ( va.pte_index * sizeof( pte ) ),
					&pt_entry, sizeof( pte ) ) != nt_status_t::success )
					return false;

				if ( !pt_entry.hard.present )
					return false;

				m_pml4_table.clear( );
				if ( !m_pml4_table.push_back_batch( pml4_table, 512 ) )
					return false;

				m_cached_pml4e = va.pml4e_index;
				m_cached_pdpte = va.pdpte_index;
				m_cached_pde = va.pde_index;
				return true;
			}

			std::uint64_t translate( std::uint64_t virtual_address, std::uint32_t* out_page_size = nullptr ) {
				if ( !m_dtb.flags ) {
					nt::dbg_print( "[orelia] invalid dtb\n" );
					return 0;
				}

				auto va = virt_addr_t{ virtual_address };
				if ( va.pml4e_index >= m_pml4_table.size( ) )
					return 0;

				auto pml4_entry = this->m_pml4_table[ va.pml4e_index ];
				if ( !pml4_entry.hard.present )
					return 0;

				pdpte pdpt_entry{};
				if ( phys::read_direct( ( pml4_entry.hard.pfn << page_shift ) + ( va.pdpte_index * sizeof( pdpte ) ),
					&pdpt_entry, sizeof( pdpte ) ) != nt_status_t::success )
					return 0;

				if ( !pdpt_entry.hard.present )
					return 0;

				if ( pdpt_entry.hard.page_size ) {
					auto final_pa = ( pdpt_entry.hard.pfn << page_shift ) + ( virtual_address & page_1gb_mask );
					if ( out_page_size )
						*out_page_size = page_1gb_size;
					nt::dbg_print( "[orelia] translated via 1GB page: VA=0x%llx -> PA=0x%llx\n", virtual_address, final_pa );
					return final_pa;
				}

				pde pd_entry{};
				if ( phys::read_direct( ( pdpt_entry.hard.pfn << page_shift ) + ( va.pde_index * sizeof( pde ) ),
					&pd_entry, sizeof( pde ) ) != nt_status_t::success )
					return 0;

				if ( !pd_entry.hard.present )
					return 0;

				if ( pd_entry.hard.page_size ) {
					auto final_pa = ( pd_entry.hard.pfn << page_shift ) + ( virtual_address & page_2mb_mask );
					if ( out_page_size )
						*out_page_size = page_2mb_size;
					nt::dbg_print( "[orelia] translated via 2MB page: VA=0x%llx -> PA=0x%llx\n", virtual_address, final_pa );
					return final_pa;
				}

				pte pt_entry{};
				if ( phys::read_direct( ( pd_entry.hard.pfn << page_shift ) + ( va.pte_index * sizeof( pte ) ),
					&pt_entry, sizeof( pte ) ) != nt_status_t::success )
					return 0;

				if ( !pt_entry.hard.present )
					return 0;

				auto final_pa = ( pt_entry.hard.pfn << page_shift ) + ( virtual_address & page_4kb_mask );
				if ( out_page_size )
					*out_page_size = page_4kb_size;
				return final_pa;
			}

			std::uint64_t map_kernel_pte( std::uint64_t physical_address ) {
				if ( !physical_address || !is_valid_pa( physical_address, page_4kb_size ) ) {
					nt::dbg_print( "[orelia] Invalid physical address 0x%llx\n", physical_address );
					return 0;
				}

				int free_pte_index = -1;
				for ( int i = 0; i < m_pt_table.size( ); ++i ) {
					if ( !m_pt_table[ i ].hard.present ) {
						free_pte_index = i;
						break;
					}
				}
				if ( free_pte_index == -1 ) {
					nt::dbg_print( "[orelia] No free PTE available.\n" );
					return 0;
				}

				auto pd_entry = m_pd_table[ m_cached_pde ];
				if ( pd_entry.hard.page_size || !pd_entry.hard.present ) {
					nt::dbg_print( "[orelia] PDE invalid or is a large page.\n" );
					return 0;
				}

				std::uint64_t user_va =
					( static_cast< std::uint64_t >( m_cached_pml4e ) << 39 ) |
					( static_cast< std::uint64_t >( m_cached_pdpte ) << 30 ) |
					( static_cast< std::uint64_t >( m_cached_pde ) << 21 ) |
					( static_cast< std::uint64_t >( free_pte_index ) << 12 );

				pte existing_pte;
				std::uint64_t pt_phys = pd_entry.hard.pfn << page_shift;
				std::uint64_t pte_phys = pt_phys + ( free_pte_index * sizeof( pte ) );
				if ( phys::read_direct( pte_phys, &existing_pte, sizeof( pte ) ) != nt_status_t::success ) {
					nt::dbg_print( "[orelia] Failed to read physical PTE.\n" );
					return 0;
				}

				if ( existing_pte.hard.present ) {
					nt::dbg_print( "[orelia] VA 0x%llx is already mapped.\n", user_va );
					return 0;
				}

				pte new_pte{};
				new_pte.hard.pfn = physical_address >> page_shift;
				new_pte.hard.present = 1;
				new_pte.hard.read_write = 1;
				new_pte.hard.user_supervisor = 1;
				new_pte.hard.no_execute = 0;

				auto result = phys::write_direct( pte_phys, &new_pte, sizeof( pte ) );
				if ( result != nt_status_t::success ) {
					nt::dbg_print( "[orelia] Failed to write PTE to physical memory with %x.\n", result );
					return 0;
				}

				m_pt_table[ free_pte_index ] = new_pte;
				__invlpg( reinterpret_cast< void* >( user_va ) );

				return user_va;
			}

			bool map_process_pte( std::uint64_t virtual_address ) {
				if ( !virtual_address ) {
					nt::dbg_print( "[orelia] Invalid virtual address 0x%llx\n", virtual_address );
					return 0;
				}

				auto va = virt_addr_t{ virtual_address };
				if ( va.pml4e_index >= m_pml4_table.size( ) )
					return false;

				auto pml4_entry = this->m_pml4_table[ va.pml4e_index ];
				if ( !pml4_entry.hard.present )
					return false;

				pdpte pdpt_entry{};
				if ( phys::read_direct( ( pml4_entry.hard.pfn << page_shift ) + ( va.pdpte_index * sizeof( pdpte ) ),
					&pdpt_entry, sizeof( pdpte ) ) != nt_status_t::success )
					return false;

				if ( !pdpt_entry.hard.present )
					return false;

				pde pd_entry{};
				if ( phys::read_direct( ( pdpt_entry.hard.pfn << page_shift ) + ( va.pde_index * sizeof( pde ) ),
					&pd_entry, sizeof( pde ) ) != nt_status_t::success )
					return false;

				if ( !pd_entry.hard.present )
					return false;

				if ( pd_entry.hard.page_size )
					return false;

				pte existing_pte;
				auto pt_phys = pd_entry.hard.pfn << page_shift;
				auto pte_phys = pt_phys + ( va.pte_index * sizeof( pte ) );
				if ( phys::read_direct( pte_phys, &existing_pte, sizeof( pte ) ) != nt_status_t::success )
					return false;

				if ( !existing_pte.hard.present )
					return false;

				pte new_pte{};
				new_pte.hard.pfn = existing_pte.hard.pfn;
				new_pte.hard.present = 1;
				new_pte.hard.read_write = 1;
				new_pte.hard.user_supervisor = 1;
				new_pte.hard.no_execute = 0;

				auto result = phys::write_direct( pte_phys, &new_pte, sizeof( pte ) );
				if ( result != nt_status_t::success )
					return false;

				__invlpg( reinterpret_cast< void* >( virtual_address ) );
				return true;
			}

			bool update_pml4e( std::uint64_t virt, std::uint64_t new_pdpt_pfn ) {
				auto va = virt_addr_t{ virt };
				auto pml4e_phys = ( this->m_dtb.flags & ~page_4kb_mask ) + ( va.pml4e_index * sizeof( pml4e ) );

				auto new_entry = this->m_pml4_table[ va.pml4e_index ];
				new_entry.hard.pfn = new_pdpt_pfn;

				auto result = mm::phys::write_direct( pml4e_phys, &new_entry, sizeof( pml4e ) );
				if ( result != nt_status_t::success )
					return false;

				this->m_pml4_table[ va.pml4e_index ] = new_entry;

				__invlpg( reinterpret_cast< void* >( virt ) );
				return true;
			}

			std::uint64_t create_page_table( ) {
				auto kernel_va = reinterpret_cast< std::uintptr_t >(
					nt::mm_allocate_non_cached_memory( page_4kb_size )
					);
				if ( !kernel_va )
					return 0;

				auto kernel_pa = nt::get_physical_address( kernel_va );
				if ( !kernel_pa ) {
					nt::mm_free_non_cached_memory( reinterpret_cast< void* >( kernel_va ), page_4kb_size );
					return 0;
				}

				auto user_va = map_kernel_pte( kernel_pa );
				if ( !user_va ) {
					nt::mm_free_non_cached_memory( reinterpret_cast< void* >( kernel_va ), page_4kb_size );
					return 0;
				}

				return user_va;
			}

			std::uint64_t copy_physical_page( std::uint64_t phys_addr ) {
				auto kernel_va = reinterpret_cast< std::uintptr_t >(
					nt::mm_allocate_non_cached_memory( page_4kb_size )
					);
				if ( !kernel_va )
					return 0;

				auto new_phys = nt::get_physical_address( kernel_va );
				if ( !new_phys ) {
					nt::mm_free_non_cached_memory( reinterpret_cast< void* >( kernel_va ), page_4kb_size );
					return 0;
				}

				auto status = mm::phys::read_direct( phys_addr, reinterpret_cast< void* >( kernel_va ), page_4kb_size );
				if ( status != nt_status_t::success ) {
					nt::mm_free_non_cached_memory( reinterpret_cast< void* >( kernel_va ), page_4kb_size );
					return 0;
				}

				auto user_va = map_kernel_pte( new_phys );
				if ( !user_va ) {
					nt::mm_free_non_cached_memory( reinterpret_cast< void* >( kernel_va ), page_4kb_size );
					return 0;
				}

				return user_va;
			}

			bool is_pt_entry_valid( std::uint64_t pfn, int index, page_table_level level ) {
				switch ( level ) {
				case page_table_level::pml4: {
					pml4e entry{};
					if ( phys::read_direct( ( pfn << page_shift ) + ( index * sizeof( pml4e ) ),
						&entry, sizeof( pml4e ) ) != nt_status_t::success )
						return false;
					return entry.hard.present;
				}
				case page_table_level::pdpt: {
					pdpte entry{};
					if ( phys::read_direct( ( pfn << page_shift ) + ( index * sizeof( pdpte ) ),
						&entry, sizeof( pdpte ) ) != nt_status_t::success )
						return false;
					return entry.hard.present;
				}
				case page_table_level::pd: {
					pde entry{};
					if ( phys::read_direct( ( pfn << page_shift ) + ( index * sizeof( pde ) ),
						&entry, sizeof( pde ) ) != nt_status_t::success )
						return false;
					return entry.hard.present;
				}
				case page_table_level::pt: {
					pte entry{};
					if ( phys::read_direct( ( pfn << page_shift ) + ( index * sizeof( pte ) ),
						&entry, sizeof( pte ) ) != nt_status_t::success )
						return false;
					return entry.hard.present;
				}
				default:
					return false;
				}
			}

			bool is_valid_pml4( std::uint64_t pfn ) {
				pml4e pml4_table[ 512 ]{};
				if ( phys::read_direct( pfn << page_shift, pml4_table, 4096Ui64 ) != nt_status_t::success )
					return false;

				int present_count = 0;
				bool has_kernel_mappings = false;

				for ( int i = 0; i < 512; i++ ) {
					if ( pml4_table[ i ].hard.present ) {
						present_count++;
						if ( i >= 256 )
							has_kernel_mappings = true;
					}
				}

				return present_count > 0 && has_kernel_mappings;
			}

			bool can_map_user_and_kernel_addresses( std::uint64_t pfn ) {
				pml4e pml4_table[ 512 ]{};
				if ( phys::read_direct( pfn << page_shift, pml4_table, 4096Ui64 ) != nt_status_t::success )
					return false;

				bool has_user_mappings = false;
				for ( int i = 0; i < 256; i++ ) {
					if ( pml4_table[ i ].hard.present ) {
						has_user_mappings = true;
						break;
					}
				}

				bool has_kernel_mappings = false;
				for ( int i = 256; i < 512; i++ ) {
					if ( pml4_table[ i ].hard.present ) {
						has_kernel_mappings = true;
						break;
					}
				}

				return has_user_mappings && has_kernel_mappings;
			}

			bool is_valid_pa( std::uint64_t physical_address, std::size_t size ) const {
				//auto lowest_pa = m_lowest_physical_page << page_shift;
				//auto highest_pa = ( m_highest_physical_page << page_shift ) + page_4kb_size;

				//return ( physical_address >= lowest_pa &&
				//	( physical_address + size ) <= highest_pa );
				return true;
			}

			bool copy_page_table( std::uint64_t dst_va, std::uint64_t src_va ) {
				std::size_t bytes_transferred = 0;
				auto result = nt::mm_copy_memory(
					reinterpret_cast< void* >( dst_va ),
					{ .m_virtual_address = src_va },
					page_4kb_size,
					copy_virtual,
					&bytes_transferred
				);
				return result == nt_status_t::success && bytes_transferred == page_4kb_size;
			}

			bool split_large_page( std::uint64_t pde_va, std::uint64_t pt_va ) {
				auto va = virt_addr_t{ pde_va };
				auto pd_entry = this->m_pd_table[ va.pde_index ];

				if ( !pd_entry.hard.present || !pd_entry.hard.page_size )
					return false;

				auto start_pfn = pd_entry.hard.pfn;
				pte* pt_entries = reinterpret_cast< pte* >( pt_va );

				for ( int idx = 0; idx < 512; idx++ ) {
					pt_entries[ idx ].value = pd_entry.value;
					pt_entries[ idx ].hard.global = 0;
					pt_entries[ idx ].hard.pfn = start_pfn + idx;
				}

				return true;
			}

			std::uint64_t get_pml4e_pfn( std::uint64_t virt ) {
				auto va = virt_addr_t{ virt };
				auto pml4_entry = this->m_pml4_table[ va.pml4e_index ];
				if ( !pml4_entry.hard.present )
					return 0;
				return pml4_entry.hard.pfn;
			}

			std::uint64_t get_pdpte_pfn( std::uint64_t virt ) {
				auto va = virt_addr_t{ virt };
				auto pdpt_entry = this->m_pdpt_table[ va.pdpte_index ];
				if ( !pdpt_entry.hard.present )
					return 0;
				return pdpt_entry.hard.pfn;
			}

			std::uint64_t get_pde_pfn( std::uint64_t virt ) {
				auto va = virt_addr_t{ virt };
				auto pd_entry = this->m_pd_table[ va.pde_index ];
				if ( !pd_entry.hard.present )
					return 0;
				return pd_entry.hard.pfn;
			}

			std::uint64_t get_pte_pfn( std::uint64_t virt ) {
				auto va = virt_addr_t{ virt };
				auto pt_entry = this->m_pt_table[ va.pte_index ];
				if ( !pt_entry.hard.present )
					return 0;
				return pt_entry.hard.pfn;
			}

			bool is_large_page( std::uint64_t virt ) {
				auto va = virt_addr_t{ virt };
				auto pd_entry = this->m_pd_table[ va.pde_index ];
				return pd_entry.hard.present && pd_entry.hard.page_size;
			}

			bool is_huge_page( std::uint64_t virt ) {
				auto va = virt_addr_t{ virt };
				auto pdpt_entry = this->m_pdpt_table[ va.pdpte_index ];
				return pdpt_entry.hard.present && pdpt_entry.hard.page_size;
			}

			std::uint64_t page_align( std::uint64_t virt ) {
				return virt & ~page_4kb_mask;
			}

			std::uint64_t page_offset( std::uint64_t virt ) {
				return virt & page_4kb_mask;
			}

			std::uintptr_t get_dtb( ) const {
				return this->m_dtb.flags;
			}
		};
	}
}