#ifndef FREECOPTER_CONTROL_PROXY_H
#define FREECOPTER_CONTROL_PROXY_H


#include <wcp.h>
#include <stdint.h>
#include <ratio>
#include <stdint.h>
#include <tuple>

#include <zthread.h>

namespace FreeCopter {


    template <typename... Types>
    class type_tuple {
    public:
        typedef std_tuple_type std::tuple<Types...>;

        template <size_t I>
        typedef get_type<I> decltype(std::tuple_element<I, std_tuple_type>::type);
    };

    template <int I>
    class CoordinateSystem {
    };

    template <typename CS = CoordinateSystem>
    class ControlProxy {
    public:
        typedef channel_name_t  uint32_t;
        typedef channel_value_t uint32_t;

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
