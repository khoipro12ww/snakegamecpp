#include <SFML/Graphics.hpp>
#include "ECS/ECS.hpp"
#include <iostream>
#include <random>

class Game {
private:
    sf::RenderWindow m_window;
    ECS::World m_world;
    
    // Systems
    std::shared_ptr<ECS::MovementSystem> m_movementSystem;
    std::shared_ptr<ECS::PhysicsSystem> m_physicsSystem;
    std::shared_ptr<ECS::CollisionSystem> m_collisionSystem;
    std::shared_ptr<ECS::AnimationSystem> m_animationSystem;
    std::shared_ptr<ECS::LifetimeSystem> m_lifetimeSystem;
    std::shared_ptr<ECS::RenderSystem> m_renderSystem;
    
    // Game objects
    ECS::EntityID m_player;
    std::vector<ECS::EntityID> m_balls;
    std::vector<ECS::EntityID> m_walls;
    
    // Resources
    sf::Font m_font;
    
    // Game state
    sf::Clock m_clock;
    bool m_isRunning;
    int m_score;
    
public:
    Game() : m_window(sf::VideoMode(1200, 800), "ECS Game Demo - SFML 3.0", sf::Style::Close),
             m_isRunning(true), m_score(0) {
        m_window.setFramerateLimit(60);
        
        // Load resources
        if (!m_font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
            std::cout << "Warning: Could not load font, text rendering may not work\n";
        }
        
        SetupECS();
        CreateGameObjects();
    }
    
    void SetupECS() {
        // Register all components
        m_world.RegisterComponent<ECS::Transform>();
        m_world.RegisterComponent<ECS::Velocity>();
        m_world.RegisterComponent<ECS::ShapeRenderer>();
        m_world.RegisterComponent<ECS::Collider>();
        m_world.RegisterComponent<ECS::CircleCollider>();
        m_world.RegisterComponent<ECS::Rigidbody>();
        m_world.RegisterComponent<ECS::Input>();
        m_world.RegisterComponent<ECS::Health>();
        m_world.RegisterComponent<ECS::Lifetime>();
        m_world.RegisterComponent<ECS::TextRenderer>();
        
        // Register and setup systems
        m_movementSystem = m_world.RegisterSystem<ECS::MovementSystem>();
        ECS::Signature movementSignature;
        movementSignature.set(m_world.GetComponentType<ECS::Transform>());
        movementSignature.set(m_world.GetComponentType<ECS::Velocity>());
        m_world.SetSystemSignature<ECS::MovementSystem>(movementSignature);
        
        m_physicsSystem = m_world.RegisterSystem<ECS::PhysicsSystem>();
        ECS::Signature physicsSignature;
        physicsSignature.set(m_world.GetComponentType<ECS::Velocity>());
        physicsSignature.set(m_world.GetComponentType<ECS::Rigidbody>());
        m_world.SetSystemSignature<ECS::PhysicsSystem>(physicsSignature);
        
        m_collisionSystem = m_world.RegisterSystem<ECS::CollisionSystem>();
        ECS::Signature collisionSignature;
        collisionSignature.set(m_world.GetComponentType<ECS::Transform>());
        m_world.SetSystemSignature<ECS::CollisionSystem>(collisionSignature);
        
        m_lifetimeSystem = m_world.RegisterSystem<ECS::LifetimeSystem>();
        ECS::Signature lifetimeSignature;
        lifetimeSignature.set(m_world.GetComponentType<ECS::Lifetime>());
        m_world.SetSystemSignature<ECS::LifetimeSystem>(lifetimeSignature);
        
        m_renderSystem = m_world.RegisterSystem<ECS::RenderSystem>(&m_window);
        ECS::Signature renderSignature;
        renderSignature.set(m_world.GetComponentType<ECS::Transform>());
        m_world.SetSystemSignature<ECS::RenderSystem>(renderSignature);
        
        // Initialize systems
        m_world.Init();
    }
    
    void CreateGameObjects() {
        CreatePlayer();
        CreateWalls();
        CreateBalls();
        CreateUI();
    }
    
