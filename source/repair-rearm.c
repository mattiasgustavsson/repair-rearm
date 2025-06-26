#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

// Need to do this before anything else is included, to get proper filenames in memory leak reporting
#if defined( _WIN32 ) && defined( _DEBUG )
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

// C standard lib includes
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Library includes
#include "libs/app.h"
#include "libs/array.h"
#include "libs/buffer.h"
#include "libs/crtemu.h"
#include "libs/frametimer.h"
#include "libs/img.h"
#include "libs/paldither.h"
#include "libs/palrle.h"
#include "libs/pixelfont.h"
#include "libs/rnd.h"
#include "libs/stb_image.h"
#include "libs/stb_truetype.h"
#include "libs/stb_vorbis.h"
#ifndef __wasm__
    #include "libs/thread.h"
#endif

typedef struct sound_channel_t {
    int age;
    short* sample_pairs;
    int sample_pairs_count;
    int current_position;
    float volume;
} sound_channel_t;


// Game includes
#include "data.h"
#include "game.h"
#include "state_launch.h"
#include "state_title.h"
#include "state_credits.h"
#include "state_intro.h"
#include "state_level.h"
#include "state_repair.h"


typedef struct sound_context_t {
    #ifndef __wasm__
        thread_mutex_t mutex;
    #endif
    short* music_sample_pairs;
    int music_sample_pairs_count;
    int music_current_position;
    sound_channel_t sounds[ 16 ];
} sound_context_t;


void sound_callback( APP_S16* sample_pairs, int sample_pairs_count, void* user_data ) {
    sound_context_t* context = (sound_context_t*) user_data;

    #ifndef __wasm__
        thread_mutex_lock( &context->mutex );
    #endif

    for( int i = 0; i < sample_pairs_count; ++i ) {
        int left = 0;
        int right = 0;
        if( context->music_sample_pairs ) {
            left = context->music_sample_pairs[ context->music_current_position * 2 + 0 ];
            right = context->music_sample_pairs[ context->music_current_position * 2 + 1 ];
            context->music_current_position++;
            if( context->music_current_position >= context->music_sample_pairs_count ) {
                context->music_current_position = 0;
            }
        }
        for( int j = 0; j < sizeof( context->sounds ) / sizeof( *context->sounds ); ++j ) {
            sound_channel_t* sound = &context->sounds[ j ];
            if( sound->current_position < sound->sample_pairs_count ) {
                if( sound->current_position >= 0 ) {
                    int sl = sound->sample_pairs[ sound->current_position * 2 + 0 ];
                    int sr = sound->sample_pairs[ sound->current_position * 2 + 1 ];
                    sound->current_position++;
                    left += (int)( sl * sound->volume );
                    right += (int)( sr * sound->volume );
                } else {
                    sound->current_position++;
                }
            }
        }
        left = left > 32767 ? 32767 : left < -32767 ? -32767 : left;
        right = right > 32767 ? 32767 : right < -32767 ? -32767 : right;
        sample_pairs[ i * 2 + 0 ] = (short)left;
        sample_pairs[ i * 2 + 1 ] = (short)right;
    }

    #ifndef __wasm__
        thread_mutex_unlock( &context->mutex );
    #endif
}


float horiz( app_gamepad_t* state, bool left, bool right ) {
    float thresh = 0.3f;
    float x;
        
    x = state->stick_left_x / 32767.0f;
    if( (float) fabs( (double) x ) > thresh ) {
        return x;
    }

    x = state->stick_right_x / 32767.0f;
    if( (float) fabs( (double) x ) > thresh ) {
        return x;
    }

    if( state->buttons & APP_GAMEPAD_BUTTON_DPAD_LEFT ) {
        return -1.0f;
    }

    if( state->buttons & APP_GAMEPAD_BUTTON_DPAD_RIGHT ) {
        return 1.0f;
    }

    if( left ) {
        return -1.0f;
    }

    if( right ) {
        return 1.0f;
    }

    return 0.0;
} 


float vert( app_gamepad_t* state, bool up, bool down ) {
    float thresh = 0.3f;
    float x;
        
    x = state->stick_left_y / 32767.0f;
    if( (float) fabs( (double) x ) > thresh ) {
        return -x;
    }

    x = state->stick_right_y / 32767.0f;
    if( (float) fabs( (double) x ) > thresh ) {
        return -x;
    }

    if( state->buttons & APP_GAMEPAD_BUTTON_DPAD_UP ) {
        return -1.0f;
    }

    if( state->buttons & APP_GAMEPAD_BUTTON_DPAD_DOWN ) {
        return 1.0f;
    }

    if( up ) {
        return -1.0f;
    }

    if( down ) {
        return 1.0f;
    }

    return 0.0;
}


