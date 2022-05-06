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

TEST(core, optional) {
  {
    mts::optional_ptr<int> p;
    EXPECT_EQ(p.owned(), false);
  }

  {
    mts::optional_ptr<int> p(new int(32), true);
    EXPECT_EQ(p.owned(), true);
  }

  {
    int a = 32;
    mts::optional_ptr<int> p(&a, false);
    EXPECT_EQ(p.owned(), false);
  }

  {
    std::unique_ptr<int> a;
    mts::optional_ptr<int> p(a);
    EXPECT_TRUE(p.get() == nullptr);
    EXPECT_EQ(p.owned(), false);
  }

  {
    std::unique_ptr<int> a;
    mts::optional_ptr<int> p(std::move(a));
    EXPECT_TRUE(p.get() == nullptr);
    EXPECT_EQ(p.owned(), false);
  }

  class ABC {
  public:
    int abc;
  };

  class ABCD : public ABC {
  public:
    int d;
  };

  {
    std::unique_ptr<ABCD> a(new ABCD);
    mts::optional_ptr<ABC> p(a);
    EXPECT_EQ((bool)a, true);
    EXPECT_EQ(p.owned(), false);
  }

  {
    std::unique_ptr<ABCD> a(new ABCD);
    mts::optional_ptr<ABC> p(std::move(a));
    EXPECT_EQ((bool)a, false);
    EXPECT_EQ(p.owned(), true);

    mts::optional_ptr<ABC> p2 = std::move(p);
    EXPECT_EQ(p2.owned(), true);
    EXPECT_EQ(p.owned(), false);
  }

  {
    mts::optional_ptr<ABC> p(new ABCD, true);
    EXPECT_EQ(p.owned(), true);
  }

  {
    ABCD a;
    mts::optional_ptr<ABC> p(&a, false);
    EXPECT_EQ(p.owned(), false);
  }

  {
    ABCD a;
    mts::optional_ptr<ABC> p(&a, false);
    EXPECT_EQ(p.owned(), false);

    mts::optional_ptr<ABC> p2 = std::move(p);
    EXPECT_EQ(p2.owned(), false);
  }

  {
    ABCD a;
    mts::optional_ptr<ABCD> p(&a, false);
    EXPECT_EQ(p.owned(), false);

    mts::optional_ptr<ABC> p2 = std::move(p);
    EXPECT_EQ(p2.owned(), false);
  }

  {
    mts::optional_ptr<ABCD> p(new ABCD, true);
    EXPECT_EQ(p.owned(), true);

    mts::optional_ptr<ABC> p2 = std::move(p);
    EXPECT_EQ(p2.owned(), true);
    EXPECT_EQ(p.owned(), false);
  }

  struct Deleted {
    Deleted(bool& deleted)
        : _deleted(deleted) {}
    ~Deleted() { _deleted = true; }
    bool& _deleted;
  };

  {

    bool deleted = false;
    mts::optional_ptr<Deleted> p(new Deleted(deleted), true);
    EXPECT_EQ(p.owned(), true);
    EXPECT_EQ(deleted, false);

    mts::optional_ptr<Deleted> p2 = std::move(p);
    EXPECT_EQ(deleted, false);

    p2.reset();
    EXPECT_EQ(p.owned(), false);
    EXPECT_EQ(deleted, true);
  }

  {

    bool deleted = false;
    mts::optional_ptr<Deleted> p(new Deleted(deleted), true);
    EXPECT_EQ(p.owned(), true);
    EXPECT_EQ(deleted, false);

    mts::optional_ptr<Deleted> p2 = std::move(p);
    EXPECT_EQ(deleted, false);

    Deleted* d = p2.release();
    EXPECT_EQ(p.owned(), false);
    EXPECT_EQ(deleted, false);

    delete d;
    EXPECT_EQ(deleted, true);
  }

  {

    bool deleted = false;

    {
      mts::optional_ptr<Deleted> p(new Deleted(deleted), true);
      EXPECT_EQ(p.owned(), true);
      EXPECT_EQ(deleted, false);

      bool deleted2 = false;
      mts::optional_ptr<Deleted> p2(new Deleted(deleted2), true);
      EXPECT_EQ(deleted2, false);

      p2 = std::move(p);
      EXPECT_EQ(deleted, false);
      EXPECT_EQ(deleted2, true);
      EXPECT_EQ(p.owned(), false);
      EXPECT_EQ(p2.owned(), true);

      p = std::move(p2);
      EXPECT_EQ(deleted, false);
      EXPECT_EQ(deleted2, true);
      EXPECT_EQ(p.owned(), true);
      EXPECT_EQ(p2.owned(), false);
    }

    EXPECT_EQ(deleted, true);
  }

  {
    mts::optional_ptr<ABCD> p(new ABCD, true);
    p->abc = 32;
    EXPECT_EQ(p.owned(), true);

    mts::optional_ptr<ABC> p2(new ABCD, true);
    p2->abc = 33;

    p2 = std::move(p);
    EXPECT_EQ(p2.owned(), true);
    EXPECT_EQ(p.owned(), false);

    EXPECT_EQ(p2->abc, 32);
  }

  {
    mts::optional_ptr<ABC> p(new ABC, true);
    std::unique_ptr<ABCD> u(new ABCD);
    p = u;
    EXPECT_EQ(p.owned(), false);
    p = std::unique_ptr<ABCD>(new ABCD);
    EXPECT_EQ(p.owned(), true);

    p = std::unique_ptr<ABCD>();
    EXPECT_EQ(p.owned(), false);

    p = mts::optional_ptr<ABC>();
    EXPECT_EQ(p.owned(), false);

    p = mts::optional_ptr<ABC>(new ABC, true);
    EXPECT_EQ(p.owned(), true);

    mts::optional_ptr<ABC> p2;
    p = std::move(p2);
    EXPECT_EQ(p.owned(), false);
  }

  {
    int a = 32;
    mts::optional_ptr<int> ptr(&a, false);

    *ptr = 55;
    EXPECT_EQ(*ptr, 55);
    EXPECT_EQ(a, 55);

    a = 65;
    EXPECT_EQ(*ptr, 65);
    EXPECT_EQ(a, 65);

    ptr = mts::optional_ptr<int>(new int(32), true);
    EXPECT_EQ(*ptr, 32);
    EXPECT_EQ(a, 65);

    *ptr = 46;
    EXPECT_EQ(*ptr, 46);
    EXPECT_EQ(a, 65);

    ptr = mts::optional_ptr<int>();
    EXPECT_FALSE(ptr.owned());
    EXPECT_FALSE((bool)ptr);
    EXPECT_FALSE(ptr.get());
  }

  {
    bool deleted = false;
    std::unique_ptr<Deleted> u(new Deleted(deleted));
    mts::optional_ptr<Deleted> p = u;
    EXPECT_EQ(p.owned(), false);

    p = { u.release(), true };
    EXPECT_EQ((bool)u, false);
    EXPECT_EQ(p.owned(), true);

    p = u;
    EXPECT_EQ(p.owned(), false);
  }

  {
    mts::optional_ptr<ABCD> a1(new ABCD, true);

    std::unique_ptr<ABCD> u1(new ABCD);
    std::unique_ptr<ABCD> u2(new ABCD);

    std::vector<mts::optional_ptr<ABC>> va;
    va.push_back(std::move(a1));
    va.push_back(u1);
    va.push_back(std::move(u2));
    va.push_back(std::make_unique<ABCD>());

    EXPECT_EQ(a1.owned(), false);

    EXPECT_EQ(va[0].owned(), true);
    EXPECT_EQ(va[1].owned(), false);
    EXPECT_EQ(va[2].owned(), true);
    EXPECT_EQ(va[3].owned(), true);

    va[0] = mts::optional_ptr<ABC>();
    EXPECT_EQ(va[0].owned(), false);

    va.pop_back();
    va.resize(4);
    EXPECT_EQ(va[3].owned(), false);
  }

  {
    mts::optional_ptr<ABC> p(nullptr);
    EXPECT_EQ(p.owned(), false);

    std::unique_ptr<ABC> u(new ABC);
    p = std::move(u);
    EXPECT_EQ(p.owned(), true);
  }
}
} // namespace
