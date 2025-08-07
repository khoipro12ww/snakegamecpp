#pragma once

#include "System.hpp"
#include "Components.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>
#include <cmath>

namespace ECS {
    
    // Movement System - xử lý velocity và movement
    class MovementSystem : public System {
    public:
        void Update(float deltaTime) override {
            for (auto const& entity : m_entities) {
                auto& transform = m_world->GetComponent<Transform>(entity);
                auto& velocity = m_world->GetComponent<Velocity>(entity);
                
                // Apply acceleration
                velocity.velocity += velocity.acceleration * deltaTime;
                
                // Apply friction
                velocity.velocity *= velocity.friction;
                
                // Clamp to max speed
                float speed = std::sqrt(velocity.velocity.x * velocity.velocity.x + 
                                      velocity.velocity.y * velocity.velocity.y);
                if (speed > velocity.maxSpeed) {
                    velocity.velocity = (velocity.velocity / speed) * velocity.maxSpeed;
                }
                
                // Update position
                transform.position += velocity.velocity * deltaTime;
            }
        }
    };
    
    // Physics System - xử lý gravity và physics
    class PhysicsSystem : public System {
    private:
        sf::Vector2f m_gravity{0.0f, 980.0f}; // pixels per second squared
        
    public:
        void SetGravity(sf::Vector2f gravity) {
            m_gravity = gravity;
        }
        
        void Update(float deltaTime) override {
            for (auto const& entity : m_entities) {
                auto& velocity = m_world->GetComponent<Velocity>(entity);
                auto& rigidbody = m_world->GetComponent<Rigidbody>(entity);
                
                // Apply gravity
                if (rigidbody.useGravity) {
                    sf::Vector2f gravityForce = {
                        m_gravity.x * rigidbody.gravityScale.x,
                        m_gravity.y * rigidbody.gravityScale.y
                    };
                    velocity.acceleration += gravityForce / rigidbody.mass;
                }
                
                // Apply drag
                velocity.velocity *= (1.0f - rigidbody.drag * deltaTime);
            }
        }
    };
    
    // Collision System - xử lý AABB collision detection
    class CollisionSystem : public System {
    public:
        struct CollisionInfo {
            EntityID entityA, entityB;
            sf::Vector2f normal;
            float penetration;
            bool isTrigger;
        };
        
    private:
        std::vector<CollisionInfo> m_collisions;
        
