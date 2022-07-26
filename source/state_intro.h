
typedef struct state_intro_t {
    point_t* stars[ 256 ];
    int texts_count;
    char const* texts[ 8 ];
    int current_text;
    label_t* text;
} state_intro_t;


void state_intro_init( game_t* game, state_intro_t* state ) {
    play_music( game, sound( game, "data/inter.ogg" ) );

    box( game, 0.0f, 0.0f, (float) game->screen_width, (float) game->screen_height, 0 )->zorder = -100.0f;
    sprite( game, 0, 0, bitmap( game, "data/officer.png" ) );

    state->texts_count = 0;
    if( game->stage  == 0 ) {
        state->texts[ state->texts_count++ ] = "Greetings, Cadet! It's good to have you on board.";
        state->texts[ state->texts_count++ ] = "Now get out there and blow up some Zoblorg ships. Any loot you grab is yours to keep.";
        state->texts[ state->texts_count++ ] = "The Federation thank you for your service.";
    } else if( game->stage  == 1 ) {
        state->texts[ state->texts_count++ ] = "Well done, Ensign. However, I'm afraid there are some bad news.";
        state->texts[ state->texts_count++ ] = "Due to cutbacks, you are going to have to pay for repairs and rearming out of your own funds.";
        state->texts[ state->texts_count++ ] = "The Federation thank you for your service.";
    } else if( game->stage  == 2 ) {
        state->texts[ state->texts_count++ ] = "Great job, Lieutenant! But despite your efforts, the Zoblorg Empire is beating us.";
        state->texts[ state->texts_count++ ] = "The price of repair and rearming have gone up, and you are liable to pay rent on your ship.";
        state->texts[ state->texts_count++ ] = "The Federation thank you for your service.";
    } else if( game->stage  == 3 ) {
        state->texts[ state->texts_count++ ] = "Good to see you, Commander. You are behind on your payments for the privilege to serve. ";
        state->texts[ state->texts_count++ ] = "All your assets have been seized, and you will be sold as a slave to the Zoblorg Empire to pay off your debt.";
        state->texts[ state->texts_count++ ] = "The Federation thank you for your service.";
    }

    box( game, 0.0f, 140.0f, 199.0f, 90.0f, 15 )->zorder = 1.0f;
    box( game, 1.0f, 141.0f, 197.0f, 88.0f, 52 )->zorder = 2.0f;

    state->text = label( game, 10.0f, 150.0f, state->texts[ 0 ], 15, font( game, "data/prstartk.ttf" ) );
    state->text->zorder = 3.0f;
    state->text->wrap = 180;
    state->text->vspacing = 3;
    state->text->limit = 0.0f;

    for( int i = 0; i < sizeof( state->stars ) / sizeof( *state->stars ); ++i ) {
        float x = (float)rnd( game, 0, game->screen_width - 1 );
        float y = (float)rnd( game, 0, game->screen_height - 1 );
        uint8_t c = (uint8_t)rnd( game, 20, 32 );
        point_t* star = point( game, x, y, c );
        star->zorder = -50.0f + (float ) c;
        state->stars[ i ] = star;
    }
}


gamestate_t state_intro_tick( game_t* game, state_intro_t* state ) {

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

    state->text->limit += 1.0f;

    if( game->input.action ) {
        state->current_text++;
        if( state->current_text >= state->texts_count ) {
            if( game->stage == 0 ) {
                return GAMESTATE_LEVEL;
            } else if( game->stage < 3 ) {
                return GAMESTATE_REPAIR;
            } else {
                return GAMESTATE_TITLE;
            }
        } else {
            state->text->limit = 0.0f;
            if( strlen( state->texts[ state->current_text ] ) < sizeof( state->text->text ) ) {
                strcpy( state->text->text, state->texts[ state->current_text ] );
            }
        }
    }

    return GAMESTATE_INTRO;
}


void state_intro_register( game_t* game ) {
    register_state( game, GAMESTATE_INTRO, sizeof( state_intro_t ), state_intro_init, NULL, state_intro_tick );
}
