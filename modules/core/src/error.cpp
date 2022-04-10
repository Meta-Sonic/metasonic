#include "mts/error.h"
#include "mts/assert.h"
#include <sstream>

MTS_BEGIN_NAMESPACE
namespace {
class error_code_category : public std::error_category {
public:
  error_code_category() noexcept = default;
  virtual ~error_code_category() noexcept override = default;
  virtual const char* name() const noexcept override { return "mts"; }

  virtual std::string message(int ev) const override {
    switch (static_cast<_VMTS::errc>(ev)) {
    case errc::exception_error:
      return "An exception was thrown";
    case errc::unknown:
      return "Unknown error";
    default:
      mts_error("Unrecognized error");
      return "Unrecognized error";
    }
  }
};
inline static const error_code_category s_error_code_category;
} // namespace.

const std::error_category& generic_category() noexcept { return s_error_code_category; }

std::string error_result::message() const noexcept {
  std::stringstream ss;
  ss << *this;
  return ss.str();
}

std::ostream& operator<<(std::ostream& stream, const error_result& er) {
  stream << er.category().name() << " : " << er.code().message();

  if (!er.content().empty()) {
    stream << " : " << er.content();
  }

  return stream;
}

MTS_END_NAMESPACE
