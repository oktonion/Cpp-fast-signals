
#include "FastDelegate.h"

#ifndef DELEGATE_H
#define DELEGATE_H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//wrapper for Fast Delegates of Don Clugston
//adds some jucy functionality
//now you could bind to some free functions that accept pointer to 'this' as first parameter (like member function but nah)

/******************************************************************************************************************************/
/* So you could now bind f.e. 'delegates::delegate<return_type, param_type1, param_type2>' to:                                */
/*                                                                                                                            */
/*   1) { return_type function(param_type1 param1, param_type2 param2); }                                                     */
/*   2) { this_type pthis, return_type function(this_type param1, param_type1 param2, param_type2 param3); }                  */
/*   3) { this_type pthis, return_type this_type::member_function(param_type1 param1, param_type2 param2); }                  */
/*   4) { this_type pthis, return_type function(const this_type param1, param_type1 param2, param_type2 param3); }            */
/*   5) { const this_type pthis, return_type this_type::member_function const (param_type1 param1, param_type2 param2); }     */
/*                                                                                                                            */
/******************************************************************************************************************************/

namespace delegates
{
	namespace detail
	{
		typedef fastdelegate::detail::DefaultVoid DefaultVoid;
	}

	template <
		class ReturnT = detail::DefaultVoid,
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
	class delegate;

