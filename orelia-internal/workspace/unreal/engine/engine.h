#define declare_member(type, name, offset) type name() { return *(type*)( this + offset ); } 
#define declare_member_bit(bit, name, offset) bool name( ) { return bool( *(char*)( this + offset) & (1 << bit)); }

#define apply_member(type, name, offset) void name( type val ) { *(type*)( this + offset ) = val; }
#define apply_member_bit(bit, name, offset) void name(bool value) { auto bitfield = *(char*)(this + offset); bitfield |= (value << bit); *(char*)(this + offset) = bitfield; }
#define invert_member_bit(bit, name, offset) void name(bool value) { auto bitfield = *(char*)(this + offset); bitfield &= ~(value << bit); *(char*)(this + offset) = bitfield; }

#define declare_member_array(type, name, size, offset) \
    type* name() { return (type*)(this + offset); } \
    type& name(int i) { return ((type*)(this + offset))[i]; }

#define apply_member_array(type, name, size, offset) \
    void name(type val, int i) { *(type*)(this + offset + (i * sizeof(type))) = val; }

namespace sdk {
    class u_function {
    public:

    };

    class u_object {
    public:
        void process_event( u_function* fn, void* params ) {
            if ( auto vtable = *( void*** )this ) {
                reinterpret_cast< void( __cdecl* )( u_object*, u_function*, void* ) >( vtable[ offsets::process_event ] )( this, fn, params );
            }
        }

        static u_object* static_find_object( u_object* fn_class, u_object* outer, const wchar_t* name, bool exact_class ) {
            return reinterpret_cast< u_object * ( __cdecl* )( u_object*, u_object*, const wchar_t*, bool ) >( pe::g_module_base + offsets::static_find_object )( fn_class, outer, name, exact_class );
        }

        template <class type>
        static type find_object( const wchar_t* name, u_object* outer = nullptr ) {
            return reinterpret_cast< type >( u_object::static_find_object( nullptr, outer, name, false ) );
        }
    };

    class u_texture : public u_object {
    public:

    };

    class u_font : public u_object {
    public:

    };

    class u_canvas : public u_object {
    public:
        declare_member( float, clip_x, 0x30 );
        declare_member( float, clip_y, 0x34 );
        declare_member( u_texture*, default_texture, 0x70 );

        void k2_draw_texture(
            u_texture* render_texture,
            fvector2d screen_position,
            fvector2d screen_size,
            fvector2d coordinate_position,
            fvector2d coordinate_size,
            flinear_color render_color,
            e_blend_mode blend_mode,
            float rotation,
            fvector2d pivot_point
        );

        void k2_draw_text(
            u_font* render_font,
            fstring render_text,
            fvector2d screen_position,
            double font_size,
            flinear_color render_color,
            bool b_centre_x,
            bool b_centre_y,
            bool b_outlined
        );

        void k2_draw_line( fvector2d screen_position_a, fvector2d screen_position_b, float thickness, flinear_color render_color );
        void k2_draw_polygon( u_texture* render_texture, fvector2d screen_position, fvector2d radius, int32_t number_of_sides, flinear_color render_color );
        fvector k2_project( fvector world_location );
        fvector2d k2_text_size( u_font* render_font, fstring render_text, fvector2d scale );
        fvector k2_deproject( fvector2d screen_position, fvector world_origin, fvector world_direction );
    };

    class u_game_viewport_client : public u_object {
    public:
        declare_member( u_world*, world, 0x78 );
        apply_member( std::uint32_t, b_is_play_in_editor_viewport, 0xA0 );
        apply_member( std::uint32_t, b_disable_world_rendering, 0xB0 );
    };

    class u_player : public u_object {
    public:
    };

    class u_localplayer : public u_player {
    public:
        declare_member( u_game_viewport_client*, viewport_client, 0x78 );
    };

    class u_game_instance : public u_object {
    public:
        declare_member( tarray<u_localplayer*>, localplayers, 0x38 );

        u_localplayer* get_localplayer( ) {
            return localplayers( )[ 0 ];
        }
    };

    class u_world : public u_object {
    public:
        declare_member( u_game_instance*, game_instance, 0x210 );
        declare_member( std::uintptr_t, camera_location_ptr, 0x128 );
        declare_member( std::uintptr_t, camera_rotation_ptr, 0x138 );

        u_world* get_front_end( ) const {
            return find_object<u_world*>( encrypt( L"Frontend" ), reinterpret_cast< u_object* >( -1 ) );
        }
    };

    class u_kismet_string_library : public u_object {
    public:
        fstring concat_str_str( fstring a, fstring b );
        bool contains( fstring search_in, fstring substring, bool use_case, bool search_from_end );
        fname conv_string_to_name( fstring in_string );
        fstring conv_name_to_string( fname in_name );
        fstring build_string_name( fstring append_to, fstring prefix, fname in_name, fstring suffix );
        fstring build_string_int( fstring append_to, fstring prefix, int in_int, fstring suffix );
        int32_t len( fstring s );
        fstring to_lower( fstring source_string );
        fstring build_string_double( fstring append_to, fstring prefix, double in_double, fstring suffix );
    };

    class u_kismet_math_library : public u_object {
    public:
        fvector get_forward_vector( frotator in_rot );
        frotator conv_vector_to_rotator( fvector in_vec );
        fvector conv_vector_2d_to_vector( fvector2d in_vector_2d, float z );
        frotator find_look_at_rotation( fvector start, fvector target );
        frotator r_interp_to( frotator current, frotator target, float delta_time, float interp_speed );
        double vector_distance( fvector v_1, fvector v_2 );
        flinear_color linear_color_lerp_using_hsv( flinear_color a, flinear_color b, float alpha );
        double sin( double a );
        double cos( double a );
        double acos( double a );
        double asin( double a );
        double atan2( double m_y, double m_x );
        double pow( double base, double exp );
        double abs( double a );
        double sqrt( double a );
        double atan( double a );
        double tan( double a );
    };
}