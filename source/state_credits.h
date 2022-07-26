
typedef struct state_credits_t {
	point_t* stars[ 256 ];
	sprite_t* player;
    int count;
} state_credits_t;


void state_credits_init( game_t* game, state_credits_t* state ) {
	play_music( game, sound( game, "data/inter.ogg" ) );

	box( game, 0.0f, 0.0f, (float) game->screen_width, (float) game->screen_height, 0 )->zorder = -100.0f;

    float ypos = 50;
    label( game, 100, ypos, "CODE/DESIGN/MUSIC", 7, font( game, "data/prstartk.ttf" ) )->centered = true;
    ypos += 2;

    label( game, 100, ypos, "Mattias Gustavsson", 15, font( game, "data/deltoid-sans.ttf" ) )->centered = true;
    ypos += 46;

    label( game, 100, ypos, "SOUND EFFECTS", 7, font( game, "data/prstartk.ttf" ) )->centered = true;
    ypos += 2;

    label( game, 100, ypos, "Valdemar Delding", 15, font( game, "data/deltoid-sans.ttf" ) )->centered = true;
    ypos += 46;

    label( game, 100, ypos, "3D MODELS FROM", 7, font( game, "data/prstartk.ttf" ) )->centered = true;
    ypos += 2;

    label( game, 100, ypos, "TurboSquid", 15, font( game, "data/deltoid-sans.ttf" ) )->centered = true;
    ypos += 18;

    label( game, 100, ypos, "3DRT.com", 15, font( game, "data/deltoid-sans.ttf" ) )->centered = true;
    ypos += 18;

    label( game, 100, ypos, "Daz 3D", 15, font( game, "data/deltoid-sans.ttf" ) )->centered = true;
    ypos += 18;

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
}


gamestate_t state_credits_tick( game_t* game, state_credits_t* state ) {
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
    if( state->count >= 600 ) {
		return GAMESTATE_TITLE;
    }

	if( game->input.action || game->input.fire ) {
		game->money = 0;
		game->stage = 0;
		return GAMESTATE_INTRO;
	}

	return GAMESTATE_CREDITS;
}


void state_credits_register( game_t* game ) {
	register_state( game, GAMESTATE_CREDITS, sizeof( state_credits_t ), state_credits_init, NULL, state_credits_tick );
}

