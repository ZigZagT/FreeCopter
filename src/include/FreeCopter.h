#include <wcp.h>
#include <stdint.h>
#include <exception>
#include <ratio>

namespace FreeCopter {

    class FreeCopterException : std::exception {

    }

    class ControlProxy {
    public:
        ControlProxy();
        ~ControlProxy();
        open(unsigned long port);

        void set_channel(uint32_t name, uint32_t value)


    private:
        bool _is_open;

    }
}