        bool AABBvsAABB(const sf::FloatRect& a, const sf::FloatRect& b, sf::Vector2f& normal, float& penetration) {
            sf::Vector2f aPos(a.left + a.width / 2.0f, a.top + a.height / 2.0f);
            sf::Vector2f bPos(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
            
            sf::Vector2f distance = bPos - aPos;
            sf::Vector2f minDistance(a.width / 2.0f + b.width / 2.0f, a.height / 2.0f + b.height / 2.0f);
            
            if (std::abs(distance.x) >= minDistance.x || std::abs(distance.y) >= minDistance.y) {
                return false;
            }
            
            // Calculate penetration
            sf::Vector2f pen = minDistance - sf::Vector2f(std::abs(distance.x), std::abs(distance.y));
            
            if (pen.x < pen.y) {
                normal = sf::Vector2f(distance.x > 0 ? 1.0f : -1.0f, 0.0f);
                penetration = pen.x;
            } else {
                normal = sf::Vector2f(0.0f, distance.y > 0 ? 1.0f : -1.0f);
                penetration = pen.y;
            }
            
            return true;
        }
        
        bool CirclevsCircle(const sf::Vector2f& posA, float radiusA, const sf::Vector2f& posB, float radiusB,
                           sf::Vector2f& normal, float& penetration) {
            sf::Vector2f distance = posB - posA;
            float distanceLength = std::sqrt(distance.x * distance.x + distance.y * distance.y);
            float minDistance = radiusA + radiusB;
            
            if (distanceLength >= minDistance) {
                return false;
            }
            
            penetration = minDistance - distanceLength;
            if (distanceLength > 0) {
                normal = distance / distanceLength;
            } else {
                normal = sf::Vector2f(1.0f, 0.0f); // Arbitrary direction for overlapping circles
            }
            
            return true;
        }
        
    public:
        void Update(float deltaTime) override {
            m_collisions.clear();
            
            // Convert set to vector for indexing
            std::vector<EntityID> entities(m_entities.begin(), m_entities.end());
            
            // Check all pairs
            for (std::size_t i = 0; i < entities.size(); ++i) {
                for (std::size_t j = i + 1; j < entities.size(); ++j) {
                    EntityID entityA = entities[i];
                    EntityID entityB = entities[j];
                    
                    // Check AABB vs AABB collision
                    if (m_world->HasComponent<Collider>(entityA) && m_world->HasComponent<Collider>(entityB)) {
                        auto& transformA = m_world->GetComponent<Transform>(entityA);
                        auto& transformB = m_world->GetComponent<Transform>(entityB);
                        auto& colliderA = m_world->GetComponent<Collider>(entityA);
                        auto& colliderB = m_world->GetComponent<Collider>(entityB);
                        
                        sf::FloatRect boundsA = colliderA.getBounds(transformA.position);
                        sf::FloatRect boundsB = colliderB.getBounds(transformB.position);
                        
                        sf::Vector2f normal;
                        float penetration;
                        
                        if (AABBvsAABB(boundsA, boundsB, normal, penetration)) {
                            CollisionInfo info;
                            info.entityA = entityA;
                            info.entityB = entityB;
                            info.normal = normal;
                            info.penetration = penetration;
                            info.isTrigger = colliderA.isTrigger || colliderB.isTrigger;
                            m_collisions.push_back(info);
                        }
                    }
                    
                    // Check Circle vs Circle collision
                    if (m_world->HasComponent<CircleCollider>(entityA) && m_world->HasComponent<CircleCollider>(entityB)) {
                        auto& transformA = m_world->GetComponent<Transform>(entityA);
                        auto& transformB = m_world->GetComponent<Transform>(entityB);
                        auto& colliderA = m_world->GetComponent<CircleCollider>(entityA);
                        auto& colliderB = m_world->GetComponent<CircleCollider>(entityB);
                        
                        sf::Vector2f centerA = colliderA.getCenter(transformA.position);
                        sf::Vector2f centerB = colliderB.getCenter(transformB.position);
                        
                        sf::Vector2f normal;
                        float penetration;
                        
                        if (CirclevsCircle(centerA, colliderA.radius, centerB, colliderB.radius, normal, penetration)) {
                            CollisionInfo info;
                            info.entityA = entityA;
                            info.entityB = entityB;
                            info.normal = normal;
                            info.penetration = penetration;
                            info.isTrigger = colliderA.isTrigger || colliderB.isTrigger;
                            m_collisions.push_back(info);
                        }
                    }
                }
            }
            
            // Resolve collisions
            for (const auto& collision : m_collisions) {
                if (!collision.isTrigger) {
                    ResolveCollision(collision);
                }
            }
        }
        
        void ResolveCollision(const CollisionInfo& collision) {
            auto& transformA = m_world->GetComponent<Transform>(collision.entityA);
            auto& transformB = m_world->GetComponent<Transform>(collision.entityB);
            
            bool staticA = false, staticB = false;
            
            // Check if entities are static
            if (m_world->HasComponent<Collider>(collision.entityA)) {
                staticA = m_world->GetComponent<Collider>(collision.entityA).isStatic;
            }
            if (m_world->HasComponent<CircleCollider>(collision.entityA)) {
                staticA = m_world->GetComponent<CircleCollider>(collision.entityA).isStatic;
            }
            if (m_world->HasComponent<Collider>(collision.entityB)) {
                staticB = m_world->GetComponent<Collider>(collision.entityB).isStatic;
            }
            if (m_world->HasComponent<CircleCollider>(collision.entityB)) {
                staticB = m_world->GetComponent<CircleCollider>(collision.entityB).isStatic;
            }
            
            // Position correction
            if (!staticA && !staticB) {
                // Both objects can move
                transformA.position -= collision.normal * (collision.penetration / 2.0f);
                transformB.position += collision.normal * (collision.penetration / 2.0f);
            } else if (!staticA && staticB) {
                // Only A can move
                transformA.position -= collision.normal * collision.penetration;
            } else if (staticA && !staticB) {
                // Only B can move
                transformB.position += collision.normal * collision.penetration;
            }
            
            // Velocity correction
            if (m_world->HasComponent<Velocity>(collision.entityA) && !staticA) {
                auto& velocityA = m_world->GetComponent<Velocity>(collision.entityA);
                // Reflect velocity if moving towards collision
                float dot = velocityA.velocity.x * collision.normal.x + velocityA.velocity.y * collision.normal.y;
                if (dot < 0) {
                    velocityA.velocity -= collision.normal * dot * 2.0f;
                }
            }
            
            if (m_world->HasComponent<Velocity>(collision.entityB) && !staticB) {
                auto& velocityB = m_world->GetComponent<Velocity>(collision.entityB);
                // Reflect velocity if moving towards collision
                float dot = velocityB.velocity.x * (-collision.normal.x) + velocityB.velocity.y * (-collision.normal.y);
                if (dot < 0) {
                    velocityB.velocity -= (-collision.normal) * dot * 2.0f;
                }
            }
        }
        
        const std::vector<CollisionInfo>& GetCollisions() const {
            return m_collisions;
        }
    };
    
