#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <unordered_map>
#include <math.h>

class Action
{
public:
  Action(const sf::Keyboard::Key &key)
  {
    _event.type = sf::Event::EventType::KeyPressed;
    _event.key.code = key;
  }

  bool test() const
  {
    bool res = false;
    res = sf::Keyboard::isKeyPressed(_event.key.code);
    return res;
  }

  sf::Event _event;
};

template <class T>
class ActionMap
{
public:
  ActionMap() = default;

  void insertKeyBind(T input, sf::Keyboard::Key key)
  {
    const Action action{key};
    _keyBindingMap.emplace(input, action);
  }

  const Action &get(const T &input) const
  {
    return _keyBindingMap.at(input);
  }

private:
  std::unordered_map<T, Action> _keyBindingMap;
};

#include <functional>
#include <list>
template <class T>
class ActionTargetMap
{
public:
  ActionTargetMap(const ActionMap<T> &actionmap) : actionMap{actionmap}
  {
  }

  using FuncType = std::function<void(const sf::Event &)>;

  void insert(const T &key, const FuncType &callback)
  {
    _eventsRealTime.emplace_back(key, callback);
  }

  void processRealEvents() const
  {
    bool res = false;
    for (auto &pair : _eventsRealTime)
    {
      const Action &action = actionMap.get(pair.first);
      if (action.test())
        pair.second(action._event);
    }
  }

private:
  std::list<std::pair<T, FuncType>> _eventsRealTime;
  const ActionMap<T> &actionMap;
};

template <class RESOURCE, class IDENTIFIER>
class ResourceManager
{
public:
  template <typename... Args>
  void load(const IDENTIFIER id, Args &&...args)
  {
    std::unique_ptr<RESOURCE> ptr(new RESOURCE);
    if (not ptr->loadFromFile(std::forward<Args>(args)...))
      throw std::runtime_error("Impossible to load file");
    _map.emplace(id, std::move(ptr));
  }

  RESOURCE &get(const IDENTIFIER &id) const
  {
    return *_map.at(id);
  }

private:
  std::unordered_map<IDENTIFIER, std::unique_ptr<RESOURCE>> _map;
};

class Configuration
{
public:
  Configuration() = default;

  enum Player : int
  {
    UP,
    LEFT,
    RIGHT
  };

  enum Skins : int
  {
    SHIPSKIN
  };

  static void initilize()
  {
    initKeyboard();
    initTexture();
  }

  static ActionMap<int> _actionMap;
  static ResourceManager<sf::Texture, int> _resourceManager;

private:
  static void initKeyboard()
  {
    _actionMap.insertKeyBind(Player::LEFT, sf::Keyboard::Key::Left);
    _actionMap.insertKeyBind(Player::RIGHT, sf::Keyboard::Key::Right);
    _actionMap.insertKeyBind(Player::UP, sf::Keyboard::Key::Up);
  }

  static void initTexture()
  {
    _resourceManager.load(Skins::SHIPSKIN, "media/Player/Ship.png");
  }
};
ActionMap<int> Configuration::_actionMap;
ResourceManager<sf::Texture, int> Configuration::_resourceManager;

class Player : public sf::Drawable, ActionTargetMap<int>
{
public:
  template <class... Args>
  void SetPosition(Args... args)
  {
    _ship.setPosition(std::forward<Args>(args)...);
  }

  Player() : ActionTargetMap<int>(Configuration::_actionMap), _isMoving(false), _rotation(0), _velocity(0, 0)
  {
    _ship.setTexture(Configuration::_resourceManager.get(Configuration::SHIPSKIN));
    _ship.setOrigin(49.5, 37.5);

    ActionTargetMap<int>::insert(Configuration::Player::UP, [this](const sf::Event &) -> void
                                 { _isMoving = true; });

    ActionTargetMap<int>::insert(Configuration::Player::LEFT, [this](const sf::Event &) -> void
                                 { _rotation = -1; });

    ActionTargetMap<int>::insert(Configuration::Player::RIGHT, [this](const sf::Event &) -> void
                                 { _rotation = 1; });
  }

  void processEvents()
  {
    _isMoving = false;
    _rotation = 0;
    ActionTargetMap<int>::processRealEvents();
  }

  void update(sf::Time deltaTime)
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
  }

private:
  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override
  {
    target.draw(_ship, states);
  }

  sf::Sprite _ship;
  sf::Vector2f _velocity;
  bool _isMoving;
  int _rotation;
};

class Game
{
public:
  Game() : _window(sf::VideoMode(800, 600), "SfmlGame")
  {
    _player.SetPosition(200, 300);
  }

  void run(int framePerSecond)
  {
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time TimePerFrame = sf::seconds(1.f / framePerSecond);
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

  sf::RenderWindow _window;
  Player _player;

  void processEvents()
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

  void update(sf::Time timePerFrame)
  {
    _player.update(timePerFrame);
  }
  
  void render()
  {
    _window.clear();
    _window.draw(_player);
    _window.display();
  }
};

  int main()
  {
    Configuration::initilize();
    Game game;
    game.run(60);
  }
