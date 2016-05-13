#include <fc-controlproxy.h>

#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>

using namespace std;
using namespace FreeCopter;

int main() {
    using t1 = type_tuple<int, long, double, char, int*>;
    t1::get<1> t11;
    t1::get<2> t12;

    cout << typeid(t11).name() << endl;
    cout << typeid(t12).name() << endl;
    cout << typeid(t1::std_tuple_type).name() << endl;
    cout << typeid(t1::crstd_tuple_type).name() << endl;
    return 0;
}