    // Animation System - xử lý sprite animations
    class AnimationSystem : public System {
    public:
        void Update(float deltaTime) override {
            for (auto const& entity : m_entities) {
                auto& animation = m_world->GetComponent<Animation>(entity);
                
                if (!animation.isPlaying || animation.frames.empty()) {
                    continue;
                }
                
                animation.currentTime += deltaTime * animation.playbackSpeed;
                
                if (animation.currentTime >= animation.frames[animation.currentFrame].duration) {
                    animation.currentTime = 0.0f;
                    animation.currentFrame++;
                    
                    if (animation.currentFrame >= animation.frames.size()) {
                        if (animation.loop) {
                            animation.currentFrame = 0;
                        } else {
                            animation.currentFrame = animation.frames.size() - 1;
                            animation.isPlaying = false;
                        }
                    }
                    
                    // Update sprite texture rect if entity has SpriteRenderer
                    if (m_world->HasComponent<SpriteRenderer>(entity)) {
                        auto& spriteRenderer = m_world->GetComponent<SpriteRenderer>(entity);
                        spriteRenderer.setTextureRect(animation.frames[animation.currentFrame].textureRect);
                    }
                }
            }
        }
    };
    
    // Lifetime System - xử lý entities có lifetime
    class LifetimeSystem : public System {
    public:
        void Update(float deltaTime) override {
            std::vector<EntityID> entitiesToDestroy;
            
            for (auto const& entity : m_entities) {
                auto& lifetime = m_world->GetComponent<Lifetime>(entity);
                lifetime.remainingTime -= deltaTime;
                
                if (lifetime.remainingTime <= 0.0f) {
                    entitiesToDestroy.push_back(entity);
                }
            }
            
            // Destroy expired entities
            for (auto entity : entitiesToDestroy) {
                m_world->DestroyEntity(entity);
            }
        }
    };
    
    // Render System - xử lý rendering với layers
    class RenderSystem : public System {
    private:
        sf::RenderWindow* m_window;
        
        struct RenderItem {
            EntityID entity;
            int layer;
        };
        
    public:
        RenderSystem(sf::RenderWindow* window) : m_window(window) {}
        
