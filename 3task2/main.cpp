#include<TypeMap.h>
#include<string>
#include<iostream>


struct DataA { std::string value; };
struct DataB { int value; };

int main() {
    tl::TypeMap<int, DataA, double, DataB> myTypeMap;

    myTypeMap.AddValue<int>(42);
    myTypeMap.AddValue<double>(3.14);
    myTypeMap.AddValue<DataA>({"Hello, TypeMap!"});
    myTypeMap.AddValue<DataB>({10});

    std::cout << "Value for int: " << myTypeMap.GetValue<int>() << std::endl;
    std::cout << "Value for double: " << myTypeMap.GetValue<double>() << std::endl;
    std::cout << "Value for DataA: " << myTypeMap.GetValue<DataA>().value << std::endl;
    std::cout << "Value for DataB: " << myTypeMap.GetValue<DataB>().value << std::endl;

    std::cout << "Contains int? " << (myTypeMap.Contains<int>() ? "Yes" : "No") << std::endl;

    myTypeMap.RemoveValue<double>();
    std::cout << "Value for double after removal: " << myTypeMap.GetValue<double>() << std::endl;
    return 0;
}