namespace sdk {
    template<typename T>
    class vector {
    private:
        T* m_data = nullptr;
        size_t m_size = 0;
        size_t m_capacity = 0;

    public:
        // Iterator support
        using iterator = T*;
        using const_iterator = const T*;

        vector( ) = default;
        ~vector( ) { if ( m_data ) free( m_data ); }

        // Copy constructor
        vector( const vector& other ) {
            if ( other.m_size > 0 ) {
                m_data = ( T* )malloc( other.m_capacity * sizeof( T ) );
                if ( m_data ) {
                    m_capacity = other.m_capacity;
                    m_size = other.m_size;
                    for ( size_t i = 0; i < m_size; i++ ) {
                        new ( &m_data[ i ] ) T( other.m_data[ i ] );
                    }
                }
            }
        }

        // Move constructor
        vector( vector&& other ) noexcept {
            m_data = other.m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }

        // Assignment operator
        vector& operator=( const vector& other ) {
            if ( this != &other ) {
                if ( m_data ) {
                    for ( size_t i = 0; i < m_size; i++ ) {
                        m_data[ i ].~T( );
                    }
                    free( m_data );
                }

                if ( other.m_size > 0 ) {
                    m_data = ( T* )malloc( other.m_capacity * sizeof( T ) );
                    if ( m_data ) {
                        m_capacity = other.m_capacity;
                        m_size = other.m_size;
                        for ( size_t i = 0; i < m_size; i++ ) {
                            new ( &m_data[ i ] ) T( other.m_data[ i ] );
                        }
                    }
                }
                else {
                    m_data = nullptr;
                    m_size = 0;
                    m_capacity = 0;
                }
            }
            return *this;
        }

        // Move assignment operator
        vector& operator=( vector&& other ) noexcept {
            if ( this != &other ) {
                if ( m_data ) {
                    for ( size_t i = 0; i < m_size; i++ ) {
                        m_data[ i ].~T( );
                    }
                    free( m_data );
                }

                m_data = other.m_data;
                m_size = other.m_size;
                m_capacity = other.m_capacity;
                other.m_data = nullptr;
                other.m_size = 0;
                other.m_capacity = 0;
            }
            return *this;
        }

        void push_back( const T& item ) {
            if ( m_size >= m_capacity ) {
                size_t new_capacity = m_capacity ? m_capacity * 2 : 16;
                T* new_data = ( T* )malloc( new_capacity * sizeof( T ) );
                if ( m_data ) {
                    memcpy( new_data, m_data, m_size * sizeof( T ) );
                    free( m_data );
                }
                m_data = new_data;
                m_capacity = new_capacity;
            }
            new ( &m_data[ m_size ] ) T( item );
            m_size++;
        }

        // Add emplace_back for constructing elements in-place
        template<typename... Args>
        T& emplace_back( Args&&... args ) {
            if ( m_size >= m_capacity ) {
                size_t new_capacity = m_capacity ? m_capacity * 2 : 16;
                T* new_data = ( T* )malloc( new_capacity * sizeof( T ) );
                if ( m_data ) {
                    for ( size_t i = 0; i < m_size; i++ ) {
                        new ( &new_data[ i ] ) T( std::move( m_data[ i ] ) );
                        m_data[ i ].~T( );
                    }
                    free( m_data );
                }
                m_data = new_data;
                m_capacity = new_capacity;
            }
            T* p = new ( &m_data[ m_size ] ) T( args... );
            m_size++;
            return *p;
        }

        // Reserve capacity
        void reserve( size_t new_capacity ) {
            if ( new_capacity > m_capacity ) {
                T* new_data = ( T* )malloc( new_capacity * sizeof( T ) );
                if ( m_data ) {
                    for ( size_t i = 0; i < m_size; i++ ) {
                        new ( &new_data[ i ] ) T( std::move( m_data[ i ] ) );
                        m_data[ i ].~T( );
                    }
                    free( m_data );
                }
                m_data = new_data;
                m_capacity = new_capacity;
            }
        }

        void resize( size_t new_size ) {
            if ( new_size > m_capacity ) {
                reserve( new_size );
            }

            if ( new_size > m_size ) {
                for ( size_t i = m_size; i < new_size; i++ ) {
                    new ( &m_data[ i ] ) T( );
                }
            }
            else if ( new_size < m_size ) {
                for ( size_t i = new_size; i < m_size; i++ ) {
                    m_data[ i ].~T( );
                }
            }

            m_size = new_size;
        }

        void clear( ) {
            for ( size_t i = 0; i < m_size; i++ ) {
                m_data[ i ].~T( );
            }
            m_size = 0;
        }

        T& operator[]( size_t index ) { return m_data[ index ]; }
        const T& operator[]( size_t index ) const { return m_data[ index ]; }
        size_t size( ) const { return m_size; }
        bool empty( ) const { return m_size == 0; }
        size_t capacity( ) const { return m_capacity; }

        iterator begin( ) { return m_data; }
        const_iterator begin( ) const { return m_data; }
        iterator end( ) { return m_data + m_size; }
        const_iterator end( ) const { return m_data + m_size; }
    };
}