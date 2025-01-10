--[[
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
]]

---@enum RendererAligmment The alignment of the renderer
RendererAlignment = {
    TOP_LEFT = 0,
    TOP_CENTER = 1,
    TOP_RIGHT = 2,
    MID_LEFT = 3,
    MID_CENTER = 4,
    MID_RIGHT = 5,
    BOT_LEFT = 6,
    BOT_CENTER = 7,
    BOT_RIGHT = 8,
    STRETCH = 9
}

---@enum RendererType The type of the renderer
RendererType = {
    TEXT = 0,
    TEXT_OUTLINED = 1,
    IMAGE = 2,
    ANIMATION = 3,
    TILE = 4
}

---@class Image The image renderer impl
---@field src string
Image = {
    -- no **real** fields.
    -- This exists purely for intellisense
}

Image_mt = {
    -- there is only one field, so just inline it rather than with a table/enum/indexer

    __index = function(self, key)
        return ValidateAndQuery(self, key, { src = 1 }, ye_lua_image_renderer_query, "Image")
    end,

    __newindex = function(self, key, value)
        return ValidateAndModify(self, key, value, { src = 1 }, ye_lua_image_renderer_modify, "Image")
    end,
}

---@class Text The text renderer impl
---@field text string
---@field fontName string
---@field colorName string
---@field fontSize number
---@field wrapWidth number
Text = {
    -- no **real** fields.
    -- This exists purely for intellisense
}

local TextIndexer = {
    text = 1,
    fontName = 2,
    fontSize = 3,
    colorName = 4,
    wrapWidth = 5,
}

Text_mt = {
    __index = function(self, key)
        return ValidateAndQuery(self, key, TextIndexer, ye_lua_text_renderer_query, "Text")
    end,

    __newindex = function(self, key, value)
        return ValidateAndModify(self, key, value, TextIndexer, ye_lua_text_renderer_modify, "Text")
    end,
}

---@class TextOutlined
---@field text string
---@field fontName string
---@field colorName string
---@field fontSize number
---@field wrapWidth number
---@field outlineSize number
---@field outlineColorName string
TextOutlined = {
    -- no **real** fields.
    -- This exists purely for intellisense
}

local TextOutlinedIndexer = {
    text = 1,
    fontName = 2,
    fontSize = 3,
    colorName = 4,
    wrapWidth = 5,
    outlineSize = 6,
    outlineColorName = 7,
}

TextOutlined_mt = {
    __index = function(self, key)
        return ValidateAndQuery(self, key, TextOutlinedIndexer, ye_lua_text_outlined_renderer_query, "TextOutlined")
    end,

    __newindex = function(self, key, value)
        return ValidateAndModify(self, key, value, TextOutlinedIndexer, ye_lua_text_outlined_renderer_modify, "TextOutlined")
    end,
}

---@class Tile
---
---The origin (0,0) of the tilemap is in the top left, and x increases to the right, y increases down.
---
---@field handle string
---@field srcX number The (pixel) offset of the x position of the tile in the tileset
---@field srcY number The (pixel) offset of the y position of the tile in the tileset
---@field srcW number The width of the tile in the tileset
---@field srcH number The height of the tile in the tileset
Tile = {
    -- no **real** fields.
    -- This exists purely for intellisense
}

local TileIndexer = {
    handle = 1,
    srcX = 2,
    srcY = 3,
    srcW = 4,
    srcH = 5,
}

Tile_mt = {
    __index = function(self, key)
        return ValidateAndQuery(self, key, TileIndexer, ye_lua_tile_renderer_query, "Tile")
    end,

    __newindex = function(self, key, value)
        return ValidateAndModify(self, key, value, TileIndexer, ye_lua_tile_renderer_modify, "Tile")
    end,
}

--- ik ur ass is coming back to overhaul this later, so hi lol

---@class Animation
---@field paused boolean
---@field metaFile string
---@field frameDelay number
---@field currentFrame number
---@field frameCount number Should probably not be changed directly, unless you know what youre doing
---@field frameWidth number Should probably not be changed directly, unless you know what youre doing
---@field frameHeight number Should probably not be changed directly, unless you know what youre doing
---@field imageHandle string Should probably not be changed directly, unless you know what youre doing
Animation = {
    -- no **real** fields.
    -- This exists purely for intellisense
}

local AnimationIndexer = {
    paused = 1,
    metaFile = 2,
    frameDelay = 3,
    currentFrame = 4,
    frameCount = 5,
    frameWidth = 6,
    frameHeight = 7,
    imageHandle = 8
}

Animation_mt = {
    __index = function(self, key)
        return ValidateAndQuery(self, key, AnimationIndexer, ye_lua_animation_renderer_query, "Animation")
    end,

    __newindex = function(self, key, value)
        return ValidateAndModify(self, key, value, AnimationIndexer, ye_lua_animation_renderer_modify, "Animation")
    end,
}

---@class Renderer
---@field isActive boolean Controls whether the camera is active
---@field isRelative boolean Controls whether the position of the camera is relative to a root transform
---@field alpha number The alpha to render at
---@field z number The z index
---@field x number The x position
---@field y number The y position
---@field w number The width
---@field h number The height
---@field alignment integer The alignment of the renderer
---@field rotation number The clockwise rotation in degrees to render at
---@field flipX boolean Flip the renderer on the x axis
---@field flipY boolean Flip the renderer on the y axis
---@field preserveOriginalSize boolean Controls whether the texture grows/shrinks or stays the same size when aligining in bounds
---@field type RendererType The current type of the renderer_impl
---@field Image Image The image renderer impl
---@field Text Text The text renderer impl
---@field TextOutlined TextOutlined The text renderer impl
---@field Animation Animation The animation renderer impl
---@field Tile Tile The tile renderer impl
Renderer = {
    -- no **real** fields.
    -- This exists purely for intellisense
}

