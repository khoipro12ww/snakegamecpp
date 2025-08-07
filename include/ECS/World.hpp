#pragma once

#include "Entity.hpp"
#include "Component.hpp"
#include "System.hpp"
#include <memory>

namespace ECS {
    
    class World {
    private:
        std::unique_ptr<ComponentManager> m_componentManager;
        std::unique_ptr<EntityManager> m_entityManager;
        std::unique_ptr<SystemManager> m_systemManager;
        
    public:
        World() {
            // Create managers
            m_componentManager = std::make_unique<ComponentManager>();
            m_entityManager = std::make_unique<EntityManager>();
            m_systemManager = std::make_unique<SystemManager>();
        }
        
        // Entity methods
        EntityID CreateEntity() {
            return m_entityManager->CreateEntity();
        }
        
        void DestroyEntity(EntityID entity) {
            m_entityManager->DestroyEntity(entity);
            m_componentManager->EntityDestroyed(entity);
            m_systemManager->EntityDestroyed(entity);
        }
        
        bool IsEntityValid(EntityID entity) const {
            return m_entityManager->IsEntityValid(entity);
        }
        
        std::uint32_t GetLivingEntityCount() const {
            return m_entityManager->GetLivingEntityCount();
        }
        
        // Component methods
        template<typename T>
        void RegisterComponent() {
            m_componentManager->RegisterComponent<T>();
        }
        
        template<typename T>
        void AddComponent(EntityID entity, T component) {
            m_componentManager->AddComponent<T>(entity, component);
            
            auto signature = m_entityManager->GetSignature(entity);
            signature.set(m_componentManager->GetComponentType<T>(), true);
            m_entityManager->SetSignature(entity, signature);
            
            m_systemManager->EntitySignatureChanged(entity, signature);
        }
        
        template<typename T>
        void RemoveComponent(EntityID entity) {
            m_componentManager->RemoveComponent<T>(entity);
            
            auto signature = m_entityManager->GetSignature(entity);
            signature.set(m_componentManager->GetComponentType<T>(), false);
            m_entityManager->SetSignature(entity, signature);
            
            m_systemManager->EntitySignatureChanged(entity, signature);
        }
        
        template<typename T>
        T& GetComponent(EntityID entity) {
            return m_componentManager->GetComponent<T>(entity);
        }
        
        template<typename T>
        bool HasComponent(EntityID entity) {
            return m_componentManager->HasComponent<T>(entity);
        }
        
        template<typename T>
        ComponentTypeID GetComponentType() {
            return m_componentManager->GetComponentType<T>();
        }
        
        // System methods
        template<typename T>
        std::shared_ptr<T> RegisterSystem() {
            auto system = m_systemManager->RegisterSystem<T>();
            system->SetWorld(this);
            return system;
        }
        
        template<typename T>
        void SetSystemSignature(Signature signature) {
            m_systemManager->SetSignature<T>(signature);
        }
        
        template<typename T>
        std::shared_ptr<T> GetSystem() {
            return m_systemManager->GetSystem<T>();
        }
        
        // System lifecycle
        void Init() {
            m_systemManager->InitAllSystems();
        }
        
        void Update(float deltaTime) {
            m_systemManager->UpdateAllSystems(deltaTime);
        }
        
        void Render() {
            m_systemManager->RenderAllSystems();
        }
        
        void Shutdown() {
            m_systemManager->ShutdownAllSystems();
        }
        
        // Utility methods
        Signature GetEntitySignature(EntityID entity) const {
            return m_entityManager->GetSignature(entity);
        }
    };
    
}