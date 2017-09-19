#ifndef SIGNAL_HPP
#define SIGNAL_HPP
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "delegates\delegate.h"

#include <memory>
#include <vector>
#include <algorithm>

namespace signals
{
	using namespace delegates;

	// usefull 'duplex' pointer that makes a link between its own and its pair
	struct d_ptr
	{
	private:
		
		friend bool operator==(const d_ptr &, const d_ptr &);
		
		friend bool operator!=(const d_ptr &, const d_ptr &);
		
		friend bool operator==(const d_ptr &, std::nullptr_t);
		
		friend bool operator!=(const d_ptr &, std::nullptr_t);
		
		friend bool operator==(std::nullptr_t, const d_ptr &);
		
		friend bool operator!=(std::nullptr_t, const d_ptr &);

	public:
		d_ptr()
			: ptr()
		{
		}

		d_ptr(d_ptr *ptr)
			: ptr(ptr)
		{
			if (ptr)
				ptr->ptr = this;
		}

		/*d_ptr(d_ptr &&other)
			: d_ptr(other.ptr)
		{
			other.ptr = nullptr;
		}*/

		/*inline d_ptr & operator=(d_ptr &&other)
		{
			if (ptr)
			{
				if (other.ptr)
					std::swap(ptr->ptr, other.ptr->ptr);
				else
					ptr->ptr = &other;
			}
			else if (other.ptr)
				other.ptr->ptr = this;
			std::swap(ptr, other.ptr);
			return *this;
		}*/

		inline void move_from(d_ptr &other)
		{
			if (this == &other)
				return;

			if (ptr)
			{
				if (other.ptr)
					std::swap(ptr->ptr, other.ptr->ptr);
				else
					ptr->ptr = &other;
			}
			else if (other.ptr)
				other.ptr->ptr = this;
			std::swap(ptr, other.ptr);

			other.cut_connection();
		}

		inline d_ptr & operator=(d_ptr *ptr)
		{
			return *this = d_ptr(ptr);
		}

		~d_ptr()
		{
			cut_connection();
		}

		inline void cut_connection()
		{
			if (ptr)
			{
				ptr->ptr = nullptr;
				ptr = nullptr;
			}
		}

		inline explicit operator bool() const
		{
			return ptr != nullptr;
		}

		inline bool operator!() const
		{
			return !ptr;
		}

	private:
		d_ptr *ptr;

	public:
		d_ptr(d_ptr &other)// use move_from
		{
			move_from(other);
		}

		inline d_ptr &operator=(d_ptr &other)// use move_from
		{
			move_from(other);

			return *this;
		}

	};
	template<typename T, typename S>
	bool operator==(const d_ptr &lhs, const d_ptr &rhs)
	{
		return lhs.ptr == rhs.ptr;
	}
	template<typename T, typename S>
	bool operator!=(const d_ptr &lhs, const d_ptr &rhs)
	{
		return !(lhs == rhs);
	}
	template<typename T, typename S>
	bool operator==(const d_ptr &lhs, std::nullptr_t)
	{
		return !lhs;
	}
	template<typename T, typename S>
	bool operator!=(const d_ptr &lhs, std::nullptr_t)
	{
		return !(lhs == nullptr);
	}
	template<typename T, typename S>
	bool operator==(std::nullptr_t, const d_ptr &rhs)
	{
		return rhs == nullptr;
	}
	template<typename T, typename S>
	bool operator!=(std::nullptr_t, const d_ptr &rhs)
	{
		return !(nullptr == rhs);
	}

	class connection
	{
		template <
			class ReturnT,
			class Param1T,
			class Param2T,
			class Param3T,
			class Param4T,
			class Param5T,
			class Param6T,
			class Param7T,
			class Param8T,
			class ParamUnusedT
		>
		friend class signal;

		template<class _Ty>
		friend class allocator;

	public:
		connection()
		{

		}

		~connection()
		{
			disconnect();
		}

		inline void move_from(const connection &other)// other will be moved to this
		{
			if (this == &other)
				return;

			m_link.move_from(other.m_link);
		}

