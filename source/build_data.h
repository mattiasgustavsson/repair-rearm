
typedef struct build_index_t {
    char filename[ 256 ];
    uint32_t offset;
    uint32_t size;
} build_index_t;


typedef struct build_t {
    array_t(build_index_t)* index;
    buffer_t* data;
    paldither_palette_t* paldither;
 } build_t;


void build_init( build_t* build ) {
    build->index = array_create( build_index_t );
    build->data = buffer_create();
    build->paldither = NULL;
}


void build_term( build_t* build ) {
    if( build->paldither ) {
        paldither_palette_destroy( build->paldither, NULL );
    }
    buffer_destroy( build->data );
    array_destroy( build->index );
}


bool build_save_data( build_t* build, char const* filename ) {
    FILE* fp = fopen( filename, "wb" );
    if( !fp ) {
        printf( "Filed to open output file: '%s'\n", filename );
        return false;
    }
    uint32_t file_count = build->index->count;
    fwrite( &file_count, sizeof( uint32_t ), 1, fp );
    for( int i = 0; i < build->index->count; ++i ) {
        build_index_t* item = &build->index->items[ i ];
        uint32_t len = (uint32_t) strlen( item->filename ) + 1;
        fwrite( &len, sizeof( uint32_t ), 1, fp );
        fwrite( item->filename, sizeof( char ), len, fp );
        fwrite( &item->offset, sizeof( uint32_t ), 1, fp );
        fwrite( &item->size, sizeof( uint32_t ), 1, fp );
    }
    uint32_t data_size = (uint32_t) buffer_position( build->data );
    fwrite( &data_size, sizeof( uint32_t ), 1, fp );
    fwrite( buffer_data( build->data ), 1, buffer_position( build->data ), fp );
    fclose( fp );
    return true;
}


bool build_add( build_t* build, char const* filename, void* data, size_t size ) {
    if( strlen( filename ) >= 256 ) {
        printf( "Filename too long: '%s'\n", filename );
        return false;
    }
    build_index_t index;
    strcpy( index.filename, filename );
    index.offset = (uint32_t) buffer_position( build->data );
    index.size = (uint32_t) size;
    array_add( build->index, &index );
    buffer_write_u8( build->data, data, (int) size );
    return true;
}


bool build_raw( build_t* build, char const* filename )  {
    FILE* fp = fopen( filename, "rb" );
    if( !fp ) {
        printf( "Failed to load file: '%s'\n", filename );
        return false;
    }
    fseek( fp, 0, SEEK_END );
    size_t size = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    void* data = malloc( size );
    fread( data, size, 1, fp );
    fclose( fp );

    bool success = build_add( build, filename, data, size );
    free( data );
    return  success;
}


bool build_palette( build_t* build, char const* filename )  {
    char plut_filename[ 256 ];
    strcpy( plut_filename, filename );
    strcpy( strrchr( plut_filename, '.' ), ".plut" );

    FILE* fp = fopen( plut_filename, "rb" );
    if( fp ) {
        fseek( fp, 0, SEEK_END );
        size_t size = ftell( fp );
        fseek( fp, 0, SEEK_SET );
        build->paldither = malloc( size );
        fread( build->paldither, size, 1, fp );
        fclose( fp );

        uint32_t palette[ 256 ];
        memset( palette, 0, sizeof( uint32_t ) * 256 );
        for( int i = 0; i < build->paldither->color_count; ++i ) {
            palette[ i ] = build->paldither->colortable[ i ];
        }

        return build_add( build, filename, palette, sizeof( uint32_t ) * 256 );
    }

    int width, height, c;
    uint32_t* image = (uint32_t*) stbi_load( filename, &width, &height, &c, 4 );

    uint32_t palette[ 256 ];
    memset( palette, 0, sizeof( uint32_t ) * 256 );
    int count = 0;
    for( int y = 0; y < height; ++y ) {
        for( int x = 0; x < width; ++x ) {
            uint32_t pixel = image[ x + y * width ];
            pixel = ( pixel & 0x00ffffff ) | 0xff000000;
            for( int i = 0; i < count; ++i ) {
                if( palette[ i ] == pixel )
                    goto skip;
            }
            if( count >= 256 ) {
                stbi_image_free( image );
                printf( "Failed to build palette from file '%s'\n", filename );
                return false;
            }
            palette[ count++ ] = pixel;
        skip:
            ;
        }
    }
    stbi_image_free( image );

    // force 18-bit color like on vga
    for( int i = 0; i < count; ++i ) {
        uint32_t col = palette[ i ];
        uint32_t b = ( col >> 16 ) & 0xff;
        uint32_t g = ( col >>  8 ) & 0xff;
        uint32_t r = ( col       ) & 0xff;
        b = ( b / 4 ) * 4;
        g = ( g / 4 ) * 4;
        r = ( r / 4 ) * 4;
        palette[ i ] = ( b << 16 ) | ( g << 8 ) | r;
    }

    if( build->paldither ) {
        paldither_palette_destroy( build->paldither, NULL );
        build->paldither = NULL;
    }
    size_t paldither_size = 0;
    build->paldither = paldither_palette_create( palette, count, &paldither_size, NULL );

    FILE* out = fopen( plut_filename, "wb" );
    fwrite( build->paldither, paldither_size, 1, out );
    fclose( out );

    return build_add( build, filename, palette, sizeof( uint32_t ) * 256 );
}


