#ifndef GAME2_HH
#define GAME2_HH

#include <iostream>
#include <math.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Action.hh"
#include "ActionTarget.hh"
#include "ActionMap.hh"

class Player : public sf::Drawable, public ActionTarget<int>
{
public:
  Player(const Player &) = delete;
  Player &operator=(const Player &) = delete;
  Player();
  template <typename... Args>
  void setPosition(Args &&...args);
  void processEvents();
  void update(sf::Time deltaTime);
  enum PlayerInputs
  {
    Up,
    Left,
    Right
  };
  static void setDefaultsInputs();

private:
  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
  sf::RectangleShape _shape;
  sf::Vector2f _velocity;
  bool _isMoving;
  int _rotation;
  static ActionMap<int> _playerInputs;
};

ActionMap<int> Player::_playerInputs;

Player::Player() : ActionTarget<int>(_playerInputs), _shape(sf::Vector2f(32, 32)), _isMoving(false), _rotation(0)
{
  _shape.setFillColor(sf::Color::Blue);
  _shape.setOrigin(16, 16);

  bind(PlayerInputs::Up, [this](const sf::Event &)
       { _isMoving = true; });

  bind(PlayerInputs::Left, [this](const sf::Event &)
       { _rotation -= 1; });

  bind(PlayerInputs::Right, [this](const sf::Event &)
       { _rotation += 1; });
}

void Player::setDefaultsInputs()
{
  _playerInputs.map(PlayerInputs::Up, Action(sf::Keyboard::Up));
  _playerInputs.map(PlayerInputs::Right, Action(sf::Keyboard::Right));
  _playerInputs.map(PlayerInputs::Left, Action(sf::Keyboard::Left));
}

void Player::processEvents()
{
  _isMoving = false;
  _rotation = 0;
  ActionTarget<int>::processEvents();
}

void Player::update(sf::Time deltaTime)
{
  float seconds = deltaTime.asSeconds();
  if (_rotation != 0)
  {
    float angle = (_rotation > 0 ? 1 : -1) * 180 * seconds;
    _shape.rotate(angle);
  }
  if (_isMoving)
  {
    float angle = _shape.getRotation() / 180 * M_PI - M_PI / 2;
    _velocity += sf::Vector2f(std::cos(angle), std::sin(angle)) *
                 60.f * seconds;
  }
  _shape.move(seconds * _velocity);
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  target.draw(_shape, states);
}

#endif