		inline void disconnect()
		{
			if (m_link)
				m_link.cut_connection();
		}

	private:
		mutable d_ptr m_link;

		connection(d_ptr &link):
			m_link(&link)
		{}

	public:
		// use move_from
		connection(const connection &other):
			m_link(other.m_link)
		{
		}

		inline connection &operator=(const connection &other)// use move_from
		{
			move_from(other);

			return *this;
		}
	};



	class slot :
		public delegate<>
	{
		typedef void ReturnT;

		template <
			class ReturnT,
			class Param1T,
			class Param2T,
			class Param3T,
			class Param4T,
			class Param5T,
			class Param6T,
			class Param7T,
			class Param8T,
			class ParamUnusedT
		>
		friend class signal;

	public:
		typedef delegate<> base_type;

		typedef slot type;

		slot() : base_type() { }

		template < class X, class Y >
		slot(Y * pthis,
			ReturnT(X::* function_to_bind)())
			: base_type(pthis, function_to_bind) { }

		template < class X, class Y >
		slot(const Y *pthis,
			ReturnT(X::* function_to_bind)() const)
			: base_type(pthis, function_to_bind)
		{  }

		template < class Y >
		slot(Y *pthis,
			ReturnT(*function_to_bind)(Y*))
			: base_type(this, &delegate::f_proxy<Y>)
		{  	}

		template < class Y >
		slot(Y *pthis,
			ReturnT(*function_to_bind)(const Y*))
			: base_type(this, &delegate::f_proxy_const<Y>)
		{  	}

		template < class Y >
		slot(const Y *pthis,
			ReturnT(*function_to_bind)(const Y*))
			: base_type(this, &delegate::f_proxy_const<Y>)
		{  	}


		slot(ReturnT(*function_to_bind)())
			: base_type(function_to_bind) { }

		inline void move_from(const slot &other)// other will be moved to this
		{
			if (this == &other)
				return;

			size_t other_connection_n = other.m_connection.size();

			m_connection.resize(other_connection_n);
			for (size_t i = 0; i < other_connection_n; ++i)
			{
				m_connection[i].move_from(other.m_connection[i]);
			}
			
			clear();
		}

		inline void clear()
		{
			m_connection.clear();
		}

		inline bool is_empty() const
		{
			return m_connection.size() == 0;
		}

	private:
		std::vector<connection> m_connection;

	public:
		// use move_from
		slot(const slot &other):
			m_connection(other.m_connection)
		{}

		inline slot &operator=(const slot &other)// use move_from
		{
			move_from(other);

			return *this;
		}
	};


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
	class signal;

	template <class ReturnT>
	class signal<ReturnT, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid, detail::DefaultVoid>
	{
	public:
		typedef slot slot_type;
		typedef delegate<ReturnT> delegate_type;

	private:
		typedef std::vector<slot_type> container_type;

	public:
		signal()
		{

		}

		~signal()
		{

		}

		ReturnT emit(void)
		{
			typedef typename container_type::iterator container_iterator;
			typedef typename container_type::value_type container_value_type;
			
			add();

			//remove from the back all empty elements
			for (container_iterator it = m_slot.rbegin(); it != m_slot.rend();)
			{
				if ((*it).is_empty())
					it = m_slot.erase(it);
				else
					break;
			}

			//run from the begining up to last element (back)
			for (container_iterator it = m_slot.begin(); it != (--(m_slot.end()));)
			{
				container_value_type &value = (*it);
				if (value.is_empty())
					it = m_slot.erase(it);
				else
				{
					value();
					++it;
				}
			}
			//return result of last element

			if (m_slot.size() != 0)
				return m_slot.back()();
			else
				return ReturnT();

		}

		void add()
		{
			std::move(m_slot_queue.begin(), m_slot_queue.end(), std::back_inserter(m_slot));
		}

	private:
		container_type m_slot;

		container_type m_slot_queue;
	};

}

#endif // SIGNAL_HPP