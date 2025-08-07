#include <SFML/Graphics.hpp>
#include "../include/ECS/ECS.hpp"
#include <iostream>

// Custom Component Example
struct PlayerController {
    float speed;
    PlayerController(float s = 200.0f) : speed(s) {}
};

// Custom System Example
class PlayerControllerSystem : public ECS::System {
public:
    void Update(float deltaTime) override {
        for (auto const& entity : m_entities) {
            auto& transform = m_world->GetComponent<ECS::Transform>(entity);
            auto& velocity = m_world->GetComponent<ECS::Velocity>(entity);
            auto& controller = m_world->GetComponent<PlayerController>(entity);
            
            // Reset velocity
            velocity.velocity = {0, 0};
            
            // Handle input
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                velocity.velocity.y = -controller.speed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                velocity.velocity.y = controller.speed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                velocity.velocity.x = -controller.speed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                velocity.velocity.x = controller.speed;
            }
        }
    }
};

int main() {
    // Create window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Simple ECS Example");
    window.setFramerateLimit(60);
    
    // Create ECS World
    ECS::World world;
    
    // Register components
    world.RegisterComponent<ECS::Transform>();
    world.RegisterComponent<ECS::Velocity>();
    world.RegisterComponent<ECS::ShapeRenderer>();
    world.RegisterComponent<PlayerController>();
    
    // Register systems
    auto playerSystem = world.RegisterSystem<PlayerControllerSystem>();
    ECS::Signature playerSignature;
    playerSignature.set(world.GetComponentType<ECS::Transform>());
    playerSignature.set(world.GetComponentType<ECS::Velocity>());
    playerSignature.set(world.GetComponentType<PlayerController>());
    world.SetSystemSignature<PlayerControllerSystem>(playerSignature);
    
    auto movementSystem = world.RegisterSystem<ECS::MovementSystem>();
    ECS::Signature movementSignature;
    movementSignature.set(world.GetComponentType<ECS::Transform>());
    movementSignature.set(world.GetComponentType<ECS::Velocity>());
    world.SetSystemSignature<ECS::MovementSystem>(movementSignature);
    
    auto renderSystem = world.RegisterSystem<ECS::RenderSystem>(&window);
    ECS::Signature renderSignature;
    renderSignature.set(world.GetComponentType<ECS::Transform>());
    world.SetSystemSignature<ECS::RenderSystem>(renderSignature);
    
    // Create player entity
    ECS::EntityID player = world.CreateEntity();
    world.AddComponent<ECS::Transform>(player, ECS::Transform(400, 300));
    world.AddComponent<ECS::Velocity>(player, ECS::Velocity());
    world.AddComponent<ECS::ShapeRenderer>(player, 
        ECS::ShapeRenderer::Rectangle({50, 50}, sf::Color::Blue));
    world.AddComponent<PlayerController>(player, PlayerController(250.0f));
    
    // Create some static objects
    for (int i = 0; i < 5; ++i) {
        ECS::EntityID obj = world.CreateEntity();
        world.AddComponent<ECS::Transform>(obj, 
            ECS::Transform(100 + i * 150, 100 + (i % 2) * 400));
        world.AddComponent<ECS::ShapeRenderer>(obj, 
            ECS::ShapeRenderer::Circle(30, sf::Color::Red));
    }
    
    // Initialize systems
    world.Init();
    
    // Game loop
    sf::Clock clock;
    
    std::cout << "Simple ECS Example" << std::endl;
    std::cout << "Use WASD to move the blue square" << std::endl;
    std::cout << "Press ESC to exit" << std::endl;
    
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
        }
        
        // Update
        world.Update(deltaTime);
        
        // Render
        window.clear(sf::Color::Black);
        world.Render();
        window.display();
    }
    
    // Cleanup
    world.Shutdown();
    
    return 0;
}