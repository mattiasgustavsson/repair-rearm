
typedef enum wave_pattern_t {
    STRAIGHT_LINE,
} wave_pattern_t;


typedef struct enemy_t {
    sprite_t* spr;
    int hits;
    int fire_count;
    int fire_rate;
    int fire_variance;
} enemy_t;


typedef struct enemy_bullet_t {
    sprite_t* spr;
    float dx;

} enemy_bullet_t;


typedef struct wave_t {
    wave_pattern_t pattern;
    int payoff;
    bool done;
    float dx;
    float dy;
    int type;
    enemy_t enemies[ 16 ];
    int enemies_count;
} wave_t;


typedef enum enemy_enum_t {
    ENEMY1,
    ENEMY2,
    ENEMY3,
    ENEMY4,
    ENEMY5,
    ENEMY6,
} enemy_enum_t;


typedef struct enemy_type_t {
    char const* filename;
    float radius;
    float origin_x;
    float origin_y;
    int explosion;
    float expl_origin_x;
    float expl_origin_y;
    int hits;
    int fire_rate;
    int fire_variance;
} enemy_type_t;


enemy_type_t enemy_types[] = {
    { "data/sprites/enemy1.png", 15.0f, 29 / 2, 21 / 2, 1, 23 / 2, 23 / 2, 1,  60, 40 },
    { "data/sprites/enemy2.png", 15.0f, 21 / 2, 23 / 2, 1, 23 / 2, 23 / 2, 1,  60, 40 },
    { "data/sprites/enemy3.png", 20.0f, 49 / 2, 27 / 2, 1, 23 / 2, 23 / 2, 2,  60, 40 },
    { "data/sprites/enemy4.png", 25.0f, 52 / 2, 41 / 2, 1, 23 / 2, 23 / 2, 4,  60, 40 },
    { "data/sprites/enemy5.png", 25.0f, 36 / 2, 47    , 2, 47 / 2, 47 / 2, 30, 40, 20 },
    { "data/sprites/enemy6.png", 25.0f, 59 / 2, 47    , 3, 47 / 2, 47 / 2, 45, 30, 25 },
};


typedef struct wave_info_t {
    int delay;
    int type;
    int count;
    float x;
    float y;
    float dx;
    float dy;
    float sx;
    float sy;
    int payoff;
} wave_info_t;


wave_info_t level_waves0[] = {
    {  30, ENEMY1, 4, -40, -40,  1.0f, 1.0f,  40, 20, 10 },
    { 200, ENEMY2, 5, 200,   0, -0.7f, 0.5f, -50, 10, 15 },
    { 500, ENEMY1, 4, -40, -40,  1.0f, 1.0f,  40, 20, 20 },
    {  30, ENEMY1, 5, 240, -40, -1.0f, 1.0f, -40, 20, 20 },
    { 200, ENEMY2, 5, 200,   0, -0.7f, 0.5f, -50, 10, 15 },
    { 230, ENEMY2, 5,-140,   0,  0.7f, 1.0f, -50, 10, 15 },
    { 400, ENEMY3, 8, 200,  80, -1.0f, 0.0f, -50,  0, 40 },
};


wave_info_t level_waves1[] = {
    {  30, ENEMY1, 4, -40, -40,  1.0f, 1.0f,  40, 20, 20 },
    {   0, ENEMY2, 5, 200,   0, -0.7f, 0.5f, -50, 10, 25 },
    { 300, ENEMY3, 4, -50, -50,  1.0f, 1.0f,  40, 20, 20 },
    { 400, ENEMY4, 3, -50, -50,  1.0f, 1.0f,  60, 60, 30 },
    { 400, ENEMY5, 1, 100, -50,  0.0f, 0.2f,   0,  0, 50 },
};


