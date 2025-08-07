#pragma once

#include "Types.hpp"
#include <unordered_map>
#include <memory>
#include <typeinfo>
#include <array>
#include <cassert>

namespace ECS {
    
    // Base interface cho component arrays
    class IComponentArray {
    public:
        virtual ~IComponentArray() = default;
        virtual void EntityDestroyed(EntityID entity) = 0;
    };
    
    // Template component array với memory pool
    template<typename T>
    class ComponentArray : public IComponentArray {
    private:
        // Array chứa components, indexed bởi entity ID
        std::array<T, MAX_ENTITIES> m_componentArray;
        
        // Map từ entity ID đến array index
        std::unordered_map<EntityID, std::size_t> m_entityToIndexMap;
        
        // Map từ array index đến entity ID
        std::unordered_map<std::size_t, EntityID> m_indexToEntityMap;
        
        // Số lượng components hiện tại
        std::size_t m_size;
        
    public:
        ComponentArray() : m_size(0) {}
        
        void InsertData(EntityID entity, T component) {
            assert(m_entityToIndexMap.find(entity) == m_entityToIndexMap.end() && "Component added to same entity more than once.");
            
            // Thêm component vào cuối array
            std::size_t newIndex = m_size;
            m_entityToIndexMap[entity] = newIndex;
            m_indexToEntityMap[newIndex] = entity;
            m_componentArray[newIndex] = component;
            ++m_size;
        }
        
        void RemoveData(EntityID entity) {
            assert(m_entityToIndexMap.find(entity) != m_entityToIndexMap.end() && "Removing non-existent component.");
            
            // Copy element cuối cùng vào vị trí của element bị remove để maintain dense array
            std::size_t indexOfRemovedEntity = m_entityToIndexMap[entity];
            std::size_t indexOfLastElement = m_size - 1;
            m_componentArray[indexOfRemovedEntity] = m_componentArray[indexOfLastElement];
            
            // Update map để point đến moved element's new location
            EntityID entityOfLastElement = m_indexToEntityMap[indexOfLastElement];
            m_entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
            m_indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;
            
            m_entityToIndexMap.erase(entity);
            m_indexToEntityMap.erase(indexOfLastElement);
            
            --m_size;
        }
        
        T& GetData(EntityID entity) {
            assert(m_entityToIndexMap.find(entity) != m_entityToIndexMap.end() && "Retrieving non-existent component.");
            return m_componentArray[m_entityToIndexMap[entity]];
        }
        
        bool HasComponent(EntityID entity) const {
            return m_entityToIndexMap.find(entity) != m_entityToIndexMap.end();
        }
        
        void EntityDestroyed(EntityID entity) override {
            if (m_entityToIndexMap.find(entity) != m_entityToIndexMap.end()) {
                RemoveData(entity);
            }
        }
        
        // Iterator support cho systems
        T* begin() { return &m_componentArray[0]; }
        T* end() { return &m_componentArray[m_size]; }
        const T* begin() const { return &m_componentArray[0]; }
        const T* end() const { return &m_componentArray[m_size]; }
        
        std::size_t size() const { return m_size; }
    };
    
    class ComponentManager {
    private:
        // Map từ string type name đến component type
        std::unordered_map<const char*, ComponentTypeID> m_componentTypes;
        
        // Map từ string type name đến component array
        std::unordered_map<const char*, std::shared_ptr<IComponentArray>> m_componentArrays;
        
        // Component type được assign tiếp theo
        ComponentTypeID m_nextComponentType;
        
        // Get component array của type T
        template<typename T>
        std::shared_ptr<ComponentArray<T>> GetComponentArray() {
            const char* typeName = typeid(T).name();
            
            assert(m_componentTypes.find(typeName) != m_componentTypes.end() && "Component not registered before use.");
            
            return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[typeName]);
        }
        
    public:
        ComponentManager() : m_nextComponentType(0) {}
        
        template<typename T>
        void RegisterComponent() {
            const char* typeName = typeid(T).name();
            
            assert(m_componentTypes.find(typeName) == m_componentTypes.end() && "Registering component type more than once.");
            
            // Add component type to map
            m_componentTypes.insert({typeName, m_nextComponentType});
            
            // Create ComponentArray pointer và add to map
            m_componentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});
            
            // Increment type counter
            ++m_nextComponentType;
        }
        
        template<typename T>
        ComponentTypeID GetComponentType() {
            const char* typeName = typeid(T).name();
            
            assert(m_componentTypes.find(typeName) != m_componentTypes.end() && "Component not registered before use.");
            
            return m_componentTypes[typeName];
        }
        
        template<typename T>
        void AddComponent(EntityID entity, T component) {
            GetComponentArray<T>()->InsertData(entity, component);
        }
        
        template<typename T>
        void RemoveComponent(EntityID entity) {
            GetComponentArray<T>()->RemoveData(entity);
        }
        
        template<typename T>
        T& GetComponent(EntityID entity) {
            return GetComponentArray<T>()->GetData(entity);
        }
        
        template<typename T>
        bool HasComponent(EntityID entity) {
            return GetComponentArray<T>()->HasComponent(entity);
        }
        
        void EntityDestroyed(EntityID entity) {
            // Notify tất cả component arrays rằng entity đã destroyed
            for (auto const& pair : m_componentArrays) {
                auto const& component = pair.second;
                component->EntityDestroyed(entity);
            }
        }
    };
    
}