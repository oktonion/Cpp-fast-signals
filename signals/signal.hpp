#ifndef SIGNAL_HPP
#define SIGNAL_HPP
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "delegates/delegate.h"

#include <algorithm>
#include <utility>

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
            struct enable_if<false, T> { typedef T type[sizeof(T)]; };

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
        typedef delegate<void> value_type;
        typedef void result_type;

    public:

        inline
        signal() {}

        inline
        signal(const signal &other) 
        {
            signal tmp = other;
            swap(tmp);
        }

        inline
        ~signal() {}

        inline
        void swap(signal &other)
        {
            using std::swap;
            
            swap(_functors, other._functors);
        }

        inline
        signal& operator=(const signal &other)
        {
            signal tmp = other;
            swap(tmp);
            return *this;
        }
        
        inline
        connection connect(value_type functor)
        {
            return _connect(functor);
        }

        template<class FunctorT>
        inline
        typename
        detail::type_traits::enable_if<
            detail::type_traits::
                is_bind_constructible<value_type, FunctorT>::value == (true),
            connection
        >::type connect(FunctorT functor)
        {
            return _connect(functor);
        }

        template<class Arg1T, class Arg2T>
        inline
        typename
        detail::type_traits::enable_if<
            detail::type_traits::
                is_bind_constructible<value_type, Arg1T, Arg2T>::value == (true),
            connection
        >::type connect(Arg1T arg1, Arg2T arg2)
        {
            return _connect(bind(arg1, arg2));
        }

        template<class FunctorT>
        inline
        typename
        detail::type_traits::enable_if<
            detail::type_traits::
                is_bind_constructible<value_type, FunctorT>::value == (true) ||
            detail::type_traits::
                is_constructible<value_type, FunctorT>::value == (true),
            bool
        >::type disconnect(FunctorT functor)
        {
            return _disconnect(functor);
        }

        template<class Arg1T, class Arg2T>
        inline
        typename
        detail::type_traits::enable_if<
            detail::type_traits::
                is_bind_constructible<value_type, Arg1T, Arg2T>::value == (true),
            bool
        >::type disconnect(Arg1T arg1, Arg2T arg2)
        {
            return _disconnect(bind(arg1, arg2));
        }
        
        inline
        void clear() throw()
        {
            _functors.clear();
        }

        inline
        result_type emit() const
        {
            typedef std::set<value_type>::const_iterator const_iterator;

            for (const_iterator it = _functors.begin(); it != _functors.end(); )
            {
                (*(it++))();
            }
        }
        
        inline
        result_type operator() () const
        {
            return emit();
        }

        inline
        bool empty() const
        {
            return _functors.empty();
        }

    private:
        std::set<value_type> _functors;

        inline
        connection _connect(value_type value)
        {
            _functors.insert(value);
            return connection();
        }

        template<class T>
        inline
        connection _connect(const T &value)
        {
            return _connect(bind(value));
        }

        inline
        bool _disconnect(value_type value)
        {
            std::set<value_type>::size_type size = _functors.size();
            _functors.erase(value);
            return (size != _functors.size());
        }

        template<class T>
        inline
        bool _disconnect(const T &value)
        {
            return _disconnect(bind(value));
        }
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