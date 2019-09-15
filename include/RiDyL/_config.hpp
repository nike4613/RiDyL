#ifndef RIDYL_CONFIG_HPP
#define RIDYL_CONFIG_HPP

#if defined(_MSC_VER)
#   define STRINGIZE_HELPER(x) #x
#   define STRINGIZE(x) STRINGIZE_HELPER(x)
#   define WARNING(...) message(__FILE__ "(" STRINGIZE(__LINE__) ") : Warning: " #__VA_ARGS__)
#endif

#if defined(_HAS_CONSTEVAL)
#   define RIDYL_CONSTEVAL consteval
#elif defined(_HAS_CONSTEXPR_BANG)
#   define RIDYL_CONSTEVAL constexpr!
#else
#   define RIDYL_CONSTEVAL constexpr
#   if defined(_MSC_VER)
#       pragma WARNING(Falling back to constexpr when consteval was requested, this may lead to different behavior)
#   else
#       warning Falling back to constexpr when consteval was requested, this may lead to different behavior
#   endif
#endif

#endif