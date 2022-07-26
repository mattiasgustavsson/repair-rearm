
typedef struct state_repair_t {
    point_t* stars[ 256 ];
    label_t* text;
    label_t* chargetext;
    float chargetextlimit;
    label_t* moneys;
    int total_to_take;
    int take_rate;
} state_repair_t;


void state_repair_init( game_t* game, state_repair_t* state ) {
    sprite( game, 0.0f, -5.0f, bitmap( game, "data/repair.png" ) )->zorder = 10.0f;

    state->moneys = label( game, 0.0f, 5.0f, "", 15, font( game, "data/prstartk.ttf" ) );
    state->moneys->centered = true;
    state->moneys->wrap = game->screen_width;
    state->moneys->zorder = 300.0f;

    int chargeslots = 0;

    char const* texts = "";
    if( game->stage == 1 ) {
        texts = "HULL DAMAGE\nLAZER RECALIB\nREFUELLING\nAMMUNITION\n";
        chargeslots = 4;
        state->chargetextlimit = -53.0f / 3.0f;
    } else if( game->stage == 2 ) {
        texts = "SHIP RENT\nWEAR AND TEAR\nFUEL COST\nFUEL TAX\nSUPERVISOR FEE\nADMINISTRATION\nADMIN SURCHARGE\nCERTIFICATION\nFLIGHT PERMIT\nWEAPONRY COST\nWEAPONRY TAX\nMISC COSTS\nBUREAUCRACY TAX\n";
        chargeslots = 13;
        state->chargetextlimit = -185.0f / 3.0f;
    }
    state->total_to_take = game->money;
    if( game->stage == 1 ) state->total_to_take /= 2;

    state->take_rate = state->total_to_take / 180;
    state->take_rate = state->take_rate <= 0 ? 1 : state->take_rate;

    float charges[ 13 ];

    float val = 0.5f;
    float sum = 0.0f;
    for( int i = 0; i < chargeslots; ++i ) {
        charges[ i ] = val;
        sum += val;
        val *= 0.5f;
    }
    shuffle( game, charges, chargeslots );

    char chargestring[ 256 ] = "";
    for( int i = 0; i < chargeslots; ++i )
    {
        float s = state->total_to_take * charges[ i ];
        if( s < 10 ) s = 10;
        strcat( chargestring, "$" );
        char str[ 16 ];
        sprintf( str, "%d", (int) s );
        strcat( chargestring, str );
        strcat( chargestring, "\n" );
    }


    if( game->stage == 1 ) {
        box( game, 0.0f, 110.0f, 199.0f, 110.0f, 15 )->zorder = 1.0f;
        box( game, 1.0f, 111.0f, 197.0f, 108.0f, 52 )->zorder = 2.0f;
    } else if( game->stage == 2 ) {
        box( game, 0.0f, 110.0f, 199.0f, 200.0f, 15 )->zorder = 1.0f;
        box( game, 1.0f, 111.0f, 197.0f, 198.0f, 52 )->zorder = 2.0f;
    }

    label_t* title = label( game, 10.0f, 120.0f, "REPAIR/REARM COSTS", 15, font( game, "data/deltoid-sans.ttf" ) );
    title->centered = true;
    title->zorder = 3.0f;
    title->wrap = 180;
    title->vspacing = 3;

    state->text = label( game, 10.0f, 150.0f, texts, 15, font( game, "data/prstartk.ttf" ) );
    state->text->centered = false;
    state->text->zorder = 3.0f;
    state->text->wrap = 180;
    state->text->vspacing = 3;
    state->text->limit = 0.0f;

    state->chargetext = label( game, 10.0f, 150.0f, chargestring, 15, font( game, "data/prstartk.ttf" ) );
    state->chargetext->right = true;
    state->chargetext->zorder = 3.0f;
    state->chargetext->wrap = 180;
    state->chargetext->vspacing = 3;
    state->chargetext->limit = 0.0f;

    play_music( game, sound( game, "data/inter.ogg" ) );

    box( game, 0.0f, 0.0f, (float) game->screen_width, (float) game->screen_height, 0 )->zorder = -100.0f;

    for( int i = 0; i < sizeof( state->stars ) / sizeof( *state->stars ); ++i ) {
        float x = (float)rnd( game, 0, game->screen_width - 1 );
        float y = (float)rnd( game, 0, game->screen_height - 1 );
        uint8_t c = (uint8_t)rnd( game, 20, 32 );
        point_t* star = point( game, x, y, c );
        star->zorder = -50.0f + (float ) c;
        state->stars[ i ] = star;
    }
}


gamestate_t state_repair_tick( game_t* game, state_repair_t* state ) {
    if( state->total_to_take > 0 ) {
        int x = state->total_to_take;
        if( x > state->take_rate ) x = state->take_rate;
        int m = game->money - x;
        if( m < 0 ) m = 0;
        game->money = m;
        state->total_to_take -= x;
    }
    char str[ 16 ];
    sprintf( str, "$%d", game->money );
    if( strlen( str ) < sizeof( state->moneys->text ) ) {
        strcpy( state->moneys->text, str );
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

    state->text->limit += 1.0f;
    state->chargetextlimit += 0.5f;
    if( state->chargetextlimit >= 0.0f ) {
        state->chargetext->limit = state->chargetextlimit;
    }

    if( state->total_to_take <= 0 && game->input.action ) {
        return GAMESTATE_LEVEL;
    }

    return GAMESTATE_REPAIR ;
}


void state_repair_register( game_t* game ) {
    register_state( game, GAMESTATE_REPAIR, sizeof( state_repair_t ), state_repair_init, NULL, state_repair_tick );
}
