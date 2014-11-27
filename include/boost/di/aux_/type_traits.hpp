//
// Copyright (c) 2014 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_DI_AUX_TYPE_TRAITS_HPP
#define BOOST_DI_AUX_TYPE_TRAITS_HPP

#include <type_traits>
#include "boost/di/aux_/utility.hpp"

#define BOOST_DI_HAS_TYPE(name)                                                 \
    template<class>                                                             \
    std::false_type has_##name##_impl(...);                                     \
                                                                                \
    template<class T>                                                           \
    std::true_type has_##name##_impl(typename T::name*);                        \
                                                                                \
    template<class T>                                                           \
    using has_##name = decltype(has_##name##_impl<T>(0))

#define BOOST_DI_HAS_METHOD(name, f)                                            \
	template<class T, class... TArgs>                                           \
    decltype(std::declval<T>().f(std::declval<TArgs>()...), std::true_type())   \
    has_##name##_impl(int);                                                     \
                                                                                \
	template<class, class...>                                                   \
    std::false_type has_##name##_impl(...);                                     \
                                                                                \
	template<class T, class... TArgs>                                           \
    using has_##name = decltype(has_##name##_impl<T, TArgs...>(0))

#define BOOST_DI_HAS_METHOD_SIGN(name, f)                                       \
    template<class T, class TSign>                                              \
    std::true_type has_##name##_impl(non_type<TSign, &T::f>* = 0);              \
                                                                                \
    template<class, class>                                                      \
    std::false_type has_##name##_impl(...);                                     \
                                                                                \
    template<class T, class TSign>                                              \
    using has_##name = decltype(has_##name##_impl<T, TSign>(0))

#define BOOST_DI_HAS_METHOD_CALL(name, f)                                       \
    struct has_##name##_base {                                                  \
        void f();                                                               \
    };                                                                          \
                                                                                \
    template<class T>                                                           \
    std::false_type has_##name##_impl(                                          \
        T*, non_type<void (has_##name##_base::*)(), &T::f>* = 0                 \
    );                                                                          \
                                                                                \
    template<class>                                                             \
    std::true_type has_##name##_impl(...);                                      \
                                                                                \
    template<class T, typename = void>                                          \
    struct has_##name                                                           \
        : decltype(has_##name##_impl<inherit<T, has_##name##_base>>(0))         \
    { };                                                                        \
                                                                                \
    template<class T>                                                           \
    struct has_##name<T, std::enable_if_t<!std::is_class<T>::value>>            \
        : std::false_type                                                       \
    { }

namespace boost {
namespace di {
namespace aux {

template<bool b, class T = void>
using enable_if_t = typename std::enable_if<b, T>::type;

template<bool b, class T, class F>
using conditional_t = typename std::conditional<b, T, F>::type;

template<class T, class... TArgs>
using is_constructible_t = typename std::is_constructible<T, TArgs...>::type;

template<class T>
using remove_pointer_t = typename std::remove_pointer<T>::type;

template<class T>
using remove_reference_t = typename std::remove_reference<T>::type;

template<class T>
using remove_accessors = std::remove_cv<aux::remove_pointer_t<aux::remove_reference_t<T>>>;

template<class T>
using remove_accessors_t = typename remove_accessors<T>::type;

template<class T, class = void>
struct deref_type;

template<typename T>
using deref_type_t = typename deref_type<T>::type;

template<class T>
using make_plain = deref_type<remove_accessors_t<deref_type_t<remove_accessors_t<T>>>>;

template<class T>
using make_plain_t = typename make_plain<T>::type;

template<class T, class>
struct deref_type {
    using type = T;
};

BOOST_DI_HAS_TYPE(element_type);

template<class T>
struct deref_type<T, enable_if_t<has_element_type<T>::value>> {
    using type = typename T::element_type;
};

BOOST_DI_HAS_TYPE(named_type);

template<class T>
struct deref_type<T, enable_if_t<has_named_type<T>::value>> {
    using type = make_plain_t<typename T::named_type>;
};

template<class T>
struct function_traits
    : function_traits<decltype(&T::operator())>
{ };

template<class R, class... TArgs>
struct function_traits<R(*)(TArgs...)> {
    using result_type = R;
    using base_type = none_t;
    using args = type_list<TArgs...>;
};

template<class R, class T, class... TArgs>
struct function_traits<R(T::*)(TArgs...)> {
    using result_type = R;
    using base_type = T;
    using args = type_list<TArgs...>;
};

template<class R, class T, class... TArgs>
struct function_traits<R(T::*)(TArgs...) const> {
    using result_type = R;
    using base_type = T;
    using args = type_list<TArgs...>;
};

template<class T, class... TArgs>
decltype(void(T{std::declval<TArgs>()...}), std::true_type())
test_is_braces_constructible(int);

template<class, class...>
std::false_type test_is_braces_constructible(...);

template<class T, class... TArgs>
using is_braces_constructible = decltype(test_is_braces_constructible<T, TArgs...>(0));

template<class T, class... TArgs>
using is_braces_constructible_t = typename is_braces_constructible<T, TArgs...>::type;

}}} // namespace boost::di::aux

#endif
