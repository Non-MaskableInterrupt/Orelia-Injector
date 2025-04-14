
namespace sdk {
    void u_canvas::k2_draw_texture(
        u_texture* render_texture,
        fvector2d screen_position,
        fvector2d screen_size,
        fvector2d coordinate_position,
        fvector2d coordinate_size,
        flinear_color render_color,
        e_blend_mode blend_mode,
        float rotation,
        fvector2d pivot_point
    ) {
        static u_function* function = 0;
        if ( !function ) {
            function = find_object< u_function* >( encrypt( L"Canvas.K2_DrawTexture" ) );
        }

        struct {
            u_texture* render_texture;
            fvector2d screen_position;
            fvector2d screen_size;
            fvector2d coordinate_position;
            fvector2d coordinate_size;
            flinear_color render_color;
            e_blend_mode blend_mode;
            float rotation;
            fvector2d pivot_point;
        } params{ render_texture, screen_position, screen_size, coordinate_position, coordinate_size, render_color, blend_mode, rotation, pivot_point };

        this->process_event( function, &params );
    }

    void u_canvas::k2_draw_text(
        u_font* render_font,
        fstring render_text,
        fvector2d screen_position,
        double font_size,
        flinear_color render_color,
        bool b_centre_x,
        bool b_centre_y,
        bool b_outlined
    ) {
        static u_function* function = 0;
        if ( !function ) {
            auto fn_name = encrypt( L"Canvas.K2_DrawText" );
            function = find_object< u_function* >( fn_name.decrypt( ) );
            fn_name.clear( );
        }

        struct {
            u_font* render_font;
            fstring render_text;
            fvector2d screen_position;
            fvector2d scale;
            flinear_color render_color;
            float kerning;
            flinear_color shadow_color;
            fvector2d shadow_offset;
            bool b_centre_x;
            bool b_centre_y;
            bool b_outlined;
            flinear_color outline_color;
        } params{ render_font, render_text, screen_position, fvector2d( 1.0, 1.0 ) , render_color , 0.f, flinear_color( ), fvector2d( ), b_centre_x , b_centre_y , b_outlined, flinear_color( 0.f, 0.f, 0.f, 1.f ) };

        this->process_event( function, &params );
    }

	void u_canvas::k2_draw_polygon( u_texture* render_texture, fvector2d screen_position, fvector2d radius, int32_t number_of_sides, flinear_color render_color ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"Canvas.K2_DrawPolygon" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			u_texture* render_texture;
			fvector2d screen_position;
			fvector2d radius;
			int32_t number_of_sides;
			flinear_color render_color;
		} params{ render_texture, screen_position, radius, number_of_sides, render_color };

