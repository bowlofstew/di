//
// Copyright (c) 2012-2015 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/di.hpp>
#include <memory>
#include <cassert>

namespace di = boost::di;

struct i1 { virtual ~i1() noexcept = default; virtual void dummy1() = 0; };
struct i2 { virtual ~i2() noexcept = default; virtual void dummy2() = 0; };
struct impl1 : i1 { void dummy1() override { } };
struct impl2 : i2 { void dummy2() override { } };

struct module {
    di::injector<i1, i2> configure() const noexcept;

    int i = 0;
};

di::injector<i1, i2> module::configure() const noexcept {
    return di::make_injector(
        di::bind<i1, impl1>
      , di::bind<i2, impl2>
    );
}

int main() {
    auto injector = di::make_injector(
        module{}
    );

    auto up1 = injector.create<std::unique_ptr<i1>>();
    assert(dynamic_cast<impl1*>(up1.get()));

    auto up2 = injector.create<std::unique_ptr<i2>>();
    assert(dynamic_cast<impl2*>(up2.get()));
}

