#include "utilities.h"
#include <sstream>
#include <iomanip>
#include <sys/ioctl.h>
#include <iostream>
#include <unistd.h>
#include <stdexcept>

bool ConsoleCanvas::unique_intances = false;

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::flush;
using std::setw;
using std::runtime_error;
using std::left;
using std::right;

ConsoleCanvas::ConsoleCanvas() :
_valid_instances(false), _is_cleared(false) {
    if (unique_intances) {
        throw runtime_error("already have another instance");
        return;
    } else {
        unique_intances = true;
        _valid_instances = true;
        _screen_size = get_screen_size();
        clear();
    }
}
ConsoleCanvas::~ConsoleCanvas() {
    if(_valid_instances) {
        unique_intances = false;
    } else {
        return;
    }
}
void ConsoleCanvas::refresh(const std::string& str) {
    if (_is_cleared) {
        cout << str;
    } else {
        clear();
        cout << str;
    }
    _is_cleared = false;
}
void ConsoleCanvas::clear() {
    _screen_size = get_screen_size();
    if (_is_cleared) {
        return;
    } else {
        for (int i = 1; i <= _screen_size.height_char; ++i) {
            move_to(i, 1);
            clear_line();
        }
        cout << flush;
        move_to(1, 1);
        _is_cleared = true;
    }
}
ConsoleCanvas::screen_size ConsoleCanvas::get_screen_size() {
    struct winsize w;
    screen_size size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    size.width_char = w.ws_col;
    size.height_char = w.ws_row;
    size.width_px = w.ws_xpixel;
    size.height_px = w.ws_ypixel;
    return size;
}
void ConsoleCanvas::move_up() {
    printf("\033[F");
}
void ConsoleCanvas::move_to(size_t row, size_t col) {
    printf("\033[%d;%dH", row, col);
}
void ConsoleCanvas::clear_line() {
    printf("\033[J");
}

string dump_status(FREECOPTER_WCP_STATUS_T* status) {
    std::ostringstream iss;
    auto screen_width = ConsoleCanvas::get_screen_size().width_char;
    string strtemp = "channel: 1234123451234[]";
    size_t bar_size = screen_width - strtemp.size();

    iss << "dump status " << status->channel_n << "\n";
    for (int i = 0; i < status->channel_n; ++i) {
        iss << "channel: " << setw(4) << left << status->channel[i].name;
        iss << "     " << setw(4) << right <<status->channel[i].value;

        string bar(static_cast<double>(status->channel[i].value) / 1000 * bar_size, '=');

        iss << "[" << setw(bar_size) << std::left << bar << "]\n";
    }
    return iss.str();
}
