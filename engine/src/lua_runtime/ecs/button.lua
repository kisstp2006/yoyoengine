--[[
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
]]

---@class Button
---@field isActive boolean Controls whether the camera is active
---@field isRelative boolean Controls whether the position of the camera is relative to a root transform
---@field x number The x position
---@field y number The y position
---@field w number The width
---@field h number The height
---@field isHovered boolean Whether the button is hovered or not
---@field isPressed boolean Whether the button is pressed down
---@field isClicked boolean Whether the button was released after a press for a "click"
Button = {
    -- no **real** fields.
    -- This exists purely for intellisense
}

local buttonIndexer = {
    isActive = 1,
    isRelative = 2,
    x = 3,
    y = 4,
    w = 5,
    h = 6,
    -- isHovered = 7,
    -- isPressed = 8,
    -- isClicked = 9,
}

Button_mt = {
    __index = function(self, key)

        local parent_ptr = rawget(self, "parent_ptr")

        -- intercept isHovered, isPressed, and isClicked
        if key == "isHovered" then
            return ye_lua_button_check_state(parent_ptr, 1)
        elseif key == "isPressed" then
            return ye_lua_button_check_state(parent_ptr, 2)
        elseif key == "isClicked" then
            return ye_lua_button_check_state(parent_ptr, 3)
        end

        return ValidateAndQuery(self, key, buttonIndexer, ye_lua_button_query, "Button")
    end,

    __newindex = function(self, key, value)
        -- modifying isXXXX is illegal, at least for now.
        
        return ValidateAndModify(self, key, value, buttonIndexer, ye_lua_button_modify, "Button")
    end,
}

---**Create a new button component.**
---
---@param x number The x position of the button
---@param y number The y position of the button
---@param w number The width of the button
---@param h number The height of the button
function Entity:AddButtonComponent(x, y, w, h) end -- fake prototype for intellisense
function AddButtonComponent(self, x, y, w, h)
    if x and y and w and h then
        Entity:addComponent(self, ye_lua_create_button, x, y, w, h)
    end
end

---**Remove the button component.**
function Entity:RemoveButtonComponent() end -- fake prototype for intellisense
function RemoveButtonComponent(self)
    ye_lua_remove_component(rawget(self, "_c_entity"), BUTTON_COMPONENT)
end