    void CreatePlayer() {
        m_player = m_world.CreateEntity();
        
        m_world.AddComponent<ECS::Transform>(m_player, ECS::Transform(600, 400));
        m_world.AddComponent<ECS::Velocity>(m_player, ECS::Velocity(0, 0, 400.0f));
        m_world.AddComponent<ECS::ShapeRenderer>(m_player, 
            ECS::ShapeRenderer::Rectangle({40, 40}, sf::Color::Blue));
        m_world.AddComponent<ECS::Collider>(m_player, ECS::Collider(40, 40));
        m_world.AddComponent<ECS::Input>(m_player, ECS::Input());
        m_world.AddComponent<ECS::Health>(m_player, ECS::Health(100));
    }
    
    void CreateWalls() {
        // Top wall
        auto topWall = m_world.CreateEntity();
        m_world.AddComponent<ECS::Transform>(topWall, ECS::Transform(600, 25));
        m_world.AddComponent<ECS::ShapeRenderer>(topWall, 
            ECS::ShapeRenderer::Rectangle({1200, 50}, sf::Color::White));
        m_world.AddComponent<ECS::Collider>(topWall, ECS::Collider(1200, 50));
        auto& topCollider = m_world.GetComponent<ECS::Collider>(topWall);
        topCollider.isStatic = true;
        m_walls.push_back(topWall);
        
        // Bottom wall
        auto bottomWall = m_world.CreateEntity();
        m_world.AddComponent<ECS::Transform>(bottomWall, ECS::Transform(600, 775));
        m_world.AddComponent<ECS::ShapeRenderer>(bottomWall, 
            ECS::ShapeRenderer::Rectangle({1200, 50}, sf::Color::White));
        m_world.AddComponent<ECS::Collider>(bottomWall, ECS::Collider(1200, 50));
        auto& bottomCollider = m_world.GetComponent<ECS::Collider>(bottomWall);
        bottomCollider.isStatic = true;
        m_walls.push_back(bottomWall);
        
        // Left wall
        auto leftWall = m_world.CreateEntity();
        m_world.AddComponent<ECS::Transform>(leftWall, ECS::Transform(25, 400));
        m_world.AddComponent<ECS::ShapeRenderer>(leftWall, 
            ECS::ShapeRenderer::Rectangle({50, 800}, sf::Color::White));
        m_world.AddComponent<ECS::Collider>(leftWall, ECS::Collider(50, 800));
        auto& leftCollider = m_world.GetComponent<ECS::Collider>(leftWall);
        leftCollider.isStatic = true;
        m_walls.push_back(leftWall);
        
        // Right wall
        auto rightWall = m_world.CreateEntity();
        m_world.AddComponent<ECS::Transform>(rightWall, ECS::Transform(1175, 400));
        m_world.AddComponent<ECS::ShapeRenderer>(rightWall, 
            ECS::ShapeRenderer::Rectangle({50, 800}, sf::Color::White));
        m_world.AddComponent<ECS::Collider>(rightWall, ECS::Collider(50, 800));
        auto& rightCollider = m_world.GetComponent<ECS::Collider>(rightWall);
        rightCollider.isStatic = true;
        m_walls.push_back(rightWall);
    }
    
    void CreateBalls() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> posDist(100, 1100);
        std::uniform_real_distribution<float> velDist(-200, 200);
        std::uniform_int_distribution<int> colorDist(0, 255);
        
