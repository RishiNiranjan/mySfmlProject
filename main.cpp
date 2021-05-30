#include "Game.hh"
#include "thread"

// void playerUpdater(sf::RectangleShape &_player)
// {
//   while (true)
//   {
//     _player.setRotation(_player.getRotation() + 0.001);
//   }
// }

int main(int argc, char *argv[])
{
  Game game;
  // std::thread t1 = std::thread(playerUpdater, std::ref(game._player1));
  // std::thread t2 = std::thread(playerUpdater, std::ref(game._player2));
  game.run(60);
  // t1.join();
  // t2.join();
}