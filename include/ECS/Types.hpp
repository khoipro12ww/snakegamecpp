#pragma once

#include <cstdint>
#include <bitset>

namespace ECS {
    // Entity ID type - sử dụng 32-bit integer
    using EntityID = std::uint32_t;
    
    // Component type ID - sử dụng 8-bit integer (hỗ trợ tối đa 256 component types)
    using ComponentTypeID = std::uint8_t;
    
    // Maximum number of entities
    constexpr EntityID MAX_ENTITIES = 5000;
    
    // Maximum number of component types
    constexpr ComponentTypeID MAX_COMPONENTS = 32;
    
    // Signature để track components của entity
    using Signature = std::bitset<MAX_COMPONENTS>;
    
    // Invalid entity constant
    constexpr EntityID INVALID_ENTITY = 0;
}