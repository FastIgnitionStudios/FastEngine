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
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender(float deltaTime) {}
        virtual void OnEvent(Event& e) {}
        
        inline const std::string& GetName() const { return DebugName; }
    protected:
        std::string DebugName;
    };
}
