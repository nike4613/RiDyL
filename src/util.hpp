#ifndef RIDYL_UTIL_H
#define RIDYL_UTIL_H

#include <utility>
#include <type_traits>

namespace RiDyL::util {

    template<typename...>
    struct type_list {
        using identity = type_list<>;
        using reverse = type_list<>;
        using rest = type_list<>;
        template<typename U>
        using append = type_list<U>;

        template<template<typename...>typename A>
        using apply = A<>;

        static constexpr auto size = 0;
    };
    template<typename First, typename ...Rest>
    struct type_list<First, Rest...> {
        using identity = type_list<First, Rest...>;

        using first = First;
        using rest = type_list<Rest...>;

        using reverse = typename rest::reverse::template append<First>;

        using except_last = typename reverse::rest::reverse;
        using last = typename reverse::first;

        template<typename T>
        using append = type_list<First, Rest..., T>;

        template<template<typename...>typename A>
        using apply = A<First, Rest...>;

        static constexpr auto size = sizeof...(Rest) + 1;
    };
    template<typename T>
    struct type_list<T> {
        using identity = type_list<T>;

        using first = T;
        using rest = type_list<>;

        using reverse = identity;

        using except_last = rest;
        using last = first;

        template<typename U>
        using append = type_list<T, U>;

        template<template<typename...>typename A>
        using apply = A<T>;

        static constexpr auto size = 1;
    };

    namespace detail {
        template<typename> struct func_ptr_helper;
        template<typename R, typename ...Args>
        struct func_ptr_helper<R(*)(Args...)> {
            using ret = R;
            using args = type_list<Args...>;
        };

        template<typename... Args>
        struct callback_wrap_helper {
            template<typename F, typename R>
            static auto get_functor() noexcept {
                return [](Args... args, void* ptr) -> R {
                    return (*reinterpret_cast<F*>(ptr))(std::forward<Args>(args)...);
                };
            }
        };
    }

    template<typename T, typename...>
    using alias_t = T;

    template<typename F, typename Ret, typename Arg>
    auto callback_wrap(Ret(&call)(Arg, void*), F&& functor) ->
        alias_t<
            std::enable_if_t<
                std::is_same_v<
                    typename detail::func_ptr_helper<Arg>::args::last,
                    void*
                >,
                Ret
            >,
            typename detail::func_ptr_helper<Arg>::ret,
            typename detail::func_ptr_helper<Arg>::args
        >
    {
        return call(
            detail::func_ptr_helper<Arg>::args::except_last
            ::template apply<detail::callback_wrap_helper>::template get_functor<F, Ret>(), &functor);
    }

}

#endif