	template<class ReturnT>
	class delegate<ReturnT, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate0<ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void* );
		typedef ReturnT(*free_function_like_member_const_t)(const void* );

	public:
		typedef fastdelegate::FastDelegate0< ReturnT  > base_type;

		typedef delegate type;

		delegate() : base_type() { }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)( ))
			: base_type(pthis, function_to_bind) { }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)( ) const)
			: base_type(pthis, function_to_bind)
		{  }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y* ))
			: base_type(this, &delegate::f_proxy<Y>),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y* ))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}


		delegate(ReturnT(*function_to_bind)( ))
			: base_type(function_to_bind) { }

		void operator = (const base_type &x) {
			*static_cast<base_type*>(this) = x;
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y* )) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, &delegate::f_proxy<Y>);
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y* )) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		using base_type::bind;

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy() 
		{ 
			typedef ReturnT(*type_free_function_like_member_t)(Y* );

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis));
		}

		template< class Y >
		ReturnT f_proxy_const() 
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y* );

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const));
		}
	};

	template<class ReturnT, class Param1T>
	class delegate<ReturnT, Param1T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate1<Param1T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T);

	public:
		typedef fastdelegate::FastDelegate1<Param1T, ReturnT> base_type;

		typedef delegate type;

		delegate() : base_type() { }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T))
			: base_type(pthis, function_to_bind) { }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T) const)
			: base_type(pthis, function_to_bind)
		{  }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T))
			: base_type(this, &delegate::f_proxy<Y>),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}


		delegate(ReturnT(*function_to_bind)(Param1T))
			: base_type(function_to_bind) { }

		void operator = (const base_type &x) {
			*static_cast<base_type*>(this) = x;
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, &delegate::f_proxy<Y>);
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		using base_type::bind;

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1)
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1)
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1);
		}
	};

	template<class ReturnT, class Param1T, class Param2T>
	class delegate<ReturnT, Param1T, Param2T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate2<Param1T, Param2T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T);

	public:
		typedef fastdelegate::FastDelegate2<Param1T, Param2T, ReturnT> base_type;

		typedef delegate type;

		delegate() : base_type() { }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T))
			: base_type(pthis, function_to_bind) { }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T) const)
			: base_type(pthis, function_to_bind)
		{  }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T))
			: base_type(this, &delegate::f_proxy<Y>),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T))
			: base_type(function_to_bind) { }

		void operator = (const base_type &x) {
			*static_cast<base_type*>(this) = x;
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, &delegate::f_proxy<Y>);
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		using base_type::bind;

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1, Param2T p2)
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T, Param2T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2)
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T, Param2T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2);
		}
	};

	template<class ReturnT, class Param1T, class Param2T, class Param3T>
	class delegate<ReturnT, Param1T, Param2T, Param3T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate3<Param1T, Param2T, Param3T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T, Param3T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T, Param3T);

	public:
		typedef fastdelegate::FastDelegate3<Param1T, Param2T, Param3T, ReturnT> base_type;

		typedef delegate type;

		delegate() : base_type() { }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T))
			: base_type(pthis, function_to_bind) { }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T) const)
			: base_type(pthis, function_to_bind)
		{  }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T))
			: base_type(this, &delegate::f_proxy<Y>),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T))
			: base_type(function_to_bind) { }

		void operator = (const base_type &x) {
			*static_cast<base_type*>(this) = x;
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, &delegate::f_proxy<Y>);
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		using base_type::bind;

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1, Param2T p2, Param3T p3)
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T, Param2T, Param3T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2, p3);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2, Param3T p3)
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T, Param2T, Param3T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2, p3);
		}
	};

	template<class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T>
	class delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate4<Param1T, Param2T, Param3T, Param4T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T, Param3T, Param4T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T, Param3T, Param4T);

	public:
		typedef fastdelegate::FastDelegate4<Param1T, Param2T, Param3T, Param4T, ReturnT> base_type;

		typedef delegate type;

		delegate() : base_type() { }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T))
			: base_type(pthis, function_to_bind) { }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T) const)
			: base_type(pthis, function_to_bind)
		{  }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T))
			: base_type(this, &delegate::f_proxy<Y>),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T))
			: base_type(function_to_bind) { }

		void operator = (const base_type &x) {
			*static_cast<base_type*>(this) = x;
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, &delegate::f_proxy<Y>);
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		using base_type::bind;

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1, Param2T p2, Param3T p3, Param4T p4)
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T, Param2T, Param3T, Param4T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2, p3, p4);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2, Param3T p3, Param4T p4)
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T, Param2T, Param3T, Param4T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2, p3, p4);
		}
	};

	template<class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T>
	class delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate5<Param1T, Param2T, Param3T, Param4T, Param5T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T, Param3T, Param4T, Param5T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T, Param3T, Param4T, Param5T);

	public:
		typedef fastdelegate::FastDelegate5<Param1T, Param2T, Param3T, Param4T, Param5T, ReturnT> base_type;

		typedef delegate type;

		delegate() : base_type() { }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T))
			: base_type(pthis, function_to_bind) { }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T) const)
			: base_type(pthis, function_to_bind)
		{  }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T))
			: base_type(this, &delegate::f_proxy<Y>),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T))
			: base_type(function_to_bind) { }

		void operator = (const base_type &x) {
			*static_cast<base_type*>(this) = x;
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, &delegate::f_proxy<Y>);
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		using base_type::bind;

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5)
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2, p3, p4, p5);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5)
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2, p3, p4, p5);
		}
	};

	template<class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T>
	class delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate6<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T);

	public:
		typedef fastdelegate::FastDelegate6<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, ReturnT> base_type;

		typedef delegate type;

		delegate() : base_type() { }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T))
			: base_type(pthis, function_to_bind) { }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T) const)
			: base_type(pthis, function_to_bind)
		{  }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T))
			: base_type(this, &delegate::f_proxy<Y>),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T))
			: base_type(function_to_bind) { }

		void operator = (const base_type &x) {
			*static_cast<base_type*>(this) = x;
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, &delegate::f_proxy<Y>);
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		using base_type::bind;

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6)
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2, p3, p4, p5, p6);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6)
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2, p3, p4, p5, p6);
		}
	};

	template<class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T>
	class delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, detail::DefaultVoid, detail::DefaultVoid> :
		public fastdelegate::FastDelegate7<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T);

	public:
		typedef fastdelegate::FastDelegate7<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, ReturnT> base_type;

		typedef delegate type;

		delegate() : base_type() { }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T))
			: base_type(pthis, function_to_bind) { }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T) const)
			: base_type(pthis, function_to_bind)
		{  }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T))
			: base_type(this, &delegate::f_proxy<Y>),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T))
			: base_type(function_to_bind) { }

		void operator = (const base_type &x) {
			*static_cast<base_type*>(this) = x;
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, &delegate::f_proxy<Y>);
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		using base_type::bind;

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6, Param7T p7)
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2, p3, p4, p5, p6, p7);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6, Param7T p7)
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2, p3, p4, p5, p6, p7);
		}
	};

	template<class ReturnT, class Param1T, class Param2T, class Param3T, class Param4T, class Param5T, class Param6T, class Param7T, class Param8T>
	class delegate<ReturnT, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T, detail::DefaultVoid> :
		public fastdelegate::FastDelegate8<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T, ReturnT>
	{
		typedef ReturnT(*free_function_like_member_t)(void*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T);
		typedef ReturnT(*free_function_like_member_const_t)(const void*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T);

	public:
		typedef fastdelegate::FastDelegate8<Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T, ReturnT> base_type;

		typedef delegate type;

		delegate() : base_type() { }

		template < class X, class Y >
		delegate(Y * pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T))
			: base_type(pthis, function_to_bind) { }

		template < class X, class Y >
		delegate(const Y *pthis,
			ReturnT(X::* function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T) const)
			: base_type(pthis, function_to_bind)
		{  }

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T))
			: base_type(this, &delegate::f_proxy<Y>),
			m_pthis(static_cast<void*>(pthis)),
			m_free_func(reinterpret_cast<free_function_like_member_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}

		template < class Y >
		delegate(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T))
			: base_type(this, &delegate::f_proxy_const<Y>),
			m_pthis_const(static_cast<const void*>(pthis)),
			m_free_func_const(reinterpret_cast<free_function_like_member_const_t>(function_to_bind))
		{  	}


		delegate(ReturnT(*function_to_bind)(Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T))
			: base_type(function_to_bind) { }

		void operator = (const base_type &x) {
			*static_cast<base_type*>(this) = x;
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T)) {
			this->clear();
			m_pthis = static_cast<void*>(pthis);
			m_free_func = reinterpret_cast<free_function_like_member_t>(function_to_bind);
			bind(this, &delegate::f_proxy<Y>);
		}

		template < class Y >
		inline void bind(Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		template < class Y >
		inline void bind(const Y *pthis, ReturnT(*function_to_bind)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T)) {
			this->clear();
			m_pthis_const = static_cast<const void*>(pthis);
			m_free_func_const = reinterpret_cast<free_function_like_member_const_t>(function_to_bind);
			bind(this, &delegate::f_proxy_const<Y>);
		}

		using base_type::bind;

	private:
		union
		{
			const void *m_pthis_const;
			void *m_pthis;
		};

		union
		{
			free_function_like_member_t m_free_func;
			free_function_like_member_const_t m_free_func_const;
		};

		template< class Y >
		ReturnT f_proxy(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6, Param7T p7, Param8T p8)
		{
			typedef ReturnT(*type_free_function_like_member_t)(Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func)(static_cast<Y*>(m_pthis), p1, p2, p3, p4, p5, p6, p7, p8);
		}

		template< class Y >
		ReturnT f_proxy_const(Param1T p1, Param2T p2, Param3T p3, Param4T p4, Param5T p5, Param6T p6, Param7T p7, Param8T p8)
		{
			typedef ReturnT(*type_free_function_like_member_t)(const Y*, Param1T, Param2T, Param3T, Param4T, Param5T, Param6T, Param7T, Param8T);

			return reinterpret_cast<type_free_function_like_member_t>(m_free_func_const)(static_cast<const Y*>(m_pthis_const), p1, p2, p3, p4, p5, p6, p7, p8);
		}
	};
}


#endif