local RendererIndexer = {
    isActive = 1,
    isRelative = 2,
    alpha = 3,
    z = 4,
    x = 5,
    y = 6,
    w = 7,
    h = 8,
    alignment = 9,
    rotation = 10,
    flipX = 11,
    flipY = 12,
    preserveOriginalSize = 13,
    type = 14
}

local function wrapRendererTypeAccess(self, indx, mt)
    local parent_ptr = rawget(self, "parent_ptr")
    if not ye_lua_check_renderer_component_type_exists(parent_ptr, indx) then return nil end
    return CreateProxyToComponent(self, mt)
end

Renderer_mt = {
    __index = function(self, key)
        -- check if we are accessing an impl field
        if key == "Text" then
            return wrapRendererTypeAccess(self, 0, Image_mt)
        elseif key == "TextOutlined" then
            return wrapRendererTypeAccess(self, 1, Text_mt)
        elseif key == "Image" then
            return wrapRendererTypeAccess(self, 2, TextOutlined_mt)
        elseif key == "Animation" then
            return wrapRendererTypeAccess(self, 3, Animation_mt)
        elseif key == "Tile" then
            return wrapRendererTypeAccess(self, 4, Tile_mt)
        end

        -- fallback: this is a field on renderer itself
        return ValidateAndQuery(self, key, RendererIndexer, ye_lua_renderer_query, "Renderer")
    end,
        
    __newindex = function(self, key, value)
        -- check if we are accessing an impl field
        if key == "Text" then
            return wrapRendererTypeAccess(self, 0, Image_mt)
        elseif key == "TextOutlined" then
            return wrapRendererTypeAccess(self, 1, Text_mt)
        elseif key == "Image" then
            return wrapRendererTypeAccess(self, 2, TextOutlined_mt)
        elseif key == "Animation" then
            return wrapRendererTypeAccess(self, 3, Animation_mt)
        elseif key == "Tile" then
            return wrapRendererTypeAccess(self, 4, Tile_mt)
        end

        -- fallback: this is a field on renderer itself
        return ValidateAndModify(self, key, value, RendererIndexer, ye_lua_renderer_modify, "Renderer")
    end,
}

-- TODO: im mirroring the C constructer, in the future provide overload params that are optional
---**Create a new image renderer component.**
---
---@param handle string The path to the image to render relative to resources/
---@param z number The z index of the renderer
function Entity:AddImageRendererComponent(handle, z) end -- fake prototype for intellisense
function AddImageRendererComponent(self, handle, z)
    Entity:addComponent(self, ye_lua_create_image_renderer, handle, z)
end

---**Create a new text renderer component.**
---
---This text will create a width and height based on its point size,
---which can be modified through its table fields afterwards and will
---resize itself following its aligment rules in the renderer component
---
---@param text string The text to render
---@param fontName string The name of the (cached) font to use
---@param fontSize number The size of the font to use
---@param colorName string The name of the (cached) color to use
---@param z number The z index of the renderer
function Entity:AddTextRendererComponent(text, fontName, fontSize, colorName, z) end -- fake prototype for intellisense
function AddTextRendererComponent(self, text, fontName, fontSize, colorName, z)
    Entity:addComponent(self, ye_lua_create_text_renderer, text, fontName, fontSize, colorName, z)
end

---**Create a new text outlined renderer component.**
---
---This text will create a width and height based on its point size,
---which can be modified through its table fields afterwards and will
---resize itself following its aligment rules in the renderer component
---
---@param text string The text to render
---@param fontName string The name of the (cached) font to use
---@param fontSize number The size of the font to use
---@param colorName string The name of the (cached) color to use
---@param outlineSize number The size of the outline
---@param outlineColorName string The name of the (cached) color to use for the outline
---@param z number The z index of the renderer
function Entity:AddTextOutlinedRendererComponent(text, fontName, fontSize, colorName, outlineSize, outlineColorName, z) end -- fake prototype for intellisense
function AddTextOutlinedRendererComponent(self, text, fontName, fontSize, colorName, outlineSize, outlineColorName, z)
    Entity:addComponent(self, ye_lua_create_text_outlined_renderer, text, fontName, fontSize, colorName, outlineSize, outlineColorName, z)
end

---**Create a new tile renderer component.**
---
---@param handle string The path to the tileset to use
---@param srcX number The (pixel) offset of the x position of the tile in the tileset
---@param srcY number The (pixel) offset of the y position of the tile in the tileset
---@param srcW number The width of the tile in the tileset
---@param srcH number The height of the tile in the tileset
---@param z number The z index of the renderer
function Entity:AddTileRendererComponent(handle, srcX, srcY, srcW, srcH, z) end -- fake prototype for intellisense
function AddTileRendererComponent(self, handle, srcX, srcY, srcW, srcH, z)
    Entity:addComponent(self, ye_lua_create_tile_renderer, handle, srcX, srcY, srcW, srcH, z)
end

---**Create a new animation renderer component.**
---
---@param metaFile string The path to the animation meta file to use
---@param z number The z index of the renderer
function Entity:AddAnimationRendererComponent(metaFile, z) end -- fake prototype for intellisense
function AddAnimationRendererComponent(self, metaFile, z)
    Entity:addComponent(self, ye_lua_create_animation_renderer, metaFile, z)
end

---**Remove the renderer component from the entity.**
function Entity:RemoveRendererComponent() end -- fake prototype for intellisense
function RemoveRendererComponent(self)
    ye_lua_remove_component(rawget(self, "_c_entity"), RENDERER_COMPONENT)
end