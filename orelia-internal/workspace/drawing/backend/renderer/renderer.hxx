namespace renderer {
    class c_renderer {
    public:
        c_renderer( sdk::u_canvas* canvas, sdk::u_font* font ) : m_shared_data( canvas, font ) { }

        void draw( internal::s_draw_info& info ) {
            if ( !m_shared_data.is_valid( ) ) {
                return;
            }

            auto canvas = m_shared_data.get_canvas( );
            if ( !canvas )
                return;

            if ( info.draw_type == internal::e_draw_type::line ) {
                canvas->k2_draw_line( info.position_a, info.position_b, info.thickness, info.color );
            }

            //switch ( info.draw_type ) {
            //case internal::e_draw_type::line: {
            //    canvas->k2_draw_line( info.position_a, info.position_b, info.thickness, info.color );
            //} break;

            //case internal::e_draw_type::rectangle: {
            //    draw_rect( canvas, info.position_a, info.position_b - info.position_a, info.color, info.thickness );
            //} break;

            //case internal::e_draw_type::rect_filled: {
            //    draw_rect_filled( canvas, info.position_a, info.position_b - info.position_a, info.color, 1.0f );
            //} break;

            //case internal::e_draw_type::gradient_rect: {
            //    draw_gradient_rect( canvas, info.position_a, info.position_b - info.position_a,
            //        info.color, info.secondary_color, 1.0f );
            //} break;

            //case internal::e_draw_type::circle: {
            //    draw_circle( canvas, info.position_a, info.color, info.radius,
            //        info.segments, info.thickness, info.filled );
            //} break;

            //case internal::e_draw_type::text: {
            //    if ( m_shared_data.get_font( ) ) {
            //        canvas->k2_draw_text( m_shared_data.get_font( ), info.content,
            //            info.position_a, sdk::fvector2d( info.size, info.size ), info.color,
            //            info.centered_x, info.centered_y, info.outlined );
            //    }
            //} break;

            //case internal::e_draw_type::diamond: {
            //    draw_diamond( canvas, info.position_a, sdk::fvector2d( info.size, info.size ), info.color );
            //} break;

            //case internal::e_draw_type::rounded_rect: {
            //    draw_rounded_rect( canvas, info.position_a, info.position_b - info.position_a,
            //        info.color, info.radius, info.filled );
            //} break;
            //}
        }

    private:
        void draw_circle(
            sdk::u_canvas* canvas,
            sdk::fvector2d position,
            sdk::flinear_color color,
            double radius,
            double segments,
            float thickness = 1.0f,
            bool filled = false
        ) {
            if ( filled ) {
                for ( double r = 0; r <= radius; r += 1.0 ) {
                    auto step = sdk::math::pi * 2.0 / segments;

                    sdk::fvector2d prev_point;
                    bool first_point = true;

                    for ( auto degree = 0.0; degree < sdk::math::pi * 2.0; degree += step ) {
                        double x = r * sdk::kismet::g_math_library->cos( degree ) + position.m_x;
                        double y = r * sdk::kismet::g_math_library->sin( degree ) + position.m_y;

                        if ( !first_point ) {
                            canvas->k2_draw_line( prev_point, sdk::fvector2d( x, y ), 1.0f, color );
                        }

                        prev_point = sdk::fvector2d( x, y );
                        first_point = false;
                    }
                }
            }
            else {
                auto step = sdk::math::pi * 2.0 / segments;

                for ( auto degree = 0.0; degree < sdk::math::pi * 2.0; degree += step ) {
                    double x_1 = radius * sdk::kismet::g_math_library->cos( degree ) + position.m_x;
                    double y_1 = radius * sdk::kismet::g_math_library->sin( degree ) + position.m_y;
                    double x_2 = radius * sdk::kismet::g_math_library->cos( degree + step ) + position.m_x;
                    double y_2 = radius * sdk::kismet::g_math_library->sin( degree + step ) + position.m_y;

                    canvas->k2_draw_line(
                        sdk::fvector2d( x_1, y_1 ),
                        sdk::fvector2d( x_2, y_2 ),
                        thickness,
                        color
                    );
                }
            }
        }

        void draw_rect(
            sdk::u_canvas* canvas,
            sdk::fvector2d position,
            sdk::fvector2d size,
            sdk::flinear_color color,
            float thickness
        ) {
            canvas->k2_draw_line( sdk::fvector2d( position.m_x, position.m_y ),
                sdk::fvector2d( position.m_x + size.m_x, position.m_y ),
                thickness, color );

            canvas->k2_draw_line( sdk::fvector2d( position.m_x + size.m_x, position.m_y ),
                sdk::fvector2d( position.m_x + size.m_x, position.m_y + size.m_y ),
                thickness, color );

            canvas->k2_draw_line( sdk::fvector2d( position.m_x + size.m_x, position.m_y + size.m_y ),
                sdk::fvector2d( position.m_x, position.m_y + size.m_y ),
                thickness, color );

            canvas->k2_draw_line( sdk::fvector2d( position.m_x, position.m_y + size.m_y ),
                sdk::fvector2d( position.m_x, position.m_y ),
                thickness, color );
        }

        void draw_rect_filled(
            sdk::u_canvas* canvas,
            sdk::fvector2d position,
            sdk::fvector2d size,
            sdk::flinear_color color,
            float thickness
        ) {
            auto default_texture = canvas->default_texture( );
            for ( float m_y = position.m_y; m_y < position.m_y + size.m_y; m_y++ ) {
                canvas->k2_draw_texture(
                    default_texture,
                    sdk::fvector2d( position.m_x, m_y ),
                    sdk::fvector2d( size.m_x, 1.0 ),
                    sdk::fvector2d( ),
                    sdk::fvector2d( 1.0, 1.0 ),
                    color,
                    sdk::e_blend_mode::translucent,
                    0.0f,
                    sdk::fvector2d( )
                );
            }
        }

        void draw_gradient_rect(
            sdk::u_canvas* canvas,
            sdk::fvector2d position,
            sdk::fvector2d size,
            sdk::flinear_color color_a,
            sdk::flinear_color color_b,
            float thickness
        ) {
            for ( auto i = position.m_y; i < position.m_y + size.m_y; i++ ) {
                float alpha = ( i - position.m_y ) / size.m_y;
                auto interpolated_color = sdk::kismet::g_math_library->linear_color_lerp_using_hsv( color_a, color_b, alpha );
                canvas->k2_draw_line( sdk::fvector2d( position.m_x, i ),
                    sdk::fvector2d( position.m_x + size.m_x, i ),
                    thickness, interpolated_color );
            }
        }

        void draw_diamond(
            sdk::u_canvas* canvas,
            sdk::fvector2d position,
            sdk::fvector2d radius,
            sdk::flinear_color color
        ) {
            sdk::fvector2d diamond_points[ 4 ];
            diamond_points[ 0 ] = { position.m_x, position.m_y - radius.m_y };
            diamond_points[ 1 ] = { position.m_x + radius.m_x, position.m_y };
            diamond_points[ 2 ] = { position.m_x, position.m_y + radius.m_y };
            diamond_points[ 3 ] = { position.m_x - radius.m_x, position.m_y };

            canvas->k2_draw_polygon( nullptr, diamond_points[ 0 ], radius, 4, color );
        }

        void draw_rounded_rect(
            sdk::u_canvas* canvas,
            sdk::fvector2d position,
            sdk::fvector2d size,
            sdk::flinear_color color,
            float rounding,
            bool filled
        ) {
            if ( filled ) {
                draw_rect_filled( canvas,
                    sdk::fvector2d( position.m_x + rounding, position.m_y ),
                    sdk::fvector2d( size.m_x - ( rounding * 2.0 ), size.m_y ),
                    color, 1.0f );

                draw_rect_filled( canvas,
                    sdk::fvector2d( position.m_x, position.m_y + rounding ),
                    sdk::fvector2d( size.m_x, size.m_y - ( rounding * 2.0 ) ),
                    color, 1.0f );
            }

            draw_diamond( canvas,
                sdk::fvector2d( position.m_x + rounding - 2.0, position.m_y + rounding + 3.0 ),
                sdk::fvector2d( rounding - 1.0, rounding - 2.0 ),
                color );

            draw_diamond( canvas,
                sdk::fvector2d( position.m_x + size.m_x - rounding + 1.0, position.m_y + rounding + 3.0 ),
                sdk::fvector2d( rounding - 1.0, rounding - 2.0 ),
                color );

            draw_diamond( canvas,
                sdk::fvector2d( position.m_x + rounding - 2.0, position.m_y + size.m_y - rounding + 5.0 ),
                sdk::fvector2d( rounding - 1.0, rounding - 2.0 ),
                color );

            draw_diamond( canvas,
                sdk::fvector2d( position.m_x + size.m_x - rounding + 1.0, position.m_y + size.m_y - rounding + 5.0 ),
                sdk::fvector2d( rounding - 1.0, rounding - 2.0 ),
                color );
        }

        internal::c_shared_data m_shared_data;
    };
}