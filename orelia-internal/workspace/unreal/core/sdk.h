#pragma once
namespace sdk {
	class u_object;
	class u_world;
	class u_class;
	class u_canvas;
	class u_kismet_math_library;
	class u_kismet_string_library;
	class u_kismet_system_library;
	class u_fort_kismet_library;
	class u_gameplay_statics;

	namespace kismet {
		u_kismet_math_library* g_math_library;
		u_kismet_string_library* g_string_library;
		u_kismet_system_library* g_system_library;
		u_fort_kismet_library* g_fort_library;
		u_gameplay_statics* g_gameplay_statics;
	}

	namespace classes {
		u_class* g_fort_weapon;
		u_class* g_fort_pickup;
		u_class* g_container;
		u_class* g_weakspot;
		u_class* g_vehicle;
		u_class* g_material_instance;
		u_class* g_building;
		u_class* g_item_definition;
		u_class* g_player_controller;
	}

	namespace keys {
		fkey g_left_mouse;
		fkey g_right_mouse;
		fkey g_insert;
		fkey g_left_shift;
		fkey g_left_alt;
		fkey g_thumb_mouse_button;
		fkey g_thumb_mouse_button2;
		fkey g_gamepad_left_trigger;
		fkey g_w;
		fkey g_a;
		fkey g_s;
		fkey g_d;
		fkey g_spacebar;
		fkey g_mouse_scroll_up;
		fkey g_mouse_scroll_down;
		fkey g_mouse_wheel_axis;
		fkey g_f7;
		fkey g_f8;
	}
}

namespace offsets {
	int process_event = 0x20;
	int static_find_object = 0x2557834;
	int draw_transition = 112;
}