        void Render() override {
            if (!m_window) return;
            
            // Collect all render items
            std::vector<RenderItem> renderItems;
            
            for (auto const& entity : m_entities) {
                RenderItem item;
                item.entity = entity;
                item.layer = 0;
                
                // Get layer from component
                if (m_world->HasComponent<SpriteRenderer>(entity)) {
                    item.layer = m_world->GetComponent<SpriteRenderer>(entity).renderLayer;
                } else if (m_world->HasComponent<ShapeRenderer>(entity)) {
                    item.layer = m_world->GetComponent<ShapeRenderer>(entity).renderLayer;
                } else if (m_world->HasComponent<TextRenderer>(entity)) {
                    item.layer = m_world->GetComponent<TextRenderer>(entity).renderLayer;
                }
                
                renderItems.push_back(item);
            }
            
            // Sort by layer
            std::sort(renderItems.begin(), renderItems.end(), 
                     [](const RenderItem& a, const RenderItem& b) {
                         return a.layer < b.layer;
                     });
            
            // Render each item
            for (const auto& item : renderItems) {
                RenderEntity(item.entity);
            }
        }
        
    private:
        void RenderEntity(EntityID entity) {
            auto& transform = m_world->GetComponent<Transform>(entity);
            
            // Render SpriteRenderer
            if (m_world->HasComponent<SpriteRenderer>(entity)) {
                auto& spriteRenderer = m_world->GetComponent<SpriteRenderer>(entity);
                if (spriteRenderer.visible && spriteRenderer.texture) {
                    spriteRenderer.sprite.setPosition(transform.position);
                    spriteRenderer.sprite.setRotation(transform.rotation);
                    spriteRenderer.sprite.setScale(transform.scale);
                    spriteRenderer.sprite.setColor(spriteRenderer.color);
                    m_window->draw(spriteRenderer.sprite);
                }
            }
            
            // Render ShapeRenderer
            if (m_world->HasComponent<ShapeRenderer>(entity)) {
                auto& shapeRenderer = m_world->GetComponent<ShapeRenderer>(entity);
                if (shapeRenderer.visible) {
                    RenderShape(transform, shapeRenderer);
                }
            }
            
            // Render TextRenderer
            if (m_world->HasComponent<TextRenderer>(entity)) {
                auto& textRenderer = m_world->GetComponent<TextRenderer>(entity);
                if (textRenderer.visible && textRenderer.font) {
                    textRenderer.text.setPosition(transform.position);
                    textRenderer.text.setRotation(transform.rotation);
                    textRenderer.text.setScale(transform.scale);
                    m_window->draw(textRenderer.text);
                }
            }
        }
        
        void RenderShape(const Transform& transform, const ShapeRenderer& shapeRenderer) {
            switch (shapeRenderer.shapeType) {
                case ShapeRenderer::ShapeType::Rectangle: {
                    sf::RectangleShape rect(shapeRenderer.size);
                    rect.setPosition(transform.position);
                    rect.setRotation(transform.rotation);
                    rect.setScale(transform.scale);
                    rect.setOrigin(shapeRenderer.size.x / 2.0f, shapeRenderer.size.y / 2.0f);
                    rect.setFillColor(shapeRenderer.fillColor);
                    rect.setOutlineColor(shapeRenderer.outlineColor);
                    rect.setOutlineThickness(shapeRenderer.outlineThickness);
                    m_window->draw(rect);
                    break;
                }
                case ShapeRenderer::ShapeType::Circle: {
                    sf::CircleShape circle(shapeRenderer.radius);
                    circle.setPosition(transform.position);
                    circle.setRotation(transform.rotation);
                    circle.setScale(transform.scale);
                    circle.setOrigin(shapeRenderer.radius, shapeRenderer.radius);
                    circle.setFillColor(shapeRenderer.fillColor);
                    circle.setOutlineColor(shapeRenderer.outlineColor);
                    circle.setOutlineThickness(shapeRenderer.outlineThickness);
                    m_window->draw(circle);
                    break;
                }
                case ShapeRenderer::ShapeType::Line: {
                    sf::Vertex line[] = {
                        sf::Vertex(transform.position, shapeRenderer.fillColor),
                        sf::Vertex(transform.position + shapeRenderer.endPoint, shapeRenderer.fillColor)
                    };
                    m_window->draw(line, 2, sf::Lines);
                    break;
                }
            }
        }
    };
    
}