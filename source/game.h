typedef palrle_data_t* bitmap_t;
typedef pixelfont_t* font_t;

typedef struct sound_t {
    short* sample_pairs;
    int sample_pairs_count;
} sound_t;


typedef enum gamestate_t {
    GAMESTATE_NONE,
    GAMESTATE_TITLE,
    GAMESTATE_CREDITS,
    GAMESTATE_INTRO,
    GAMESTATE_LEVEL,
    GAMESTATE_REPAIR,

    GAMESTATECOUNT
} gamestate_t;


typedef struct registered_state_t {
    gamestate_t state;
    void (*init)( void*, void* );
    void (*term)( void*, void* );
    gamestate_t (*tick)( void*, void* );
} registered_state_t;


typedef struct sprite_t {
    float x;
    float y;
    bitmap_t bitmap;
    bool visible;
    float zorder;
    float origin_x;
    float origin_y;
    float user_value;
} sprite_t;


typedef struct point_t {
    float x;
    float y;
    uint8_t color;
    float zorder;
    float origin_x;
    float origin_y;
} point_t;


typedef struct box_t {
    float x;
    float y;
    float w;
    float h;
    uint8_t color;
    float zorder;
    float origin_x;
    float origin_y;
} box_t;


typedef struct label_t {
    float x;
    float y;
    char text[ 256 ];
    uint8_t color;
    font_t font;
    bool centered;
    bool right;
    bool visible;
    int wrap;
    int vspacing;
    float limit;
    float zorder;
    float origin_x;
    float origin_y;
} label_t;


typedef enum sprite_type_t {
    SPRITE_TYPE_DELETED,
    SPRITE_TYPE_SPRITE,
    SPRITE_TYPE_POINT,
    SPRITE_TYPE_BOX,
    SPRITE_TYPE_LABEL,
} sprite_type_t;


typedef struct sprite_data_t {
    sprite_type_t type;
    struct {
        sprite_t sprite;
        point_t point;
        box_t box;
        label_t label;
    } data;
} sprite_data_t;


typedef struct sprite_order_t {
    float zorder;
    int index;
} sprite_order_t;


typedef struct input_t {
    bool fire;
    bool action;
    float horiz;
    float vert;
} input_t;


typedef struct game_t {
    data_t* data;

    uint8_t* screen;
    int screen_width;
    int screen_height;

    int states_count;
    registered_state_t states[ GAMESTATECOUNT ];
    int max_state_size;

    gamestate_t state;
    void* state_context;
    gamestate_t next_state;
    float transition_time;

    int sprites_count;
    sprite_data_t sprites[ 4096 ];
    sprite_order_t sprite_order[ 4096 ];

    input_t input;

    int stage;
    int money;

    rnd_pcg_t pcg;

    int frame_counter;
    sound_t music;
    sound_channel_t sounds[ 16 ];
} game_t;


font_t font( game_t* game, char const* filename );
void font_blit( game_t* game, font_t font, int x, int y, char const* text, uint8_t color, bool centered, bool right, int wrap_width, int vspacing, int limit );

bitmap_t bitmap( game_t* game, char const* filename );
void bitmap_blit( game_t* game, bitmap_t bitmap, int x, int y );

sound_t sound( game_t* game, const char* filename );
void play_music( game_t* game, sound_t music );

#define register_state( game, state, size, init, term, tick ) register_state_internal( (game), (state), size, \
    (void(*)(void*,void*))(init), (void(*)(void*,void*))(term), (gamestate_t(*)(void*,void*))(tick) )


void register_state_internal( game_t* game, gamestate_t state, size_t size, void (*init)( void*, void* ), void (*term)( void*, void* ), gamestate_t (*tick)( void*, void* ) ) {
    int index = (int) state;
    if( index <= 0 || index >= sizeof( game->states ) / sizeof( *game->states ) ) {
        printf( "Gamestate out of range\n" );
        return;
    }

    if( game->states[ index ].state != GAMESTATE_NONE ) {
        printf( "Gamestate already registered\n" );
        return;
    }

    if( (int) size > game->max_state_size ) {
        game->max_state_size = (int) size;
    }

    registered_state_t registered_state;
    registered_state.state = state;
    registered_state.init = init;
    registered_state.term = term;
    registered_state.tick = tick;
    game->states[ index ] = registered_state;
}


int sprite_compare( void const* a, void const* b ) {
    sprite_order_t* spr_a = ( sprite_order_t*) a;
    sprite_order_t* spr_b = ( sprite_order_t*) b;
    if( spr_a->zorder < spr_b->zorder ) {
        return -1;
    } else if( spr_a->zorder > spr_b->zorder ) {
        return 1;
    } else {
        return 0;
    }
}


