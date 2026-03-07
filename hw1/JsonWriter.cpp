#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <vector>

class JsonWriter {
public:
    explicit JsonWriter(const std::string& filename) 
        : file(filename)
    {
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        file << "{";
    }
    
    void Add(const std::string& key, const std::string& value) {
        if (!first) {
            file << ",";
        }
        
        file << "\"" << key << "\":\"" << value << "\"";
        first = false;
    }

    ~JsonWriter() {
        if (file.is_open())
        {
            file << "}";
        }

    }
    
    JsonWriter(const JsonWriter&) = delete;
    JsonWriter& operator=(const JsonWriter&) = delete;
    
    JsonWriter(JsonWriter&& other) noexcept 
        : file(std::move(other.file))
        , first(other.first)
    {
        other.first = true;
    }
    
    JsonWriter& operator=(JsonWriter&& other) noexcept {
        if (this != &other) {
            file = std::move(other.file);
            first = other.first;
            other.first = true;
        }
        return *this;
    }
    
private:
    std::ofstream file;
    bool first = true;
};

class Entity {
public:
    explicit Entity(const std::string& filename): writer(filename){}
    
    virtual ~Entity() = default;

    virtual void Add(const std::string& key, const std::string& value) {
        writer.Add(key, value);
    }
    
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    
    Entity(Entity&& other) noexcept = default;
    Entity& operator=(Entity&& other) noexcept = default;
    
protected:
    JsonWriter writer;
};


class Customer : public Entity {
public:
    Customer() : Entity("customer.json") {
        Add("type", "customer");
    }
    
    void SetName(const std::string& name) {
        Add("name", name);
    }
    
    void SetAge(const std::string& age) {
        Add("age", age);
    }
};

class Product : public Entity {
public:
    Product() : Entity("product.json") {
        Add("type", "product");
    }
    
    void SetPrice(const std::string& price) {
        Add("price", price);
    }
};

int main() {
        Customer customer1;
        customer1.SetName("Daniil");
        customer1.SetAge("19");
        
        Customer customer2;
        customer2.SetName("Yuriy");
        customer2.SetAge("34");
        
        Product product;
        product.SetPrice("100");
        
        std::vector<Customer> customers;
        customers.push_back(std::move(customer1));
        customers.push_back(std::move(customer2));
    
    return 0;
}