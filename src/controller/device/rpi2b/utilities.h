#ifndef FREECOPTER_UTILITIES_H
#define FREECOPTER_UTILITIES_H

#include <wcp-controller.h>

#ifdef __cplusplus
#include <string>
#endif

#ifdef __cplusplus
extern "C" {
#endif

std::string dump_status(FREECOPTER_WCP_STATUS_T* status);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class ConsoleCanvas {
public:
    class screen_size {
    public:
        size_t width_char;
        size_t height_char;
        size_t width_px;
        size_t height_px;
    };
    ConsoleCanvas();
    ~ConsoleCanvas();
    void refresh(const std::string&);
    void clear();
    static screen_size get_screen_size();
private:
    static bool unique_intances;
    bool _valid_instances;
    bool _is_cleared;
    screen_size _screen_size;

    void move_up();
    void move_to(size_t row = 1, size_t col = 1);
    void clear_line();
};
#endif

#endif
