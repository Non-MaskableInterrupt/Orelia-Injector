namespace internal {
	enum class e_draw_type : std::uint32_t {
		line,
		rectangle,
		circle,
		text,
		rect_filled,
		gradient_rect,
		diamond,
		rounded_rect
	};

    class c_shared_data {
    public:
        c_shared_data( sdk::u_canvas* canvas, sdk::u_font* font ) :
            m_canvas( canvas ),
            m_font( font ),
            m_screen_size( canvas ? sdk::fvector2d( canvas->clip_x( ), canvas->clip_y( ) ) : sdk::fvector2d( ) ),
            m_screen_center( m_screen_size / 2.0 )
        {
        }

        sdk::u_canvas* get_canvas( ) const { return m_canvas; }
        sdk::u_font* get_font( ) const { return m_font; }
        const sdk::fvector2d& get_screen_size( ) const { return m_screen_size; }
        const sdk::fvector2d& get_screen_center( ) const { return m_screen_center; }

        bool is_valid( ) const { return m_canvas != nullptr && m_font != nullptr; }

    private:
        sdk::u_canvas* m_canvas;
        sdk::u_font* m_font;
        sdk::fvector2d m_screen_size;
        sdk::fvector2d m_screen_center;
    };

	struct s_draw_info {
		e_draw_type draw_type{};
		sdk::fvector2d position_a{};
		sdk::fvector2d position_b{};
		sdk::fvector2d position_c{};
		sdk::flinear_color color{};
		sdk::flinear_color secondary_color{};
		float thickness{};
		double radius{};
		double segments{};
		double size{};
		sdk::fstring content{};
		bool filled{ false };
		bool outlined{ false };
		bool centered_x{ false };
		bool centered_y{ false };
		bool rounded{ false };
	};
}