bool build_bitmap( build_t* build, char const* filename )  {
    int width, height, c;
    uint32_t* image = (uint32_t*) stbi_load( filename, &width, &height, &c, 4 );
    if( !image ) {
        printf( "Failed to load image file: '%s'\n", filename );
        return false;
    }

    uint8_t* pixels = (uint8_t*) malloc( sizeof( uint8_t ) * width * height * 2 );
    paldither_palettize( image, width, height, build->paldither, PALDITHER_TYPE_DEFAULT, pixels );

    uint8_t* mask = pixels + width * height;
    for( int i = 0; i < width * height; ++i ) {
        mask[ i ] = (uint8_t)( image[ i ] >> 24 );
    }
    stbi_image_free( image );

    palrle_data_t* rle = palrle_encode_mask( pixels, mask, width, height, build->paldither->colortable, build->paldither->color_count, NULL );
    free( pixels );
    bool success = build_add( build, filename, rle, rle->size );
    palrle_free( rle, NULL );
    return  success;
}


bool build_sprite( build_t* build, char const* filename )  {
    int width, height, c;
    uint32_t* image = (uint32_t*) stbi_load( filename, &width, &height, &c, 4 );
    if( !image ) {
        printf( "Failed to load image file: '%s'\n", filename );
        return false;
    }

    img_t img = img_from_abgr32( image, width, height );
    img_adjust_brightness( &img, 0.03f );
    img_sharpen( &img, 0.25f, 0.4f );
    img_adjust_contrast( &img, 1.05f );
    img_to_argb32( &img, image );
    img_free( &img );

    uint8_t* pixels = (uint8_t*) malloc( sizeof( uint8_t ) * width * height * 2 );
    paldither_palettize( image, width, height, build->paldither, PALDITHER_TYPE_DEFAULT, pixels );

    uint8_t* mask = pixels + width * height;
    for( int i = 0; i < width * height; ++i ) {
        mask[ i ] = (uint8_t)( image[ i ] >> 24 );
    }
    stbi_image_free( image );

    palrle_data_t* rle = palrle_encode_mask( pixels, mask, width, height, build->paldither->colortable, build->paldither->color_count, NULL );
    free( pixels );
    bool success = build_add( build, filename, rle, rle->size );
    palrle_free( rle, NULL );
    return  success;
}


