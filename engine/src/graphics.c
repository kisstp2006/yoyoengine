/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdio.h>

#if defined __linux__ || defined __APPLE__ || defined __unix__
    #include <unistd.h>
#elif WIN32
    #include <platform/windows/unistd.h>
#endif

#include <math.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <yoyoengine/ui/ui.h>
#include <yoyoengine/yep.h>
#include <yoyoengine/cache.h>
#include <yoyoengine/tricks.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/camera.h>
#include <yoyoengine/ecs/button.h>
#include <yoyoengine/ecs/renderer.h>
#include <yoyoengine/ecs/lua_script.h>

// define globals for file
SDL_Window *pWindow = NULL;
SDL_Surface *pScreenSurface = NULL;
SDL_Renderer *pRenderer = NULL;

/*
    Texture used for missing textures
*/
SDL_Texture *missing_texture = NULL;

TTF_Font * ye_load_font(const char *pFontPath/*, int fontSize*/) {
    /*
        if(fontSize > 500){
            ye_logf(error, "ERROR: FONT SIZE TOO LARGE\n");
            return YE_STATE.engine.pEngineFont;
        }
    */
    const char *fontpath = pFontPath;
    if(access(fontpath, F_OK) == -1){
        ye_logf(error, "Could not access file '%s'.\n", fontpath);
        return YE_STATE.engine.pEngineFont;
    }
    TTF_Font *pFont = TTF_OpenFont(fontpath, /*fontSize*/ 1); // lets just open this at size 1 since it will be reloaded later, this should take minimal time
    if (pFont == NULL) {
        ye_logf(error, "Failed to load font: %s\n", SDL_GetError());
        return YE_STATE.engine.pEngineFont;
    }
    ye_logf(debug, "Loaded font: %s\n", pFontPath);
    return pFont;
}

/*
    https://gamedev.stackexchange.com/questions/119642/how-to-use-sdl-ttfs-outlines
    
    NOTE: The actual width modifier depends on the resolution of the passed font.
    if we want to allow better consistency over the width between different res,
    we need to identify the font by its loaded width (can be stored with its data in cache)
    and then we could pass in a relative width modifier
*/
SDL_Texture *createTextTextureWithOutline(const char *pText, int width, TTF_Font *pFont, SDL_Color *pColor, SDL_Color *pOutlineColor) {
    int temp = TTF_GetFontOutline(pFont);

    SDL_Surface *fg_surface = TTF_RenderText_Blended(pFont, pText, 9, *pColor); 

    TTF_SetFontOutline(pFont, width);
    
    SDL_Surface *bg_surface = TTF_RenderText_Blended(pFont, pText, 0, *pOutlineColor); 
    
    SDL_Rect rect = {width, width, fg_surface->w, fg_surface->h}; 

    /* blit text onto its outline */ 
    SDL_SetSurfaceBlendMode(fg_surface, SDL_BLENDMODE_BLEND); 
    SDL_BlitSurface(fg_surface, NULL, bg_surface, &rect); 
    SDL_DestroySurface(fg_surface); 
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, bg_surface);
    SDL_DestroySurface(bg_surface);
    
    // error out if texture creation failed
    if (pTexture == NULL) {
        ye_logf(error, "Failed to create texture: %s\n", SDL_GetError());
        return NULL;
    }
    
    TTF_SetFontOutline(pFont, temp);

    return pTexture;
}

SDL_Texture *createTextTextureWithOutlineWrapped(const char *pText, int width, TTF_Font *pFont, SDL_Color *pColor, SDL_Color *pOutlineColor, int wrapLength) {
    int temp = TTF_GetFontOutline(pFont);

    SDL_Surface *fg_surface = TTF_RenderText_Blended_Wrapped(pFont, pText, 0, *pColor, wrapLength); 

    TTF_SetFontOutline(pFont, width);
    
    SDL_Surface *bg_surface = TTF_RenderText_Blended_Wrapped(pFont, pText, 0, *pOutlineColor, wrapLength); 
    
    SDL_Rect rect = {width, width, fg_surface->w, fg_surface->h}; 

    /* blit text onto its outline */ 
    SDL_SetSurfaceBlendMode(fg_surface, SDL_BLENDMODE_BLEND); 
    SDL_BlitSurface(fg_surface, NULL, bg_surface, &rect); 
    SDL_DestroySurface(fg_surface); 
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, bg_surface);
    SDL_DestroySurface(bg_surface);
    
    // error out if texture creation failed
    if (pTexture == NULL) {
        ye_logf(error, "Failed to create texture: %s\n", SDL_GetError());
        return NULL;
    }
    
    TTF_SetFontOutline(pFont, temp);

    return pTexture;
}

