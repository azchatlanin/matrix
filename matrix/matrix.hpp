#pragma once

#include <memory>
#include <vector>
#include <tuple>
#include <algorithm>

template<typename T, size_t N>
struct generate_tuple {
  typedef decltype(std::tuple_cat(typename generate_tuple<T, N - 1>::type{}, std::make_tuple(T{}))) type;
};

template<typename T>
struct generate_tuple<T, 1> {
  typedef std::tuple<T> type;
};

template<typename T, size_t N>
using tuple_t = typename generate_tuple<T, N>::type;

template <typename... T, std::size_t... I>
auto subtuple_(const std::tuple<T...>& t, std::index_sequence<I...>) {
  return std::make_tuple(std::get<I>(t)...);
}

template <typename... T>
auto subtuple(const std::tuple<T...>& t) {
  return subtuple_(t, std::make_index_sequence<sizeof...(T) - 1>());
}

template<typename T, std::size_t dimensions, T fill_value = 0>
class Matrix
{
 public:
  using index_t = tuple_t<std::size_t, dimensions>;
  using value_t = decltype(std::tuple_cat(index_t{}, std::make_tuple(T{})));
  using storage_t = std::vector<value_t>;

  Matrix() : local_storage{ new storage{} }
  {
  }

  int size() const
  {
    return local_storage->values.size();
  }

  auto operator[](std::size_t index)
  {
    return proxy<storage, std::tuple<std::size_t>>{ local_storage, std::make_tuple(index) };
  }
  
  auto begin() noexcept
  {
    return local_storage->values.begin();
  }

  auto begin() const noexcept
  {
    return local_storage->values.begin();
  }

  auto end() noexcept
  {
    return local_storage->values.end();
  }

  auto end() const noexcept
  {
    return local_storage->values.end();
  }

  auto cbegin() const noexcept
  {
    return local_storage->values.cbegin();
  }

  auto cend() const noexcept
  {
    return local_storage->values.cend();
  }

 private:
  struct storage
  {
    void set_value(const index_t& index, const T& v)
    {
      auto value = std::tuple_cat(index, std::make_tuple(v));
      values.push_back(value);
    }

    auto get_value(const index_t& index)
    {
      auto it = std::find_if(values.begin(), values.end(), [&](auto a) { return subtuple(a) == index; });
      if (it == values.end()) return fill_value;
      return std::get<dimensions>(*it);
    }

    int size() const
    {
      return values.size();
    }

    storage_t values;
  };

  template<typename storage_t, typename index_t>
  class proxy
  {
    using next_index_t = decltype(std::tuple_cat(index_t{}, std::make_tuple(std::size_t{})));

   public:
    proxy(const std::weak_ptr<storage_t>& local_storage, const index_t& index) : local_storage{ local_storage }, index{ index } {};

    auto operator[](std::size_t i) const
    {
      return proxy<storage_t, next_index_t>{ local_storage, std::tuple_cat(index, std::make_tuple(i)) };
    }

    auto& operator=(const T& value)
    {
      std::shared_ptr<storage>(local_storage)->set_value(index, value);
      return *this;
    }

    operator T() const
    {
      std::shared_ptr<storage_t> intermediate_matrix{ local_storage };
      return intermediate_matrix->get_value(index);
    }

    auto operator==(const T& value) const
    {
      std::shared_ptr<storage_t> intermediate_matrix{ local_storage };
      return value == intermediate_matrix->get_value(index);
    }

    friend bool operator==(const T& lhs, const proxy& rhs)
    {
      return rhs == lhs;
    }

   private:
    std::weak_ptr<storage_t> local_storage;
    index_t index;
  };

 private:
  std::shared_ptr<storage> local_storage;
};
