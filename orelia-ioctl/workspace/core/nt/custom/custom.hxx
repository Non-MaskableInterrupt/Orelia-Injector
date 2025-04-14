
template <typename type, size_t max_size = 1000>
class c_vector {
private:
    type m_data[ max_size ];
    size_t m_size;
    kspin_lock_t m_lock;

public:
    c_vector( ) : m_size( 0 ) {
        nt::ke_initialize_spin_lock( &m_lock );
    }

    c_vector( const c_vector& ) = delete;
    c_vector& operator=( const c_vector& ) = delete;

    bool push_back_batch( const type* values, size_t count ) {
        if ( !values || count == 0 )
            return false;

        kirql_t old_irql;
        nt::ke_acquire_spin_lock( &m_lock, &old_irql );

        if ( m_size + count > max_size ) {
            nt::ke_release_spin_lock( &m_lock, old_irql );
            return false;
        }

        nt::rtl_copy_memory( &m_data[ m_size ], values, count * sizeof( type ) );
        m_size += count;
        nt::ke_release_spin_lock( &m_lock, old_irql );
        return true;
    }

    bool push_back( const type& value ) {
        kirql_t old_irql;
        nt::ke_acquire_spin_lock( &m_lock, &old_irql );

        if ( m_size >= max_size ) {
            nt::ke_release_spin_lock( &m_lock, old_irql );
            return false;
        }

        m_data[ m_size++ ] = value;
        nt::ke_release_spin_lock( &m_lock, old_irql );
        return true;
    }

    bool pop_back( ) {
        kirql_t old_irql;
        nt::ke_acquire_spin_lock( &m_lock, &old_irql );

        if ( m_size == 0 ) {
            nt::ke_release_spin_lock( &m_lock, old_irql );
            return false;
        }

        --m_size;

        nt::ke_release_spin_lock( &m_lock, old_irql );
        return true;
    }

    void clear( ) {
        kirql_t old_irql;
        nt::ke_acquire_spin_lock( &m_lock, &old_irql );

        nt::rtl_zero_memory( m_data, sizeof( type ) * m_size );
        m_size = 0;

        nt::ke_release_spin_lock( &m_lock, old_irql );
    }

    bool erase( size_t index ) {
        kirql_t old_irql;
        nt::ke_acquire_spin_lock( &m_lock, &old_irql );

        if ( index >= m_size ) {
            nt::ke_release_spin_lock( &m_lock, old_irql );
            return false;
        }

        if ( index < m_size - 1 ) {
            nt::rtl_move_memory(
                &m_data[ index ],
                &m_data[ index + 1 ],
                ( m_size - index - 1 ) * sizeof( type )
            );
        }

        --m_size;

        nt::ke_release_spin_lock( &m_lock, old_irql );
        return true;
    }

    bool at( size_t index, type& out_value ) const {
        kirql_t old_irql;
        nt::ke_acquire_spin_lock( &m_lock, &old_irql );

        if ( index >= m_size ) {
            nt::ke_release_spin_lock( &m_lock, old_irql );
            return false;
        }

        out_value = m_data[ index ];

        nt::ke_release_spin_lock( &m_lock, old_irql );
        return true;
    }

    type operator[]( size_t index ) const {
        if ( index >= m_size )
            return type{};
        return m_data[ index ];
    }

    type* data( ) { return m_data; }
    const type* data( ) const { return m_data; }

    const type* begin( ) const { return m_data; }
    const type* end( ) const { return m_data + m_size; }

    bool reserve( size_t new_size ) {
        return new_size <= max_size;
    }

    size_t size( ) const { return m_size; }
    bool empty( ) const { return m_size == 0; }
    bool full( ) const { return m_size >= max_size; }
    size_t capacity( ) const { return max_size; }
};