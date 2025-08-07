#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <string>
#include <memory>

namespace ECS {
    
    // Transform Component - vị trí, rotation, scale
    struct Transform {
        sf::Vector2f position{0.0f, 0.0f};
        float rotation{0.0f};
        sf::Vector2f scale{1.0f, 1.0f};
        
        Transform() = default;
        Transform(float x, float y) : position(x, y) {}
        Transform(sf::Vector2f pos) : position(pos) {}
        Transform(float x, float y, float rot) : position(x, y), rotation(rot) {}
        Transform(sf::Vector2f pos, float rot, sf::Vector2f sc) : position(pos), rotation(rot), scale(sc) {}
        
        // Helper methods
        sf::Transform getTransform() const {
            sf::Transform transform;
            transform.translate(position);
            transform.rotate(rotation);
            transform.scale(scale);
            return transform;
        }
    };
    
    // Velocity Component - movement
    struct Velocity {
        sf::Vector2f velocity{0.0f, 0.0f};
        sf::Vector2f acceleration{0.0f, 0.0f};
        float maxSpeed{300.0f};
        float friction{0.95f};
        
        Velocity() = default;
        Velocity(float vx, float vy) : velocity(vx, vy) {}
        Velocity(sf::Vector2f vel) : velocity(vel) {}
        Velocity(sf::Vector2f vel, float max) : velocity(vel), maxSpeed(max) {}
    };
    
    // Sprite Renderer Component
    struct SpriteRenderer {
        sf::Sprite sprite;
        sf::Texture* texture;
        sf::Color color{sf::Color::White};
        bool visible{true};
        int renderLayer{0};
        
        SpriteRenderer() : texture(nullptr) {}
        SpriteRenderer(sf::Texture* tex) : texture(tex) {
            if (texture) {
                sprite.setTexture(*texture);
            }
        }
        
        void setTexture(sf::Texture* tex) {
            texture = tex;
            if (texture) {
                sprite.setTexture(*texture);
            }
        }
        
        void setTextureRect(const sf::IntRect& rect) {
            sprite.setTextureRect(rect);
        }
        
        void setOrigin(float x, float y) {
            sprite.setOrigin(x, y);
        }
        
        void setOriginCenter() {
            if (texture) {
                auto bounds = sprite.getLocalBounds();
                sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
            }
        }
    };
    
    // Shape Renderer Component
    struct ShapeRenderer {
        enum class ShapeType { Rectangle, Circle, Line };
        
        ShapeType shapeType{ShapeType::Rectangle};
        sf::Color fillColor{sf::Color::White};
        sf::Color outlineColor{sf::Color::Transparent};
        float outlineThickness{0.0f};
        bool visible{true};
        int renderLayer{0};
        
        // Rectangle specific
        sf::Vector2f size{50.0f, 50.0f};
        
        // Circle specific
        float radius{25.0f};
        
        // Line specific
        sf::Vector2f endPoint{0.0f, 0.0f};
        
        ShapeRenderer() = default;
        
        // Rectangle constructor
        static ShapeRenderer Rectangle(sf::Vector2f sz, sf::Color color = sf::Color::White) {
            ShapeRenderer renderer;
            renderer.shapeType = ShapeType::Rectangle;
            renderer.size = sz;
            renderer.fillColor = color;
            return renderer;
        }
        
        // Circle constructor
        static ShapeRenderer Circle(float r, sf::Color color = sf::Color::White) {
            ShapeRenderer renderer;
            renderer.shapeType = ShapeType::Circle;
            renderer.radius = r;
            renderer.fillColor = color;
            return renderer;
        }
        
        // Line constructor
        static ShapeRenderer Line(sf::Vector2f end, sf::Color color = sf::Color::White, float thickness = 1.0f) {
            ShapeRenderer renderer;
            renderer.shapeType = ShapeType::Line;
            renderer.endPoint = end;
            renderer.fillColor = color;
            renderer.outlineThickness = thickness;
            return renderer;
        }
    };
    
    // Collider Component - AABB collision
    struct Collider {
        sf::Vector2f size{50.0f, 50.0f};
        sf::Vector2f offset{0.0f, 0.0f};
        bool isTrigger{false};
        bool isStatic{false};
        std::string tag{"default"};
        
        Collider() = default;
        Collider(float w, float h) : size(w, h) {}
        Collider(sf::Vector2f sz) : size(sz) {}
        Collider(sf::Vector2f sz, sf::Vector2f off) : size(sz), offset(off) {}
        
