
typedef struct state_launch_t {
	label_t* click_to_start;
	int blink_wait;
	int blink_count;
	int count;
} state_launch_t;


void state_launch_init( game_t* game, state_launch_t* state ) {
	state->click_to_start = label( game, 100, 160, "CLICK TO START", 15, font( game, "data/prstartk.ttf" ) );
	state->click_to_start->centered = true;

	state->blink_wait = 200;
	state->blink_count = 0;
}


gamestate_t state_launch_tick( game_t* game, state_launch_t* state ) {
	if( state->blink_count > 0 ) {
		--state->blink_count;
		if( state->blink_count > 0 ) {
			state->click_to_start->visible = state->blink_count % 30 < 15;
		} else {
			state->click_to_start->visible = true;
		}
	} else {
		--state->blink_wait;
		if( state->blink_wait <= 0 ) {
			state->blink_wait = 200;
			state->blink_count = 100;
		}
	}

	if( game->input.action || game->input.fire || game->input.click ) {
		game->money = 0;
		game->stage = 0;
		return GAMESTATE_TITLE;
	}

	return GAMESTATE_LAUNCH;
}


void state_launch_register( game_t* game ) {
	register_state( game, GAMESTATE_LAUNCH, sizeof( state_launch_t ), state_launch_init, NULL, state_launch_tick );
}
