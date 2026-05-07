#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>


class JsonWriter {
private:
    std::ofstream file;
    bool first = true;
    bool moved_from = false;
    
public:
    explicit JsonWriter(const std::string& filename) : file(filename) {
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        file << "{";
    }
    
    ~JsonWriter() {
        if (!moved_from && file.is_open()) {
            file << "}";
        }
    }
    
    void Add(const std::string& key, const std::string& value) {
        if (!first) file << ",";
        file << "\"" << key << "\":\"" << value << "\"";
        first = false;
    }
    
    JsonWriter(JsonWriter&& other) noexcept
        : file(std::move(other.file))
        , first(other.first)
        , moved_from(false)
    {
        other.moved_from = true;
        other.first = true;
    }
    
    JsonWriter& operator=(JsonWriter&& other) noexcept {
        if (this != &other) {
            file = std::move(other.file);
            first = other.first;
            moved_from = false;
            
            other.moved_from = true;
            other.first = true;
        }
        return *this;
    }
    
    JsonWriter(const JsonWriter&) = delete;
    JsonWriter& operator=(const JsonWriter&) = delete;
};

class Customer {
private:
    JsonWriter writer;
    std::string name;
    int age = 0;
    
public:
    explicit Customer(const std::string& filename) : writer(filename) {
        writer.Add("type", "customer");
    }
    
    void SetName(const std::string& n) {
        name = n;
        writer.Add("name", n);
    }
    
    void SetAge(int a) {
        age = a;
        writer.Add("age", std::to_string(a));
    }
    
    Customer(Customer&& other) noexcept = default;
    Customer& operator=(Customer&& other) noexcept = default;

    Customer(const Customer&) = delete;
    Customer& operator=(const Customer&) = delete;
};

class Product {
private:
    JsonWriter writer;
    double price = 0.0;
    
public:
    explicit Product(const std::string& filename) : writer(filename) {
        writer.Add("type", "product");
    }
    
    void SetPrice(double p) {
        price = p;
        writer.Add("price", std::to_string(p));
    }
    
    Product(Product&& other) noexcept = default;
    Product& operator=(Product&& other) noexcept = default;
    
    Product(const Product&) = delete;
    Product& operator=(const Product&) = delete;
};

int main() {

        Customer alice("daniil.json");
        alice.SetName("Daniil");
        alice.SetAge(19);
        
        Customer bob("yuriy.json");
        bob.SetName("Yuriy");
        bob.SetAge(34);
        
        Product laptop("laptop.json");
        laptop.SetPrice(999.99);
        
        std::vector<Customer> customers;
        customers.reserve(2);
        customers.push_back(std::move(alice));
        customers.push_back(std::move(bob));
    
    return 0;
}