/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <SDL_mixer.h>
#include <uthash/uthash.h>

#include <yoyoengine/yep.h>
#include <yoyoengine/audio.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/ecs/audiosource.h>

int totalChunks = 0;

Mix_Music *music = NULL;

/*
    ==========================================
                MIXER CACHE IMPL
    ==========================================
*/

struct ye_mixer_cache_item * mix_cache_table = NULL;

void ye_init_mixer_cache()
{
    // initialize the mixer cache
    mix_cache_table = NULL;
    ye_logf(info, "Initialized mixer cache.\n");
}

void ye_shutdown_mixer_cache()
{
    ye_purge_mixer_cache();
    ye_logf(info, "Shut down mixer cache.\n");
}

// free all chunks
void ye_purge_mixer_cache(){
    // iterate through the cache
    struct ye_mixer_cache_item *item, *tmp;
    HASH_ITER(hh, mix_cache_table, item, tmp) {
        // remove the item from the cache
        HASH_DEL(mix_cache_table, item);

        // free the chunk
        Mix_FreeChunk(item->chunk);

        // free the item
        free(item->handle);
        free(item);
    }

    // set the cache to null
    mix_cache_table = NULL;
}

/*
    This is intended to cache a brand NEW item, and that we've already checked for duplicates
*/
void ye_mixer_cache(const char *handle)
{
    // create struct to cache
    struct ye_mixer_cache_item *item = malloc(sizeof(struct ye_mixer_cache_item));

    // set the handle
    item->handle = strdup(handle);

    // if in editor mode, retrieve from disk, if runtime load from pack
    if(YE_STATE.editor.editor_mode){
        // load from disk
        item->chunk = Mix_LoadWAV(ye_path_resources(handle));

    } else {
        // load from pack
        item->chunk = yep_resource_audio(handle);
    }

    // check if the chunk is null
    if(item->chunk == NULL){
        ye_logf(error, "Failed to load audio chunk %s.\n", handle);
        free(item);
        return;
    }

    // add the item to the cache
    HASH_ADD_KEYPTR(hh, mix_cache_table, item->handle, strlen(item->handle), item);
}

/*
    Cache a mixer chunk from engine data
*/
void _ye_mixer_engine_cache(char *handle)
{
    // create struct to cache
    struct ye_mixer_cache_item *item = malloc(sizeof(struct ye_mixer_cache_item));

    // set the handle
    item->handle = strdup(handle);

    // if in editor mode, retrieve from disk, if runtime load from pack
    if(YE_STATE.editor.editor_mode){
        // load from disk
        item->chunk = Mix_LoadWAV(ye_get_engine_resource_static(handle));

    } else {
        // load from pack
        item->chunk = yep_engine_resource_audio(handle);
    }

    // check if the chunk is null
    if(item->chunk == NULL){
        ye_logf(error, "Failed to load engine audio chunk %s.\n", handle);
        free(item);
        return;
    }

    // add the item to the cache
    HASH_ADD_KEYPTR(hh, mix_cache_table, item->handle, strlen(item->handle), item);
}

/*
    Api to return a mix chunk from a handle, and load it if not existant
*/
Mix_Chunk *ye_audio(const char *handle){
    // check if the cache has an existing chunk by this handle
    struct ye_mixer_cache_item *item = NULL;
    HASH_FIND_STR(mix_cache_table, handle, item);

    // if the item is null, we need to cache it
    if(item == NULL){
        // cache the item
        ye_mixer_cache(handle);

        // retrieve the item
        HASH_FIND_STR(mix_cache_table, handle, item);
    }

    // return the chunk
    return item->chunk;
}

/*
    ==========================================
*/










/*
    ==========================================
            AUDIO PLAYBACK INTERFACE
    ==========================================
*/

/*
    This is the mechanism by which the engine routes audio to SDL_Mixer.
    We will dynamically manage the number of channels open at any given time, as we
    can have as many as we want to.
*/

#ifndef YE_MIXER_DEFAULT_CHANNELS
    // do not allocate channels by default
    #define YE_MIXER_DEFAULT_CHANNELS 0
#endif

int audio_mix_allocated_channels = 0;
int audio_mix_busy_channels = 0;

