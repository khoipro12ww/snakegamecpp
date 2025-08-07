#include "ECS/Entity.hpp"
#include <cassert>

namespace ECS {
    
    EntityManager::EntityManager() : m_livingEntityCount(0) {
        // Initialize queue với available entity IDs
        for (EntityID entity = 1; entity < MAX_ENTITIES; ++entity) {
            m_availableEntities.push(entity);
        }
    }
    
    EntityID EntityManager::CreateEntity() {
        assert(m_livingEntityCount < MAX_ENTITIES && "Too many entities in existence.");
        
        // Take ID từ front của queue
        EntityID id = m_availableEntities.front();
        m_availableEntities.pop();
        ++m_livingEntityCount;
        
        return id;
    }
    
    void EntityManager::DestroyEntity(EntityID entity) {
        assert(entity < MAX_ENTITIES && "Entity out of range.");
        
        // Invalidate signature
        m_signatures[entity].reset();
        
        // Put destroyed ID vào back của queue
        m_availableEntities.push(entity);
        --m_livingEntityCount;
    }
    
    void EntityManager::SetSignature(EntityID entity, Signature signature) {
        assert(entity < MAX_ENTITIES && "Entity out of range.");
        
        m_signatures[entity] = signature;
    }
    
    Signature EntityManager::GetSignature(EntityID entity) const {
        assert(entity < MAX_ENTITIES && "Entity out of range.");
        
        return m_signatures[entity];
    }
    
    bool EntityManager::IsEntityValid(EntityID entity) const {
        if (entity == INVALID_ENTITY || entity >= MAX_ENTITIES) {
            return false;
        }
        
        // Check if entity has any components (signature is not empty)
        return m_signatures[entity].any();
    }
    
    std::uint32_t EntityManager::GetLivingEntityCount() const {
        return m_livingEntityCount;
    }
    
}