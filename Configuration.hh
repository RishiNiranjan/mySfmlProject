#ifndef CONFIGURATION_HH
#define CONFIGURATION_HH

#include "ResourceManager.hh"
#include "Action.hh"
#include "ActionMap.hh"

class Configuration
{
public:
  Configuration() = delete;
  Configuration(const Configuration &) = delete;
  Configuration &operator=(const Configuration &) = delete;
  enum Textures : int
  {
    Skin1
  };
  enum PlayerInputs : int
  {
    Up,
    Left,
    Right
  };
  static ResourceManager<sf::Texture, int> texturesMap;
  static ActionMap<int> player_inputs;
  static void initialize();

private:
  static void initTextures();
  static void initPlayerInputs();
};

ResourceManager<sf::Texture, int> Configuration::texturesMap;

ActionMap<int> Configuration::player_inputs;

void Configuration::initialize()
{
  initTextures();
  initPlayerInputs();
}
void Configuration::initTextures()
{
  texturesMap.load(Textures::Skin1, "media/Player/Ship.png");
}
void Configuration::initPlayerInputs()
{
  player_inputs.map(PlayerInputs::Up, Action(sf::Keyboard::Up));
  player_inputs.map(PlayerInputs::Right, Action(sf::Keyboard::Right));
  player_inputs.map(PlayerInputs::Left, Action(sf::Keyboard::Left));
}

#endif