void delete_sprite( game_t* game, void* sprite ) {
    for( int i = 0; i < game->sprites_count; ++i ) {
        if( &game->sprites[ i ].data == sprite ) {
            game->sprites[ i ].type = SPRITE_TYPE_DELETED;
            break;
        }
    }
}


sprite_t* sprite( game_t* game, float x, float y, bitmap_t bitmap ) {
    int index = -1;
    for( int i = 0; i < game->sprites_count; ++i ) {
        if( game->sprites[ i ].type == SPRITE_TYPE_DELETED ) {
            index = i;
            break;
        }
    }
    if( index < 0 ) {
        if( game->sprites_count >= sizeof( game->sprites ) / sizeof( *game->sprites ) ) {
            printf( "Too many sprites\n" );
            return NULL;
        }
        index = game->sprites_count++;
    }
    game->sprites[ index ].type = SPRITE_TYPE_SPRITE;
    sprite_t* sprite = &game->sprites[ index ].data.sprite;
    sprite->x = x;
    sprite->y = y;
    sprite->bitmap = bitmap;
    sprite->visible = true;
    sprite->zorder = 0.0f;
    sprite->origin_x = 0.0f;
    sprite->origin_y = 0.0f;
    sprite->user_value = 0.0f;
    return sprite;
}

point_t* point( game_t* game, float x, float y, uint8_t color ) {
    int index = -1;
    for( int i = 0; i < game->sprites_count; ++i ) {
        if( game->sprites[ i ].type == SPRITE_TYPE_DELETED ) {
            index = i;
            break;
        }
    }
    if( index < 0 ) {
        if( game->sprites_count >= sizeof( game->sprites ) / sizeof( *game->sprites ) ) {
            printf( "Too many sprites\n" );
            return NULL;
        }
        index = game->sprites_count++;
    }
    game->sprites[ index ].type = SPRITE_TYPE_POINT;
    point_t* point = &game->sprites[ index ].data.point;
    point->x = x;
    point->y = y;
    point->color = color;
    point->zorder = 0.0f;
    point->origin_x = 0.0f;
    point->origin_y = 0.0f;
    return point;
}



box_t* box( game_t* game, float x, float y, float w, float h, uint8_t color ) {
    int index = -1;
    for( int i = 0; i < game->sprites_count; ++i ) {
        if( game->sprites[ i ].type == SPRITE_TYPE_DELETED ) {
            index = i;
            break;
        }
    }
    if( index < 0 ) {
        if( game->sprites_count >= sizeof( game->sprites ) / sizeof( *game->sprites ) ) {
            printf( "Too many sprites\n" );
            return NULL;
        }
        index = game->sprites_count++;
    }
    game->sprites[ index ].type = SPRITE_TYPE_BOX;
    box_t* box = &game->sprites[ index ].data.box;
    box->x = x;
    box->y = y;
    box->w = w;
    box->h = h;
    box->color = color;
    box->zorder = 0.0f;
    box->origin_x = 0.0f;
    box->origin_y = 0.0f;
    return box;
}


label_t* label( game_t* game, float x, float y, char const* text, uint8_t color, font_t font ) {
    int index = -1;
    for( int i = 0; i < game->sprites_count; ++i ) {
        if( game->sprites[ i ].type == SPRITE_TYPE_DELETED ) {
            index = i;
            break;
        }
    }
    if( index < 0 ) {
        if( game->sprites_count >= sizeof( game->sprites ) / sizeof( *game->sprites ) ) {
            printf( "Too many sprites\n" );
            return NULL;
        }
        index = game->sprites_count++;
    }
    game->sprites[ index ].type = SPRITE_TYPE_LABEL;
    label_t* label = &game->sprites[ index ].data.label;
    label->x = x;
    label->y = y;
    if( strlen( text ) < sizeof( label->text ) ) {
        strcpy( label->text, text );
    }
    label->color = color;
    label->font = font;
    label->centered = false;
    label->right = false;
    label->visible = true;
    label->wrap = 0;
    label->vspacing = 0;
    label->limit = -1.0f;
    label->zorder = 0.0f;
    label->origin_x = 0.0f;
    label->origin_y = 0.0f;
    return label;
}


void gamestate_init( game_t* game, gamestate_t state ) {
    int index = (int) state;
    if( index <= 0 || index >= sizeof( game->states ) / sizeof( *game->states ) ) {
        printf( "Gamestate out of range\n" );
        return;
    }

    if( game->state_context == NULL ) {
        game->state_context = malloc( game->max_state_size );
    }

    memset( game->state_context, 0, game->max_state_size );

    if( game->states[ index ].init ) {
         game->states[ index ].init( game, game->state_context );
    }
}


