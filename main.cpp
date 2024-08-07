#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>

const int WIDTH = 1200;
const int HEIGHT = 900;

class GravitySource {
public:
    GravitySource(float posX, float posY, float s, float r, sf::Color fillColour = sf::Color::White)
        : srenght(s), radius(r) {
        pos.x = posX;
        pos.y = posY;

        shape.setPosition(pos);
        shape.setFillColor(fillColour);
        shape.setRadius(radius);
        shape.setOrigin(radius, radius); // Center the shape
    }

    void render(sf::RenderWindow& w) {
        w.draw(shape);
    }

    sf::Vector2f getPos() const {
        return pos;
    }

    float getStrenght() const {
        return srenght;
    }

    float getRadius() const {
        return radius;
    }

private:
    sf::Vector2f pos;
    float srenght;
    float radius;
    sf::CircleShape shape;
};

class Particle {
public:
    Particle(float posX, float posY, sf::Vector2f velocity, float radius)
        : vel(velocity) {
        pos.x = posX;
        pos.y = posY;

        shape.setPosition(pos);
        shape.setRadius(radius);
        shape.setOrigin(radius, radius); // Center the shape
    }

    void render(sf::RenderWindow& w) {
        shape.setPosition(pos);
        w.draw(shape);

        if (trail.size() > 1) {
            sf::VertexArray trailLine(sf::LineStrip, trail.size());
            for (std::size_t i = 0; i < trail.size(); ++i) {
                trailLine[i].position = trail[i];
                trailLine[i].color = shape.getFillColor();
            }
            w.draw(trailLine);
        }
    }

    void setColor(sf::Color fillColor){
        shape.setFillColor(fillColor);
    }

    void updatePhysics(const GravitySource& source, float deltaTime) {
        float distanceX = source.getPos().x - pos.x;
        float distanceY = source.getPos().y - pos.y;

        // total distance using Pythagorean theorem
        float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);
        float distanceInverse = 1.f / distance;
        float normalizedX = distanceInverse * distanceX;
        float normalizedY = distanceInverse * distanceY;
        float distanceInverseSquare_dropOff = distanceInverse * distanceInverse;

        float accelerationX = normalizedX * source.getStrenght() * distanceInverseSquare_dropOff;
        float accelerationY = normalizedY * source.getStrenght() * distanceInverseSquare_dropOff;

        vel.x += accelerationX * deltaTime;
        vel.y += accelerationY * deltaTime;

        pos.x += vel.x * deltaTime;
        pos.y += vel.y * deltaTime;

        /*trail.push_back(pos);
        if (trail.size() > maxTrailLength) {
            trail.erase(trail.begin());
        }*/
    }

private:
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::CircleShape shape;
    std::vector<sf::Vector2f> trail;
    static const int maxTrailLength = 150;
};

sf::Color mapValueToColor(float value){
    if(value  < 0.0f) value = 0;
    if(value  > 1) value = 1;

    int r = 0,g = 0,b = 0;
    if(value < 0.5){
        b = 255 * (1.0f - 2 * value);
        g = 255 * 2 * value;
    }
    else{
        g = 255 * (2.0f - 2* value);
        r = 255 * (2. * value - 1);
    }
    return sf::Color(r,g,b);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "2D Solar System with Trails");
    window.setFramerateLimit(60);


    std::vector<GravitySource> sources;

    GravitySource source(WIDTH / 2 - 300, HEIGHT / 2, 7000, 20, sf::Color::White);
    GravitySource source2(WIDTH / 2 + 300, HEIGHT / 2, 7000, 20, sf::Color::White);
    sources.push_back(source);
    sources.push_back(source2);

    int numOfParticles = 2000;

    std::vector<Particle> particles;

    for(int i =0; i< numOfParticles; i++){
        sf::Vector2f initialVelocity(4, 0.2 + (0.1 / numOfParticles * i)); // y value is being distributed betwenn 0.2 and 0.3
        particles.push_back(Particle(600, 700, initialVelocity,5));

        float val=  (float)i / (float) numOfParticles;
        sf::Color col = mapValueToColor(val);
        particles[i].setColor(col);
    }


    /*sf::Vector2f MinitialVelocity(3.5, 4);
    Particle mercury(WIDTH / 2 - 60, HEIGHT / 2 + 60, MinitialVelocity, 6, sf::Color::Cyan);

    sf::Vector2f VinitialVelocity(2.4, 3);
    Particle venus(WIDTH / 2 - 110, HEIGHT / 2 + 110, VinitialVelocity, 10, sf::Color::Green);

    sf::Vector2f EinitialVelocity(2, 2.6);
    Particle Earth(WIDTH / 2 - 160, HEIGHT / 2 + 160, EinitialVelocity, 12, sf::Color::Blue);

    sf::Vector2f MarsinitialVelocity(1.8, 2.2);
    Paticle mars(WIDTH / 2 - 220, HEIGHT / 2 + 220, MarsinitialVelocity, 12, sf::Color::Red);*/

    float speedFactor = 1.0f / 5.0f;

    // Load font
    sf::Font font;
    if (!font.loadFromFile("ARIAL.ttf")) {
        return -1;
    }
    sf::Text speedText;
    speedText.setFont(font);
    speedText.setCharacterSize(24);
    speedText.setFillColor(sf::Color::White);
    speedText.setPosition(10.f, 10.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    speedFactor *= 1.1f;
                } else if (event.key.code == sf::Keyboard::Down) {
                    speedFactor *= 0.9f;
                }
            }
        }
        float daysPerSecond = speedFactor * 5.0f;
        speedText.setString("1 sec = " + std::to_string(daysPerSecond) + " days");

        window.clear();
        //mercury.updatePhysics(source, speedFactor);
        //venus.updatePhysics(source, speedFactor);
        //Earth.updatePhysics(source, speedFactor);
        //mars.updatePhysics(source, speedFactor);

        // Draw
        for(int i =0; i< sources.size(); i++){
            for(int j =0; j <particles.size();j++){
                particles[j].updatePhysics(sources[i],speedFactor);
            }
        }
        for(int i =0; i< sources.size(); i++){
            sources[i].render(window);
        }
         for(int j =0; j <particles.size();j++){
             particles[j].render(window);
         }
        //mercury.render(window);
        //venus.render(window);
        //Earth.render(window);
        //mars.render(window);
       // window.draw(speedText);

        window.display();
    }

    return 0;
}
