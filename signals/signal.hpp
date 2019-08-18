#ifndef SIGNAL_HPP
#define SIGNAL_HPP
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "delegates/delegate.h"

namespace signals
{
    using namespace delegates;

    using delegates::bind;

    namespace detail
    {
        namespace type_traits
        {
            template<bool, class T = void>
            struct enable_if { typedef T type; };
            template<class T>
            struct enable_if<false, T> { static const int type = sizeof(T); };

            template<class T, class From>
            struct is_constructible;

            struct void_type {};

            template<class T, class T1, class T2 = void_type>
            struct is_bind_constructible;
        }
    }

    struct connection {};

    class signal
    {
    public:
        typedef delegate<void> functor;
        typedef void result_type;

    public:

        signal() {}
        ~signal() {}

        template<class FunctorT>
        inline
        typename
        detail::type_traits::enable_if<
            detail::type_traits::
                is_bind_constructible<functor, FunctorT>::value == (true) ||
            detail::type_traits::
                is_constructible<functor, FunctorT>::value == (true),
            connection
        >::type connect(FunctorT functor)
        {
            return connection();
        }

        template<class Arg1T, class Arg2T>
        inline
        typename
        detail::type_traits::enable_if<
            detail::type_traits::
                is_bind_constructible<functor, Arg1T, Arg2T>::value == (true),
            connection
        >::type connect(Arg1T arg1, Arg2T arg2)
        {
            return connection();
        }

    private:

    };


    namespace detail
    {
        namespace type_traits
        {
            struct no_type {};
            struct yes_type { char padding[sizeof(no_type) * 8]; };
            struct true_type { static const bool value = true; };
            struct false_type { static const bool value = false; };

            template<class T>
            yes_type operator,(T, no_type);
            no_type operator,(no_type, no_type);

            template<class T> T declval();
            struct any_constructible { template<class T> any_constructible(T) { } };
            template<unsigned N> struct priority_tag : priority_tag < N - 1 > { };
            template<> struct priority_tag<0> { };
        }

        namespace type_traits
        {
			using signals::bind;
            type_traits::no_type bind(type_traits::any_constructible);
            type_traits::no_type bind(type_traits::any_constructible, type_traits::any_constructible);

            template<class T>
            static yes_type is_constructible_tester(T, priority_tag<1>);
            template<class T>
            static no_type is_constructible_tester(any_constructible, priority_tag<0>);

            template<class T, class From>
            struct is_constructible
            {
                static const bool value =
                    sizeof(is_constructible_tester<T>(declval<From>(), priority_tag<1>())) == sizeof(yes_type);
            };
            template<class T, class FromT, int Size>
            struct is_constructible<T, FromT[Size]>
            {
                static const bool value =
                    sizeof(is_constructible_tester<T>(declval<FromT(&)[Size]>(), priority_tag<1>())) == sizeof(yes_type);
            };
            template<class From>
            struct is_constructible<void, From> : false_type { };
            template<class T>
            struct is_constructible<T, void> : false_type { };
            template<>
            struct is_constructible<void, void> : false_type { };

            template<bool, class IfTrue, class>
            struct conditional { typedef IfTrue type; };
            template<class IfTrue, class IfFalse>
            struct conditional<false, IfTrue, IfFalse> { typedef IfFalse type; };

            template<class T, class T1, class T2>
            struct is_bind_constructible
            {
                static const bool value =
                    sizeof(
						bind(
                            declval<T1>(),
                            declval<T2>()
                        ),
                        declval<no_type>()
                    ) != sizeof(no_type)
                    &&
                    sizeof(
                        is_constructible_tester<T>(
							bind(
                                declval<T1>(),
                                declval<T2>()
                            ),
                            priority_tag<1>()
                        )
                    ) == sizeof(yes_type);
            };

            template<class T, class T1>
            struct is_bind_constructible<T, T1, void_type>
            {
                static const bool value =
                    sizeof(
						bind(
                            declval<T1>()
                        ),
                        declval<no_type>()
                    ) != sizeof(no_type)
                    &&
                    sizeof(
                        is_constructible_tester<T>(
							bind(
                                declval<T1>()
                            ),
                            priority_tag<1>()
                        )
                    ) == sizeof(yes_type);
            };
        }
    }

} // namespace signals

#endif // SIGNAL_HPP