wave_info_t level_waves2[] = {
    {  30, ENEMY1, 4, -40, -40,  1.0f, 1.0f,  40, 20, 30 },
    {   0, ENEMY2, 5, 200,   0, -0.7f, 0.5f, -50, 10, 40 },
    { 300, ENEMY3, 4, -50, -50,  1.0f, 1.0f,  40, 20, 20 },
    { 400, ENEMY4, 3, -50, -50,  1.0f, 1.0f,  60, 60, 15 },
    {  30, ENEMY1, 5, 240, -40, -1.0f, 1.0f, -40, 20, 30 },
    { 200, ENEMY2, 5, 200,   0, -0.7f, 0.5f, -50, 10, 50 },
    { 600, ENEMY6, 1, 100, -60,  0.0f, 0.1f,   0,  0, 150 },
};


typedef struct explosion_t {
    const char** animation;
    float cel;
    sprite_t* spr;
} explosion_t;

typedef struct state_level_t {
    point_t* stars[ 256 ];
    sprite_t* player;
    wave_t enemy_waves[ 32 ];
    int enemy_waves_count;
    explosion_t explosions[ 64 ];
    int explosions_count;
    sprite_t* player_bullets[ 256 ];
    int player_bullets_count;
    sprite_t* dollars[ 256 ];
    int dollars_count;
    enemy_bullet_t enemy_bullets[ 256 ];
    int enemy_bullets_count;
    int current_level_wave;
    int current_level_countdown;
    int player_hits;
    int player_ammo;
    label_t* gameover;
    bool exiting;
    label_t* moneys;
    box_t* ammo;
    box_t* damage;
    int dollarsound_cooldown;
    int level_waves_count;
    wave_info_t* level_waves;
} state_level_t;


char const* explosions[4][9] = {
    {
    "data/sprites/expl1_001.png",
    "data/sprites/expl1_002.png",
    "data/sprites/expl1_003.png",
    "data/sprites/expl1_004.png",
    "data/sprites/expl1_005.png",
    "data/sprites/expl1_006.png",
    "data/sprites/expl1_007.png",
    NULL
    },

    {
    "data/sprites/expl2_001.png",
    "data/sprites/expl2_002.png",
    "data/sprites/expl2_003.png",
    "data/sprites/expl2_004.png",
    "data/sprites/expl2_005.png",
    "data/sprites/expl2_006.png",
    "data/sprites/expl2_007.png",
    "data/sprites/expl2_008.png",
    NULL
    },

    {
    "data/sprites/expl3_001.png",
    "data/sprites/expl3_002.png",
    "data/sprites/expl3_003.png",
    "data/sprites/expl3_004.png",
    "data/sprites/expl3_005.png",
    "data/sprites/expl3_006.png",
    "data/sprites/expl3_007.png",
    NULL
    },

    {
    "data/sprites/expl4_001.png",
    "data/sprites/expl4_002.png",
    "data/sprites/expl4_003.png",
    "data/sprites/expl4_004.png",
    "data/sprites/expl4_005.png",
    "data/sprites/expl4_006.png",
    "data/sprites/expl4_007.png",
    NULL
    }
};