// main game loop and setup
int app_proc( app_t* app, void* user_data ) {
    (void) user_data;

    // position window centered on main display
    app_displays_t displays = app_displays( app );
    if( displays.count > 0 ) {
        // find main display
        int disp = 0;
        for( int i = 0; i < displays.count; ++i ) {
            if( displays.displays[ i ].x == 0 && displays.displays[ i ].y == 0 ) {
                disp = i;
                break;
            }
        }
        // calculate aspect locked width/height
        int scrwidth = displays.displays[ disp ].width - 80;
        int scrheight = displays.displays[ disp ].height - 80;
        int aspect_width = (int)( ( scrheight * 3 ) / 4 );
        int aspect_height = (int)( ( scrwidth * 4 ) / 3 );
        int target_width, target_height;
        if( aspect_height <= scrheight ) {
            target_width = scrwidth;
            target_height = aspect_height;
        } else {
            target_width = aspect_width;
            target_height = scrheight;
        }
        // set window size and position
        int x = displays.displays[ disp ].x + ( displays.displays[ disp ].width - target_width ) / 2;
        int y = displays.displays[ disp ].y + ( displays.displays[ disp ].height - target_height ) / 2;
        int w = target_width;
        int h = target_height;
        app_window_pos( app, x, y );
        app_window_size( app, w, h );
    }

    #ifndef __wasm__
        bool fullscreen = true;
    #else
        bool fullscreen = false;
    #endif
    app_interpolation( app, APP_INTERPOLATION_NONE );
    app_screenmode( app, fullscreen ? APP_SCREENMODE_FULLSCREEN : APP_SCREENMODE_WINDOW );
    app_title( app, "Repair/Rearm" );

    // No mouse cursor
    app_pointer( app, APP_POINTER_HIDE );

    data_t data;
    if( !load_data( &data, "repair-rearm.dat" ) ) {
        printf( "Failed to load data file 'repair-rearm.dat'\n" );
        return EXIT_FAILURE;
    }

    uint8_t* screen = (uint8_t*) malloc( 200 * 320 * sizeof( uint8_t ) );
    memset( screen, 0, 200 * 320 * sizeof( uint8_t ) );

    uint32_t* screen_xbgr = (uint32_t*) malloc( 228  * 348 * sizeof( uint32_t ) );
    memset( screen_xbgr, 0, 228 * 348 * sizeof( uint32_t ) );

    game_t* game = (game_t*) malloc( sizeof( game_t ) );
    game_init( game, &data, screen, 200, 320 );

    state_launch_register( game );
    state_title_register( game );
    state_credits_register( game );
    state_intro_register( game );
    state_level_register( game );
    state_repair_register( game );

    #ifdef __wasm__
        gamestate_switch( game, GAMESTATE_LAUNCH );
    #else
        gamestate_switch( game, GAMESTATE_TITLE );
    #endif

    frametimer_t* frametimer = frametimer_create( NULL );
    frametimer_lock_rate( frametimer, 60 );

    crtemu_t* crtemu = NULL;
    crtemu = crtemu_create( NULL );
    size_t frame_size = 0;
    void* frame_data = load_file( &data, "data/crtframe_tv.png", &frame_size );
    if( frame_data ) {
        int w, h, c;
        CRTEMU_U32* pixels = (CRTEMU_U32*) stbi_load_from_memory( (stbi_uc*) frame_data, (int) frame_size, &w, &h, &c, 4 );
        if( pixels ) {
            crtemu_frame( crtemu, pixels, w, h );
            stbi_image_free( (stbi_uc*) pixels );
        }
    }
  
    uint32_t palette[ 256 ];
    load_palette( game, "data/pal.png", palette );

    input_t input = { false };
    bool key_left = false;
    bool key_right = false;
    bool key_up = false;
    bool key_down = false;
    bool key_enter = false;
    bool key_space = false;
    bool key_control = false;
    bool action = false;
    bool prev_action = false;
    bool fire = false;
    bool prev_fire = false;

    sound_context_t sound_context;
    memset( &sound_context, 0, sizeof( sound_context ) );
    #ifndef __wasm__
        thread_mutex_init( &sound_context.mutex );
    #endif
    app_sound( app, 735 * 5, sound_callback,  &sound_context );

    // main loop
    bool exit_flag = false;
    APP_U64 time = 0;
    while( app_yield( app ) != APP_STATE_EXIT_REQUESTED && !exit_flag ) {
        frametimer_update( frametimer );
        app_input_t appinp = app_input( app );
        input.click = false;
        for( int i = 0; i < appinp.count; ++i ) {
            app_input_event_t* event = &appinp.events[ i ];
            if( event->type == APP_INPUT_KEY_DOWN ) {
                if( event->data.key == APP_KEY_LBUTTON ) input.click = true;
                if( event->data.key == APP_KEY_LEFT ) key_left = true;
                if( event->data.key == APP_KEY_RIGHT ) key_right = true;
                if( event->data.key == APP_KEY_UP ) key_up = true;
                if( event->data.key == APP_KEY_DOWN ) key_down = true;
                if( event->data.key == APP_KEY_RETURN ) key_enter = true;
                if( event->data.key == APP_KEY_SPACE ) key_space = true;
                if( event->data.key == APP_KEY_CONTROL ) key_control = true;
                if( event->data.key == APP_KEY_F11 ) {
                    fullscreen = !fullscreen;
                    app_screenmode( app, fullscreen ? APP_SCREENMODE_FULLSCREEN : APP_SCREENMODE_WINDOW );
                }
            } else if( event->type == APP_INPUT_KEY_UP ) {
                if( event->data.key == APP_KEY_LEFT ) key_left = false;
                if( event->data.key == APP_KEY_RIGHT ) key_right = false;
                if( event->data.key == APP_KEY_UP ) key_up = false;
                if( event->data.key == APP_KEY_DOWN ) key_down = false;
                if( event->data.key == APP_KEY_RETURN ) key_enter = false;
                if( event->data.key == APP_KEY_SPACE ) key_space = false;
                if( event->data.key == APP_KEY_CONTROL ) key_control = false;
            }
        }

        app_gamepad_t padstate = { 0 };
        app_gamepad( app, 0, &padstate, NULL );        

        fire = false;
        if( key_control || ( padstate.buttons & APP_GAMEPAD_BUTTON_A ) != 0 ) {
            fire = true;
        }

        action = false;
        if( key_enter || key_space || ( padstate.buttons & APP_GAMEPAD_BUTTON_B ) != 0 || ( padstate.buttons & APP_GAMEPAD_BUTTON_START ) != 0 ) {
            action = true;
        }

        input.fire = fire && !prev_fire;
        input.action = action && !prev_action;
        input.horiz = horiz( &padstate, key_left, key_right );
        input.vert = vert( &padstate, key_up, key_down );     

        prev_action = action;
        prev_fire = fire;


        game_tick( game, &input );

        #ifndef __wasm__
            thread_mutex_lock( &sound_context.mutex );
        #endif
        if( sound_context.music_sample_pairs != game->music.sample_pairs ) {
            sound_context.music_sample_pairs = game->music.sample_pairs;
            sound_context.music_sample_pairs_count = game->music.sample_pairs_count;
            sound_context.music_current_position = 0;
        }
        for( int i = 0; i < sizeof( sound_context.sounds ) / sizeof( *sound_context.sounds ); ++i ) {
            if( game->sounds[ i ].age > sound_context.sounds[ i ].age ) {
                sound_context.sounds[ i ] = game->sounds[ i ];
            }
        }
        #ifndef __wasm__
            thread_mutex_unlock( &sound_context.mutex );
        #endif

        int hborder = 28;
        int vborder = 28;
        for( int y = 0; y < 320; ++y ) {
            for( int x = 0; x < 200; ++x ) {
                screen_xbgr[ ( x + hborder / 2 ) + ( ( y + vborder / 2 ) * ( 200 + hborder ) ) ] = palette[ screen[ x + y * 200 ] ];
            }
        }

        time += 1000000 / 60;
        crtemu_present( crtemu, time, screen_xbgr, 200 + hborder, 320 + vborder, 0xffffffff, 0x00000000 );
        app_present( app, NULL, 1, 1, 0xffffff, 0x000000 );
    }

    app_sound( app, 0, NULL, NULL );
    #ifndef __wasm__
        thread_mutex_term( &sound_context.mutex );
        #endif
    game_term( game );
    free( game );
    free( screen_xbgr );
    free( screen );
    free_data( &data );
    frametimer_destroy( frametimer );
    crtemu_destroy( crtemu );
    return EXIT_SUCCESS;
}


