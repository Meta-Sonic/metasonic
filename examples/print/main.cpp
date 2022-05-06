
#include "mts/print.h"
#include <map>
#include <string>
#include <sstream>
#include <vector>

// Custom print separator.
struct my_separator {
  static constexpr const char* value = " -> ";
};

// Custom print descriptor with separator and prefix.
struct cc {
  struct separator {
    static constexpr const char* value = " -- ";
  };
  static std::ostream& prefix(std::ostream& s) { return s << "[My Prefix] : "; }
};

// Custom print descriptor with prefix only.
struct cc2 {
  static std::ostream& prefix(std::ostream& s) { return s << "[My Prefix] : "; }
};

// Custom print descriptor with separator only.
struct cc3 {
  using separator = my_separator;
};

// Custom print descriptor that does nothing.
struct cc4 {};

// Custom print function to print with cc print descriptor.
template <typename... Ts>
inline void my_print(const Ts&... ts) {
  mts::custom_print<cc>(std::cout, ts...);
}

// Custom object with output stream operator.
struct my_object {
  int value;

  friend std::ostream& operator<<(std::ostream& stream, const my_object& obj) {
    return stream << "my_object -- " << obj.value;
  }
};

// Custom object without output stream operator.
struct my_unknown_object {};

inline void print_example_index() {
  static int index = 1;
  std::cout << "#" << std::setfill('0') << std::setw(2) << index++ << ": ";
}
# 2 "43"

int main(int argc, const char* argv[]) {
  // Normal print.
  print_example_index();
  mts::print("Bingo", "Bango");

  // Warning print.
  print_example_index();
  mts::warnprint("Bingo");

  // Error print.
  print_example_index();
  mts::errprint("Bingo");

  // Timestamped print.
  print_example_index();
  mts::tmprint("Bingo");

  // Basic print to std::cerr.
  print_example_index();
  mts::basic_print(std::cerr, "Bingo", "Bango");

  // Basic print to stringstream..
  print_example_index();
  std::stringstream ss;
  mts::basic_print(ss, "Banana");
  std::cout << ss.str();

  // Basic header print to std::cout.
  print_example_index();
  mts::basic_header_print(std::cout, "My Header", "Bongo");

  // Separator empty_string.
  print_example_index();
  mts::print<mts::empty_string>("Bingo", "Bango");

  // Separator space_string.
  print_example_index();
  mts::print<mts::space_string>("Bingo", "Bango");

  // Separator comma_string.
  print_example_index();
  mts::print<mts::comma_string>("Bingo", "Bango");

  // Separator comma_space_string (this is the default one).
  print_example_index();
  mts::print<mts::comma_space_string>("Bingo", "Bango");

  // Separator equal_string.
  print_example_index();
  mts::print<mts::equal_string>("Bingo", 32);

  // Separator custom.
  print_example_index();
  mts::print<my_separator>("Bingo", "Bango");

  // Custom print.
  print_example_index();
  mts::custom_print<cc>(std::cout, "Bingo", "Bango");

  // Custom print function.
  print_example_index();
  my_print("Bingo", "Bango");

  // Custom print descriptor with prefix only.
  print_example_index();
  mts::custom_print<cc2>(std::cout, "Bingo", "Bango");

  // Custom print descriptor with separator only.
  print_example_index();
  mts::custom_print<cc3>(std::cout, "Bingo", "Bango");

  // Custom print descriptor that does nothing.
  // Same as mts::basic_print.
  print_example_index();
  mts::custom_print<cc4>(std::cout, "Bingo", "Bango");

  // Vector print.
  print_example_index();
  mts::print(std::vector<int>{ 1, 2, 3 });

  // Pair print.
  print_example_index();
  mts::print(std::pair<std::string, float>{ "Value", 34.2f });

  // Tuple print.
  print_example_index();
  mts::print(std::tuple<int, float>{ 1, 34.2f });

  // Map print.
  print_example_index();
  mts::print(std::map<std::string, int>{ { "Bingo", 32 }, { "Bango", 1232 } });

  // Object print with ostream operator.
  print_example_index();
  mts::print(my_object{ 123 }, my_object{ 456 });

  // Object print without ostream operator.
  print_example_index();
  mts::print(my_unknown_object{});

  // Pair print with object.
  print_example_index();
  mts::print(std::pair<std::string, my_object>{ "Value", my_object{ 12345 } });

  return 0;
}
