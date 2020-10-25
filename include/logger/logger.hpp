#pragma once

#include <iostream>
#include <algorithm>
#include <type_traits>
#include <vector>
#include <list>
#include <tuple>
#include <iterator>
#include <string>

#define LOGGER___INFO_COLOR_SCHEME  "\e[94m INFO: \033[0m"
#define LOGGER___WARN_COLOR_SCHEME  "\e[33m WARN: \033[0m"
#define LOGGER___ERROR_COLOR_SCHEME "\e[31m ERROR: "
#define LOGGER___PART_SEPARATOR     ": --> "
#define LOGGER___DEVIDER            ", "
#define LOGGER___STRINGIFY(x)       #x
#define LOGGER___TOSTRING(x)        LOGGER___STRINGIFY(x)
#define LOGGER___TRACE_ON           __FILE__ ":" LOGGER___TOSTRING(__LINE__)

/*
  You may change this designation of log.
  For examle INFO_LOG(...) or other.
*/
#define LOG(...)   log::info(LOGGER___TRACE_ON, __VA_ARGS__)
#define WARN(...)  log::warn(LOGGER___TRACE_ON, __VA_ARGS__)
#define ERROR(...) log::error(LOGGER___TRACE_ON, __VA_ARGS__)

template<class T, class charT = char, class traits = std::char_traits<char>>
class log_ostream_iterator
  : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
 private:
  std::basic_ostream<charT, traits>* os;
  bool first_elem;

 public:
  typedef std::basic_ostream<charT, traits> ostream_type;

  log_ostream_iterator(ostream_type& s) : os(&s), first_elem(true)
  {
  }

  log_ostream_iterator<T, charT, traits>& operator=(T const& item)
  {
    if (!first_elem)
      *os << LOGGER___DEVIDER;
    *os << item;
    first_elem = false;
    return *this;
  }

  log_ostream_iterator<T, charT, traits>& operator*()
  {
    return *this;
  }
  log_ostream_iterator<T, charT, traits>& operator++()
  {
    return *this;
  }
};

template<typename T>
struct is_stl_container : std::false_type {
};

template<typename... Args>
struct is_stl_container<std::vector<Args...>> : std::true_type {
};

template<typename... Args>
struct is_stl_container<std::list<Args...>> : std::true_type {
};

template<typename T>
constexpr bool is_stl_container_v = is_stl_container<std::remove_cv_t<T>>::value;

template<typename T>
constexpr bool is_integral_v = std::is_integral<T>::value;

template <typename T>
struct is_tuple : std::false_type{};

template <typename ... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type{};

template <typename T>
constexpr bool is_tuple_v = is_tuple<std::remove_cv_t<T>>::value;

class log
{
  inline static bool first_elem = false;

  static void print()
  {
    std::cout << '\n';
  }

  template<typename T, typename... Args>
  static void print(T first, Args... args)
  {
    print_v(first);
    print(args...);
  };

 public:
  template<typename T, typename... Args>
  static void info(T first, Args... args)
  {
    first_elem = false;
    std::cout << LOGGER___INFO_COLOR_SCHEME << first << LOGGER___PART_SEPARATOR;
    print(args...);
  };

  template<typename T, typename... Args>
  static void warn(T first, Args... args)
  {
    first_elem = false;
    std::cout << LOGGER___WARN_COLOR_SCHEME << first << LOGGER___PART_SEPARATOR;
    print(args...);
  };

  template<typename T, typename... Args>
  static void error(T first, Args... args)
  {
    first_elem = false;
    std::cout << LOGGER___ERROR_COLOR_SCHEME << first << LOGGER___PART_SEPARATOR;
    print(args...);
    std::cout << "\033[0m";
  };

  template<typename T, std::enable_if_t<is_integral_v<T>, int> = 0>
  static void print_v(const T& data)
  {
    if (first_elem)
      std::cout << LOGGER___DEVIDER;
    first_elem = true;
    std::cout << data;
  }

  template<typename T, std::enable_if_t<is_stl_container_v<T>, int> = 0>
  static void print_v(const T& data)
  {
    if (data.empty())
      return;

    if (first_elem)
      std::cout << LOGGER___DEVIDER;
    first_elem = true;

    std::copy(std::cbegin(data),
              std::cend(data), 
              log_ostream_iterator<typename T::value_type>(std::cout));
  }

  template<typename T, std::enable_if_t<!(is_integral_v<T> || is_stl_container_v<T> || is_tuple_v<T>), int> = 0>
  static void print_v(const T& data)
  {
    if (first_elem)
      std::cout << LOGGER___DEVIDER;
    first_elem = true;
    std::cout << data;
  }

  template<typename T, size_t ... idx>
  static void print_tuple(const T& data, std::index_sequence<idx...>)
  {
    if (first_elem)
      std::cout << LOGGER___DEVIDER;
    first_elem = true;
    ((std::cout << (idx == 0 ? "" : LOGGER___DEVIDER) << std::get<idx>(data)), ...);
  }

  template<typename T, std::enable_if_t<is_tuple_v<T>, int> = 0>
  static void print_v(const T& data)
  {
    print_tuple(data, std::make_index_sequence<std::tuple_size<T>::value>{});
  }
};
