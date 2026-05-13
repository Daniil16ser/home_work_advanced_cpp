#include <iostream>
#include <cassert>

template <typename T>
class less_than_comparable {
public:
    bool operator>(T const& other) const {
        return other < static_cast<T const&>(*this);
    }
    
    bool operator<=(T const& other) const {
        return !(other < static_cast<T const&>(*this));
    }
    
    bool operator>=(T const& other) const {
        return !(static_cast<T const&>(*this) < other);
    }
    
    bool operator==(T const& other) const {
        return !(static_cast<T const&>(*this) < other) && 
               !(other < static_cast<T const&>(*this));
    }
    
    bool operator!=(T const& other) const {
        return !(*this == other);
    }
};


template <typename T>
class counter {
private:
    static int m_count;
    
public:
    counter() { ++m_count; }
    
    counter(counter const&) { ++m_count; }
    
    counter(counter&&) noexcept { ++m_count; }
    
    virtual ~counter() { --m_count; }
    
    static int count() { return m_count; }
};

template <typename T>
int counter<T>::m_count = 0;

// example
class Number: public less_than_comparable<Number>, public counter<Number> {
public:
    Number(int value): m_value{value} {}
    int value() const { return m_value; }
    
    bool operator<(Number const& other) const {
        return m_value < other.m_value;
    }
    
private:
    int m_value;
};

int main() {
    Number one{1};
    Number two{2};
    Number three{3};
    Number four{4};
    
    assert(one >= one);
    assert(three <= four);
    assert(two == two);
    assert(three > two);
    assert(one < two);
    
    std::cout << "Count: " << counter<Number>::count() << std::endl;
    
    return 0;
}