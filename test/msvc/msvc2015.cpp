//
// Copyright (c) 2012-2015 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#if defined(_MSC_VER)
#include <boost/di.hpp>
#include <functional>

namespace di = boost::di;

auto name = []{};
auto other_name = []{};
auto my_int = []{};
auto my_int2 = []{};

auto i_ = 2;

struct i {
    virtual ~i() noexcept = default;
    virtual void dummy() = 0;
};

struct i1 { virtual ~i1() noexcept = default; virtual void dummy1() = 0; };
struct impl1 : i1 { void dummy1() override { } };
struct impl1_int : i1 { impl1_int(int i) : i(i) { } void dummy1() override { } int i = 0; };

struct i2 {
    virtual ~i2() noexcept = default;
    virtual void dummy2() = 0;
};

struct impl : i {
    impl(int i, std::shared_ptr<i2>){
        expect_eq(87, i);
    }
    void dummy() override { };
};

struct impl2 : i2 {
    BOOST_DI_INJECT(impl2) { }
    void dummy2() override { }
};

struct i3 {
    virtual ~i3() noexcept = default;
    virtual void dummy3() = 0;
};

struct impl3 : i3 {
    BOOST_DI_INJECT(impl3, (named = my_int) const int& i, double) {
        expect_eq(21, i);
    }
    void dummy3() override { }
};

struct c {
    BOOST_DI_INJECT(c, (named = my_int2) int& ref, int, int, int, double, double, std::shared_ptr<i>, (named = name) std::shared_ptr<i3>) {
		expect_eq(&i_, &ref);
	}
};

struct module {
    di::injector<i> configure() const {
        return di::make_injector(
                di::bind<i, impl>()
              , di::bind<i2, impl2>()
              , di::bind<int>().to(87)
        );
    }
};

test ft = [] {
    auto injector = di::make_injector(
          di::bind<int>().to(42)
        , module{}
	    , di::bind<int>().named(my_int2).to(i_)
        , di::bind<i3, impl3>().named(name)
        , di::bind<int>().named(my_int).to(21)
    );

    auto object = injector.create<i*>();
    expect(object != nullptr);
    expect_eq(42, injector.create<int>());

    injector.create<c>();
};

test ctor_refs = [] {
    struct c {
        BOOST_DI_INJECT(c
        , const std::shared_ptr<i1>& sp
        , int& i
        , const double& d
        , const std::string& str
        , (named = name) const std::string& nstr
        //, const std::function<int()>& f
        , long&& l
        , short s)
            : i(i), d(d), str(str), nstr(nstr), l(std::move(l)), s(s)
        {
            sp->dummy1();
        }

        int& i;
        const double& d;
        std::string str;
        std::string nstr;
        //std::function<int()> f;
        long l = 0;
        short s = 0;
    };

    struct c_inject {
        BOOST_DI_INJECT(c_inject
                      , const std::shared_ptr<i1>& sp
                      , int& i
                      , const double& d
                      , const std::string& str
                      , (named = name) const std::string& nstr
                      //, std::function<int()> f
                      , long&& l
                      , short s)
            : i(i), d(d), str(str), nstr(nstr), /*f(f),*/ l(std::move(l)), s(s)
        {
            sp->dummy1();
        }

        int& i;
        const double& d;
        std::string str;
        std::string nstr;
        //std::function<int()> f;
        long l = 0;
        short s = 0;
    };

    struct c_aggregate {
        const std::shared_ptr<i1>& sp;
        int& i;
        const double& d;
        std::string str;
        std::string nstr;
        //std::function<int()> f;
        long l = 0;
        short s = 0;
    };

    auto test = [](auto type, const auto& bind_i1) {
        auto i = 0;
        constexpr auto d = 0.0;

        auto injector = di::make_injector(
            di::bind<int>().to(i)
          , di::bind<double>().to(d)
          , di::bind<std::string>().to("str")
          , di::bind<std::string>().named(name).to("named str")
          , bind_i1
          , di::bind<short>().to(short{42})
          , di::bind<long>().to(123l)
          //, di::bind<std::function<int()>>().to([]{return 87;})
        );

        auto object = injector.template create<typename decltype(type)::type>();
        expect_eq(&i, &object.i);
        expect_eq(&d, &object.d);
        expect_eq("str", object.str);
        expect_eq(42, object.s);
        //expect_eq(87, object.f());
        expect_eq(123, object.l);
    };

    test(test_type<c>{}, di::bind<i1, impl1>());
    test(test_type<c_inject>{}, di::bind<i1, impl1>());
    //test(test_type<c_aggregate>{}, di::bind<i1, impl1>());

    test(test_type<c>{}, di::bind<i1>().to(std::make_shared<impl1>()));
    test(test_type<c_inject>{}, di::bind<i1>().to(std::make_shared<impl1>()));
    //test(test_type<c_aggregate>{}, di::bind<i1>().to(std::make_shared<impl1>()));
};

