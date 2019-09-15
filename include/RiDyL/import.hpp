#ifndef RIDYL_IMPORT_HPP
#define RIDYL_IMPORT_HPP

#include <utility>
#include <type_traits>
#include <string_view>
#include <array>

#include "RiDyL/_config.hpp"

namespace RiDyL::import {

    template<typename> struct dynamic_binding;

    namespace detail {
        struct dynamic_binding_base {
            void* pointer;
            std::string_view name_;
            std::array<char const,8> const type;

            constexpr dynamic_binding_base(std::string_view name, std::array<char const,8> type)
                : name_{name}, type{type}, pointer{nullptr} {}

            template<typename T>
            auto as() const noexcept
            { return reinterpret_cast<T*>(pointer); }
            auto name() const noexcept
            { return name_; }
            operator bool() const noexcept
            { return pointer != nullptr; }
        };

        template<typename P>
        RIDYL_CONSTEVAL auto gen_binding(std::string_view name) noexcept 
        { return dynamic_binding<P>{name}; }

        // linked by dynamic linker
        extern void construct(dynamic_binding_base*) noexcept;

        template<typename P>
        void construct_binding(dynamic_binding<P>& b) noexcept {
            construct(static_cast<dynamic_binding_base*>(&b));
        }
    }

    template<typename R, typename ...Args>
    struct dynamic_binding<R(Args...)> : private detail::dynamic_binding_base {
        using detail::dynamic_binding_base::name;
        using detail::dynamic_binding_base::operator bool;

        template<typename ...U>
        auto operator()(U&&... args) const ->
            std::enable_if_t<
                sizeof...(Args) == sizeof...(U) && (std::is_convertible_v<U,Args> && ...),
                R
            >
        { return ptr()(std::forward<U>(args)...); }

    private:
        constexpr explicit dynamic_binding(std::string_view name) : 
            detail::dynamic_binding_base{name, {"functor"}} {}

        friend RIDYL_CONSTEVAL auto detail::gen_binding<R(Args...)>(std::string_view) noexcept;
        friend RIDYL_CONSTEVAL void detail::construct_binding<R(Args...)>(dynamic_binding<R(Args...)>&) noexcept;

        auto ptr() const noexcept
        { return this->as<R(Args...)>(); }
    };

    template<typename T>
    struct dynamic_binding : private detail::dynamic_binding_base {
        using detail::dynamic_binding_base::name;
        using detail::dynamic_binding_base::operator bool;

        T& operator*() const noexcept
        { return *ptr(); }
        T* operator->() const noexcept
        { return ptr(); }

    private:
        constexpr explicit dynamic_binding(std::string_view name) : 
            detail::dynamic_binding_base{name, {"functor"}} {}

        friend RIDYL_CONSTEVAL auto detail::gen_binding<T>(std::string_view) noexcept;
        friend RIDYL_CONSTEVAL void detail::construct_binding<T>(dynamic_binding<T>&) noexcept;

        auto ptr() const noexcept
        { return this->as<T>(); }
    };

    template<typename P>
    auto csimport(std::string_view name) noexcept {
        auto bind = detail::gen_binding<P>(name);
        detail::construct_binding(bind);
        return bind;
    }

}

#endif