		this->process_event( function, &params );
	}

	fvector u_canvas::k2_project( fvector world_location ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"Canvas.k2_Project" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			fvector world_location;
			fvector return_value;
		} params{ world_location };

		this->process_event( function, &params );

		return params.return_value;
	}

	fvector2d u_canvas::k2_text_size( u_font* render_font, fstring render_text, fvector2d scale ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"Canvas.K2_TextSize" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			u_font* render_font;
			fstring render_text;
			fvector2d scale;
			fvector2d return_value;
		} params{ render_font, render_text, scale };

		this->process_event( function, &params );

		return params.return_value;
	}

	void u_canvas::k2_draw_line( fvector2d screen_position_a, fvector2d screen_position_b, float thickness, flinear_color render_color ) {
		static u_function* function = 0;
		if ( !function ) {
			function = find_object< u_function* >( encrypt( L"Canvas.K2_DrawLine" ) );
		}

		struct {
			fvector2d screen_position_a;
			fvector2d screen_position_b;
			float thickness;
			flinear_color render_color;
		} params{ screen_position_a, screen_position_b, thickness, render_color };

		this->process_event( function, &params );
	}

	fvector u_canvas::k2_deproject( fvector2d screen_position, fvector world_origin, fvector world_direction ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"Canvas.k2_Deproject" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			fvector2d screen_position;
			fvector world_origin;
			fvector world_direction;
			fvector return_value;
		} params{ screen_position, world_origin, world_direction };

		this->process_event( function, &params );

		return params.return_value;
	}

    fstring u_kismet_string_library::concat_str_str( fstring a, fstring b ) {
        static u_function* function = nullptr;
        if ( !function ) {
            auto fn_name = encrypt( L"KismetStringLibrary.Concat_StrStr" );
            function = find_object<u_function*>( fn_name.decrypt( ) );
            fn_name.clear( );
        }

        struct {
            fstring a;
            fstring b;
            fstring return_value;
        } params{
            a,
            b
        };

        this->process_event( function, &params );

        return params.return_value;
    }

    bool u_kismet_string_library::contains( fstring search_in, fstring substring, bool use_case, bool search_from_end ) {
        static u_function* function = nullptr;
        if ( !function ) {
            auto fn_name = encrypt( L"KismetStringLibrary.Contains" );
            function = find_object<u_function*>( fn_name.decrypt( ) );
            fn_name.clear( );
        }

        struct {
            fstring search_in;
            fstring substring;
            bool use_case;
            bool search_from_end;
            bool return_value;
        } params{
            search_in,
            substring,
            use_case,
            search_from_end
        };

        this->process_event( function, &params );

        return params.return_value;
    }

    fname u_kismet_string_library::conv_string_to_name( fstring in_string ) {
        static u_function* function = nullptr;
        if ( !function ) {
            auto fn_name = encrypt( L"KismetStringLibrary.Conv_StringToName" );
            function = find_object<u_function*>( fn_name.decrypt( ) );
            fn_name.clear( );
        }

        struct {
            fstring in_string;
            fname return_value;
        } params{
            in_string
        };

        this->process_event( function, &params );
        return params.return_value;
    }

    fstring u_kismet_string_library::conv_name_to_string( fname in_name ) {
        static u_function* function = nullptr;
        if ( !function ) {
            auto fn_name = encrypt( L"KismetStringLibrary.Conv_NameToString" );
            function = find_object<u_function*>( fn_name.decrypt( ) );
            fn_name.clear( );
        }

        struct {
            fname in_name;
            fstring return_value;
        } params{
            in_name
        };

        this->process_event( function, &params );
        return params.return_value;
    }

    fstring u_kismet_string_library::build_string_name( fstring append_to, fstring prefix, fname in_name, fstring suffix ) {
        static u_function* function = nullptr;
        if ( !function ) {
            auto fn_name = encrypt( L"KismetStringLibrary.BuildString_Name" );
            function = find_object<u_function*>( fn_name.decrypt( ) );
            fn_name.clear( );
        }

        struct {
            fstring append_to;
            fstring prefix;
            fname in_name;
            fstring suffix;
            fstring return_value;
        } params{
            append_to,
            prefix,
            in_name,
            suffix
        };

        this->process_event( function, &params );
        return params.return_value;
    }

    fstring u_kismet_string_library::build_string_int( fstring append_to, fstring prefix, int in_int, fstring suffix ) {
        static u_function* function = nullptr;
        if ( !function ) {
            auto fn_name = encrypt( L"KismetStringLibrary.BuildString_Int" );
            function = find_object<u_function*>( fn_name.decrypt( ) );
            fn_name.clear( );
        }

        struct {
            fstring append_to;
            fstring prefix;
            int in_int;
            fstring suffix;
            fstring return_value;
        } params{
            append_to,
            prefix,
            in_int,
            suffix
        };

        this->process_event( function, &params );
        return params.return_value;
    }

    int32_t u_kismet_string_library::len( fstring s ) {
        static u_function* function = nullptr;
        if ( !function ) {
            auto fn_name = encrypt( L"KismetStringLibrary.Len" );
            function = find_object<u_function*>( fn_name.decrypt( ) );
            fn_name.clear( );
        }

        struct {
            fstring s;
            int32_t return_value;
        } params{
            s
        };

        this->process_event( function, &params );
        return params.return_value;
    }

    fstring u_kismet_string_library::to_lower( fstring source_string ) {
        static u_function* function = nullptr;
        if ( !function ) {
            auto fn_name = encrypt( L"KismetStringLibrary.ToLower" );
            function = find_object<u_function*>( fn_name.decrypt( ) );
            fn_name.clear( );
        }

        struct {
            fstring source_string;
            fstring return_value;
        } params{
            source_string
        };

        this->process_event( function, &params );
        return params.return_value;
    }

    fstring u_kismet_string_library::build_string_double( fstring append_to, fstring prefix, double in_double, fstring suffix ) {
        static u_function* function = nullptr;
        if ( !function ) {
            auto fn_name = encrypt( L"KismetStringLibrary.BuildString_Double" );
            function = find_object<u_function*>( fn_name.decrypt( ) );
            fn_name.clear( );
        }

        struct {
            fstring append_to;
            fstring prefix;
            double in_double;
            fstring suffix;
            fstring return_value;
        } params{
            append_to,
            prefix,
            in_double,
            suffix
        };

        this->process_event( function, &params );
        return params.return_value;
    }

	fvector u_kismet_math_library::get_forward_vector( frotator in_rot ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.GetForwardVector" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			frotator in_vec;
			fvector return_value;
		} params{ in_rot };

		this->process_event( function, &params );

		return params.return_value;
	}

	frotator u_kismet_math_library::conv_vector_to_rotator( fvector in_vec ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.Conv_VectorToRotator" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			fvector in_vec;
			frotator return_value;
		} params{ in_vec };

		this->process_event( function, &params );

		return params.return_value;
	}

	fvector u_kismet_math_library::conv_vector_2d_to_vector( fvector2d in_vector_2d, float z ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.Conv_Vector2DToVector" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			fvector2d in_vector_2d;
			float z;
			fvector return_value;
		} params{ in_vector_2d, z };

		this->process_event( function, &params );

		return params.return_value;
	}

	frotator u_kismet_math_library::find_look_at_rotation( fvector start, fvector target ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.FindLookAtRotation" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			fvector start;
			fvector target;
			frotator return_value;
		} params{ start, target };

		this->process_event( function, &params );

		return params.return_value;
	}

	frotator u_kismet_math_library::r_interp_to( frotator current, frotator target, float delta_time, float interp_speed ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.RInterpTo" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			frotator current;
			frotator target;
			float delta_time;
			float interp_speed;
			frotator return_value;
		} params{ current, target, delta_time, interp_speed };

		this->process_event( function, &params );

		return params.return_value;
	}

	double u_kismet_math_library::vector_distance( fvector v_1, fvector v_2 ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.Vector_Distance" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			fvector v_1;
			fvector v_2;
			double return_value;
		} params{ v_1, v_2 };

		this->process_event( function, &params );

		return params.return_value;
	}

	flinear_color u_kismet_math_library::linear_color_lerp_using_hsv( flinear_color a, flinear_color b, float alpha ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.LinearColorLerpUsingHSV" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			flinear_color a;
			flinear_color b;
			float alpha;
			flinear_color return_value;
		} params{ a, b, alpha };

		this->process_event( function, &params );

		return params.return_value;
	}

	double u_kismet_math_library::sin( double a ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.sin" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			double a;
			double return_value;
		} params{ a };

		this->process_event( function, &params );

		return params.return_value;
	}

	double u_kismet_math_library::cos( double a ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.cos" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			double a;
			double return_value;
		} params{ a };

		this->process_event( function, &params );

		return params.return_value;
	}

	double u_kismet_math_library::acos( double a ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.acos" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			double a;
			double return_value;
		} params{ a };

		this->process_event( function, &params );

		return params.return_value;
	}

	double u_kismet_math_library::asin( double a ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.asin" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			double a;
			double return_value;
		} params{ a };

		this->process_event( function, &params );

		return params.return_value;
	}

	double u_kismet_math_library::atan2( double m_y, double m_x ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.Atan2" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			double m_y;
			double m_x;
			double return_value;
		} params{ m_y , m_x };

		this->process_event( function, &params );

		return params.return_value;
	}

	double u_kismet_math_library::pow( double base, double exp ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.MultiplyMultiply_FloatFloat" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			double base;
			double exp;
			double return_value;
		} params{ base , exp };

		this->process_event( function, &params );

		return params.return_value;
	}

	double u_kismet_math_library::abs( double a ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.abs" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			double a;
			double return_value;
		} params{ a };

		this->process_event( function, &params );

		return params.return_value;
	}

	double u_kismet_math_library::sqrt( double a ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.sqrt" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			double a;
			double return_value;
		} params{ a };

		this->process_event( function, &params );

		return params.return_value;
	}

	double u_kismet_math_library::atan( double a ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.atan" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			double a;
			double return_value;
		} params{ a };

		this->process_event( function, &params );

		return params.return_value;
	}

	double u_kismet_math_library::tan( double a ) {
		static u_function* function = 0;
		if ( !function ) {
			auto fn_name = encrypt( L"KismetMathLibrary.tan" );
			function = find_object< u_function* >( fn_name.decrypt( ) );
			fn_name.clear( );
		}

		struct {
			double a;
			double return_value;
		} params{ a };

		this->process_event( function, &params );

		return params.return_value;
	}
}