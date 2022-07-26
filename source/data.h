
typedef struct data_file_t {
    char filename[ 256 ];
    uint32_t offset;
    uint32_t size;
    short* sound;
    int sample_pairs_count;
} data_file_t;


typedef struct data_t {
    int files_count;
    data_file_t* files;
    uint8_t* data;
} data_t;


bool load_data( data_t* data, char const* filename ) {
    FILE* fp = fopen( filename, "rb" );
    if( !fp ) {
        printf( "Filed to open data file: '%s'\n", filename );
        return false;
    }

    uint32_t file_count = 0;
    fread( &file_count, sizeof( uint32_t ), 1, fp );
    data->files_count = (int) file_count;
    data->files = (data_file_t*) malloc( sizeof( data_file_t ) * data->files_count );
    for( int i = 0; i < data->files_count; ++i ) {
        uint32_t len = 0;
        fread( &len, sizeof( uint32_t ), 1, fp );
        data_file_t* file = &data->files[ i ];
        fread( file->filename, sizeof( char ), len, fp );
        fread( &file->offset, sizeof( uint32_t ), 1, fp );
        fread( &file->size, sizeof( uint32_t ), 1, fp );
        file->sound = NULL;
        file->sample_pairs_count = 0;
    }

    uint32_t data_size = 0;
    fread( &data_size, sizeof( uint32_t ), 1, fp );
    data->data = (uint8_t*) malloc( sizeof( uint8_t ) * data_size );
    fread( data->data, sizeof( uint8_t ) * data_size, 1, fp );
    fclose( fp );

    for( int i = 0; i < data->files_count; ++i ) {
        data_file_t* file = &data->files[ i ];
        char const* ext = strrchr( file->filename, '.' );
        if( ext && strcmp( ext, ".ogg") == 0 ) {
            int channels = 0;
            int sample_rate = 0;
            short* output = NULL;
            int samples = stb_vorbis_decode_memory( data->data + file->offset, file->size, &channels, &sample_rate, &output );
            if( samples > 0 && output ) {
                file->sound = output;
                file->sample_pairs_count = samples;
            }
        }
    }

    return true;
}


void free_data( data_t* data ) {
    for( int i = 0; i < data->files_count; ++i ) {
        data_file_t* file = &data->files[ i ];
        if( file->sound ) {
           free( file->sound );
        }
    }
    free( data->files );
    free( data->data );
}


void* load_file( data_t* data, char const* filename, size_t* size ) {
    for( int i = 0; i < data->files_count; ++i ) {
        if( strcmp( data->files[ i ].filename, filename ) == 0 ) {
            if( size ) {
                *size = (size_t) data->files[ i ].size;
            }
            return data->data + data->files[ i ].offset;
        }
    }
    if( size ) {
        *size = 0;
    }
    return NULL;
}


short* load_samples( data_t* data, char const* filename, int* sample_pairs_count ) {
    for( int i = 0; i < data->files_count; ++i ) {
        if( strcmp( data->files[ i ].filename, filename ) == 0 ) {
            if( data->files[ i ].sound ) {
                if( sample_pairs_count ) {
                    *sample_pairs_count = data->files[ i ].sample_pairs_count;
                }
                return data->files[ i ].sound;
            } else {
                if( sample_pairs_count ) {
                    *sample_pairs_count = 0;
                }
                return NULL;
            }
        }
    }
    if( sample_pairs_count ) {
        *sample_pairs_count = 0;
    }
    return NULL;
}