void ye_init_audio(){
    audio_mix_allocated_channels = 0;
    audio_mix_busy_channels = 0;

    // if (!Mix_Init(MIX_INIT_MP3 | MIX_INIT_WAVPACK) )
    // {
    //     ye_logf(error, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", SDL_GetError());
    //     exit(1);
    // }

    // will call Mix_Init internally
    if(!Mix_OpenAudio(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL)){
        ye_logf(error, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", SDL_GetError());
        exit(1);
    }
    ye_logf(info, "Initialized SDL_mixer.\n");

    // allocate our desired max channels to the mixer
    if(YE_MIXER_DEFAULT_CHANNELS > 0)
        audio_mix_allocated_channels = Mix_AllocateChannels(YE_MIXER_DEFAULT_CHANNELS);

    ye_init_mixer_cache();

    // make all channels capped at the engine volume
    Mix_MasterVolume(YE_STATE.engine.volume);

    // debug: acknowledge audio initialization
    ye_logf(info, "Initialized audio.\n");
}

/*
    TODO: better way to do this? we have to make sure we dont
    free chunks and let audiosource system tentatively reschedule
    them without knowledge that we are done with those sources
*/
bool mid_audio_shutdown = false;

// stop playing and clear mixer cache, shutdown mixer
void ye_shutdown_audio(){
    mid_audio_shutdown = true;

    // Halt all playing channels
    Mix_HaltChannel(-1);
    // ye_logf(debug, "Halted playing all channels.\n");

    // free all chunks
    ye_shutdown_mixer_cache();
    // free music
    Mix_FreeMusic(music);
    music = NULL;

    // reset channel counts
    audio_mix_allocated_channels = 0;
    audio_mix_busy_channels = 0;

    // Close the audio mixer
    Mix_Quit();
    ye_logf(info, "Shut down audio.\n");

    mid_audio_shutdown = false;
}

/*
    Callback for when a channel finishes playing
*/
void ye_finished_channel(int channel){
    audio_mix_busy_channels--;

    /*
        if we are about to load a new scene, we actually DO NOT
        want audiosource to begin scheduling new sounds, since
        this callback simulates a natural channel end
    */
    if(!mid_audio_shutdown){
        // reach out to audiosource manager and let it know a channel finished,
        // it can go through and re-request a repeat sound if it wants to
        ye_audiosource_channel_finished(channel);
    }

    // SDL_Mixer will free the empty channels later as needed
}

/*
    Play a sound by its handle.
    retrieves from cache, creates new channel for it
*/
int ye_play_sound(const char *handle, int loops, float volume_scale){ // loops will be decreased and passed to the channel finished callback to replay
    // retrieve the chunk from the mixer cache
    Mix_Chunk *chunk = ye_audio(handle);

    // if the chunk is null, we failed to load it
    if(chunk == NULL){
        ye_logf(error, "Failed to play audio chunk %s.\n", handle);
        return -2; // nonexistant channel
    }

    // allocate a new channel to play audio on if full
    int channel;
    if(audio_mix_allocated_channels - audio_mix_busy_channels <= 0){
        channel = Mix_AllocateChannels(Mix_AllocateChannels(-1) + 1);
        if(channel == 0){
            ye_logf(error, "Failed to allocate new audio channel.\n");
            return -2; // nonexistant channel
        }
    }

    // Free audio memory when channel finishes
    Mix_ChannelFinished(ye_finished_channel);

    // play the chunk on the channel
    channel = Mix_PlayChannel(-1, chunk, loops);

    totalChunks = audio_mix_busy_channels++;

    // adjust the channel volume
    Mix_Volume(channel, (int)(YE_STATE.engine.volume * volume_scale));

    return channel;
}

/*
    Currently, I disabled freeing that data that the rwops for ye_music
    uses because it caused a windows page fault. TODO: investigate
*/
void ye_play_music(const char *handle, int loops, float volume_scale){

    // #ifdef __linux__

    // if music is already playing, stop it and free it
    if(music != NULL){
        Mix_FreeMusic(music);
        music = NULL;
    }

    // if in editor mode, retrieve from disk, if runtime load from pack
    if(YE_STATE.editor.editor_mode){
        music = Mix_LoadMUS(ye_path_resources(handle));
    }
    else{
        music = yep_resource_music(handle);
    }
    
    // if the music is null, we failed to load it
    if(music == NULL){
        ye_logf(error, "Failed to play music %s.\n", handle);
        return;
    }

    // play the music
    Mix_PlayMusic(music, loops);

    // adjust the music volume
    Mix_VolumeMusic((int)((YE_STATE.engine.volume * volume_scale)));
}

/*
    ==========================================
*/

void ye_set_volume(float volume){
    YE_STATE.engine.volume = (128 * volume);
    Mix_VolumeMusic((128 * volume));
    Mix_Volume(-1, (128 * volume));
    ye_logf(debug, "Set audio volume to %d.\n", (int)(128 * volume));
}