void state_level_init( game_t* game, state_level_t* state ) {
    state->dollarsound_cooldown = 0;
    state->exiting = false;

    if( game->stage == 0 ) {
        state->level_waves_count = sizeof( level_waves0 ) / sizeof( *level_waves0 );
        state->level_waves = level_waves0;
    } else if( game->stage == 1 ) {
        state->level_waves_count = sizeof( level_waves1 ) / sizeof( *level_waves1 );
        state->level_waves = level_waves1;
    } else if( game->stage == 2 ) {
        state->level_waves_count = sizeof( level_waves2 ) / sizeof( *level_waves2 );
        state->level_waves = level_waves2;
    }

    state->moneys = label( game, 0.0f, 5.0f, "", 15, font( game, "data/prstartk.ttf" ) );
    state->moneys->centered = true;
    state->moneys->wrap = game->screen_width;
    state->moneys->zorder = 300.0f;

    state->gameover = label( game, 0.0f, 130.0f, "GAME OVER", 15, font( game, "data/prstartk.ttf" ) );
    state->gameover->centered = true;
    state->gameover->wrap = game->screen_width;
    state->gameover->zorder = 300.0f;
    state->gameover->visible = false;

    state->player_hits = 25;
    state->player_ammo = 150;
    state->current_level_wave = 0;
    state->current_level_countdown = state->level_waves[ 0 ].delay;

    play_music( game, sound( game, "data/ingame.ogg" ) );

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

    sprite( game, 0, 4, bitmap( game, "data/ammo.png" ) )->zorder = 300.0f;
    box( game, 14, 6, 42, 6, 15 )->zorder = 300.0f;
    box( game, 15, 7, 40, 4, 0 )->zorder = 305.0f;
    state->ammo = box( game, 15, 7, 40, 4, 9 );
    state->ammo->zorder = 310.0f;

    sprite( game, 145, 5, bitmap( game, "data/damage.png" ) )->zorder = 300.0f;
    box( game, 157, 6, 42, 6, 15 )->zorder = 300.0f;
    box( game, 158, 7, 40, 4, 0 )->zorder = 305.0f;
    state->damage = box( game, 158, 7, 0, 4, 4 );
    state->damage->zorder = 310.0f;
}


void spawn_wave( game_t* game, state_level_t* state, int type, int count, float x, float y, float dx, float dy, float sx, float sy, int payoff ) {
    if( state->enemy_waves_count < sizeof( state->enemy_waves ) / sizeof( *state->enemy_waves ) ) {
        wave_t w;
        w.pattern = STRAIGHT_LINE;
        w.payoff = payoff;
        w.done = false;
        w.dx = dx;
        w.dy = dy;
        w.enemies_count = 0;
        w.type = type;
        x -= sx * count;
        y -= sy * count;
        for( int i = 0 ; i < count; ++i ) {
            enemy_t e;
            e.spr = sprite( game, x, y, bitmap( game, enemy_types[ type ].filename ) );
            e.spr->origin_x = enemy_types[ type ].origin_x;
            e.spr->origin_y = enemy_types[ type ].origin_y;
            e.hits = enemy_types[ type ].hits;
            e.fire_rate = enemy_types[ type ].fire_rate;
            e.fire_variance = enemy_types[ type ].fire_variance;
            e.fire_count = rnd( game, e.fire_rate - e.fire_variance, e.fire_rate + e.fire_variance );
            x += sx;
            y += sy;
            w.enemies[ w.enemies_count++ ] = e;
        }
        state->enemy_waves[ state->enemy_waves_count++ ] = w;;
    }
}


void update_wave( game_t* game, state_level_t* state, wave_t* w ) {
    bool all_outside = true;
    for( int i = 0; i < w->enemies_count; ++i ) {
        enemy_t* e = &w->enemies[ i ];
        float x = e->spr->x;
        float y = e->spr->y;
        x += w->dx;
        y += w->dy;
        e->spr->x = x;
        e->spr->y = y;
        if( x >= 0 && y >= 0 && x < game->screen_width && y < game->screen_height && --e->fire_count <= 0 && state->player->visible ) {
            if( state->enemy_bullets_count < sizeof( state->enemy_bullets ) / sizeof( *state->enemy_bullets ) ) {
                play_sound( game, sound( game, "data/sound/ship_laser_2.ogg" ), 1.0f );
                enemy_bullet_t bullet;
                bullet.spr = sprite( game, x, y + e->spr->origin_y, bitmap( game, "data/sprites/enemy_bullet.png" ) );
                bullet.spr->origin_x = 5.0f;
                bullet.spr->origin_y = 4.0f;
                bullet.spr->zorder = -1.0f;
                bullet.dx = w->dx;
                state->enemy_bullets[ state->enemy_bullets_count++ ] = bullet;
                e->fire_count = rnd( game, e->fire_rate - e->fire_variance, e->fire_rate + e->fire_variance );
            }
        }
        if( x > -100 && x < game->screen_width + 100 && y > -100 && y < game->screen_height + 100 ) {
            all_outside = false;
        }
    }
    if( all_outside || w->enemies_count == 0 ) {
        w->done = true;
    }
}

