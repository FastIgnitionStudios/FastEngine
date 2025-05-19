#pragma once

#include "Event.h"

#include <sstream>

namespace Engine
{
    class KeyEvent : public Event
    {
    public:
        inline int GetKeyCode() const { return KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        KeyEvent(int keycode) : KeyCode(keycode) {}
        int KeyCode;
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(int keycode, int repeatCount)
            : KeyEvent(keycode), RepeatCount(repeatCount) {}

        inline int GetRepeatCount() const { return RepeatCount; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << KeyCode << "(" << RepeatCount << " repeates)";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        int RepeatCount;
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(int keycode)
            : KeyEvent(keycode) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << KeyCode;
            return ss.str();
        }
        
        EVENT_CLASS_TYPE(KeyReleased)
        
    };
}