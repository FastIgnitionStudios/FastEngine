#pragma once

// Create basic types that can be retrieved and modified in EDITOR

struct PropertyBaseParams
{
    const char* PropertyName;
    const char* PropertyType;
    void (*PropertySetter)(void* Instance, const void* InValue);
    void (*PropertyGetter)(const void* Intstance, void* InValue);
};