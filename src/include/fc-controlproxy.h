#ifndef FREECOPTER_CONTROL_PROXY_H
#define FREECOPTER_CONTROL_PROXY_H

#include <stdint.h>
#include <ratio>
#include <stdint.h>
#include <tuple>
#include <type_traits>

#include <wcp.h>
#include <zthread.h>
#include <fc-exception.h>

namespace FreeCopter {


    template <typename... Types>
    class type_tuple {
    public:
        using std_tuple_type = std::tuple<Types...>;
        using cstd_tuple_type = std::tuple<typename std::add_const<Types>::type...>;
        using rstd_tuple_type = std::tuple<typename std::add_rvalue_reference<Types>::type...>;
        using crstd_tuple_type = std::tuple<typename std::add_const<typename std::add_lvalue_reference<Types>::type>::type...>;

        template <size_t I>
        using get = typename std::tuple_element<I, std_tuple_type>::type;
        static constexpr size_t size = sizeof...(Types);
    };

    template <typename coordinate_value_types = type_tuple<double, double, double>, typename original_value_types = type_tuple<uint32_t, uint32_t, uint32_t, uint32_t>, int version = 0>
    class CoordinateSystem {
    public:
        template <size_t axis, typename... Args>
        static typename coordinate_value_types::template get<axis> get_axis_value(typename std::add_const<typename std::add_lvalue_reference<Args>::type>::type... original_values);
        template <size_t axis, typename... Args>
        static typename original_value_types::template get<axis> get_original_value(typename std::add_const<typename std::add_lvalue_reference<Args>::type>::type... coordinate_values);
    };

    template <typename CS = CoordinateSystem<>>
    class ControlProxy {
    public:
        using channel_name_t = uint32_t;
        using channel_value_t = uint32_t;

        void set_attitude(attitude atti);
        ControlProxy();
        virtual ~ControlProxy() noexcept;



    private:
        bool _is_open;
        void open(unsigned long port);
        void set_channel(uint32_t name, uint32_t value);
        uint32_t get_channel(uint32_t name);


    };
}

#endif
