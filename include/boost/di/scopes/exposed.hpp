//
// Copyright (c) 2012-2015 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_DI_SCOPES_EXPOSED_HPP
#define BOOST_DI_SCOPES_EXPOSED_HPP

#include <memory>
#include <type_traits>
#include "boost/di/scopes/deduce.hpp"

namespace boost { namespace di { inline namespace v1 { namespace scopes {

template<class TScope = scopes::deduce>
class exposed {
public:
    template<class TExpected, class TGiven>
    class scope {
        using type = std::conditional_t<
            std::is_copy_constructible<TExpected>::value
          , TExpected
          , TExpected*
        >;

        struct iprovider {
            virtual ~iprovider() noexcept = default;
            virtual TExpected* get(const type_traits::heap& = {}) const noexcept = 0;
            virtual type get(const type_traits::stack&) const noexcept = 0;
        };

        template<class TInjector>
        class provider_impl : public iprovider {
        public:
            explicit provider_impl(const TInjector& injector) noexcept
                : injector_(injector)
            { }

            TExpected* get(const type_traits::heap&) const noexcept override {
                return injector_.create_impl(aux::type<TExpected*>{});
            }

            type get(const type_traits::stack&) const noexcept override {
                return injector_.create_impl(aux::type<type>{});
            }

        private:
            TInjector injector_;
        };

    public:
        template<class>
        using is_referable = std::false_type;

        template<class TInjector>
        explicit scope(const TInjector& injector) noexcept
            : provider_{std::make_shared<provider_impl<TInjector>>(injector)}
        { }

        template<class T, class TProvider>
        T try_create(const TProvider&);

        template<class T, class TProvider>
        auto create(const TProvider&) {
            return scope_.template create<T>(*provider_);
        }

    private:
        std::shared_ptr<iprovider> provider_;
        typename TScope::template scope<TExpected, TGiven> scope_;
    };
};

BOOST_DI_WKND(BOOST_DI_GCC)(
    template<class TScope>
    template<class TExpected, class TGiven>
    TExpected* exposed<TScope>::scope<TExpected, TGiven>::iprovider::get(const type_traits::heap&) const noexcept {
        return nullptr;
    }
)()

}}}} // boost::di::v1::scopes

#endif