        for (int i = 0; i < 10; ++i) {
            auto ball = m_world.CreateEntity();
            
            m_world.AddComponent<ECS::Transform>(ball, 
                ECS::Transform(posDist(gen), posDist(gen)));
            m_world.AddComponent<ECS::Velocity>(ball, 
                ECS::Velocity(velDist(gen), velDist(gen), 300.0f));
            m_world.AddComponent<ECS::ShapeRenderer>(ball, 
                ECS::ShapeRenderer::Circle(20, sf::Color(colorDist(gen), colorDist(gen), colorDist(gen))));
            m_world.AddComponent<ECS::CircleCollider>(ball, ECS::CircleCollider(20));
            m_world.AddComponent<ECS::Rigidbody>(ball, ECS::Rigidbody(1.0f, false));
            
            m_balls.push_back(ball);
        }
    }
    
    void CreateUI() {
        // Score text
        auto scoreText = m_world.CreateEntity();
        m_world.AddComponent<ECS::Transform>(scoreText, ECS::Transform(10, 10));
        m_world.AddComponent<ECS::TextRenderer>(scoreText, 
            ECS::TextRenderer(&m_font, "Score: 0"));
        auto& textRenderer = m_world.GetComponent<ECS::TextRenderer>(scoreText);
        textRenderer.renderLayer = 10; // Render on top
        textRenderer.characterSize = 24;
        
        // Instructions
        auto instructionText = m_world.CreateEntity();
        m_world.AddComponent<ECS::Transform>(instructionText, ECS::Transform(10, 50));
        m_world.AddComponent<ECS::TextRenderer>(instructionText, 
            ECS::TextRenderer(&m_font, "Use WASD to move the blue square"));
        auto& instrRenderer = m_world.GetComponent<ECS::TextRenderer>(instructionText);
        instrRenderer.renderLayer = 10;
        instrRenderer.characterSize = 16;
        instrRenderer.color = sf::Color::Yellow;
        
        auto instructionText2 = m_world.CreateEntity();
        m_world.AddComponent<ECS::Transform>(instructionText2, ECS::Transform(10, 75));
        m_world.AddComponent<ECS::TextRenderer>(instructionText2, 
            ECS::TextRenderer(&m_font, "Avoid the bouncing balls!"));
        auto& instr2Renderer = m_world.GetComponent<ECS::TextRenderer>(instructionText2);
        instr2Renderer.renderLayer = 10;
        instr2Renderer.characterSize = 16;
        instr2Renderer.color = sf::Color::Yellow;
    }
    
    void HandleInput() {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_isRunning = false;
            }
        }
        
        // Handle player input
        auto& input = m_world.GetComponent<ECS::Input>(m_player);
        auto& velocity = m_world.GetComponent<ECS::Velocity>(m_player);
        
        input.reset();
        velocity.acceleration = {0, 0};
        
        const float acceleration = 1000.0f;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            input.up = true;
            velocity.acceleration.y = -acceleration;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            input.down = true;
            velocity.acceleration.y = acceleration;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            input.left = true;
            velocity.acceleration.x = -acceleration;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            input.right = true;
            velocity.acceleration.x = acceleration;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            m_isRunning = false;
        }
    }
    
    void Update(float deltaTime) {
        m_world.Update(deltaTime);
        
        // Check collisions for score
        const auto& collisions = m_collisionSystem->GetCollisions();
        for (const auto& collision : collisions) {
            // Check if player collides with ball
            if (collision.entityA == m_player || collision.entityB == m_player) {
                ECS::EntityID ball = (collision.entityA == m_player) ? collision.entityB : collision.entityA;
                
                // Check if it's a ball
                if (std::find(m_balls.begin(), m_balls.end(), ball) != m_balls.end()) {
                    // Player hit by ball - take damage
                    auto& health = m_world.GetComponent<ECS::Health>(m_player);
                    health.takeDamage(10);
                    
                    if (health.isDead) {
                        std::cout << "Game Over! Final Score: " << m_score << std::endl;
                        m_isRunning = false;
                    }
                }
            }
        }
        
        // Update score based on survival time
        static float scoreTimer = 0.0f;
        scoreTimer += deltaTime;
        if (scoreTimer >= 1.0f) {
            m_score += 10;
            scoreTimer = 0.0f;
        }
    }
    
    void Render() {
        m_window.clear(sf::Color::Black);
        m_world.Render();
        m_window.display();
    }
    
    void Run() {
        while (m_isRunning && m_window.isOpen()) {
            float deltaTime = m_clock.restart().asSeconds();
            
            HandleInput();
            Update(deltaTime);
            Render();
        }
        
        m_world.Shutdown();
    }
};

int main() {
    try {
        std::cout << "=== ECS Game Demo with SFML 3.0 ===" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  WASD or Arrow Keys - Move blue square" << std::endl;
        std::cout << "  ESC - Exit game" << std::endl;
        std::cout << "Goal: Avoid the bouncing balls as long as possible!" << std::endl;
        std::cout << "=====================================" << std::endl;
        
        Game game;
        game.Run();
        
        std::cout << "Thanks for playing!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}