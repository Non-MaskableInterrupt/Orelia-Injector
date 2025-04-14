#pragma once

namespace vmt {
	class c_vmt {
	public:
		c_vmt( ) = default;

		bool setup( void* address, int index ) {
			if ( !address )
				return false;

			m_index = index;
			m_address = reinterpret_cast< std::uintptr_t >( address );
			m_original_vtable = *reinterpret_cast< std::uintptr_t** >( address );
			m_size_vtable = 0;

			while ( m_original_vtable[ m_size_vtable ] )
				m_size_vtable++;

			return true;
		}

		template <typename fn_t>
		bool create_hook( void* function, fn_t* original ) {
			if ( !m_address || !function || !original )
				return false;

			if ( m_index >= m_size_vtable )
				return false;

			*original = reinterpret_cast< fn_t >( m_original_vtable[ m_index ] );
			if ( !*original )
				return false;

			m_new_vtable = reinterpret_cast< std::uintptr_t* > (
				malloc( m_size_vtable * sizeof( std::uintptr_t ) )
				);
			if ( !m_new_vtable )
				return false;
						
			crt::memcpy( m_new_vtable, m_original_vtable, m_size_vtable * sizeof( std::uintptr_t ) );
			m_new_vtable[ m_index ] = reinterpret_cast< std::uintptr_t >( function );

			*reinterpret_cast< std::uintptr_t** >( m_address ) = m_new_vtable;

			auto current_vtable = *reinterpret_cast< std::uintptr_t** >( m_address );
			return current_vtable[ m_index ] == reinterpret_cast< std::uintptr_t >( function );
		}

		void* get_original( int index ) {
			return reinterpret_cast< void* >( m_original_vtable[ index ] );
		}

		bool unhook( ) {
			if ( !m_original_vtable || !m_new_vtable || !m_address )
				return false;

			*( std::uintptr_t** )( m_address ) = m_original_vtable;

			free( m_new_vtable );
			m_new_vtable = nullptr;
			m_original_vtable = nullptr;
			m_address = 0;
			m_index = 0;
			m_size_vtable = 0;

			return true;
		}

		bool is_hooked( ) const {
			return m_original_vtable != nullptr;
		}

		void* get_function( int index ) {
			if ( index >= m_size_vtable )
				return nullptr;
			return reinterpret_cast< void* >(
				m_new_vtable ? m_new_vtable[ index ] : m_original_vtable[ index ]
				);
		}

		bool is_hooked_function( int index, void* function ) {
			if ( index >= m_size_vtable || !function || !m_original_vtable )
				return false;
			return m_new_vtable[ index ] == reinterpret_cast< std::uintptr_t >( function );
		}

	private:
		std::uintptr_t m_address = 0;
		int m_index = 0;
		std::uintptr_t* m_original_vtable = nullptr;
		std::uintptr_t* m_new_vtable = nullptr;
		int m_size_vtable = 0;
	};
}