#ifndef GAME_HH
#define GAME_HH

#include "Player.hh"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class Game
{
public:
  Game(int x=800, int y=600);
  void run(int framePerSecond);

  ~Game() = default;
  Game(const Game &) = delete;
  Game(const Game &&) = delete;
  Game &operator=(Game &) = delete;
  Game &operator=(Game &&) = delete;

  sf::RenderWindow _window;
  Player _player;
  int _x;
  int _y;

  void processEvents();
  void update(sf::Time timePerFrame);
  void render();
};

Game::Game(int x, int y) : _window(sf::VideoMode(x, y), "SfmlGame"), _x{x}, _y{y}
{
  _player.setPosition(_x/2, _y/2);
}

void Game::run(int frame_per_seconds)
{
  sf::Clock clock;
  sf::Time timeSinceLastUpdate = sf::Time::Zero;
  sf::Time TimePerFrame = sf::seconds(1.f / frame_per_seconds);
  while (_window.isOpen())
  {
    processEvents();
    bool repaint = false;
    timeSinceLastUpdate += clock.restart();
    while (timeSinceLastUpdate > TimePerFrame)
    {
      timeSinceLastUpdate -= TimePerFrame;
      repaint = true;
      update(TimePerFrame);
    }
    if (repaint)
      render();
  }
}

void Game::processEvents()
{
  sf::Event event;
  while (_window.pollEvent(event))
  {
    if (event.type == sf::Event::Closed)
      _window.close();
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        _window.close();
  }
  _player.processEvents();
}

void Game::update(sf::Time timePerFrame)
{
  _player.update(timePerFrame);
  
  sf::Vector2f playerPosition = _player.getPosition();

  if(playerPosition.x < 0)
    playerPosition.x = _x;
  else if(playerPosition.x > _x)
    playerPosition.x = 0;

  if(playerPosition.y < 0)
    playerPosition.y = _y;
  else if(playerPosition.y > _y)
    playerPosition.y = 0;

  _player.setPosition(playerPosition);
}

void Game::render()
{
  _window.clear();
  _window.draw(_player);
  _window.display();
}





// class Game
// {
// public:
//   Game();
//   void run(int framePerSecond);

//   ~Game() = default;
//   Game(const Game&) = delete;
//   Game(const Game&&) = delete;
//   Game& operator =(Game&) = delete;
//   Game& operator =(Game&&) = delete;

// public:
//   sf::RenderWindow _window;
//   sf::RectangleShape _player1;
//   sf::RectangleShape _player2;
//   void process_events();
//   void update(sf::Time timePerFrame);
//   void render();
// };

// Game::Game() : _window(sf::VideoMode(800, 600), "SfmlGame")
// {
//   _player1.setSize(sf::Vector2f(100, 50));
//   _player1.setOutlineColor(sf::Color::Red);
//   _player1.setOutlineThickness(5);
//   _player1.setPosition(200, 300);

//   _player2.setSize(sf::Vector2f(100, 50));
//   _player2.setOutlineColor(sf::Color::Blue);
//   _player2.setOutlineThickness(3);
//   _player2.setPosition(400, 300);
// }

// void Game::run(int framePerSecond)
// {
//   sf::Clock clock;
//   sf::Time timeSinceLastUpdate = sf::Time::Zero;
//   sf::Time timePerFrame = sf::seconds(1.f/framePerSecond);

//   while (_window.isOpen())
//   {
//     process_events();

//     bool repaint = false;
//     timeSinceLastUpdate += clock.restart();
//     while (timeSinceLastUpdate > timePerFrame)
//     {
//       timeSinceLastUpdate -= timePerFrame;
//       repaint = true;
//       update(timePerFrame);
//     }

//     if(repaint)
//       render();
//   }
// }

// void Game::process_events()
// {
//   sf::Event event;
//   //process events
//   while (_window.pollEvent(event))
//   {
//     if ((event.type == sf::Event::Closed) or ((event.type == sf::Event::KeyPressed) and (event.key.code == sf::Keyboard::Escape)))
//     {
//       _window.close();
//     }
//   }
// }

// void Game::update(sf::Time timePerFrame)
// {
//   _player1.setRotation(_player1.getRotation()+0.05+timePerFrame.asSeconds());
//   _player2.setRotation(_player2.getRotation()+0.05+timePerFrame.asSeconds());
// }

// void Game::render()
// {
//   _window.clear();
//   _window.draw(_player1);
//   _window.draw(_player2);
//   _window.display();
// }

#endif