#ifndef __wasm__
    #include "build_data.h"

    void threads_init( void );
#endif


int main( int argc, char** argv ) {
    (void) argc, (void ) argv;

    // Enable windows memory leak detection (will report leaks in the Output window)
    #if defined( _WIN32 ) && defined( _DEBUG )
        int flag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG ); // Get current flag
        flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
        _CrtSetDbgFlag( flag ); // Set flag to the new value
        //_CrtSetBreakAlloc( 0 ); // Can be manually commented back in to break at a certain allocation
    #endif

    #ifndef __wasm__
        threads_init();
    #endif

    #ifndef __wasm__
        if( argc > 1 && ( strcmp( argv[ 1 ], "-b" ) == 0 || strcmp( argv[ 1 ], "--build" ) == 0 ) ) {
            if( !build_data( "repair-rearm.dat" ) ) {
                printf( "Failed to build data\n" );
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        }
    #endif

    return app_run( app_proc, NULL, NULL, NULL );
}


// pass-through so the program will build with either /SUBSYSTEM:WINDOWS or /SUBSYSTEM:CONSOLE
#if defined( _WIN32 )
    #ifdef __cplusplus
        extern "C" int __stdcall WinMain( struct HINSTANCE__*, struct HINSTANCE__*, char*, int ) {
            return main( __argc, __argv );
        }
    #else
        struct HINSTANCE__;
        int __stdcall WinMain( struct HINSTANCE__* a, struct HINSTANCE__* b, char* c, int d ) {
            (void) a, b, c, d;
            return main( __argc, __argv );
        }
    #endif
