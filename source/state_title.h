
typedef struct state_title_t {
	point_t* stars[ 256 ];
	sprite_t* player;
	label_t* press_start;
	int blink_wait;
	int blink_count;
	int count;
} state_title_t;


void state_title_init( game_t* game, state_title_t* state ) {
	play_music( game, sound( game, "data/inter.ogg" ) );

	sprite( game, 0.0f, 0.0f, bitmap( game, "data/title.png" ) );
	state->press_start = label( game, 100, 160, "PRESS START", 15, font( game, "data/prstartk.ttf" ) );
	state->press_start->centered = true;
	box( game, 0.0f, 0.0f, (float) game->screen_width, (float) game->screen_height, 0 )->zorder = -100.0f;
	state->player = sprite( game, game->screen_width / 2.0f, game->screen_height - 70.0f, bitmap( game, "data/sprites/player.png" ) );
	state->player->origin_x = 36.0f / 2.0f;
	state->player->origin_y = 34.0f / 2.0f;
	state->player->zorder = 1.0f;

	for( int i = 0; i < sizeof( state->stars ) / sizeof( *state->stars ); ++i ) {
		float x = (float)rnd( game, 0, game->screen_width - 1 );
		float y = (float)rnd( game, 0, game->screen_height - 1 );
		uint8_t c = (uint8_t)rnd( game, 20, 32 );
		point_t* star = point( game, x, y, c );
		star->zorder = -50.0f + (float ) c;
		state->stars[ i ] = star;
	}

	state->blink_wait = 200;
	state->blink_count = 0;
}


gamestate_t state_title_tick( game_t* game, state_title_t* state ) {
	if( state->blink_count > 0 ) {
		--state->blink_count;
		if( state->blink_count > 0 ) {
			state->press_start->visible = state->blink_count % 30 < 15;
		} else {
			state->press_start->visible = true;
		}
	} else {
		--state->blink_wait;
		if( state->blink_wait <= 0 ) {
			state->blink_wait = 200;
			state->blink_count = 100;
		}
	}

	for( int i = 0; i < sizeof( state->stars ) / sizeof( *state->stars ); ++i ) {
		float star_speed = ( ( (int)state->stars[ i ]->color ) - 19 ) / 13.0f;
		float sx = state->stars[ i ]->x;
		float sy = state->stars[ i ]->y;
		sy += star_speed;
		if( (int) sy >= game->screen_height ) {
			sx = (float)rnd( game, 0, game->screen_width - 1 );
			sy = 0.0f;
			uint8_t c = (uint8_t)rnd( game, 20, 32 );
			state->stars[ i ]->color = c;
			state->stars[ i ]->zorder = -50.0f + (float) c ;
			}
		state->stars[ i ]->x = sx;
		state->stars[ i ]->y = sy;
	}

	float const speed = 3.0f;
	float x = state->player->x;
	float y = state->player->y;
	x += game->input.horiz * speed;
	y += game->input.vert * speed;
	if( x < state->player->origin_x ) {
		x = state->player->origin_x;
	}
	if( y < state->player->origin_y ) {
		y = state->player->origin_y;
	}
	if( x > game->screen_width - state->player->origin_x ) {
		x = game->screen_width - state->player->origin_x;
	}
	if( y > game->screen_height - state->player->origin_y ) {
		y = game->screen_height - state->player->origin_y;
	}
	state->player->x = x;
	state->player->y = y;

    state->count++;
    if( state->count >= 720 ) {
		return GAMESTATE_CREDITS;
    }

	if( game->input.action || game->input.fire ) {
		game->money = 0;
		game->stage = 0;
		return GAMESTATE_INTRO;
	}

	return GAMESTATE_TITLE;
}


void state_title_register( game_t* game ) {
	register_state( game, GAMESTATE_TITLE, sizeof( state_title_t ), state_title_init, NULL, state_title_tick );
}
