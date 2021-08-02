#ifndef GAME2_HH
#define GAME2_HH

#include <iostream>
#include <math.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Configuration.hh"
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
  void setPosition(Args &&...args)
  {
    _ship.setPosition(std::forward<Args>(args)...);
  }

  void processEvents();
  void update(sf::Time deltaTime);
  static void setDefaultsInputs();
  sf::Vector2f& getPosition();

private:
  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
  sf::Sprite _ship;
  sf::Vector2f _velocity;
  bool _isMoving;
  int _rotation;
};

Player::Player() : ActionTarget<int>(Configuration::player_inputs), _isMoving(false), _rotation(0)
{
  _ship.setTexture(Configuration::texturesMap.get(Configuration::Textures::Skin1));

  _ship.setOrigin(49.5, 37.5);

  bind(Configuration::PlayerInputs::Up, [this](const sf::Event &)
       { _isMoving = true; });

  bind(Configuration::PlayerInputs::Left, [this](const sf::Event &)
       { _rotation -= 1; });

  bind(Configuration::PlayerInputs::Right, [this](const sf::Event &)
       { _rotation += 1; });
}

sf::Vector2f& Player::getPosition()
{
  return _ship.getPosition();
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
    _ship.rotate(angle);
  }
  if (_isMoving)
  {
    float angle = _ship.getRotation() / 180 * M_PI - M_PI / 2;
    _velocity += sf::Vector2f(std::cos(angle), std::sin(angle)) *
                 60.f * seconds;
  }
  _ship.move(seconds * _velocity);

  std::cout<<_ship.getPosition().x<<":"<<_ship.getPosition().y<<std::endl;
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  target.draw(_ship, states);
}

#endif