test refs_vs_copy = [] {
    struct cc {
        BOOST_DI_INJECT(cc
         , (named = name) const int& i
         , (named = name) const std::string& s
         , (named = other_name) int& i_ref
         , int& ii)
            : str(s), i(i), i_ref(i_ref), ii(ii)
        { }

        std::string str;
        int i = 0;
        int& i_ref;
        int& ii;
    };

    struct cc_inject {
        BOOST_DI_INJECT(cc_inject
                      , (named = name) const int& i
                      , (named = name) const std::string& s
                      , (named = other_name) int& i_ref
                      , int& ii)
            : str(s), i(i), i_ref(i_ref), ii(ii)
        { }

        std::string str;
        int i = 0;
        int& i_ref;
        int& ii;
    };

    std::string ref = "named str";
    auto i = 42;

    {
    auto injector = di::make_injector(
        di::bind<std::string>().named(name).to(ref)
      , di::bind<int>().named(name).to(i)
      , di::bind<int>().named(other_name).to(i)
      , di::bind<int>().to(i)
    );
    auto object = injector.create<cc>();
    expect_eq(ref, object.str);
    expect_eq(i, object.i);
    expect_eq(i, object.i_ref);
    expect_eq(&i, &object.i_ref);
    expect_eq(i, object.ii);
    expect_eq(&i, &object.ii);
    }

    {
    auto injector = di::make_injector(
        di::bind<std::string>().named(name).to(ref)
      , di::bind<int>().named(name).to(i)
      , di::bind<int>().named(other_name).to(i)
      , di::bind<int>().to(i)
    );
    auto object = injector.create<cc_inject>();
    expect_eq(ref, object.str);
    expect_eq(i, object.i);
    expect_eq(i, object.i_ref);
    expect_eq(&i, &object.i_ref);
    expect_eq(i, object.ii);
    expect_eq(&i, &object.ii);
    }

    {
    auto injector = di::make_injector(
        di::bind<std::string>().named(name).to(ref)
      , di::bind<int>().named(name).to(i)
      , di::bind<int>().named(other_name).to(i)
      , di::bind<int>().to(i)
    );
    auto object = injector.create<cc>();
    expect_eq(ref, object.str);
    expect_eq(i, object.i);
    expect_eq(i, object.i_ref);
    expect_eq(&i, &object.i_ref);
    expect_eq(i, object.ii);
    expect_eq(&i, &object.ii);
    }

    {
    auto injector = di::make_injector(
        di::bind<std::string>().named(name).to(ref)
      , di::bind<int>().named(name).to(i)
      , di::bind<int>().named(other_name).to(i)
      , di::bind<int>().to(i)
    );
    auto object = injector.create<cc_inject>();
    expect_eq(ref, object.str);
    expect_eq(i, object.i);
    expect_eq(i, object.i_ref);
    expect_eq(&i, &object.i_ref);
    expect_eq(i, object.ii);
    expect_eq(&i, &object.ii);
    }
};

test create_with_default_values = [] {
    constexpr auto i = 42;

    struct default_values {
        BOOST_DI_INJECT_TRAITS(int);

        default_values(int i, float f = 42.0, double d = 87.0)
            : i(i), f(f), d(d)
        { }

        int i = 0;
        float f = 0.0;
        double d = 0.0;
    };

    auto injector = make_injector(
        di::bind<int>().to(i)
    );

    auto object = injector.create<default_values>();

    expect_eq(i, object.i);
    expect_eq(42.0, object.f);
    expect_eq(87.0, object.d);
};

