//
// Copyright (c) 2012-2015 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_DI_CONFIG_HPP
#define BOOST_DI_CONFIG_HPP

#include "boost/di/aux_/utility.hpp"
#include "boost/di/aux_/type_traits.hpp"
#include "boost/di/core/pool.hpp"
#include "boost/di/concepts/callable.hpp"
#include "boost/di/providers/stack_over_heap.hpp"

#if defined(BOOST_DI_CFG)
    class BOOST_DI_CFG;
#else
    #define BOOST_DI_CFG boost::di::config
#endif

namespace boost { namespace di { inline namespace v1 {

template<class... TPolicies, BOOST_DI_REQUIRES_MSG(concepts::callable<TPolicies...>)>
inline auto make_policies(const TPolicies&... args) noexcept {
    return core::pool<aux::type_list<TPolicies...>>(args...);
}

class config {
public:
    auto provider() const noexcept {
        return providers::stack_over_heap{};
    }

    auto policies() noexcept {
        return make_policies();
    }
};

}}} // boost::di::v1

#endif