bool build_font( build_t* build, char const* filename ) {
    FILE* fp = fopen( filename, "rb" );
    if( !fp ) {
        printf( "Failed to load file: '%s'\n", filename );
        return false;
    }
    fseek( fp, 0, SEEK_END );
    size_t sz = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    void* data = malloc( sz );
    fread( data, sz, 1, fp );
    fclose( fp );

    stbtt_fontinfo font;
    stbtt_InitFont( &font, data, stbtt_GetFontOffsetForIndex( data, 0) );

    int size = 0;
    for( int i = 1; i < 32; ++i ) {
        float scale = stbtt_ScaleForPixelHeight( &font, (float) i );
        int w, h;
        PIXELFONT_U8* bitmap = stbtt_GetGlyphBitmap( &font, scale, scale, 'A', &w, &h, 0, 0 );
        int empty = 1;
        int antialiased = 0;
        for( int j = 0; j < w * h; ++j ) {
            if( bitmap[ j ] > 0 ) {
                empty = 0;
                if( bitmap[ j ] < 255 ) { antialiased = 1; break; }
            }
        }
        stbtt_FreeBitmap( bitmap, 0 );
        if( !empty && !antialiased ) { size = i; break; }
    }

    if( !size ) {
        printf( "Not a pixel font (size detection failed)\n" );
        free( data );
        return false;
    }

    float scale = stbtt_ScaleForPixelHeight( &font, (float) size );

    int ascent, descent;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, 0 );
    ascent = (int)( scale * ascent );
    descent = (int)( scale * descent );

    int x0, y0, x1, y1;
    stbtt_GetFontBoundingBox( &font, &x0, &y0, &x1, &y1 );
    x0 = (int)( scale * x0 );
    x1 = (int)( scale * x1 );
    y0 = (int)( scale * y0 );
    y1 = (int)( scale * y1 );

    int line_spacing = ascent - descent + 1; // TODO: verify this

    for( int c = 0; c < 256; ++c ) {
        int gi = stbtt_FindGlyphIndex( &font, c );
        if( gi > 0 && gi < font.numGlyphs ) {
            if( !stbtt_IsGlyphEmpty( &font, gi ) ) {
                int ix0, iy0, ix1, iy1;
                stbtt_GetGlyphBitmapBox(&font, gi, scale, scale, &ix0, &iy0, &ix1, &iy1);
                iy0 += ascent;
                iy1 += ascent;
                if( ix0 < x0 ) x0 = ix0;
                if( iy0 < y0 ) y0 = iy0;
                if( ix1 > x1 ) x1 = ix1;
                if( iy1 > y1 ) y1 = iy1;
            }
        }
    }

    pixelfont_builder_t* builder = pixelfont_builder_create( y1 - y0 + 1, ascent, line_spacing, 0 );

    PIXELFONT_U8 dummy;
    pixelfont_builder_glyph( builder, 0, 0, &dummy, 0, 0 );
    for( int c = 1; c < 256; ++c ) {
        int gi = stbtt_FindGlyphIndex( &font, c );
        if( gi > 0 && gi < font.numGlyphs ) {
            int advance;
            int left;
            stbtt_GetGlyphHMetrics( &font, gi, &advance, &left );
            advance = (int)( scale * advance );
            left = (int)( scale * left );
            advance -= left;

            if( !stbtt_IsGlyphEmpty( &font, gi ) ) {
                int ix0, iy0, ix1, iy1;
                stbtt_GetGlyphBitmapBox(&font, gi, scale, scale, &ix0, &iy0, &ix1, &iy1);
                int w, h, xo, yo;
                PIXELFONT_U8* bitmap = stbtt_GetGlyphBitmap( &font, scale, scale, gi, &w, &h, &xo, &yo );
                PIXELFONT_U8* temp_bmp = (PIXELFONT_U8*) malloc( (size_t) ( y1 - y0 + 1 ) * w  );
                memset( temp_bmp, 0, (size_t) ( y1 - y0 + 1 ) * w );
                int top = ascent + yo;
                top = top < 0 ? 0 : top;
                PIXELFONT_U8* out = temp_bmp + top * w;
                for( int y = 0; y < h; ++y )
                    for( int x = 0; x < w; ++x )
                        *out++ = bitmap[ x + y * w ] ? 1U : 0U; // font pixel
                pixelfont_builder_glyph( builder, c, w, temp_bmp, left, advance );
                if( bitmap ) stbtt_FreeBitmap( bitmap, NULL );
                if( temp_bmp ) free( temp_bmp );
            } else if( advance || left ) {
                pixelfont_builder_glyph( builder, c, 0, &dummy, left, advance );
            }

            for( int k = 0; k < 256; ++k ) {
                int kern = stbtt_GetCodepointKernAdvance( &font, c, k );
                kern = (int)( scale * kern );
                if( kern ) pixelfont_builder_kerning( builder, c, k, kern );
            }
        }
    }
    free( data );

    pixelfont_t* builtfont = pixelfont_builder_font( builder );
    bool success = build_add( build, filename, builtfont, builtfont->size_in_bytes );
    pixelfont_builder_destroy( builder );
    return success;
}


