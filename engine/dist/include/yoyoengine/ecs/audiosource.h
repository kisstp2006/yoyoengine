/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file audiosource.h
 * @brief Contains audio emitter component code
 * @note In the future, some sort of audio group isolation would be nice in order to easily scale the volume of different effects.
 */

#ifndef YE_AUDIOSOURCE_H
#define YE_AUDIOSOURCE_H

#include <yoyoengine/export.h>

#include <stdbool.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/utils.h>

struct ye_component_audiosource {
    bool active;            // whether or not the audio source is active

    bool simulated;         // whether or not the audio source is simulated (if it is, it will be affected by the audio listener)

    char *handle;           // the resource handle of the audio source
    float volume;           // the volume of the audio source (scaled against the engine volume as a ceiling)
    
    // controls the position - the only thing you can set in this is width which updates height as well
    struct ye_rectf range;  // the range of the audio source (used to calculate distance), the middle of this is considered the origin
    bool relative;          // whether or not the audio source is relative to the transform

    bool play_on_awake;     // whether or not the audio source should play on awake
    int loops;              // the number of times to loop the audio source

    int channel;            // holds the channel the audio source is playing on (assigned by the engine)
    bool playing;           // whether or not the audio source is playing (triggered by player but this value is tracked by the engine)
};

/**
 * @brief Constructs an audiosource component onto a given entity.
 * 
 * @param entity The target entity
 * @param handle The resource handle of the sound to play
 * @param volume The volume (0-1) of the sound to play (this will be converted and scaled to the projects volume global cap)
 * @param play_on_awake Whether or not the sound should play on awake
 * @param loops The number of times to loop the sound (-1 for infinite looping)
 * @param simulated Whether or not the sound should be simulated (affected by the audio listener)
 */
YE_API void ye_add_audiosource_component(struct ye_entity *entity, const char *handle, float volume, bool play_on_awake, int loops, bool simulated, struct ye_rectf range);

/**
 * @brief Will play the audio source on the entity <loops> times until paused, destroyed, disabled, or finished playing <loops>
 * 
 * @param entity The target entity
 */
YE_API void ye_play_audiosource(struct ye_entity *entity);

/**
 * @brief Pauses the audio source on the entity
 * 
 * @param entity The target entity
 * 
 * @note This function will reset the audio source to its initial state (like loops) and will dereference the channel in the mixcache
 */
YE_API void ye_pause_audiosource(struct ye_entity *entity);

/**
 * @brief Removes the audiosource component from the entity
 * 
 * @param entity The target entity
 */
YE_API void ye_remove_audiosource_component(struct ye_entity *entity);

/**
 * @brief The system in charge of processing audiosource components
 */
YE_API void ye_system_audiosource();

/**
 * @brief Called from audio.c when a channel finishes, this function is in charge of re-scheduling audio chunks
 * 
 * @param channel The channel that finished
 */
YE_API void ye_audiosource_channel_finished(int channel);

#endif