#endif


#define APP_IMPLEMENTATION
#ifdef _WIN32
    #define APP_WINDOWS
#elif __wasm__
    #define APP_WASM
#else
    #define APP_SDL
#endif
#include "libs/app.h"

#define ARRAY_IMPLEMENTATION
#include "libs/array.h"

#define BUFFER_IMPLEMENTATION
#include "libs/buffer.h"

#define CRTEMU_IMPLEMENTATION
#include "libs/crtemu.h"

#define FRAMETIMER_IMPLEMENTATION
#include "libs/frametimer.h"

#define IMG_IMPLEMENTATION
#include "libs/img.h"

#define PALDITHER_IMPLEMENTATION
#include "libs/paldither.h"

#define PALRLE_IMPLEMENTATION
#include "libs/palrle.h"

#define PIXELFONT_IMPLEMENTATION
#define PIXELFONT_BUILDER_IMPLEMENTATION
#include "libs/pixelfont.h"

#define RND_IMPLEMENTATION
#include "libs/rnd.h"

#pragma warning( push )
#pragma warning( disable: 4255 )
#pragma warning( disable: 4668 )
#define STB_IMAGE_IMPLEMENTATION
#if defined( _WIN32 ) && ( defined( __clang__ ) || defined( __TINYC__ ) )
    #define STBI_NO_SIMD
#endif
#include "libs/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION
#pragma warning( pop )

#define STB_TRUETYPE_IMPLEMENTATION
#include "libs/stb_truetype.h"

#define STB_VORBIS_IMPLEMENTATION
#pragma warning( push )
#pragma warning( disable: 4242 )
#pragma warning( disable: 4244 )
#pragma warning( disable: 4245 )
#pragma warning( disable: 4456 )
#pragma warning( disable: 4457 )
#pragma warning( disable: 4701 )
#include "libs/stb_vorbis.h"
#pragma warning( pop )

#ifndef __wasm__
    #if defined( __TINYC__ )
        typedef struct _RTL_CONDITION_VARIABLE { PVOID Ptr; } RTL_CONDITION_VARIABLE, *PRTL_CONDITION_VARIABLE;
        typedef RTL_CONDITION_VARIABLE CONDITION_VARIABLE, *PCONDITION_VARIABLE;
        static VOID (*InitializeConditionVariable)( PCONDITION_VARIABLE );
        static VOID (*WakeConditionVariable)( PCONDITION_VARIABLE );
        static BOOL (*SleepConditionVariableCS)( PCONDITION_VARIABLE, PCRITICAL_SECTION, DWORD );
    #endif


    #define THREAD_IMPLEMENTATION
    #include "libs/thread.h"

    void threads_init( void ) {
        #if defined( __TINYC__ )
            HMODULE kernel = LoadLibrary( "kernel32" );
            InitializeConditionVariable = GetProcAddress( kernel, "InitializeConditionVariable");
            WakeConditionVariable = GetProcAddress( kernel, "WakeConditionVariable");
            SleepConditionVariableCS = GetProcAddress( kernel, "SleepConditionVariableCS");
        #endif
    }
#endif