SDL_Texture *createTextTexture(const char *pText, TTF_Font *pFont, SDL_Color *pColor) {
    // create surface from parameters
    SDL_Surface *pSurface = TTF_RenderText_Blended(pFont, pText, 0, *pColor); // MEMLEAK: valgrind says so but its not my fault, internal in TTF
    
    // error out if surface creation failed
    if (pSurface == NULL) {
        ye_logf(error, "Failed to render text: %s\n", SDL_GetError());
        return missing_texture; // return missing texture, error has been logged
    }

    // create texture from surface
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);

    // error out if texture creation failed
    if (pTexture == NULL) {
        ye_logf(error, "Failed to create texture: %s\n", SDL_GetError());
        return missing_texture; // return missing texture, error has been logged
    }

    // free the surface memory
    SDL_DestroySurface(pSurface);

    // return the created texture
    return pTexture;
}

SDL_Texture *createTextTextureWrapped(const char *pText, TTF_Font *pFont, SDL_Color *pColor, int wrapLength) {
    // create surface from parameters
    SDL_Surface *pSurface = TTF_RenderText_Blended_Wrapped(pFont, pText, 0, *pColor, wrapLength);

    // error out if surface creation failed
    if (pSurface == NULL) {
        ye_logf(error, "Failed to render text: %s\n", SDL_GetError());
        return missing_texture; // return missing texture, error has been logged
    }

    // create texture from surface
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);

    // error out if texture creation failed
    if (pTexture == NULL) {
        ye_logf(error, "Failed to create texture: %s\n", SDL_GetError());
        return missing_texture; // return missing texture, error has been logged
    }

    // free the surface memory
    SDL_DestroySurface(pSurface);

    // return the created texture
    return pTexture;
}

SDL_Texture * ye_create_image_texture(const char *pPath) {
    // check the file exists
    if(access(pPath, F_OK) == -1){
        ye_logf(error, "Could not access file '%s'.\n", pPath);
        return missing_texture; // return missing texture, error has been logged
    }

    // create surface from loading the image
    SDL_Surface *pImage_surface = IMG_Load(pPath);
    
    // error out if surface load failed
    if (!pImage_surface) {
        ye_logf(error, "Error loading image: %s\n", SDL_GetError());
        return missing_texture; // return missing texture, error has been logged
    }

    // create texture from surface
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pImage_surface);
    
    // error out if texture creation failed
    if (!pTexture) {
        ye_logf(error, "Error creating texture: %s\n", SDL_GetError());
        return missing_texture; // return missing texture, error has been logged
    }

    // release surface from memory
    SDL_DestroySurface(pImage_surface);

    // return the created texture
    return pTexture;
}

// variables for render all :3
int frame_counter = 0;
int desired_frame_time = 0;
int fpsUpdateTime = 0;
int fps = 0;

void ye_render_all() {
    int frameStart = SDL_GetTicks();

    // TODO: potential optimization here, only count fps if we need to.
    if(true){
        // increment the frame counter
        frame_counter++;

        // if we have waited long enough to update the display
        if (SDL_GetTicks() - fpsUpdateTime >= 250) {
            // get the elapsed time and scale it to our time factor to get fps
            fpsUpdateTime = SDL_GetTicks();
            fps = frame_counter * 4;
            frame_counter = 0; // reset counted frames
            YE_STATE.runtime.fps = fps;
        }
    }

    /*
        Clear the screen
    */
    // SDL_SetRenderTarget(pRenderer, NULL); Disabled, redundant since we dont change targets anymore
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(pRenderer);

    /*
        If we are in editor mode paint to a viewport

        This viewport is for the scene preview
    */
    if(YE_STATE.editor.editor_mode){
        SDL_Rect viewport;
        viewport.x = 0;
        viewport.y = 35;
        viewport.w = YE_STATE.engine.screen_width / 1.5;
        viewport.h = 35 + YE_STATE.engine.screen_height / 1.5;
        SDL_SetRenderViewport(pRenderer, &viewport);
        // note: this is in practice useless, because in editor set logical size is overwriting this and we are painting to full screen buffer
    }
    else{
        /*
            If we need to set a custom viewport to maintain ratio, do so here

            stretch res determines the value of need boxing on resize events and init
        */
        if(YE_STATE.engine.need_boxing){
            SDL_SetRenderViewport(pRenderer, &YE_STATE.engine.letterbox);
        }
        else{
            SDL_SetRenderViewport(pRenderer, NULL);
        }
    }

    /*
        If we are not keeping the viewport aspect ratio,we can calculate
        our scale to scale the results as if we were zooming in or out

        2 month later edit: wtf is the purpose of this??
    */
    if(!YE_STATE.engine.stretch_viewport){
        // credit to my goat: github copilot for this one
        SDL_SetRenderLogicalPresentation(pRenderer,
                                         (int)YE_STATE.engine.target_camera->camera->view_field.w,
                                         (int)YE_STATE.engine.target_camera->camera->view_field.h,
                                         SDL_LOGICAL_PRESENTATION_LETTERBOX);
    }

    ye_renderer_v2(pRenderer);

    /*
        Reset the viewport and scale to render the ui on top.

        TODO: profile the performance of doing this even if these havent changed
    */
    SDL_SetRenderViewport(pRenderer, NULL);
    SDL_SetRenderScale(pRenderer, (float)1, (float)1);

    ui_render();

    SDL_RenderPresent(pRenderer);
    SDL_UpdateWindowSurface(pWindow);

    // set the end of the render frame
    int frameEnd = SDL_GetTicks();

    YE_STATE.runtime.paint_time = frameEnd - frameStart;

    // if we arent on vsync we need to preform some frame calculations to delay next frame
    if(YE_STATE.engine.framecap != -1){
        // check the desired FPS cap and add delay if needed
        if(frameEnd - frameStart < desired_frame_time){
            SDL_Delay(desired_frame_time - (frameEnd - frameStart));
        }
    }
}


