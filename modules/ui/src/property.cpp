#include "mts/ui/property.h"
#include "mts/print.h"

MTS_BEGIN_NAMESPACE
std::ostream& operator<<(std::ostream& stream, const property& msg) {
  using msg_type = enum property::type;

  switch (msg.type()) {
  case msg_type::real:
    stream << msg.value<double>();
    break;
  case msg_type::integer:
    stream << msg.value<long>();
    break;
  case msg_type::uinteger:
    stream << msg.value<unsigned long>();
    break;
  case msg_type::string_view:
    stream << msg.value<std::string_view>();
    break;
  case msg_type::pointer:
    stream << msg.value<void*>();
    break;
  case msg_type::object:
    stream << msg.value<object*>();
    break;
  case msg_type::point:
    stream << msg.value<_VMTS::point<int>>();
    break;
  case msg_type::fpoint:
    stream << msg.value<_VMTS::point<float>>();
    break;
  case msg_type::size:
    stream << msg.value<_VMTS::size<int>>();
    break;
  case msg_type::fsize:
    stream << msg.value<_VMTS::size<float>>();
    break;
  case msg_type::rect:
    stream << msg.value<_VMTS::rect<int>>();
    break;
  case msg_type::frect:
    stream << msg.value<_VMTS::rect<float>>();
    break;
  case msg_type::color:
    stream << msg.value<_VMTS::color>();
    break;
  case msg_type::empty:
    stream << "Empty";
    break;
  default:
    stream << "Empty";
  }
  return stream;
}
MTS_END_NAMESPACE
