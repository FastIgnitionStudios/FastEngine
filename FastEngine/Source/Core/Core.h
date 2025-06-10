#pragma once

#include <memory>
#include <mutex>
#include <unordered_set>

#ifdef PLATFORM_WINDOWS
    #ifdef ENGINE_BUILD_DLL
        #define ENGINE_API
    #else
        #define ENGINE_API
    #endif
#endif

#ifdef ENGINE_ENABLE_ASSERTS
    #define ENGINE_ASSERT(x, ...) { if(!(x)) { ENGINE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
    #define ENGINE_CORE_ASSERT(x, ...) { if(!(x)) { ENGINE_CORE_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
    #define ENGINE_ASSERT(x, ...)
    #define ENGINE_CORE_ASSERT(x, ...)
#endif


#define STR(string) #string
#define TO_STRING(string) STR(string)

#define BIT(x) (1 << x)

#define ENGINE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Engine
{
    template<typename T>
    using Scope = std::unique_ptr<T>;

    //-- Engine base class --//

    class Engine
    {
    public:
        virtual ~Engine() = default;

        void addRef() const
        {
            ++refCount;
        }

        void removeRef() const
        {
            --refCount;
        }

        uint32_t getRefCount() const { return refCount.load(); }

    private:
        mutable std::atomic<uint32_t> refCount = 0;
    };

    namespace Utils
    {
        void AddLivingReference(void* instance);
        void RemoveDeadReference(void* instance);
        bool isAlive(void* instance);
    }
    
}

//-- Reference counting templates --//

    
    template <typename T>
    class Ref
    {
    public:
        Ref() : instance(nullptr)
        {
        }

        Ref(std::nullptr_t n) : instance(nullptr)
        {
        }

        Ref(T* instance) : instance(instance)
        {
            static_assert(std::is_base_of<Engine::Engine, T>::value, "Class is not derived from Engine");

            IncRef();
        }

        template <typename T2>
        Ref(const Ref<T2>& other)
        {
            instance = (T*)other.instance;
            IncRef();
        }

        template <typename T2>
        Ref(Ref<T2>&& other)
        {
            instance = (T*)other.instance;
            other.instance = nullptr;
        }

        ~Ref()
        {
            DecRef();
        }

        Ref(const Ref<T>& other) : instance(other.instance)
        {
            IncRef();
        }

        Ref& operator=(std::nullptr_t)
        {
            DecRef();
            instance = nullptr;
            return *this;
        }

        Ref& operator=(const Ref<T>& other)
        {
            if (this == &other)
                return *this;

            other.IncRef();
            DecRef();

            instance = other.instance;
            return *this;
        }

        template <typename T2>
        Ref& operator=(const Ref<T>& other)
        {
            other.IncRef();
            DecRef();

            instance = other.instance;
            return *this;
        }

        template <typename T2>
        Ref& operator=(Ref<T2>&& other)
        {
            DecRef();

            instance = other.instance;
            other.instance = nullptr;
            return *this;
        }

        operator bool() { return instance != nullptr; }
        operator bool() const { return instance != nullptr; }

        T* operator->() { return instance; }
        const T* operator->() const { return instance; }

        T& operator*() { return *instance; }
        const T& operator*() const { return *instance; }

        T* Raw() { return instance; }
        const T* Raw() const { return instance; }

        void Reset(T* inInstance = nullptr)
        {
            DecRef();
            instance = inInstance;
        }

        template <typename T2>
        Ref<T2> As() const
        {
            return Ref<T2>(*this);
        }

        template <typename... Args>
        static Ref<T> Create(Args&&... args)
        {
            return Ref<T>(new T(std::forward<Args>(args)...));
        }

        bool operator==(const Ref<T>& other) const
        {
            return instance == other.instance;
        }

        bool operator!=(const Ref<T>& other) const
        {
            return instance != other.instance;
        }

        bool EqualsObject(const Ref<T>& other)
        {
            if (!instance || !other.instance)
                return false;
            return *instance == *other.instance;
        }

    private:
        void IncRef() const
        {
            if (instance)
            {
                instance->addRef();
                Engine::Utils::AddLivingReference((void*)instance);
            }
        }

        void DecRef() const
        {
            if (instance)
            {
                instance->removeRef();

                if (instance->getRefCount() == 0)
                {
                    delete instance;
                    Engine::Utils::RemoveDeadReference(instance);
                    instance = nullptr;
                }
            }
        }

        template <class T2>
        friend class Ref;
        mutable T* instance;
    };

#include "Utils/Log.h"
#include "Reflection/Types.h"