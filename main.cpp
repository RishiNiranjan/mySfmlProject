#include "Game.hh"
#include "thread"
#include "Configuration.hh"
// void playerUpdater(sf::RectangleShape &_player)
// {
//   while (true)
//   {
//     _player.setRotation(_player.getRotation() + 0.001);
//   }
// }

int main(int argc, char *argv[])
{
  Configuration::initialize();
  Game game(800,600);
  game.run(60);
}