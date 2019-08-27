#ifndef SIGNAL_HPP
#define SIGNAL_HPP
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "delegates/delegate.h"

#include <algorithm>
#include <utility>
#include <set>


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

    template<class ValueType, class ResultType>
    class signal_base
    {
    public:
        typedef ValueType value_type;
        typedef ResultType result_type;

    public:

        inline
        signal_base() {}

        inline
        signal_base(const signal_base &other) :
			_functors(other._functors)
        { }

        inline
        ~signal_base() {}

        inline
        void swap(signal_base &other)
        {
            using std::swap;
            
            swap(_functors, other._functors);
        }

        inline
        signal_base& operator=(const signal_base &other)
        {
            _functors = other._functors;
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
        >::type connect(const FunctorT &functor)
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
        >::type connect(const Arg1T &arg1, const Arg2T &arg2)
        {
            return _connect(bind(arg1, arg2));
        }

        inline
        bool disconnect(value_type functor)
        {
            return _disconnect(functor);
        }

        template<class FunctorT>
        inline
        typename
        detail::type_traits::enable_if<
            detail::type_traits::
                is_bind_constructible<value_type, FunctorT>::value == (true),
            bool
        >::type disconnect(const FunctorT &functor)
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
        >::type disconnect(const Arg1T &arg1, const Arg2T &arg2)
        {
            return _disconnect(bind(arg1, arg2));
        }
        
        inline
        void clear() throw()
        {
            _functors.clear();
        }

        inline
        bool empty() const throw()
        {
            return _functors.empty();
        }

        inline 
        bool operator!() const throw()
        {
            return empty(); 
        }

        inline
        bool operator==(const signal_base &other) const throw()
        {
            return _functors == other._functors;
        }

        inline
        bool operator!=(const signal_base &other) const throw()
        {
            return _functors != other._functors;
        }

        inline
        bool operator>(const signal_base &other) const throw()
        {
            return _functors > other._functors;
        }

        inline
        bool operator<(const signal_base &other) const throw()
        {
            return _functors < other._functors;
        }
    private:
        typedef result_type (*StaticFunctionPtr)();
        typedef struct SafeBoolStruct {
            int a_data_pointer_to_this_is_0_on_buggy_compilers;
            StaticFunctionPtr m_nonzero;
        } UselessTypedef;
        typedef StaticFunctionPtr SafeBoolStruct::*unspecified_bool_type;
    
    public:
        inline 
        operator unspecified_bool_type() const  throw()
        {
            return empty()? 0: &SafeBoolStruct::m_nonzero;
        }
        // necessary to allow ==0 to work despite the safe_bool idiom
        inline 
        bool operator==(const SafeBoolStruct *) const throw()
        {
            return empty();
        }
        inline 
        bool operator!=(const SafeBoolStruct *) const throw()
        { 
            return !empty();
        }

    protected:
        std::set<value_type> _functors;
    
    private:

        inline
        connection _connect(const value_type &value)
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
        bool _disconnect(const value_type &value)
        {
            typename std::set<value_type>::size_type size = _functors.size();
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
        typedef fastdelegate::detail::DefaultVoid DefaultVoid;
    }

	template <
		class Param1T = detail::DefaultVoid,
		class Param2T = detail::DefaultVoid,
		class Param3T = detail::DefaultVoid,
		class Param4T = detail::DefaultVoid,
		class Param5T = detail::DefaultVoid,
		class Param6T = detail::DefaultVoid,
		class Param7T = detail::DefaultVoid,
		class Param8T = detail::DefaultVoid,
		class ParamUnusedT = detail::DefaultVoid
	>
	struct signal;

    template<>
	struct signal<detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid>:
        public signal_base<delegate<void>, void>
    {
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
    };

    template<class Param1T>
	struct signal<Param1T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid>:
        public signal_base<delegate<void, Param1T>, void>
    {
    private:
        typedef signal_base<delegate<void, Param1T>, void> base_type;
    public:
        typedef typename base_type::result_type result_type;
        typedef typename base_type::value_type value_type;

        inline
        result_type emit(Param1T p1) const
        {
            typedef typename std::set<value_type>::const_iterator const_iterator;

            for (const_iterator it = base_type::_functors.begin(); it != base_type::_functors.end(); )
            {
                (*(it++))(p1);
            }
        }
        
        inline
        result_type operator() (Param1T p1) const
        {
            return emit(p1);
        }
    };

    template<class Param1T, class Param2T>
	struct signal<Param1T, Param2T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid>:
        public signal_base<delegate<void, Param1T, Param2T>, void>
    {
    private:
        typedef signal_base<delegate<void, Param1T, Param2T>, void> base_type;
    public:
        typedef typename base_type::result_type result_type;
        typedef typename base_type::value_type value_type;

        inline
        result_type emit(Param1T p1, Param2T p2) const
        {
            typedef typename std::set<value_type>::const_iterator const_iterator;

            for (const_iterator it = base_type::_functors.begin(); it != base_type::_functors.end(); )
            {
                (*(it++))(p1, p2);
            }
        }
        
        inline
        result_type operator() (Param1T p1, Param2T p2) const
        {
            return emit(p1, p2);
        }
    };

    template<class Param1T, class Param2T, class Param3T>
	struct signal<Param1T, Param2T, Param3T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid>:
        public signal_base<delegate<void, Param1T, Param2T, Param3T>, void>
    {
    private:
        typedef signal_base<delegate<void, Param1T, Param2T, Param3T>, void> base_type;
    public:
        typedef typename base_type::result_type result_type;
        typedef typename base_type::value_type value_type;

        inline
        result_type emit(Param1T p1, Param2T p2, Param3T p3) const
        {
            typedef typename std::set<value_type>::const_iterator const_iterator;

            for (const_iterator it = base_type::_functors.begin(); it != base_type::_functors.end(); )
            {
                (*(it++))(p1, p2, p3);
            }
        }
        
        inline
        result_type operator() (Param1T p1, Param2T p2, Param3T p3) const
        {
            return emit(p1, p2, p3);
        }
    };

    template<class Param1T, class Param2T, class Param3T, class Param4T>
	struct signal<Param1T, Param2T, Param3T, Param4T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid>:
        public signal_base<delegate<void, Param1T, Param2T, Param3T, Param4T>, void>
    {
    private:
        typedef signal_base<delegate<void, Param1T, Param2T, Param3T, Param4T>, void> base_type;
    public:
        typedef typename base_type::result_type result_type;
        typedef typename base_type::value_type value_type;

        inline
        result_type emit(Param1T p1, Param2T p2, Param3T p3, Param4T p4) const
        {
            typedef typename std::set<value_type>::const_iterator const_iterator;

            for (const_iterator it = base_type::_functors.begin(); it != base_type::_functors.end(); )
            {
                (*(it++))(p1, p2, p3, p4);
            }
        }
        
        inline
        result_type operator() (Param1T p1, Param2T p2, Param3T p3, Param4T p4) const
        {
            return emit(p1, p2, p3, p4);
        }
    };

    template<class Param1T, class Param2T, class Param3T, class Param4T, class Param5T>
	struct signal<Param1T, Param2T, Param3T, Param4T, Param5T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid>:
        public signal_base<delegate<void, Param1T, Param2T, Param3T, Param4T, Param5T>, void>
    {
    private:
        typedef signal_base<delegate<void, Param1T, Param2T, Param3T, Param4T, Param5T>, void> base_type;
    public:
        typedef typename base_type::result_type result_type;
        typedef typename base_type::value_type value_type;

        inline
        result_type emit(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5) const
        {
            typedef typename std::set<value_type>::const_iterator const_iterator;

            for (const_iterator it = base_type::_functors.begin(); it != base_type::_functors.end(); )
            {
                (*(it++))(p1, p2, p3, p4, p5);
            }
        }
        
        inline
        result_type operator() (Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5) const
        {
            return emit(p1, p2, p3, p4, p5);
        }
    };

    template<class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T>
	struct signal<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid>:
        public signal_base<delegate<void, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T>, void>
    {
    private:
        typedef signal_base<delegate<void, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T>, void> base_type;
    public:
        typedef typename base_type::result_type result_type;
        typedef typename base_type::value_type value_type;

        inline
        result_type emit(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6) const
        {
            typedef typename std::set<value_type>::const_iterator const_iterator;

            for (const_iterator it = base_type::_functors.begin(); it != base_type::_functors.end(); )
            {
                (*(it++))(p1, p2, p3, p4, p5, p6);
            }
        }
        
        inline
        result_type operator() (Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6) const
        {
            return emit(p1, p2, p3, p4, p5, p6);
        }
    };

    template<class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T>
	struct signal<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, detail::DefaultVoid, detail::DefaultVoid>:
        public signal_base<delegate<void, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T>, void>
    {
    private:
        typedef signal_base<delegate<void, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T>, void> base_type;
    public:
        typedef typename base_type::result_type result_type;
        typedef typename base_type::value_type value_type;

        inline
        result_type emit(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6, Param7T p7) const
        {
            typedef typename std::set<value_type>::const_iterator const_iterator;

            for (const_iterator it = base_type::_functors.begin(); it != base_type::_functors.end(); )
            {
                (*(it++))(p1, p2, p3, p4, p5, p6, p7);
            }
        }
        
        inline
        result_type operator() (Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6, Param7T p7) const
        {
            return emit(p1, p2, p3, p4, p5, p6, p7);
        }
    };

    template<class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T, class Param8T>
	struct signal<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T, detail::DefaultVoid>:
        public signal_base<delegate<void, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T>, void>
    {
    private:
        typedef signal_base<delegate<void, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T>, void> base_type;
    public:
        typedef typename base_type::result_type result_type;
        typedef typename base_type::value_type value_type;

        inline
        result_type emit(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6, Param7T p7, Param8T p8) const
        {
            typedef typename std::set<value_type>::const_iterator const_iterator;

            for (const_iterator it = base_type::_functors.begin(); it != base_type::_functors.end(); )
            {
                (*(it++))(p1, p2, p3, p4, p5, p6, p7, p8);
            }
        }
        
        inline
        result_type operator() (Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6, Param7T p7, Param8T p8) const
        {
            return emit(p1, p2, p3, p4, p5, p6, p7, p8);
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
            yes_type operator,(const T&, no_type);
            //no_type operator,(no_type, no_type);

            template<class T> T declval();
            struct any_constructible { template<class T> any_constructible(T) { } };
            template<unsigned N> struct priority_tag : priority_tag < N - 1 > { };
            template<> struct priority_tag<0> { };
        }

        namespace type_traits
        {
			using signals::bind;
            void bind(type_traits::any_constructible);
            void bind(type_traits::any_constructible, type_traits::any_constructible);

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
            struct is_bind_constructible_impl
            {
                static const bool value =
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
            struct is_bind_constructible_impl<T, T1, void_type>
            {
                static const bool value =
                    sizeof(
                        is_constructible_tester<T>(
							bind(
                                declval<T1>()
                            ),
                            priority_tag<1>()
                        )
                    ) == sizeof(yes_type);
            };

            template<class T, class T1, class T2>
            struct has_bind
            {
                static const bool value =
                    sizeof(
						bind(
                            declval<T1>(),
                            declval<T2>()
                        ),
                        declval<no_type>()
                    ) != sizeof(no_type);
            };

            template<class T, class T1>
            struct has_bind<T, T1, void_type>
            {
                static const bool value =
                    sizeof(
						bind(
                            declval<T1>()
                        ),
                        declval<no_type>()
                    ) != sizeof(no_type);
            };

            template<class T, class T1, class T2>
            struct is_bind_constructible:
                conditional<
                    has_bind<T, T1, T2>::value == (true),
                    is_bind_constructible_impl<T, T1, T2>,
                    false_type
                >::type
            { };
        }
    }

} // namespace signals

#endif // SIGNAL_HPP