void ye_recompute_boxing(){
    // if we are ok playing with stretched res, we dont need to do any boxing
    if(YE_STATE.engine.stretch_resolution){
        YE_STATE.engine.need_boxing = false;
        ye_logf(debug,"Stretching viewport enabled, no boxing will occur.\n");
        return;
    }

    /*
        If we have a non 16:9 aspect ratio, we need to calculate the viewport and pillarbox paint
    */

    // get the screen size
    struct ScreenSize screenSize = ye_get_screen_size();

    // calculate the aspect ratio
    float aspectRatio = (float)screenSize.width / (float)screenSize.height;

    // print the screen size and aspect ratio
    // ye_logf(debug,"Screen size: %d %d\n", screenSize.width, screenSize.height);
    // ye_logf(debug,"Aspect ratio: %f\n", aspectRatio);

    // if the aspect ratio is not 16:9 TODO: change this to custom target and this whole thing to be generic
    if(aspectRatio != 16.0f / 9.0f){
        ye_logf(info,"Non targeted aspect ratio detected. Boxing will occur if stretching is disabled.\n");
        YE_STATE.engine.need_boxing = true;

        /*
            We are targeting a 16:9 viewport in the center of the screen with pillarboxing on the sides or top depending on ratio

            To achieve this, we figure out what the ratio actually is and then calculate the viewport and pillarbox size
        */
        // float targetAspectRatio = 16.0f / 9.0f;

        // calculate the target viewport size
        int targetWidth = screenSize.height * 16 / 9;
        int targetHeight = screenSize.height;

        // calculate the pillarbox size
        int pillarboxWidth = (screenSize.width - targetWidth) / 2;
        int pillarboxHeight = 0;

        // set the viewport
        SDL_Rect viewport;
        viewport.x = pillarboxWidth;
        viewport.y = pillarboxHeight;
        viewport.w = targetWidth;
        viewport.h = targetHeight;

        YE_STATE.engine.letterbox = viewport;

        ye_logf(info, "Computed viewport with size: %d %d and offset: %d %d\n", targetWidth, targetHeight, pillarboxWidth, pillarboxHeight);

        // printf("current screen size: %d %d\n", screenSize.width, screenSize.height);
        // printf("target aspect ratio: 16:9\n");
        // printf("viewport size: %d %d\n", targetWidth, targetHeight);
    }
    else{
        YE_STATE.engine.need_boxing = false;
        YE_STATE.engine.letterbox = (SDL_Rect){0,0,0,0};
        ye_logf(debug,"Targeted aspect ratio detected. No boxing will occur.\n");
    }
}


