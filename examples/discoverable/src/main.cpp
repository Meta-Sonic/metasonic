#include "mts/print.h"
#include "mts/meta.h"
#include "json/json.h"

template <typename T, mts::enable_if_t<mts::is_reflectable_v<T>> = mts::enabled>
nlohmann::json serialize(const T& obj) {
  nlohmann::json jobj;

  mts::meta::iterate(obj, [&jobj](auto ptype) {
    using type = typename decltype(ptype)::type;

    if constexpr (mts::is_reflectable_v<type>) {
      jobj[ptype.key.value] = serialize(ptype.value);
    }
    else if constexpr (mts::is_usual_v<type>) {
      jobj[ptype.key.value] = ptype.value;
    }
    else if constexpr (mts::is_iterable_v<type>) {
      using value_type = typename type::value_type;

      if constexpr (mts::is_reflectable_v<value_type>) {
        nlohmann::json arr;

        for (auto it = ptype.value.begin(); it != ptype.value.end(); ++it) {
          arr.push_back(serialize(*it));
        }

        jobj[ptype.key.value] = std::move(arr);
      }
    }
  });

  return jobj;
}

template <typename T, mts::enable_if_t<mts::is_reflectable_v<T>> = mts::enabled>
void load(const nlohmann::json& jobj, T& obj) {

  mts::meta::iterate(obj, [&jobj](auto ptype) {
    using type = typename decltype(ptype)::type;

    if (auto it = jobj.find(ptype.key.value); it != jobj.end()) {

      if constexpr (mts::is_reflectable_v<type>) {
        load(it.value(), ptype.value);
      }
      else if constexpr (mts::is_usual_v<type>) {
        ptype.value = it.value();
      }
      else if constexpr (mts::is_iterable_v<type>) {
        using value_type = typename type::value_type;

        if constexpr (mts::is_reflectable_v<value_type>) {
          const nlohmann::json& arr = it.value();
          for (const auto& item : arr) {
            value_type v;
            load(item, v);
            ptype.value.push_back(std::move(v));
          }
        }
      }
    }
  });
}

template <typename T, mts::enable_if_t<mts::is_reflectable_v<T>> = mts::enabled>
inline std::string send(const T& obj) {
  nlohmann::json jobj;
  return serialize(obj).dump();
}

template <typename T, mts::enable_if_t<mts::is_reflectable_v<T>> = mts::enabled>
inline bool receive(const std::string& data, T& obj) {
  nlohmann::json jobj = nlohmann::json::parse(data, nullptr, false);
  if (jobj.is_discarded()) {
    return false;
  }

  load(jobj, obj);
  return true;
}

namespace mine {
struct abc {
  int a;
  float b;
  std::string c;
};
} // namespace mine.

MTS_DECLARE_DISCOVERABLE(mine::abc, a, b, c);

struct my_combined_type {
  mine::abc a;
  std::string b;
};

MTS_DECLARE_DISCOVERABLE(my_combined_type, a, b);

template <class T, mts::enable_if_t<mts::is_reflectable_v<T>> = mts::enabled>
void do_something(T& obj) {
  if constexpr (mts::meta::has<T, "b">()) {
    mts::meta::get<"b">(obj) = 3.3f;
  }
}

template <class T, mts::enable_if_t<mts::is_reflectable_v<T>> = mts::enabled>
void do_something_2(const T& obj) {
  if constexpr (mts::meta::has<T, "b">()) {
    mts::print("::", mts::meta::get<"b">(obj));
  }
}

int main() {
  mts::print(mts::is_discoverable_v<mine::abc>, mts::meta::has<mine::abc, "a">(),
      mts::meta::has_all<mine::abc, "a", "b", "c">());

  mine::abc a0 = { 1, 2.2f, "alex" };
  do_something(a0);
  mts::print(a0.b);

  using meta_type = mts::meta::object<mts::meta::property<"a", int>, mts::meta::property<"b", float>,
      mts::meta::property<"c", std::string>>;
  meta_type mt = { 2, 2.5f, "banana" };
  do_something(mt);
  mts::print(mt.get<"b">());

  do_something_2(a0);
  do_something_2(mt);
  do_something_2(meta_type{ 2, 2.5f, "banana" });
  do_something_2(mine::abc{ 2, 2.5f, "banana" });

  do_something_2(mts::meta::make_object<"a", "b", "c">(2, 8.5f, "banana"));

  nlohmann::json obj = serialize(a0);
  mts::print(obj.dump());

  mts::print(mts::meta::get<"a">(mt), mts::meta::get<"b">(mt), mts::meta::get<"c">(mt));
  load(obj, mt);
  mts::print(mts::meta::get<"a">(mt), mts::meta::get<"b">(mt), mts::meta::get<"c">(mt));

  {
    my_combined_type c0 = { { 322, 33.3f, "bingo" }, "banana" };
    my_combined_type c1;
    load(serialize(c0), c1);
    mts::print<mts::colon_string>("c0 == c1", c0.a.a == c1.a.a && c0.a.b == c1.a.b && c0.a.c == c1.a.c && c0.b == c1.b);
  }

  {
    mine::abc t0 = { 37, 33.3f, "bingo" };
    meta_type t1;

    std::string data = send(t0);
    receive(data, t1);

    mts::print<mts::colon_string>("t0 == t1", t0.a == t1.get<"a">() && t0.b == t1.get<"b">() && t0.c == t1.get<"c">());
  }

  return 0;
}
