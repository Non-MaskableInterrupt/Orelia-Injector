namespace game {
	void( *draw_transition_original )( sdk::u_game_viewport_client*, sdk::u_canvas* ) = nullptr;
	void draw_transition( sdk::u_game_viewport_client* viewport_client, sdk::u_canvas* canvas ) {
		if ( !drawing::g_backend.update( canvas ) )
			draw_transition_original( viewport_client, canvas );

	/*
		drawing::g_backend.add_line(
			sdk::fvector2d( 1.0, 1.0 ),
			sdk::fvector2d( 200, 200 ),
			sdk::flinear_color( 1.0, 0.0, 0.0, 1.0 )
		);*/

		draw_transition_original( viewport_client, canvas );
	}
}