void spawn_dollars( game_t* game, state_level_t* state, float x, float y, int amount ) {
    for( int i = 0; i < amount; ++i ) {
        if( state->dollars_count < sizeof( state->dollars ) / sizeof( *state->dollars ) ) {
            float xp = (float)rnd( game, (int)x - 10, (int)x + 10 );
            float yp = (float)rnd( game, (int)y - 10, (int)y + 10 );
            float dx = rndf( game ) * 2.0f - 1.0f;
            float dy = rndf( game ) * 2.0f - 1.0f;
            sprite_t* spr = sprite( game, xp, yp, bitmap( game, "data/sprites/dollar.png" ) );
            spr->origin_x = 6 + dx;
            spr->origin_y = 7 + dy;
            spr->user_value = 180.0f;
            state->dollars[ state->dollars_count++ ] = spr;
        }
    }
}


void update_dollars( game_t* game, state_level_t* state ) {
    for( int i = 0; i < state->dollars_count; i++ ) {
        sprite_t* spr = state->dollars[ i ];
        float x = spr->x;
        float y = spr->y;
        x += ( spr->origin_x - 6 ) * 0.3f;
        y += ( spr->origin_y - 7 ) * 0.3f;
        spr->x = x;
        spr->y = y;
        float life = spr->user_value;
        life -= 1;
        if( life < 40 ) {
            spr->visible = ( ((int)life) % 10 < 5 );
        }
        if( life <= 0.0f ) {
            delete_sprite( game, state->dollars[ i ] );
            state->dollars[ i-- ] = state->dollars[ --state->dollars_count ];
        } else {
            spr->user_value = life;
            float dx = x - state->player->x;
            float dy = y - state->player->y;
            if( dx * dx + dy * dy < 20.0f * 20.0f ) {
                delete_sprite( game, state->dollars[ i ] );
                state->dollars[ i-- ] = state->dollars[ --state->dollars_count ];
                game->money = game->money + 100;
                if( state->dollarsound_cooldown <= 0 ) {
                    play_sound( game, sound( game, "data/sound/pickup_powerup.ogg" ), 1.0f );
                    state->dollarsound_cooldown = 10;
                }
            }
        }
    }
}


