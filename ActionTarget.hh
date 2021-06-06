#ifndef ACTIONTARGET_HH
#define ACTIONTARGET_HH

#include <functional>
#include <list>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class ActionMap;

template<typename T>
class ActionTarget
{
public:
  using FuncType = std::function<void(const sf::Event &)>;

  ActionTarget<T> (const ActionTarget<T> &) = delete;
  ActionTarget<T> &operator=(const ActionTarget<T> &) = delete;

  ActionTarget<T>(const ActionMap<T> &actionMap);

  bool processEvent(const sf::Event &event) const;
  void processEvents() const;

  void bind(const T &input, const FuncType &callback);

  void unbind(const T &input);

private:
  std::list<std::pair<Action, FuncType>> _eventsRealTime;
  std::list<std::pair<Action, FuncType>> _eventsPoll;
  const ActionMap<T> &_actionMap;
};

template <typename T>
ActionTarget<T>::ActionTarget(const ActionMap<T> &actionMap) : _actionMap(actionMap)
{
}

template <typename T>
bool ActionTarget<T>::processEvent(const sf::Event &event) const
{
  bool res = false;
  for (auto &action : _eventsPoll)
  {
    if (action.first == event)
    {
      action.second(event);
      res = true;
      break;
    }
  }
  return res;
}

template <typename T>
void ActionTarget<T>::processEvents() const
{
  for (auto &pair : _eventsRealTime)
  {
    const Action &action = _actionMap.get(pair.first);

    if (action.test())
      pair.second(action._event);
  }
}

template <typename T>
void ActionTarget<T>::bind(const T &key, const FuncType &callback)
{
  const Action& action = _actionMap.get(key);

  if (action._type & Action::Type::RealTime)
    _eventsRealTime.emplace_back(action, callback);
  else
    _eventsPoll.emplace_back(action, callback);
}

template <typename T>
void ActionTarget<T>::unbind(const T &key)
{
  auto remove_func = [&key](const std::pair<T, FuncType> &pair) -> bool
  {
    return pair.first == key;
  };

  const Action &action = _actionMap.get(key);
  if (action._type & Action::Type::RealTime)
    _eventsRealTime.remove_if(remove_func);
  else
    _eventsPoll.remove_if(remove_func);
}

#endif