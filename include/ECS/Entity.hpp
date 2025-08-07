#pragma once

#include "Types.hpp"
#include <queue>
#include <array>

namespace ECS {
    
    class EntityManager {
    private:
        // Queue các entity ID có thể sử dụng
        std::queue<EntityID> m_availableEntities;
        
        // Signature của từng entity
        std::array<Signature, MAX_ENTITIES> m_signatures;
        
        // Số lượng entity hiện tại
        std::uint32_t m_livingEntityCount;
        
    public:
        EntityManager();
        
        // Tạo entity mới
        EntityID CreateEntity();
        
        // Hủy entity
        void DestroyEntity(EntityID entity);
        
        // Set signature cho entity
        void SetSignature(EntityID entity, Signature signature);
        
        // Get signature của entity
        Signature GetSignature(EntityID entity) const;
        
        // Check entity có hợp lệ không
        bool IsEntityValid(EntityID entity) const;
        
        // Get số lượng entity hiện tại
        std::uint32_t GetLivingEntityCount() const;
    };
    
}