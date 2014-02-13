//
// Copyright (c) 2014 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#define BOOST_DI_INJECT_CFG_BRACKET

#include "boost/di/injector.hpp"

#include <boost/config.hpp>

#if !defined(BOOST_MSVC)
    #include <tr1/memory>
#else
    #include <memory>
#endif

#include <boost/test/unit_test.hpp>

namespace boost {
namespace di {

struct sp
{
    BOOST_DI_INJECT(sp, (boost::shared_ptr<int>, boost::shared_ptr<double>)) { }
};

BOOST_AUTO_TEST_CASE(lack_of_shared_ptr_fail) {
    injector<>().create<sp>();
}

} // namespace di
} // namespace boost
