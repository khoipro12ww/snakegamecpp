#pragma once

// Core ECS
#include "Types.hpp"
#include "Entity.hpp"
#include "Component.hpp"
#include "System.hpp"
#include "World.hpp"

// Components
#include "Components.hpp"

// Systems
#include "Systems.hpp"

// Convenience using declarations
namespace ECS {
    // Make commonly used types easier to access
    using Entity = EntityID;
}