test runtime_factory_impl = [] {
    constexpr auto i = 42;

    auto test = [&](bool debug_property) {
        auto injector = make_injector(
            di::bind<int>().to(i)
          , di::bind<i1>().to([&](const auto& injector) -> std::shared_ptr<i1> {
                if (debug_property) {
                    return std::make_shared<impl1>();
                }

                return injector.template create<std::shared_ptr<impl1_int>>();
            })
        );

        return injector.create<std::shared_ptr<i1>>();
    };

    {
    auto object = test(false);
    expect(dynamic_cast<impl1_int*>(object.get()));
    expect_eq(i, dynamic_cast<impl1_int*>(object.get())->i);
    }

    {
    auto object = test(true);
    expect(dynamic_cast<impl1*>(object.get()));
    }
};

test override_priority = [] {
    auto injector = di::make_injector(
        di::bind<int>().to(12) [di::override]
      , di::bind<int, std::integral_constant<int, 42>>()
    );

    auto object = injector.create<int>();

    expect_eq(12, object);
};

test override_priority_order = [] {
    auto injector = di::make_injector(
        di::bind<int, std::integral_constant<int, 41>>()
      , di::bind<int>().to([]{return 42;}) [di::override]
    );

    expect_eq(42, injector.create<int>());
};

test override_priority_interface = [] {
    auto injector = di::make_injector(
        di::bind<i1, impl1>()
      , di::bind<i1, impl1_int>() [di::override]
    );

    auto object = injector.create<std::unique_ptr<i1>>();
    expect(dynamic_cast<impl1_int*>(object.get()));
};

test override_priority_interface_module = [] {
    struct module {
        auto configure() const {
            return di::make_injector(
                di::bind<i1, impl1_int>()
            );
        }
    };

    auto injector = di::make_injector(
        module{}
      , di::bind<i1, impl1>() [di::override]
    );

    auto object = injector.create<std::unique_ptr<i1>>();
    expect(dynamic_cast<impl1*>(object.get()));
};

test scopes_injector_lambda_injector = [] {
    constexpr short s = 42;
    auto injector = di::make_injector(
        di::bind<short>().to(s)
      , di::bind<int>().to([](const auto& injector){ return static_cast<int>(injector.template create<short>()); })
    );

    expect_eq(s, injector.create<int>());
};

test string_creation = [] {
	struct string {
		std::string str;
	};

	expect_eq("", di::make_injector().create<string>().str);
};

test request_value_and_ptr_in_unique = [] {
    struct c {
        int i = 0;
        int* ptr = nullptr;
    };

    auto injector = di::make_injector(
        di::bind<int>().in(di::unique)
    );

    auto object = injector.create<c>();
    delete object.ptr;
};

test inject = [] {
    constexpr auto i = 42;

    struct c {
        c(std::initializer_list<int>) { }

        c(int, double, float) { }

        BOOST_DI_INJECT(c, int i, double d)
            : i(i), d(d)
        { }

        int i = 0;
        double d = 0.0;
    };

    auto injector = di::make_injector(
        di::bind<int>().to(i)
    );

    auto object = injector.create<c>();

    expect_eq(i, object.i);
    expect_eq(0.0, object.d);
};

test automatic_inject = [] {
    constexpr auto i = 42;

    struct c {
        c(std::initializer_list<int>) { }

        c(int i, double d)
            : i(i), d(d)
        { }

        int i = 0;
        double d = 0.0;
    };

    auto injector = di::make_injector(
        di::bind<int>().to(i)
    );

    auto object = injector.create<c>();

    expect_eq(i, object.i);
    expect_eq(0.0, object.d);
};

test automatic_inject_with_initializer_list = [] {
    constexpr auto i = 42;

    struct c {
        c(int i, std::initializer_list<int> il)
            : i(i), il(il)
        { }

        int i = 0;
        std::initializer_list<int> il;
    };

    auto injector = di::make_injector(
        di::bind<int>().to(i)
    );

    auto object = injector.create<c>();

    expect_eq(i, object.i);
    expect_eq(0, object.il.size());
};

#endif

