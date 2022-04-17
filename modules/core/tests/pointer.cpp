#include <gtest/gtest.h>
#include "mts/pointer.h"

namespace {
class obj : public mts::managed {
public:
  obj(int v = 0)
      : value(v) {}
  int value = 0;
};

TEST(core, wait) {
  {
    mts::managed_ptr<obj> ptr(new obj(), true);
    EXPECT_TRUE(ptr.is_valid());
    EXPECT_TRUE(ptr.is_shared());
    EXPECT_FALSE(ptr.is_reference());
  }

  {
    mts::managed_ptr<obj> ptr(new obj(), false);
    EXPECT_TRUE(ptr.is_valid());
    EXPECT_TRUE(ptr.is_reference());
    EXPECT_FALSE(ptr.is_shared());
  }

  {
    obj o;
    mts::managed_ptr<obj> ptr(&o, false);
    EXPECT_TRUE(ptr.is_valid());
    EXPECT_TRUE(ptr.is_reference());
    EXPECT_FALSE(ptr.is_shared());
  }

  {
    mts::managed_ptr<obj> p;
    {
      obj o;
      mts::managed_ptr<obj> ptr(&o, false);
      EXPECT_TRUE(ptr.is_valid());
      EXPECT_TRUE(ptr.is_reference());
      EXPECT_FALSE(ptr.is_shared());

      p = ptr;
    }
    EXPECT_FALSE(p.is_valid());
    EXPECT_TRUE(p.is_reference());
  }

  {
    mts::managed_ptr<obj> p;
    {
      mts::managed_ptr<obj> ptr(new obj, true);
      EXPECT_TRUE(ptr.is_valid());
      EXPECT_FALSE(ptr.is_reference());
      EXPECT_TRUE(ptr.is_shared());

      p = ptr;
    }
    EXPECT_TRUE(p.is_valid());
    EXPECT_FALSE(p.is_reference());
    EXPECT_TRUE(p.is_shared());

    mts::weak_managed_ptr<obj> w = p;

    EXPECT_TRUE(w.is_valid());

    p.reset();
    EXPECT_FALSE(w.is_valid());
  }

  {
    mts::managed_ptr<obj> p;
    {
      mts::managed_ptr<obj> ptr(new obj(78), false);
      EXPECT_TRUE(ptr.is_valid());
      EXPECT_TRUE(ptr.is_reference());
      EXPECT_FALSE(ptr.is_shared());

      p = ptr;
    }
    EXPECT_TRUE(p.is_valid());
    EXPECT_TRUE(p.is_reference());
    EXPECT_FALSE(p.is_shared());

    mts::weak_managed_ptr<obj> w = p;

    EXPECT_TRUE(w.is_valid());

    p.reset();
    EXPECT_TRUE(w.is_valid());

    EXPECT_EQ(w->value, 78);
    delete w.get();
    EXPECT_FALSE(w.is_valid());
  }

  {
    mts::managed_ptr<obj> ptr(new obj(), true);
    mts::managed_ptr<const obj> p = ptr;
  }

  {
    mts::managed_ptr<obj> ptr = mts::make_managed<obj>(67);
    EXPECT_EQ(ptr->value, 67);
    EXPECT_TRUE(ptr.is_valid());
    EXPECT_FALSE(ptr.is_reference());
    EXPECT_TRUE(ptr.is_shared());
  }
}

TEST(core, managed) {
  {
    obj* o = new obj;
    mts::managed::validator v(*o);
    delete o;
    EXPECT_TRUE(v.expired());
  }
}
} // namespace
