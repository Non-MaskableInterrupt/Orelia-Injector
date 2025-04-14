namespace sdk {
	namespace init {
		fkey create_key_binding( const wchar_t* key_name ) {
			return fkey{
				fname{
					kismet::g_string_library->conv_string_to_name( key_name )
				},
				0
			};
		}

		void init_libraries( ) {
			kismet::g_math_library = u_object::find_object<u_kismet_math_library*>( L"Engine.Default__KismetMathLibrary" );
			kismet::g_string_library = u_object::find_object<u_kismet_string_library*>( L"Engine.Default__KismetStringLibrary" );
			kismet::g_system_library = u_object::find_object<u_kismet_system_library*>( L"Engine.Default__KismetSystemLibrary" );
			kismet::g_fort_library = u_object::find_object<u_fort_kismet_library*>( L"FortniteGame.Default__FortKismetLibrary" );
			kismet::g_gameplay_statics = u_object::find_object<u_gameplay_statics*>( L"Engine.Default__GameplayStatics" );
		}

		void init_classes( ) {
			classes::g_fort_weapon = u_object::find_object<u_class*>( L"FortniteGame.FortWeaponRanged" );
			classes::g_fort_pickup = u_object::find_object<u_class*>( L"FortniteGame.FortPickup" );
			classes::g_container = u_object::find_object<u_class*>( L"FortniteGame.BuildingContainer" );
			classes::g_weakspot = u_object::find_object<u_class*>( L"FortniteGame.BuildingWeakspot" );
			classes::g_vehicle = u_object::find_object<u_class*>( L"FortniteGame.FortAthenaVehicle" );
			classes::g_player_controller = u_object::find_object<u_class*>( L"Engine.Controller" );
			classes::g_building = u_object::find_object<u_class*>( L"FortniteGame.BuildingActor" );
			classes::g_item_definition = u_object::find_object<u_class*>( L"FortniteGame.FortItemDefinition" );
			classes::g_material_instance = u_object::find_object<u_class*>( L"Engine.MaterialInstance" );
		}

		void init_keys( ) {
			const std::pair<fkey*, const wchar_t*> key_mappings[ ] = {
				{&keys::g_left_mouse, L"LeftMouseButton"},
				{&keys::g_right_mouse, L"RightMouseButton"},
				{&keys::g_insert, L"Insert"},
				{&keys::g_left_shift, L"LeftShift"},
				{&keys::g_left_alt, L"LeftAlt"},
				{&keys::g_gamepad_left_trigger, L"Gamepad_LeftTrigger"},
				{&keys::g_w, L"W"},
				{&keys::g_a, L"A"},
				{&keys::g_s, L"S"},
				{&keys::g_d, L"D"},
				{&keys::g_spacebar, L"Spacebar"},
				{&keys::g_thumb_mouse_button2, L"ThumbMouseButton2"},
				{&keys::g_mouse_wheel_axis, L"MouseWheelAxis"},
				{&keys::g_f7, L"F7"},
				{&keys::g_f8, L"F8"}
			};

			for ( const auto& [key_ptr, key_name] : key_mappings )
				*key_ptr = create_key_binding( key_name );
		}
	}
}