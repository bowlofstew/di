//
// Copyright (c) 2012-2015 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_DI_CORE_POLICY_HPP
#define BOOST_DI_CORE_POLICY_HPP

#include "boost/di/aux_/utility.hpp"
#include "boost/di/aux_/type_traits.hpp"
#include "boost/di/core/binder.hpp"
#include "boost/di/core/pool.hpp"

namespace boost { namespace di { inline namespace v1 { namespace core {

BOOST_DI_HAS_METHOD(call_operator, operator());

template<
    class T
  , class TName
  , class TIsRoot
  , class TDeps
  , class TIgnore
> struct arg_wrapper {
    using type BOOST_DI_UNUSED = T;
    using name BOOST_DI_UNUSED = TName;
    using is_root BOOST_DI_UNUSED = TIsRoot;
    using ignore = TIgnore;

    template<class T_, class TName_, class TDefault_>
    using resolve =
        decltype(core::binder::resolve<T_, TName_, TDefault_>((TDeps*)nullptr));
};

template<class TDeps>
class policy {
public:
    template<
        class T
      , class TName
      , class TIsRoot
      , class TInitialization
      , class TDependency
      , class TIgnore
      , class... TCtor
      , class... TPolicies
    > static auto call(BOOST_DI_UNUSED const pool<aux::type_list<TPolicies...>>& policies
                     , BOOST_DI_UNUSED TDependency dependency
                     , aux::pair<TInitialization, aux::type_list<TCtor...>>, const TIgnore&) noexcept {

        int _[]{0, (call_impl<TPolicies, T, TName, TIsRoot, TPolicies, TDependency, TIgnore, TCtor...>(
            policies, dependency), 0)...}; (void)_;

       return std::is_same<
            aux::bool_list<aux::always<TPolicies>::value...>
          , aux::bool_list<decltype(call_impl<TPolicies, T, TName, TIsRoot, TPolicies, TDependency, TIgnore, TCtor...>(policies, dependency))::value...>
        >{};
    }

private:
    template<
        class TPolicy
      , class T
      , class TName
      , class TIsRoot
      , class TPolicies
      , class TDependency
      , class TIgnore
      , class... TCtor
    > static auto call_impl(const TPolicies& policies, TDependency dependency) noexcept {
        return call_impl_type<arg_wrapper<T, TName, TIsRoot, TDeps, TIgnore>, TDependency, TPolicy, TCtor...>(
            static_cast<const TPolicy&>(policies), dependency
        );
    }

    template<class TArg, class TDependency, class TPolicy, class... TCtor>
    static auto call_impl_type(const TPolicy& policy, TDependency dependency) noexcept {
        call_impl_args<TArg, TDependency, TPolicy, TCtor...>(policy, dependency);
        using type = decltype(call_impl_args<TArg, TDependency, TPolicy, TCtor...>(policy, dependency));
        return std::conditional_t<std::is_same<type, void>::value, std::true_type, type>{};
    }

    template<class TArg, class TDependency, class TPolicy, class... TCtor
           , BOOST_DI_REQUIRES(!has_call_operator<TPolicy, TArg, TDependency, TCtor...>::value)>
    static auto call_impl_args(const TPolicy& policy, TDependency) noexcept {
        return (policy)(TArg{});
    }

    template<class TArg, class TDependency, class TPolicy, class... TCtor
           , BOOST_DI_REQUIRES(has_call_operator<TPolicy, TArg, TDependency, TCtor...>::value)>
    static auto call_impl_args(const TPolicy& policy, TDependency dependency) noexcept {
        return (policy)(TArg{}, dependency, aux::type<TCtor>{}...);
    }
};

}}}} // boost::di::v1::core

#endif

