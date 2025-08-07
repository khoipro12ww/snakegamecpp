#pragma once

#include "Types.hpp"
#include <set>
#include <unordered_map>
#include <memory>
#include <typeinfo>
#include <cassert>

namespace ECS {
    
    // Forward declaration
    class World;
    
    // Base System class
    class System {
    protected:
        std::set<EntityID> m_entities;
        World* m_world;
        
    public:
        System() : m_world(nullptr) {}
        virtual ~System() = default;
        
        // Set world reference
        void SetWorld(World* world) { m_world = world; }
        
        // Virtual methods để override
        virtual void Init() {}
        virtual void Update(float deltaTime) {}
        virtual void Render() {}
        virtual void Shutdown() {}
        
        // Entity management
        void AddEntity(EntityID entity) {
            m_entities.insert(entity);
        }
        
        void RemoveEntity(EntityID entity) {
            m_entities.erase(entity);
        }
        
        const std::set<EntityID>& GetEntities() const {
            return m_entities;
        }
        
        std::size_t GetEntityCount() const {
            return m_entities.size();
        }
    };
    
    class SystemManager {
    private:
        // Map từ string type name đến signature
        std::unordered_map<const char*, Signature> m_signatures;
        
        // Map từ string type name đến system pointer
        std::unordered_map<const char*, std::shared_ptr<System>> m_systems;
        
    public:
        template<typename T>
        std::shared_ptr<T> RegisterSystem() {
            const char* typeName = typeid(T).name();
            
            assert(m_systems.find(typeName) == m_systems.end() && "Registering system more than once.");
            
            // Create system và add to map
            auto system = std::make_shared<T>();
            m_systems.insert({typeName, system});
            
            return system;
        }
        
        template<typename T>
        void SetSignature(Signature signature) {
            const char* typeName = typeid(T).name();
            
            assert(m_systems.find(typeName) != m_systems.end() && "System used before registered.");
            
            // Set signature cho system
            m_signatures.insert({typeName, signature});
        }
        
        template<typename T>
        std::shared_ptr<T> GetSystem() {
            const char* typeName = typeid(T).name();
            
            assert(m_systems.find(typeName) != m_systems.end() && "System not registered.");
            
            return std::static_pointer_cast<T>(m_systems[typeName]);
        }
        
        void EntityDestroyed(EntityID entity) {
            // Remove entity từ tất cả systems
            for (auto const& pair : m_systems) {
                auto const& system = pair.second;
                system->RemoveEntity(entity);
            }
        }
        
        void EntitySignatureChanged(EntityID entity, Signature entitySignature) {
            // Notify tất cả systems rằng entity signature đã thay đổi
            for (auto const& pair : m_systems) {
                auto const& type = pair.first;
                auto const& system = pair.second;
                auto const& systemSignature = m_signatures[type];
                
                // Check nếu entity signature match với system signature
                if ((entitySignature & systemSignature) == systemSignature) {
                    system->AddEntity(entity);
                } else {
                    system->RemoveEntity(entity);
                }
            }
        }
        
        // Initialize tất cả systems
        void InitAllSystems() {
            for (auto const& pair : m_systems) {
                pair.second->Init();
            }
        }
        
        // Update tất cả systems
        void UpdateAllSystems(float deltaTime) {
            for (auto const& pair : m_systems) {
                pair.second->Update(deltaTime);
            }
        }
        
        // Render tất cả systems
        void RenderAllSystems() {
            for (auto const& pair : m_systems) {
                pair.second->Render();
            }
        }
        
        // Shutdown tất cả systems
        void ShutdownAllSystems() {
            for (auto const& pair : m_systems) {
                pair.second->Shutdown();
            }
        }
    };
    
}