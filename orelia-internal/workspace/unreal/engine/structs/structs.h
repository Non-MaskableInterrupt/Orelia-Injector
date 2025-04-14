namespace sdk {
    template<class type>
    struct tarray {
        tarray( ) {
            m_data = nullptr;
            m_num_elements = m_max_elements = 0;
        }

        type* m_data;
        int m_num_elements, m_max_elements;

        bool add( type element ) {
            if ( get_slack( ) <= 0 )
                return false;

            m_data[ m_num_elements ] = element;
            m_num_elements++;

            return true;
        }

        sdk::vector<type> get_itter( ) {
            if ( this->m_num_elements > this->m_max_elements )
                return {};

            sdk::vector<type> buffer( this->m_num_elements );
            memcpy(
                buffer.data( ),
                this->m_data,
                sizeof( type ) * this->m_num_elements
            );

            return buffer;
        }

        int32_t get_slack( ) const {
            return m_max_elements - m_num_elements;
        }

        type& operator[]( int i ) {
            return m_data[ i ];
        }

        int size( ) {
            return m_num_elements;
        }

        bool valid( int i ) {
            return i < 0 && i < m_num_elements;
        }
    };

    template<class type>
    struct t_set {
        struct t_element {
            type m_value;
            int32_t m_hash_next_id; // Links to the next element in the same bucket
            int32_t m_hash_index; // Index in the hash table
        };

        tarray<t_element> m_elements; // Stores the actual elements
        tarray<int32_t> m_hash;       // Hash table (indices into `Elements`)
        int32_t m_num_elements;        // Number of valid elements
        int32_t m_free_list_head;       // Index of the first free slot in `Elements`
    };

    struct fstring : private tarray<wchar_t> {
        fstring( ) { }
        fstring( const wchar_t* other ) {
            m_max_elements = m_num_elements = *other ?
                static_cast< int >( std::wcslen( other ) ) + 1 : 0;

            if ( m_num_elements )
                m_data = const_cast< wchar_t* >( other );
        }

        template <size_t N, char K1, char K2, typename T>
        fstring( skc::skCrypter<N, K1, K2, T> crypter ) {
            const wchar_t* decrypted = crypter.decrypt( );

            m_max_elements = m_num_elements = ( decrypted && *decrypted ) ?
                static_cast< int >( std::wcslen( decrypted ) ) + 1 : 0;

            if ( m_num_elements )
                m_data = const_cast< wchar_t* >( decrypted );
        }

        operator bool( ) { return bool( m_data ); }
        wchar_t* c_str( ) { return m_data; }
        int size( ) { return m_num_elements; }
    };

    struct fvector;
    struct frotator {
        frotator( ) : m_pitch( ), m_yaw( ), m_roll( ) { }
        frotator( double pitch, double yaw, double roll ) :
            m_pitch( pitch ), m_yaw( yaw ), m_roll( roll ) {
        }

        frotator operator+( const frotator& other ) const {
            return { m_pitch + other.m_pitch, m_yaw + other.m_yaw, m_roll + other.m_roll };
        }

        frotator operator-( const frotator& other ) const {
            return { m_pitch - other.m_pitch, m_yaw - other.m_yaw, m_roll - other.m_roll };
        }

        frotator operator*( double offset ) const {
            return { m_pitch * offset, m_yaw * offset, m_roll * offset };
        }

        frotator operator/( double offset ) const {
            return { m_pitch / offset, m_yaw / offset, m_roll / offset };
        }

        frotator& operator*=( const double other ) {
            m_pitch *= other;
            m_yaw *= other;
            m_roll *= other;
            return *this;
        }

        frotator& operator/=( const double other ) {
            m_pitch /= other;
            m_yaw /= other;
            m_roll /= other;
            return *this;
        }

        frotator& operator=( const frotator& other ) {
            m_pitch = other.m_pitch;
            m_yaw = other.m_yaw;
            m_roll = other.m_roll;
            return *this;
        }

        frotator& operator+=( const frotator& other ) {
            m_pitch += other.m_pitch;
            m_yaw += other.m_yaw;
            m_roll += other.m_roll;
            return *this;
        }

        frotator& operator-=( const frotator& other ) {
            m_pitch -= other.m_pitch;
            m_yaw -= other.m_yaw;
            m_roll -= other.m_roll;
            return *this;
        }

        operator bool( ) {
            return bool( m_pitch || m_yaw || m_roll );
        }

        friend bool operator==( const frotator& a, const frotator& b ) {
            return a.m_pitch == b.m_pitch && a.m_yaw == b.m_yaw && a.m_roll == b.m_roll;
        }

        friend bool operator!=( const frotator& a, const frotator& b ) {
            return !( a == b );
        }

        frotator normalize( ) {
            while ( m_yaw > 180.0 )
                m_yaw -= 360.0;
            while ( m_yaw < -180.0 )
                m_yaw += 360.0;

            while ( m_pitch > 180.0 )
                m_pitch -= 360.0;
            while ( m_pitch < -180.0 )
                m_pitch += 360.0;

            m_roll = 0.0;
            return *this;
        }

        fvector get_forward_vector( );

        double m_pitch, m_yaw, m_roll;
    };

    struct fvector {
        fvector( ) : m_x( ), m_y( ), m_z( ) { }
        fvector( double m_x, double m_y, double z ) : m_x( m_x ), m_y( m_y ), m_z( z ) { }

        bool operator==( const fvector& other ) const {
            return m_x == other.m_x &&
                m_y == other.m_y &&
                m_z == other.m_z;
        }

        fvector operator+( const fvector& other ) const {
            return { m_x + other.m_x, m_y + other.m_y, m_z + other.m_z };
        }

        fvector operator-( const fvector& other ) const {
            return { m_x - other.m_x, m_y - other.m_y, m_z - other.m_z };
        }

        fvector operator*( double offset ) const {
            return { m_x * offset, m_y * offset, m_z * offset };
        }

        fvector operator/( double offset ) const {
            return { m_x / offset, m_y / offset, m_z / offset };
        }

        fvector& operator*=( const double other ) {
            m_x *= other; m_y *= other; m_z *= other;
            return *this;
        }

        fvector& operator/=( const double other ) {
            m_x /= other; m_y /= other; m_z /= other;
            return *this;
        }

        fvector& operator=( const fvector& other ) {
            m_x = other.m_x; m_y = other.m_y; m_z = other.m_z;
            return *this;
        }

        fvector normalize( ) const {
            fvector result = *this;
            float length = sqrt(
                result.m_x * result.m_x +
                result.m_y * result.m_y +
                result.m_z * result.m_z
            );

            if ( length > 0.0f ) {
                float inv_length = 1.0f / length;
                result.m_x *= inv_length;
                result.m_y *= inv_length;
                result.m_z *= inv_length;
            }
            else {
                result.m_x = result.m_y = result.m_z = 0.0f;
            }

            return result;
        }

        operator bool( ) {
            return bool( this->m_x && this->m_y && this->m_z );
        }

        frotator to_rotator( );
        double distance_to( const fvector& other );
        double dot( const fvector& other ) { return m_x * other.m_x + m_y * other.m_y + m_z * other.m_z; }

        static fvector cross_product( const fvector& a, const fvector& b ) {
            return fvector(
                a.m_y * b.m_z - a.m_z * b.m_y,
                a.m_z * b.m_x - a.m_x * b.m_z,
                a.m_x * b.m_y - a.m_y * b.m_x
            );
        }

        double m_x, m_y, m_z;
    };

    struct fvector2d {
        fvector2d( ) : m_x( ), m_y( ) { }
        fvector2d( double m_x, double m_y ) : m_x( m_x ), m_y( m_y ) { }

        fvector2d operator+( const fvector2d& other ) const {
            return { m_x + other.m_x, m_y + other.m_y };
        }

        fvector2d operator-( const fvector2d& other ) const {
            return { m_x - other.m_x, m_y - other.m_y };
        }

        fvector2d operator*( double offset ) const {
            return { m_x * offset, m_y * offset };
        }

        fvector2d operator/( double offset ) const {
            return { m_x / offset, m_y / offset };
        }

        fvector2d& operator*=( const double other ) {
            m_x *= other;
            m_y *= other;
            return *this;
        }

        fvector2d& operator/=( const double other ) {
            m_x /= other;
            m_y /= other;
            return *this;
        }

        fvector2d& operator=( const fvector2d& other ) {
            m_x = other.m_x;
            m_y = other.m_y;
            return *this;
        }

        fvector2d& operator+=( const fvector2d& other ) {
            m_x += other.m_x;
            m_y += other.m_y;
            return *this;
        }

        fvector2d& operator-=( const fvector2d& other ) {
            m_x -= other.m_x;
            m_y -= other.m_y;
            return *this;
        }

        fvector2d& operator*=( const fvector2d& other ) {
            m_x *= other.m_x;
            m_y *= other.m_y;
            return *this;
        }

        fvector2d& operator/=( const fvector2d& other ) {
            m_x /= other.m_x;
            m_y /= other.m_y;
            return *this;
        }

        operator bool( ) {
            return bool( m_x || m_y );
        }

        friend bool operator==( const fvector2d& a, const fvector2d& b ) {
            return a.m_x == b.m_x && a.m_y == b.m_y;
        }

        friend bool operator!=( const fvector2d& a, const fvector2d& b ) {
            return !( a == b );
        }

        double distance_to( const fvector2d& other );

        double m_x, m_y;
    };

    struct fname {
        fname( ) : m_index( ) { }
        fname( int index ) : m_index( index ) { }

        operator bool( ) {
            return bool( m_index );
        }

        friend bool operator==( const fname& a, const fname& b ) {
            return a.m_index == b.m_index;
        }

        friend bool operator!=( const fname& a, const fname& b ) {
            return !( a == b );
        }

        std::uint32_t m_index;
        std::uint32_t m_number;
    };

    struct fplane : public fvector {
        fplane( ) : m_w( ) { }
        fplane( double w ) : m_w( w ) { }

        double m_w;
    };

    struct fmatrix {
        fmatrix( ) : m_x_plane( ), m_y_plane( ), m_z_plane( ), m_w_plane( ) { }
        fmatrix( fplane x_plane, fplane y_plane, fplane z_plane, fplane w_plane ) :
            m_x_plane( x_plane ), m_y_plane( y_plane ), m_z_plane( z_plane ), m_w_plane( w_plane ) {
        }

        fplane m_x_plane, m_y_plane, m_z_plane, m_w_plane;
    };

    class f_text_data {
    public:
        char m_unknown_data[ 0x18 ];
        wchar_t* m_name;
        __int32 m_length;
    };

    struct ftext {
        f_text_data* m_data;
        char m_unknown_data[ 0x10 ];

        fstring get( ) {
            if ( m_data )
                return m_data->m_name;
            return nullptr;
        }
    };

    struct f_hit_result {
        int32_t m_face_index;                        // 0x00(0x04)
        float m_time;                                // 0x04(0x04)
        float m_distance;                            // 0x08(0x04)
        char m_pad_c[ 0x4 ];                          // 0x0C(0x04)
        struct fvector m_location;                   // 0x10(0x18)
        struct fvector m_impact_point;               // 0x28(0x18)
        struct fvector m_normal;                     // 0x40(0x18)
        struct fvector m_impact_normal;              // 0x58(0x18)
        struct fvector m_trace_start;                // 0x70(0x18)
        struct fvector m_trace_end;                  // 0x88(0x18)
        float m_penetration_depth;                   // 0xA0(0x04)
        int32_t m_my_item;                          // 0xA4(0x04)
        int32_t m_item;                             // 0xA8(0x04)
        char m_element_index;                        // 0xAC(0x01)
        char m_blocking_hit : 1;                     // 0xAD(0x01)
        char m_start_penetrating : 1;                // 0xAD(0x01)
        char m_pad_ad_2 : 6;                        // 0xAD(0x01)
        char m_pad_b2[ 0x2 ];                         // 0xAE(0x02)
        void* m_phys_material;                      // 0xB0(0x08)
        void* m_hit_object_handle;                  // 0xB8(0x20)
        void* m_component;                          // 0xD8(0x08)
        char m_pad_e4[ 0x10 ];                        // 0xE0(0x0c)
        struct fname m_bone_name;                    // 0xF0(0x04)
        struct fname m_my_bone_name;                 // 0xF4(0x04)
    };

    template<class t_enum>
    class t_enum_ss_byte {
    public:
        t_enum_ss_byte( ) {
        }

        t_enum_ss_byte( t_enum value )
            : m_value( static_cast< uint8_t >( value ) ) {
        }

        explicit t_enum_ss_byte( int32_t value )
            : m_value( static_cast< uint8_t >( value ) ) {
        }

        explicit t_enum_ss_byte( uint8_t value )
            : m_value( value ) {
        }

        operator t_enum( ) const {
            return ( t_enum )m_value;
        }

        t_enum get_value( ) const {
            return ( t_enum )m_value;
        }

    private:
        uint8_t m_value;
    };

    struct fguid {
        int m_a;
        int m_b;
        int m_c;
        int m_d;
    };

    struct fquat {
        double m_x;
        double m_y;
        double m_z;
        double m_w;
    };

    struct f_fast_array_serializer_item {
        int32_t replica_id;
        int32_t replica_id_next;
        uint8_t pad_08[ 0x4 ];
    };

    struct f_item_component_data_list {
        uint8_t pad[ 0x108 ];
    };

    struct fkey {
        fname m_key_name;
        char m_detials[ 0x10 ];
    };

    struct fbox_sphere_bounds final {
    public:
        struct fvector orgin;
        struct fvector box_extent;
        double sphere_radius;
    };

    struct flinear_color final {
    public:
        float m_r;
        float m_g;
        float m_b;
        float m_a;

        flinear_color with_alpha( float alpha ) const {
            return flinear_color{ m_r, m_g, m_b, alpha };
        }
    };

    struct f_gameplay_tag_query {
        uint8_t pad[ 0x48 ];
    };

    struct f_gameplay_tag {
    public:
        fname tag_name;
    };

    struct f_gameplay_tag_container {
    public:
        tarray<f_gameplay_tag*> gameplay_tags;
        tarray<f_gameplay_tag*> parent_tags;
    };

    struct f_fort_damage_number_color_info {
        fstring display_text;
        flinear_color color;
        flinear_color critical_color;
        f_gameplay_tag_container tags;
    };

    class f_weak_object_ptr {
    public:
        uint32_t m_object_index;                  // 0x0000(0x0004)
        uint32_t m_object_serial_number;          // 0x0004(0x0004)

    public:
        class u_object* get( );
        class u_object* operator->( ) const;
        bool operator==( const f_weak_object_ptr& other ) const;
        bool operator!=( const f_weak_object_ptr& other ) const;
        bool operator==( const class u_object* other ) const;
        bool operator!=( const class u_object* other ) const;
    };

    template<typename t_type>
    class t_weak_object_ptr : public f_weak_object_ptr {
    public:
        t_type* get( ) {
            return static_cast< t_type* >( f_weak_object_ptr::get( ) );
        }

        t_type* operator->( ) {
            return static_cast< t_type* >( f_weak_object_ptr::get( ) );
        }
    };

    template<typename t_object_id>
    class t_persistent_object_ptr {
    public:
        f_weak_object_ptr m_weak_ptr;
        t_object_id m_object_id;

    public:
        class u_object* get( ) {
            return m_weak_ptr.get( );
        }

        class u_object* operator->( ) const {
            return m_weak_ptr.get( );
        }
    };

    struct f_soft_object_path {
    public:
        fname m_asset_path_name;
        fstring m_sub_path_string;
    };

    class f_soft_object_ptr : public t_persistent_object_ptr<f_soft_object_path> {
    };

    template<typename t_type>
    class t_soft_object_ptr : public f_soft_object_ptr {
    public:
        t_type* get( ) {
            return static_cast< t_type* >( t_persistent_object_ptr::get( ) );
        }

        t_type* operator->( ) const {
            return static_cast< t_type* >( t_persistent_object_ptr::get( ) );
        }
    };

    struct f_quick_bar_slot {
    public:
        tarray<fguid*> m_items;
        uint8_t m_enabled : 1;
        uint8_t m_is_dirty : 1;
        uint8_t m_is_reserved : 1;
        uint8_t m_is_occupied : 1;
        int32_t m_used_by_slot_index;
        fguid m_used_by_item_guid;
    };

    struct f_curve_table_row_handle {
        class u_curve_table* m_curve_table;
        fname m_row_name;
    };

    struct f_runtime_float_curve {
        float m_min_value;
        float m_max_value;
        class u_curve_float* m_curve;
    };

    struct f_scalable_float {
        float m_value;
        f_curve_table_row_handle m_curve;
        f_runtime_float_curve m_curve_table;
    };

    struct f_fort_weapon_ramping_data {
    public:
        bool m_b_is_ramping_weapon;
        f_scalable_float m_max_ramp_stacks;
        f_scalable_float m_ramp_fire_rate_to_add;
        f_scalable_float m_ramp_grace_duration;
    };

    struct f_ranked_progress_replicated_data {
        fstring m_rank_type;
        int32_t m_rank;
    };

    struct root_motion_finish_velocity_settings {
        e_root_motion_finish_velocity_mode m_mode;
        fvector m_set_velocity;
        float m_clamp_velocity;
    };
}