void ye_init_graphics(){
    // test for video init, alarm if failed
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        ye_logf(debug, "SDL initialization failed: %s\n", SDL_GetError());
        exit(1);
    }

    ye_logf(info, "SDL initialized.\n");

    // OBSOLETE in SDL3: we need to pass this to CreateTexture
    // // Set the texture filtering hint
    // switch(YE_STATE.engine.sdl_quality_hint){
    //     case 0:
    //         SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    //         break;
    //     case 1:
    //         SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    //         break;
    //     case 2:
    //         SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
    //         break;
    //     default:
    //         SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    //         break;
    // }

    // test for window init, alarm if failed
    pWindow = SDL_CreateWindow(
        YE_STATE.engine.window_title, 
        YE_STATE.engine.screen_width,
        YE_STATE.engine.screen_height,
        YE_STATE.engine.window_mode | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    if (pWindow == NULL) {
        ye_logf(debug, "Window creation failed: %s\n", SDL_GetError());
        exit(1);
    }
    
    ye_logf(info, "Window initialized.\n");
    
    // set our fps cap to the frame cap param
    // (-1) for vsync
    desired_frame_time = (int)(1000 / YE_STATE.engine.framecap);  

    pRenderer = SDL_CreateRenderer(pWindow, NULL);
    if (pRenderer == NULL) {
        ye_logf(error, "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        exit(1);
    }
    
    /*
        Enable vsync if specified
    */
    SDL_SetRenderVSync(pRenderer, YE_STATE.engine.framecap == -1);

    if(YE_STATE.engine.framecap == -1)
        ye_logf(info, "Starting renderer with vsync... \n");
    else
        ye_logf(debug, "Starting renderer with maxfps %d... \n",YE_STATE.engine.framecap);

    /*
        load our missing texture into memory

        If we are the edtior, it exists as a loose file
    */
    SDL_Surface *missing_surface = NULL;
    if(YE_STATE.editor.editor_mode){
        missing_surface = IMG_Load(ye_get_engine_resource_static("missing.png"));
    }
    else{
        missing_surface = yep_engine_resource_image("missing.png");
    }
    missing_texture = SDL_CreateTextureFromSurface(pRenderer, missing_surface);
    SDL_DestroySurface(missing_surface);

    // set the runtime window and renderer references
    YE_STATE.runtime.window = pWindow;
    YE_STATE.runtime.renderer = pRenderer;

    init_ui(pWindow,pRenderer);

    // test for TTF init, alarm if failed
    if (!TTF_Init()) {
        ye_logf(error, "SDL2_ttf could not initialize! SDL2_ttf Error: %s\n", SDL_GetError());
        exit(1);
    }
    ye_logf(info, "TTF initialized.\n");

    /*
        load icon to surface
    
        if we are editor, make sure to do it from loose file
        
        TODO: clean this up, I made a quick patch but this could be better
    */
    SDL_Surface *pIconSurface = NULL;
    if(YE_STATE.editor.editor_mode){
        pIconSurface = IMG_Load(ye_get_engine_resource_static(YE_STATE.engine.icon_path));
    }
    else{
        if(strcmp(YE_STATE.engine.icon_path,"enginelogo.png") == 0){
            pIconSurface = yep_engine_resource_image("enginelogo.png");
        }
        else{
            // check if YE_STATE.engine.icon_path is not empty
            if(strcmp(YE_STATE.engine.icon_path,"") == 0){
                ye_logf(warning, "No icon path set, using default.\n");
                pIconSurface = yep_engine_resource_image("enginelogo.png");
            }
            else{
                pIconSurface = yep_resource_image(YE_STATE.engine.icon_path);
            }
        }
    }

    if (pIconSurface == NULL) {
        ye_logf(error, "IMG_Load error: %s", SDL_GetError());
        exit(1);
    }
    // set icon
    SDL_SetWindowIcon(pWindow, pIconSurface);
    
    // release surface
    SDL_DestroySurface(pIconSurface);

    ye_logf(info, "Window icon set.\n");
}

void ye_shutdown_graphics(){
    // shutdown TTF
    TTF_Quit();
    ye_logf(info, "Shut down TTF.\n");

    // free the missing texture
    SDL_DestroyTexture(missing_texture);

    shutdown_ui();

    // shutdown renderer
    SDL_DestroyRenderer(pRenderer);
    ye_logf(info, "Shut down renderer.\n");

    // shutdown window
    SDL_DestroyWindow(pWindow);
    ye_logf(info, "Shut down window.\n");
}

// helper function to get the current window size, if fullscreen it gets the monitor size
struct ScreenSize ye_get_screen_size(){
    struct ScreenSize screenSize;
    if(SDL_GetWindowFlags(pWindow) & SDL_WINDOW_FULLSCREEN){
        const SDL_DisplayMode * DM = SDL_GetDesktopDisplayMode(0);
        screenSize.width = DM->w;
        screenSize.height = DM->h;
    }
    else if(SDL_GetWindowFlags(pWindow) & SDL_WINDOW_FULLSCREEN){
        SDL_GetWindowSize(pWindow, &screenSize.width, &screenSize.height);
    }
    else {
        SDL_GetWindowSize(pWindow, &screenSize.width, &screenSize.height);
    }
    return screenSize;
}

/*
    API
*/

void ye_set_window_mode(int mode){
    SDL_SetWindowFullscreen(pWindow, mode);
    YE_STATE.engine.window_mode = mode;
    ye_recompute_boxing();
}