#pragma once

namespace Engine
{
    class Event;

    class Layer
    {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer();

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate() {}
        virtual void OnRender() {}
        virtual void OnEvent(Event& e) {}
        
        inline const std::string& GetName() const { return DebugName; }
    protected:
        std::string DebugName;
    };
}
