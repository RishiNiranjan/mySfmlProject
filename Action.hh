#ifndef ACTION_HH
#define ACTION_HH

#include <cstring>
#include <iostream>
#include <math.h>
#include <list>
#include <functional>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Action
{
public:
  enum Type
  {
    RealTime = 1,
    Pressed = 1 << 1,
    Released = 1 << 2
  };

  Action(const sf::Keyboard::Key &key, int type = Type::RealTime | Type::Pressed);
  Action(const sf::Mouse::Button &button, int type = Type::RealTime | Type::Pressed);
  Action(const Action &other);
  Action& operator=(const Action &other);

  bool test() const;

  bool operator==(const sf::Event &event) const;
  bool operator==(const Action &other) const;
  
  sf::Event _event;
  int _type;
};

Action::Action(const Action &other) : _type(other._type)
{
  std::memcpy(&_event, &other._event, sizeof(sf::Event));
}

Action &Action::operator=(const Action &other)
{
  std::memcpy(&_event, &other._event, sizeof(sf::Event));
  _type = other._type;
  return *this;
}

Action::Action(const sf::Keyboard::Key& key, int type) : _type(type)
{
  std::cout<<"Action(): type= "<<_type<<std::endl;
  _event.type = sf::Event::EventType::KeyPressed;
  _event.key.code = key;
}

Action::Action(const sf::Mouse::Button &button, int type) : _type(type)
{
  _event.type = sf::Event::EventType::MouseButtonPressed;
  _event.mouseButton.button = button;
}

bool Action::operator==(const sf::Event &event) const
{
  bool res = false;
  switch (event.type)
  {
  case sf::Event::EventType::KeyPressed:
    if (_type & Type::Pressed and _event.type == sf::Event::EventType::KeyPressed)
      res = event.key.code == _event.key.code;
    break;

  case sf::Event::EventType::KeyReleased:
    if (_type & Type::Released and _event.type == sf::Event::EventType::KeyPressed)
      res = event.key.code == _event.key.code;
    break;

  case sf::Event::EventType::MouseButtonPressed:
    if (_type & Type::Pressed and _event.type == sf::Event::EventType::MouseButtonPressed)
      res = event.mouseButton.button == _event.mouseButton.button;
    break;

  case sf::Event::EventType::MouseButtonReleased:
    if (_type & Type::Released and _event.type == sf::Event::EventType::MouseButtonPressed)
      res = event.mouseButton.button == _event.mouseButton.button;
    break;

  default:
    break;
  }
  return res;
}

bool Action::operator==(const Action &other) const
{
  return _type == other._type and other == _event;
}

bool Action::test() const
{
  bool res = false;
  if (_event.type == sf::Event::EventType::KeyPressed)
  {
    if (_type & Type::Pressed)
      res = sf::Keyboard::isKeyPressed(_event.key.code);
  }
  else if (_event.type == sf::Event::EventType::MouseButtonPressed)
  {
    if (_type & Type::Pressed)
      res = sf::Mouse::isButtonPressed(_event.mouseButton.button);
  }
  return res;
}

#endif