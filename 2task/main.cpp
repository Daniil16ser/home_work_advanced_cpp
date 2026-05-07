#include "TypeList.h"
#include <type_traits>
#include <iostream>

using MyList = tl::TypeList<int, double, char>;

static_assert(MyList::size == 3, "Size should be 3");

static_assert(std::is_same_v<tl::Get<0, MyList>::type, int>);
static_assert(std::is_same_v<tl::Get<1, MyList>::type, double>);
static_assert(std::is_same_v<tl::Get<2, MyList>::type, char>);

static_assert(tl::contains_v<int, MyList>);
static_assert(tl::contains_v<double, MyList>);
static_assert(tl::contains_v<char, MyList>);
static_assert(!tl::contains_v<float, MyList>);

static_assert(tl::index_of_v<int, MyList> == 0);
static_assert(tl::index_of_v<double, MyList> == 1);
static_assert(tl::index_of_v<char, MyList> == 2);

using ListWithBack = tl::PushBack<MyList, float>::type;
static_assert(ListWithBack::size == 4);
static_assert(std::is_same_v<tl::Get<3, ListWithBack>::type, float>);

using ListWithFront = tl::PushFront<MyList, bool>::type;
static_assert(ListWithFront::size == 4);
static_assert(std::is_same_v<tl::Get<0, ListWithFront>::type, bool>);

using ExtendedList = tl::PushBack<
                        tl::PushFront<MyList, long long>::type,
                        unsigned short
                     >::type;
// should be: long long, int, double, char, unsigned short
static_assert(ExtendedList::size == 5);
static_assert(std::is_same_v<tl::Get<0, ExtendedList>::type, long long>);
static_assert(std::is_same_v<tl::Get<4, ExtendedList>::type, unsigned short>);
static_assert(tl::contains_v<long long, ExtendedList>);
static_assert(tl::index_of_v<unsigned short, ExtendedList> == 4);

int main() {
    std::cout << "All tests passed!" << std::endl;
    return 0;
}