bool build_data( char const* output_file ) {
    build_t build;
    build_init( &build );
    bool success = true;
    //////////////////////////////////////////////////////////////////////////

    success = success && build_raw( &build, "data/crtframe_tv.png" );
    success = success && build_palette( &build, "data/pal.png" );
    success = success && build_bitmap( &build, "data/title.png" );
    success = success && build_bitmap( &build, "data/officer.png" );
    success = success && build_bitmap( &build, "data/repair.png" );
    success = success && build_bitmap( &build, "data/ammo.png" );
    success = success && build_bitmap( &build, "data/damage.png" );
    success = success && build_font( &build, "data/deltoid-sans.ttf" );
    success = success && build_font( &build, "data/prstartk.ttf" );
    success = success && build_sprite( &build, "data/sprites/bullet.png" );
    success = success && build_sprite( &build, "data/sprites/bullet1.png" );
    success = success && build_sprite( &build, "data/sprites/dollar.png" );
    success = success && build_sprite( &build, "data/sprites/enemy1.png" );
    success = success && build_sprite( &build, "data/sprites/enemy2.png" );
    success = success && build_sprite( &build, "data/sprites/enemy3.png" );
    success = success && build_sprite( &build, "data/sprites/enemy4.png" );
    success = success && build_sprite( &build, "data/sprites/enemy5.png" );
    success = success && build_sprite( &build, "data/sprites/enemy6.png" );
    success = success && build_sprite( &build, "data/sprites/enemy_bullet.png" );
    success = success && build_sprite( &build, "data/sprites/expl1_001.png" );
    success = success && build_sprite( &build, "data/sprites/expl1_002.png" );
    success = success && build_sprite( &build, "data/sprites/expl1_003.png" );
    success = success && build_sprite( &build, "data/sprites/expl1_004.png" );
    success = success && build_sprite( &build, "data/sprites/expl1_005.png" );
    success = success && build_sprite( &build, "data/sprites/expl1_006.png" );
    success = success && build_sprite( &build, "data/sprites/expl1_007.png" );
    success = success && build_sprite( &build, "data/sprites/expl2_001.png" );
    success = success && build_sprite( &build, "data/sprites/expl2_002.png" );
    success = success && build_sprite( &build, "data/sprites/expl2_003.png" );
    success = success && build_sprite( &build, "data/sprites/expl2_004.png" );
    success = success && build_sprite( &build, "data/sprites/expl2_005.png" );
    success = success && build_sprite( &build, "data/sprites/expl2_006.png" );
    success = success && build_sprite( &build, "data/sprites/expl2_007.png" );
    success = success && build_sprite( &build, "data/sprites/expl2_008.png" );
    success = success && build_sprite( &build, "data/sprites/expl3_001.png" );
    success = success && build_sprite( &build, "data/sprites/expl3_002.png" );
    success = success && build_sprite( &build, "data/sprites/expl3_003.png" );
    success = success && build_sprite( &build, "data/sprites/expl3_004.png" );
    success = success && build_sprite( &build, "data/sprites/expl3_005.png" );
    success = success && build_sprite( &build, "data/sprites/expl3_006.png" );
    success = success && build_sprite( &build, "data/sprites/expl3_007.png" );
    success = success && build_sprite( &build, "data/sprites/expl4_001.png" );
    success = success && build_sprite( &build, "data/sprites/expl4_002.png" );
    success = success && build_sprite( &build, "data/sprites/expl4_003.png" );
    success = success && build_sprite( &build, "data/sprites/expl4_004.png" );
    success = success && build_sprite( &build, "data/sprites/expl4_005.png" );
    success = success && build_sprite( &build, "data/sprites/expl4_006.png" );
    success = success && build_sprite( &build, "data/sprites/expl4_007.png" );
    success = success && build_sprite( &build, "data/sprites/player.png" );
    success = success && build_raw( &build, "data/ingame.ogg" );
    success = success && build_raw( &build, "data/inter.ogg" );
    success = success && build_raw( &build, "data/sound/pickup_powerup.ogg" );
    success = success && build_raw( &build, "data/sound/ship_damaged.ogg" );
    success = success && build_raw( &build, "data/sound/ship_explosion.ogg" );
    success = success && build_raw( &build, "data/sound/ship_laser_1.ogg" );
    success = success && build_raw( &build, "data/sound/ship_laser_2.ogg" );

    //////////////////////////////////////////////////////////////////////////
    success = success && build_save_data( &build, output_file );
    build_term( &build );
    return success;
}