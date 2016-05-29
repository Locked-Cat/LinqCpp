#pragma once

#include <boost\preprocessor.hpp>
#include <boost\preprocessor\facilities\is_empty.hpp>
#include <boost\range.hpp>
#include <boost\range\adaptor\filtered.hpp>
#include <boost\range\adaptor\transformed.hpp>
#include <type_traits>

#ifdef _MSC_VER
#define LINQ_IS_PAREN_CHECK_RES(x) x
#define LINQ_IS_PAREN_CHECK_II(x, n, ...) n
#define LINQ_IS_PAREN_CHECK_I(x) LINQ_IS_PAREN_CHECK_RES(LINQ_IS_PAREN_CHECK_II x)
#define LINQ_IS_PAREN_CHECK_N(...) LINQ_IS_PAREN_CHECK_I((__VA_ARGS__))
#else
#define LINQ_IS_PAREN_CHECK_N(x, n, ...) n
#endif
#define LINQ_IS_PAREN_CHECK(...) LINQ_IS_PAREN_CHECK_N(__VA_ARGS__, 0)
#define LINQ_IS_PAREN_PRED(...) ~, 1,
#define LINQ_IS_PAREN(x) LINQ_IS_PAREN_CHECK(LINQ_IS_PAREN_PRED x)

#define LINQ_IS_EMPTY_0(x) BOOST_PP_IS_EMPTY(x)
#define LINQ_IS_EMPTY_1(x) 0
#define LINQ_IS_EMPTY(x) BOOST_PP_CAT(LINQ_IS_EMPTY_, LINQ_IS_PAREN(x))(x)

#ifdef _MSC_VER
#define LINQ_PICK_HEAD_RES(x) x
#define LINQ_PICK_HEAD_III(x, ...) x
#define LINQ_PICK_HEAD_II(x) LINQ_PICK_HEAD_RES(LINQ_PICK_HEAD_III x)
#define LINQ_PICK_HEAD_I(...) LINQ_PICK_HEAD_II((__VA_ARGS__))
#else
#define LINQ_PICK_HEAD_I(x, ...) x
#endif
#define LINQ_PICK_HEAD(...) LINQ_PICK_HEAD_I(__VA_ARGS__,)
#define LINQ_MARK(...) (__VA_ARGS__),
#define LINQ_HEAD(x) LINQ_PICK_HEAD(LINQ_MARK x)

#define LINQ_EAT(...)
#define LINQ_TAIL(x) LINQ_EAT x

#define LINQ_KEYWORD(x) BOOST_PP_CAT(LINQ_KEYWORD_, x)
#define LINQ_IS_KEYWORD(x) LINQ_IS_PAREN(LINQ_KEYWORD(x))

#define LINQ_TO_SEQ_PAREN(prev, tail) (prev (LINQ_HEAD(tail)), LINQ_TAIL(tail))
#define LINQ_TO_SEQ_REPLACE(prev, tail) (prev LINQ_HEAD(tail), LINQ_TAIL(tail))
#define LINQ_TO_SEQ_KEYWORD(prev, tail) LINQ_TO_SEQ_REPLACE(prev, LINQ_KEYWORD(tail))
#define LINQ_TO_SEQ_O(prev, tail) BOOST_PP_IF(LINQ_IS_PAREN(tail),\
											  LINQ_TO_SEQ_PAREN,\
											  LINQ_TO_SEQ_KEYWORD)(prev, tail)
#define LINQ_TO_SEQ_WHILE_O(d, state) LINQ_TO_SEQ_O state
#define LINQ_TO_SEQ_P(prev, tail) BOOST_PP_NOT(LINQ_IS_EMPTY(tail))
#define LINQ_TO_SEQ_WHILE_P(d, state) LINQ_TO_SEQ_P state
#define LINQ_TO_SEQ_M(prev, tail) prev
#define LINQ_TO_SEQ_WHILE_M(state) LINQ_TO_SEQ_M  state 
#define LINQ_TO_SEQ(x) LINQ_TO_SEQ_WHILE_M(BOOST_PP_WHILE(LINQ_TO_SEQ_WHILE_P, LINQ_TO_SEQ_WHILE_O, (,x)))

#define LINQ_RETURN(...) -> decltype(__VA_ARGS__) { return (__VA_ARGS__); }
namespace linq
{
	namespace internal
	{
		template <typename Func>
		struct transformer
		{
			Func func;

			transformer(Func f)
				: func(f)
			{
			}

			template <typename F>
			struct result
			{
			};

			template <typename F, typename T>
			struct result<F(T)>
			{
				typedef decltype(std::declval<F>()(std::declval<T>())) type;
			};

			template <typename T>
			auto operator()(T && x) const LINQ_RETURN(func(std::forward<T>(x)))

			template <typename T>
			auto operator()(T && x) LINQ_RETURN(func(std::forward<T>(x)))
		};

		template <typename Func>
		transformer<Func> make_transformer(Func f)
		{
			return transformer<Func>(f);
		}

		struct where_t
		{
			template <typename Pred>
			auto operator+(Pred pred) LINQ_RETURN(boost::adaptors::filtered(pred))
		};

		struct select_t
		{
			template <typename Tran>
			auto operator+(Tran tran) LINQ_RETURN(boost::adaptors::transformed(make_transformer(tran)))
		};
	}

	internal::select_t select;
	internal::where_t where;
}

#define LINQ_LAMBDA_BLOCK(...) {return (__VA_ARGS__);}
#define LINQ_LAMBDA_HEADER(var, col) [&](decltype(*(boost::begin(col))) var)

#define LINQ_KEYWORD_from ()
#define LINQ_SELECT(var, col) linq::select + LINQ_LAMBDA_HEADER(var, col) LINQ_LAMBDA_BLOCK
#define LINQ_KEYWORD_select (LINQ_SELECT)
#define LINQ_WHERE(var, col) linq::where + LINQ_LAMBDA_HEADER(var, col) LINQ_LAMBDA_BLOCK
#define LINQ_KEYWORD_where (LINQ_WHERE)

#define LINQ_EXPAND(...) __VA_ARGS__

#define LINQ_SEQ_OUT_EACH(r, data, x) x
#define LINQ_SEQ_OUT(seq) BOOST_PP_SEQ_FOR_EACH(LINQ_SEQ_OUT_EACH, ~, seq)

#define LINQ_COL(var, col) col
#define LINQ_PROCESS_PAREN(data, x) x
#ifdef _MSC_VER
#define LINQ_PROCESS_KEYWORD_RES(x) x
#define LINQ_PROCESS_KEYWORD(data, x) | LINQ_PROCESS_KEYWORD_RES(x data)
#else
#define LINQ_PROCESS_KEYWORD(data, x) | x data
#endif
#define LINQ_SELECT_WHERE_O(s, data, x) BOOST_PP_IF(LINQ_IS_PAREN(x), LINQ_PROCESS_PAREN, LINQ_PROCESS_KEYWORD)(data, x)
#define LINQ_SELECT_WHERE(data, seq)  LINQ_COL data LINQ_SEQ_OUT(BOOST_PP_SEQ_TRANSFORM(LINQ_SELECT_WHERE_O, data, seq))
#define LINQ_TRANSFORM(seq) LINQ_EXPAND(LINQ_SELECT_WHERE(BOOST_PP_SEQ_ELEM(1, seq), BOOST_PP_SEQ_REST_N(2, seq))) 
#define LINQ(x) LINQ_TRANSFORM(LINQ_TO_SEQ(x))