gamestate_t state_level_tick( game_t* game, state_level_t* state ) {
    if( state->dollarsound_cooldown > 0 ) {
        --state->dollarsound_cooldown;
    }

    char str[ 16 ];
    sprintf( str, "$%d", game->money );
    if( strlen( str ) < sizeof( state->moneys->text ) ) {
        strcpy( state->moneys->text, str );
    }

    int ammo_width = ( ( 40 * state->player_ammo ) / 150 );
    state->ammo->w = (float)ammo_width;

    int damage_width = ( ( 40 * ( 25 - state->player_hits ) ) / 25 );
    state->damage->w = (float)damage_width;

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

    if( game->input.fire && state->player->visible && state->player_ammo > 0 ) {
        if( state->player_bullets_count < sizeof( state->player_bullets ) / sizeof( *state->player_bullets ) ) {
            play_sound( game, sound( game, "data/sound/ship_laser_1.ogg" ), 1.0f );
            sprite_t* bullet = sprite( game, x, y - state->player->origin_y, bitmap( game, "data/sprites/bullet.png" ) );
            bullet->origin_x = 5.0f;
            bullet->origin_y = 7.0f;
            state->player_bullets[ state->player_bullets_count++ ] = bullet;
            --state->player_ammo;
        }
    }

    update_dollars( game, state );

    for( int i = 0; i < state->player_bullets_count; ++i ) {
        float bx = state->player_bullets[ i ]->x;
        float by = state->player_bullets[ i ]->y;
        by -= 5.0f;
        if( by < -7.0f ) {
            delete_sprite( game, state->player_bullets[ i ] );
            state->player_bullets[ i-- ] = state->player_bullets[ --state->player_bullets_count ];
            continue;
        } else {
            state->player_bullets[ i ]->x = bx;
            state->player_bullets[ i ]->y = by;
        }

        for( int j = 0; j < state->enemy_waves_count; ++j ) {
            wave_t* w = &state->enemy_waves[ j ];
            for( int k = 0; k < w->enemies_count; ++k ) {
                enemy_t* e = &w->enemies[ k ];
                float ex = e->spr->x;
                float ey = e->spr->y;
                float dx = ex - bx;
                float dy = ey - by;
                float l = dx * dx + dy * dy;
                float r = enemy_types[ w->type ].radius;
                if( l < r * r ) {
                    delete_sprite( game, state->player_bullets[ i ] );
                    state->player_bullets[ i-- ] = state->player_bullets[ --state->player_bullets_count ];
                    --e->hits;
                    if( e->hits <= 0 ) {
                        delete_sprite( game, w->enemies[ k ].spr );
                        w->enemies[ k ] = w->enemies[ --w->enemies_count ];
                        if( w->enemies_count == 0 ) {
                            spawn_dollars( game, state, bx, by, w->payoff );
                        }

                        if( state->explosions_count < sizeof( state->explosions ) / sizeof( *state->explosions ) ) {
                            explosion_t* exp = &state->explosions[ state->explosions_count++ ];
                            exp->animation = explosions[ enemy_types[ w->type ].explosion ];
                            exp->spr = sprite( game, ex, ey, bitmap( game, exp->animation[ 0 ] ) );
                            exp->spr->origin_x = enemy_types[ w->type ].expl_origin_x;
                            exp->spr->origin_y = enemy_types[ w->type ].expl_origin_y;
                            exp->spr->zorder = 3.0f;
                            exp->cel = 0.0f;
                        }
                        play_sound( game, sound( game, "data/sound/ship_explosion.ogg" ), 2.0f );
                    } else {
                        if( state->explosions_count < sizeof( state->explosions ) / sizeof( *state->explosions ) ) {
                            explosion_t* exp = &state->explosions[ state->explosions_count++ ];
                            exp->animation = explosions[ 0 ];
                            exp->spr = sprite( game, bx, by, bitmap( game, exp->animation[ 0 ] ) );
                            exp->spr->origin_x = 7.0f;
                            exp->spr->origin_y = 7.0f;
                            exp->spr->zorder = 3.0f;
                            exp->cel = 0.0f;
                        }
                        play_sound( game, sound( game, "data/sound/ship_damaged.ogg" ), 1.0f );

                    }
                    goto next_bullet;
                }
            }
        }

    next_bullet:
        ;
    }

    for( int i = 0; i < state->enemy_bullets_count; ++i ) {
        float bx = state->enemy_bullets[ i ].spr->x;
        float by = state->enemy_bullets[ i ].spr->y;
        bx += state->enemy_bullets[ i ].dx;
        by += 2.0f;
        if( by - 4.0f > game->screen_height ) {
            delete_sprite( game, state->enemy_bullets[ i ].spr );
            state->enemy_bullets[ i-- ] = state->enemy_bullets[ --state->enemy_bullets_count ];
            continue;
        } else {
            state->enemy_bullets[ i ].spr->x = bx;
            state->enemy_bullets[ i ].spr->y = by;
        }
        float px = state->player->x;
        float py = state->player->y;
        float dx = px - bx;
        float dy = py - by;
        float l = dx * dx + dy * dy;
        float r = 20.0f;
        if( state->player->visible && l < r * r ) {
            delete_sprite( game, state->enemy_bullets[ i ].spr );
            state->enemy_bullets[ i-- ] = state->enemy_bullets[ --state->enemy_bullets_count ];
            --state->player_hits;
            if( state->player_hits <= 0 ) {
                state->player->visible = false;
                if( state->explosions_count < sizeof( state->explosions ) / sizeof( *state->explosions ) ) {
                    explosion_t* exp = &state->explosions[ state->explosions_count++ ];
                    exp->animation = explosions[ 3 ];
                    exp->spr = sprite( game, bx, by, bitmap( game, exp->animation[ 0 ] ) );
                    exp->spr->origin_x = 47.0f / 2.0f;
                    exp->spr->origin_y = 47.0f / 2.0f;
                    exp->spr->zorder = 3.0f;
                    exp->cel = 0.0f;
                }
                play_sound( game, sound( game, "data/sound/ship_explosion.ogg" ), 3.0f );
                queue_sound( game, sound( game, "data/sound/ship_explosion.ogg" ), 2.5f, 0.25f );
                queue_sound( game, sound( game, "data/sound/ship_explosion.ogg" ), 2.0f, 0.5f );
                state->gameover->visible = true;
                game->transition_time = 3.0f;
                return GAMESTATE_TITLE;
            } else {
                if( state->explosions_count < sizeof( state->explosions ) / sizeof( *state->explosions ) ) {
                    explosion_t* exp = &state->explosions[ state->explosions_count++ ];
                    exp->animation = explosions[ 0 ];
                    exp->spr = sprite( game, bx, by, bitmap( game, exp->animation[ 0 ] ) );
                    exp->spr->origin_x = 7.0f;
                    exp->spr->origin_y = 7.0f;
                    exp->spr->zorder = 3.0f;
                    exp->cel = 0.0f;
                }
                play_sound( game, sound( game, "data/sound/ship_damaged.ogg"), 1.0f );
            }
        }
    }

    bool all_done = true;
    for( int i = 0; i < state->enemy_waves_count; ++i ) {
        update_wave( game, state, &state->enemy_waves[ i] );
        if( !state->enemy_waves[ i ].done ) {
            all_done = false;
        }
    }

    if( !state->exiting && all_done && state->current_level_wave >= state->level_waves_count && state->dollars_count == 0 ) {
        state->exiting = true;
        game->stage++;
        game->transition_time = 3.0f;
        return GAMESTATE_INTRO;
    }

    for( int i = 0; i <  state->explosions_count; ++i ) {
        explosion_t* exp = &state->explosions[ i ];
        float cel = exp->cel;
        cel += 0.2f;
        if( (int) cel > (int) exp->cel ) {
            char const* bmp = exp->animation[ (int)( exp->cel + 1 ) ];
            if( !bmp ) {
                delete_sprite( game, exp->spr );
                state->explosions[ i-- ] = state->explosions[ --state->explosions_count ];
                continue;
            } else {
                exp->spr->bitmap = bitmap( game, bmp );
            }
        }
        exp->cel = cel;
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

    state->current_level_countdown--;
    if( state->current_level_countdown <= 0 ) {
        state->current_level_countdown = 0;
        if( state->current_level_wave < state->level_waves_count ) {
            wave_info_t* w = &state->level_waves[ state->current_level_wave ];
            spawn_wave( game, state, w->type, w->count, w->x, w->y, w->dx, w->dy, w->sx, w->sy, w->payoff );
            state->current_level_wave++;
            if( state->current_level_wave < state->level_waves_count ) {
                state->current_level_countdown = state->level_waves[ state->current_level_wave ].delay;
            }
        }
    }

    return GAMESTATE_LEVEL;
}


void state_level_register( game_t* game ) {
    register_state( game, GAMESTATE_LEVEL, sizeof( state_level_t ), state_level_init, NULL, state_level_tick );
}