void gamestate_term( game_t* game, gamestate_t state ) {
    int index = (int) state;
    if( index <= 0 || index >= sizeof( game->states ) / sizeof( *game->states ) ) {
        printf( "Gamestate out of range\n" );
        return;
    }

    if( game->states[ index ].term ) {
        game->states[ index ].term( game, game->state_context );
    }
}


gamestate_t gamestate_tick( game_t* game, gamestate_t state ) {
    int index = (int) state;
    if( index <= 0 || index >= sizeof( game->states ) / sizeof( *game->states ) ) {
        printf( "Gamestate out of range\n" );
        return game->state;
    }

    gamestate_t new_state = game->state;
    if( game->states[ index ].tick) {
        new_state = game->states[ index ].tick( game, game->state_context );
    }
    return new_state;
}


void gamestate_switch( game_t* game, gamestate_t state ) {
    if( game->state != GAMESTATE_NONE ) {
        gamestate_term( game, game->state );
    }
    game->state = state;
    game->next_state = GAMESTATE_NONE;
    game->sprites_count = 0;
    game->transition_time = 0.0f;
    gamestate_init( game, state );
}


void game_init( game_t* game, data_t* data, uint8_t* screen, int screen_width, int screen_height ) {
    memset( game, 0, sizeof( *game ) );
    game->data = data;
    game->screen = screen;
    game->screen_width = screen_width;
    game->screen_height = screen_height;

    rnd_pcg_seed( &game->pcg, 0 );

    game->state = GAMESTATE_NONE;
}


void game_term( game_t* game ) {
    gamestate_term( game, game->state );
    if( game->state_context ) {
        free( game->state_context );
    }
}


void game_tick( game_t* game, input_t* input ) {
    game->frame_counter++;
    game->input = *input;
    gamestate_t new_state = gamestate_tick( game, game->state );
    if( game->next_state == GAMESTATE_NONE ) {
        if( game->transition_time > 0.0f ) {
            game->next_state = new_state;
        } else {
            if( game->state != new_state ) {
                gamestate_switch( game, new_state );
            }
        }
    } else {
        game->transition_time -= 1.0f / 60.0f;
        if( game->transition_time <= 0.0f ) {
            gamestate_switch( game, game->next_state );
        }
    }

    int sorted_sprites_count = 0;
    for( int i = 0; i < game->sprites_count; ++i ) {
        game->sprite_order[ sorted_sprites_count ].index = i;
        sprite_data_t* spr = &game->sprites[ i ];
        switch( spr->type ) {
            case SPRITE_TYPE_DELETED: {
                continue;
            } break;

            case SPRITE_TYPE_SPRITE: {
                sprite_t* sprite = &spr->data.sprite;
                game->sprite_order[ sorted_sprites_count ].zorder = sprite->zorder;
            } break;

            case SPRITE_TYPE_POINT: {
                point_t* point = &spr->data.point;
                game->sprite_order[ sorted_sprites_count ].zorder = point->zorder;
            } break;

            case SPRITE_TYPE_BOX: {
                box_t* box = &spr->data.box;
                game->sprite_order[ sorted_sprites_count ].zorder = box->zorder;
            } break;

            case SPRITE_TYPE_LABEL: {
                label_t* label = &spr->data.label;
                game->sprite_order[ sorted_sprites_count ].zorder = label->zorder;
            } break;
        }
        sorted_sprites_count++;
    }

    qsort( game->sprite_order, sorted_sprites_count, sizeof( sprite_order_t ), sprite_compare );

    for( int i = 0; i < sorted_sprites_count; ++i ) {
        sprite_data_t* spr = &game->sprites[ game->sprite_order[ i ].index ];
        switch( spr->type ) {
            case SPRITE_TYPE_DELETED: {
                continue;
            } break;

            case SPRITE_TYPE_SPRITE: {
                sprite_t* sprite = &spr->data.sprite;
                if( sprite->visible ) {
                    int x = (int)( sprite->x - sprite->origin_x );
                    int y = (int)( sprite->y - sprite->origin_y );
                    if( sprite->bitmap ) {
                        bitmap_blit( game, sprite->bitmap, x, y );
                    }
                }
            } break;

            case SPRITE_TYPE_POINT: {
                point_t* point = &spr->data.point;
                int x = (int)( point->x - point->origin_x );
                int y = (int)( point->y - point->origin_y );
                if( x >= 0 && y >= 0 && x < game->screen_width && y < game->screen_height ) {
                    game->screen[ x + y * game->screen_width ] = point->color;
                }
            } break;

            case SPRITE_TYPE_BOX: {
                box_t* box = &spr->data.box;
                int x1 = (int)( box->x - box->origin_x );
                int y1 = (int)( box->y - box->origin_y );
                int x2 = x1 + (int)box->w - 1;
                int y2 = y1 + (int)box->h - 1;
                x1 = x1 < 0 ? 0 : x1;
                y1 = y1 < 0 ? 0 : y1;
                x2 = x2 >= game->screen_width ? game->screen_width - 1 : x2;
                y2 = y2 >= game->screen_height ? game->screen_height - 1 : y2;
                for( int y = y1; y <= y2; ++y ) {
                    memset( game->screen + x1 + y * game->screen_width, box->color, x2 - x1 + 1 );
                }
            } break;

            case SPRITE_TYPE_LABEL: {
                label_t* label = &spr->data.label;
                if( label->visible ) {
                    int x = (int)( label->x - label->origin_x );
                    int y = (int)( label->y - label->origin_y );
                    font_blit( game, label->font, x, y, label->text, label->color, label->centered, label->right, label->wrap, label->vspacing, (int)label->limit );
                }
            } break;
        }
    }
}


