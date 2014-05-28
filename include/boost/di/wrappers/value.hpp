//
// Copyright (c) 2014 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_DI_WRAPPERS_VALUE_HPP
#define BOOST_DI_WRAPPERS_VALUE_HPP

#include "boost/di/aux_/config.hpp"
#include "boost/di/aux_/memory.hpp"

#include <utility>
#include <boost/type.hpp>
#include <boost/config.hpp>

namespace boost {
namespace di {
namespace wrappers {

template<typename T>
class value
{
public:
    value(T value) // non explicit
        : value_(value)
    { }

    template<typename I>
    aux::shared_ptr<I> operator()(const type<aux::shared_ptr<I> >&) const {
        return aux::shared_ptr<I>(new I(value_));
    }

    template<typename I>
    aux_::shared_ptr<I> operator()(const type<aux_::shared_ptr<I> >&) const {
        return aux_::shared_ptr<I>(new I(value_));
    }

    template<typename I>
    I* operator()(const type<I*>&) const {
        return new I(value_);
    }

    template<typename I>
    I operator()(const type<I>&) const {
        return value_;
    }

    BOOST_DI_FEATURE(RVALUE_REFERENCES)(
        template<typename I>
        I&& operator()(const type<I&&>&) const {
            return std::move(value_);
        }
    )

    template<typename I>
    I& operator()(const type<I&>&) {
        return value_;
    }

private:
    T value_;
};

} // namespace wrappers
} // namespace di
} // namespace boost

#endif
