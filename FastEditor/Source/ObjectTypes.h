#pragma once
#include <map>

struct ObjectDeferredInitInfo
{
    void*           (*ClassRegisterFunc)();
    const char*     ClassName;
};

inline void RegisterDeferredInit(const ObjectDeferredInitInfo* info)
{
    info->ClassRegisterFunc();
}

class ObjectRegistry
{
public:
    void Register(const ObjectDeferredInitInfo* info)
    {
        DeferredInitList[info->ClassName] = (info);
    }
    const std::map<std::string, const ObjectDeferredInitInfo*> GetDeferredInitList() { return DeferredInitList; }
    void ForceRegistration()
    {
        for (auto& info : GetDeferredInitList())
        {
            info.second->ClassRegisterFunc();
        }
        volatile auto* ptr = &DeferredInitList;
        (void)ptr;
    }

    static ObjectRegistry& Get()
    {
        static ObjectRegistry registry;
        return registry;
    }
private:
    std::map<std::string, const ObjectDeferredInitInfo*> DeferredInitList;
};


struct RegisterObjectDeferred
{
    template<typename ... Args>
    RegisterObjectDeferred(Args&&... args)
    {
        ObjectRegistry::Get().Register(std::forward<Args>(args)...);
        RegisterDeferredInit(std::forward<Args>(args)...);
    }
};