int rnd( game_t* game, int low, int high ) {
    return rnd_pcg_range( &game->pcg, low, high );
}


float rndf( game_t* game ) {
    return rnd_pcg_nextf( &game->pcg );
}


void shuffle( game_t* game, float* items, int count ) {
    for( int i = 0; i < count; ++i ) {
        int r = rnd( game, i, count - 1 );
        float t = items[ i ];
        items[ i ] = items[ r ];
        items[ r ] = t;
    }
}


void load_palette( game_t* game, char const* filename, uint32_t palette[ 256 ] ) {
    size_t size = 0;
    void* palfile = load_file( game->data, filename, &size );
    if( palfile && size == sizeof( uint32_t ) * 256 ) {
        memcpy( palette, palfile, sizeof( uint32_t ) * 256 );
    }
}


bitmap_t bitmap( game_t* game, char const* filename ) {
    size_t size = 0;
    void* file = load_file( game->data, filename, &size );
    if( !file ) return NULL;
    return (bitmap_t) file;
}


void bitmap_blit( game_t* game, bitmap_t bitmap, int x, int y ) {
    palrle_blit( bitmap, x, y, game->screen, game->screen_width, game->screen_height );
}


font_t font( game_t* game, char const* filename ) {
    size_t size = 0;
    void* file = load_file( game->data, filename, &size );
    if( !file ) return NULL;
    return (font_t) file;
}


void font_blit( game_t* game, font_t font, int x, int y, char const* text, uint8_t color, bool centered, bool right, int wrap_width, int vspacing, int limit ) {
    pixelfont_align_t align = centered ? PIXELFONT_ALIGN_CENTER : right ? PIXELFONT_ALIGN_RIGHT : PIXELFONT_ALIGN_LEFT;
    int hspacing = 0;
    pixelfont_bold_t bold = PIXELFONT_BOLD_OFF;
    pixelfont_italic_t italic = PIXELFONT_ITALIC_OFF;
    pixelfont_underline_t underline = PIXELFONT_UNDERLINE_OFF;
    pixelfont_blit( font, x, y, text, color, game->screen, game->screen_width, game->screen_height,
        align, wrap_width, hspacing, vspacing, limit, bold, italic, underline, NULL );
}


sound_t sound( game_t* game, const char* filename ) {
    sound_t snd;
    snd.sample_pairs = load_samples( game->data, filename, &snd.sample_pairs_count );
    return snd;
}


void play_music( game_t* game, sound_t music ) {
    game->music = music;
}


void queue_sound( game_t* game, sound_t sound, float volume, float delay ) {
    int index = -1;
    int age = game->frame_counter + 1;
    for( int i = 0; i < sizeof( game->sounds ) / sizeof( *game->sounds ); ++i ) {
        if( game->sounds[ i ].age < age ) {
            index = i;
            age = game->sounds[ i ].age;
        }
    }
    sound_channel_t* channel = &game->sounds[ index ];
    channel->age = game->frame_counter;
    channel->current_position = -(int)( 44100.0f * delay );
    channel->sample_pairs = sound.sample_pairs;
    channel->sample_pairs_count = sound.sample_pairs_count;
    channel->volume = volume;
}


void play_sound( game_t* game, sound_t sound, float volume ) {
    queue_sound( game, sound, volume, 0.0f );
}


