#ifndef GAME2_HH
#define GAME2_HH

#include <iostream>
#include <math.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Player : public sf::Drawable
{
public:
  Player();

  template <typename... Args>
  void setPosition(Args &&...args)
  {
    _playerShape.setPosition(std::forward<Args>(args)...);
  }

  void processEvents(sf::Event event);
  void update(sf::Time deltaTime);
  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
  bool isMoving;
  int rotation;
  sf::RectangleShape _playerShape;
  sf::Vector2f _velocity;
};

Player::Player() : _playerShape(sf::Vector2f(32, 32))
{
  std::cout<<"Player()"<<std::endl;
  _playerShape.setFillColor(sf::Color::White);
  _playerShape.setOrigin(16, 16);
  setPosition(200, 200);
}

void Player::processEvents(sf::Event event)
{
  if (event.type == sf::Event::KeyPressed)
  {
    if (event.key.code == sf::Keyboard::Up)
      isMoving = true;
    else if (event.key.code == sf::Keyboard::Left)
      rotation = -1;
    else if (event.key.code == sf::Keyboard::Right)
      rotation = 1;
  }

  if (event.type == sf::Event::KeyReleased)
  {
    if (event.key.code == sf::Keyboard::Up)
      isMoving = false;
    else if (event.key.code == sf::Keyboard::Left)
      rotation = 0;
    else if (event.key.code == sf::Keyboard::Right)
      rotation = 0;
  }
}

void Player::update(sf::Time deltaTime)
{
  float seconds = deltaTime.asSeconds();
  if (rotation != 0)
  {
    float angle = (rotation > 0 ? 1 : -1) * 180 * seconds;
    _playerShape.rotate(angle);
  }
  if (isMoving)
  {
    float angle = _playerShape.getRotation() / 180 * M_PI - M_PI / 2;
    _velocity += sf::Vector2f(std::cos(angle), std::sin(angle)) *
                 60.f * seconds;
  }
  _playerShape.move(seconds * _velocity);
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  target.draw(_playerShape, states);
}

#endif