        // Get bounding box
        sf::FloatRect getBounds(const sf::Vector2f& position) const {
            return sf::FloatRect(
                position.x + offset.x - size.x / 2.0f,
                position.y + offset.y - size.y / 2.0f,
                size.x,
                size.y
            );
        }
    };
    
    // Circle Collider Component
    struct CircleCollider {
        float radius{25.0f};
        sf::Vector2f offset{0.0f, 0.0f};
        bool isTrigger{false};
        bool isStatic{false};
        std::string tag{"default"};
        
        CircleCollider() = default;
        CircleCollider(float r) : radius(r) {}
        CircleCollider(float r, sf::Vector2f off) : radius(r), offset(off) {}
        
        // Get center position
        sf::Vector2f getCenter(const sf::Vector2f& position) const {
            return position + offset;
        }
    };
    
    // Rigidbody Component - physics properties
    struct Rigidbody {
        float mass{1.0f};
        float drag{0.0f};
        float angularDrag{0.05f};
        bool useGravity{true};
        bool freezeRotation{false};
        sf::Vector2f gravityScale{1.0f, 1.0f};
        
        Rigidbody() = default;
        Rigidbody(float m) : mass(m) {}
        Rigidbody(float m, bool gravity) : mass(m), useGravity(gravity) {}
    };
    
    // Animation Component
    struct Animation {
        struct Frame {
            sf::IntRect textureRect;
            float duration;
            
            Frame(sf::IntRect rect, float dur) : textureRect(rect), duration(dur) {}
        };
        
        std::vector<Frame> frames;
        std::size_t currentFrame{0};
        float currentTime{0.0f};
        bool isPlaying{false};
        bool loop{true};
        float playbackSpeed{1.0f};
        
        Animation() = default;
        
        void addFrame(sf::IntRect rect, float duration) {
            frames.emplace_back(rect, duration);
        }
        
        void play() {
            isPlaying = true;
        }
        
        void pause() {
            isPlaying = false;
        }
        
        void stop() {
            isPlaying = false;
            currentFrame = 0;
            currentTime = 0.0f;
        }
        
        void restart() {
            currentFrame = 0;
            currentTime = 0.0f;
            isPlaying = true;
        }
        
        bool isFinished() const {
            return !loop && currentFrame >= frames.size() - 1;
        }
    };
    
    // Text Renderer Component
    struct TextRenderer {
        sf::Text text;
        sf::Font* font;
        std::string string;
        sf::Color color{sf::Color::White};
        unsigned int characterSize{30};
        sf::Text::Style style{sf::Text::Regular};
        bool visible{true};
        int renderLayer{0};
        
        TextRenderer() : font(nullptr) {}
        TextRenderer(sf::Font* f, const std::string& str) : font(f), string(str) {
            if (font) {
                text.setFont(*font);
                text.setString(string);
                text.setFillColor(color);
                text.setCharacterSize(characterSize);
            }
        }
        
        void setFont(sf::Font* f) {
            font = f;
            if (font) {
                text.setFont(*font);
            }
        }
        
        void setString(const std::string& str) {
            string = str;
            text.setString(string);
        }
        
        void updateText() {
            if (font) {
                text.setFont(*font);
                text.setString(string);
                text.setFillColor(color);
                text.setCharacterSize(characterSize);
                text.setStyle(style);
            }
        }
    };
    
    // Lifetime Component - tự động destroy sau một thời gian
    struct Lifetime {
        float remainingTime;
        
        Lifetime(float time) : remainingTime(time) {}
    };
    
    // Health Component
    struct Health {
        int maxHealth;
        int currentHealth;
        bool isDead{false};
        
        Health(int max) : maxHealth(max), currentHealth(max) {}
        
        void takeDamage(int damage) {
            currentHealth -= damage;
            if (currentHealth <= 0) {
                currentHealth = 0;
                isDead = true;
            }
        }
        
        void heal(int amount) {
            currentHealth = std::min(currentHealth + amount, maxHealth);
            if (currentHealth > 0) {
                isDead = false;
            }
        }
        
        float getHealthPercentage() const {
            return static_cast<float>(currentHealth) / static_cast<float>(maxHealth);
        }
    };
    
    // Input Component - để track input state
    struct Input {
        bool up{false}, down{false}, left{false}, right{false};
        bool action1{false}, action2{false}, action3{false};
        sf::Vector2f mousePosition{0.0f, 0.0f};
        bool mouseLeftPressed{false}, mouseRightPressed{false};
        
        void reset() {
            up = down = left = right = false;
            action1 = action2 = action3 = false;
            mouseLeftPressed = mouseRightPressed = false;
        }
    };
    
}