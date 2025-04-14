namespace backend {
    class c_backend {
    public:
        bool update( sdk::u_canvas* canvas ) {
            if ( !canvas )
                return false;

            if ( !m_font ) {
                m_font = sdk::u_object::find_object<sdk::u_font*>(
                    encrypt( L"Engine/EngineFonts/Roboto.Roboto" ),
                    reinterpret_cast< sdk::u_object* >( -1 )
                );
                if ( !m_font )
                    return false;
            }

            auto renderer = renderer::c_renderer( canvas, m_font );
            if ( !m_background.empty( ) ) {
                for ( auto i = 0; i < m_background.size( ); i++ ) {
                    auto& element = m_background[ i ];
                    renderer.draw( element );
                }
                m_background.clear( );
            }

            if ( !m_foreground.empty( ) ) {
                for ( auto i = 0; i < m_foreground.size( ); i++ ) {
                    auto& element = m_foreground[ i ];
                    renderer.draw( element );
                }
                m_foreground.clear( );
            }

            return true;
        }

        void reserve( size_t render_count ) {
            m_background.resize( render_count );
            m_foreground.resize( render_count );
        }

        void add_element( internal::s_draw_info draw_info, bool foreground = true ) {
            auto& target = foreground ? m_foreground : m_background;
            target.emplace_back( draw_info );
        }

        inline void add_line( const sdk::fvector2d& start, const sdk::fvector2d& end,
            const sdk::flinear_color& color, float thickness = 1.0f, bool foreground = true ) {
            internal::s_draw_info info{};
            info.draw_type = internal::e_draw_type::line;
            info.position_a = start;
            info.position_b = end;
            info.color = color;
            info.thickness = thickness;

            add_element( info, foreground );
        }

        inline void add_rectangle( const sdk::fvector2d& min, const sdk::fvector2d& max,
            const sdk::flinear_color& color, float thickness = 1.0f, bool foreground = true ) {
            internal::s_draw_info info{};
            info.draw_type = internal::e_draw_type::rectangle;
            info.position_a = min;
            info.position_b = max;
            info.color = color;
            info.thickness = thickness;

            add_element( info, foreground );
        }

        inline void add_filled_rectangle( const sdk::fvector2d& min, const sdk::fvector2d& max,
            const sdk::flinear_color& color, bool foreground = true ) {
            internal::s_draw_info info{};
            info.draw_type = internal::e_draw_type::rect_filled;
            info.position_a = min;
            info.position_b = max;
            info.color = color;

            add_element( info, foreground );
        }

        inline void add_gradient_rectangle( const sdk::fvector2d& min, const sdk::fvector2d& max,
            const sdk::flinear_color& color1, const sdk::flinear_color& color2,
            bool foreground = true ) {
            internal::s_draw_info info{};
            info.draw_type = internal::e_draw_type::gradient_rect;
            info.position_a = min;
            info.position_b = max;
            info.color = color1;
            info.secondary_color = color2;

            add_element( info, foreground );
        }

        inline void add_circle( const sdk::fvector2d& center, double radius,
            const sdk::flinear_color& color, float thickness = 1.0f,
            double segments = 32.0, bool foreground = true ) {
            internal::s_draw_info info{};
            info.draw_type = internal::e_draw_type::circle;
            info.position_a = center;
            info.radius = radius;
            info.color = color;
            info.thickness = thickness;
            info.segments = segments;

            add_element( info, foreground );
        }

        inline void add_text( const sdk::fvector2d& position, const sdk::fstring& content,
            const sdk::flinear_color& color, double size = 12.0,
            bool centered_x = false, bool centered_y = false, bool foreground = true ) {
            internal::s_draw_info info{};
            info.draw_type = internal::e_draw_type::text;
            info.position_a = position;
            info.content = content;
            info.color = color;
            info.size = size;
            info.centered_x = centered_x;
            info.centered_y = centered_y;

            add_element( info, foreground );
        }

        inline void add_rounded_rectangle( const sdk::fvector2d& min, const sdk::fvector2d& max,
            double radius, const sdk::flinear_color& color,
            float thickness = 1.0f, bool filled = false, bool foreground = true ) {
            internal::s_draw_info info{};
            info.draw_type = internal::e_draw_type::rounded_rect;
            info.position_a = min;
            info.position_b = max;
            info.color = color;
            info.thickness = thickness;
            info.radius = radius;
            info.filled = filled;

            add_element( info, foreground );
        }

        inline void add_diamond( const sdk::fvector2d& center, double size,
            const sdk::flinear_color& color, float thickness = 1.0f, bool foreground = true ) {
            internal::s_draw_info info{};
            info.draw_type = internal::e_draw_type::diamond;
            info.position_a = center;
            info.size = size;
            info.color = color;
            info.thickness = thickness;

            add_element( info, foreground );
        }

    private:
        sdk::u_font* m_font = nullptr;
        sdk::vector<internal::s_draw_info> m_background;
        sdk::vector<internal::s_draw_info> m_foreground;
    };
}