#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <stdexcept>

// (Компоновщик)
class Expression {
public:
    virtual ~Expression() = default;
    virtual double evaluate(const std::map<std::string, double>& context) const = 0;
    virtual void print(std::ostream& os = std::cout, int depth = 0) const = 0;
    virtual size_t getHash() const = 0;
    virtual bool isEqual(const Expression& other) const = 0;
    
    friend std::ostream& operator<<(std::ostream& os, const Expression& expr) {
        expr.print(os);
        return os;
    }
};

// Приспособленец для констант
class Constant : public Expression {
private:
    double value_;
    
    // Приватный конструктор, доступ только через фабрику
    friend class ExpressionFactory;
    explicit Constant(double value) : value_(value) {}
    
public:
    double getValue() const { return value_; }
    
    double evaluate(const std::map<std::string, double>&) const override {
        return value_;
    }
    
    void print(std::ostream& os = std::cout, int = 0) const override {
        os << value_;
    }
    
    size_t getHash() const override {
        return std::hash<double>{}(value_);
    }
    
    bool isEqual(const Expression& other) const override {
        const Constant* c = dynamic_cast<const Constant*>(&other);
        return c && c->value_ == value_;
    }
};

// Приспособленец для переменных
class Variable : public Expression {
private:
    std::string name_;
    
    friend class ExpressionFactory;
    explicit Variable(const std::string& name) : name_(name) {}
    
public:
    const std::string& getName() const { return name_; }
    
    double evaluate(const std::map<std::string, double>& context) const override {
        auto it = context.find(name_);
        if (it != context.end()) {
            return it->second;
        }
        throw std::runtime_error("Variable '" + name_ + "' not found in context");
    }
    
    void print(std::ostream& os = std::cout, int = 0) const override {
        os << name_;
    }
    
    size_t getHash() const override {
        return std::hash<std::string>{}(name_);
    }
    
    bool isEqual(const Expression& other) const override {
        const Variable* v = dynamic_cast<const Variable*>(&other);
        return v && v->name_ == name_;
    }
};

class Addition : public Expression {
private:
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
    
public:
    Addition(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : left_(std::move(left)), right_(std::move(right)) {}
    
    double evaluate(const std::map<std::string, double>& context) const override {
        return left_->evaluate(context) + right_->evaluate(context);
    }
    
    void print(std::ostream& os = std::cout, int depth = 0) const override {
        os << "(";
        left_->print(os, depth + 1);
        os << " + ";
        right_->print(os, depth + 1);
        os << ")";
    }
    
    size_t getHash() const override {
        return left_->getHash() ^ (right_->getHash() << 1);
    }
    
    bool isEqual(const Expression& other) const override {
        const Addition* add = dynamic_cast<const Addition*>(&other);
        return add && left_->isEqual(*add->left_) && right_->isEqual(*add->right_);
    }
};

class Subtraction : public Expression {
private:
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
    
public:
    Subtraction(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : left_(std::move(left)), right_(std::move(right)) {}
    
    double evaluate(const std::map<std::string, double>& context) const override {
        return left_->evaluate(context) - right_->evaluate(context);
    }
    
    void print(std::ostream& os = std::cout, int depth = 0) const override {
        os << "(";
        left_->print(os, depth + 1);
        os << " - ";
        right_->print(os, depth + 1);
        os << ")";
    }
    
    size_t getHash() const override {
        return left_->getHash() ^ (right_->getHash() << 1) ^ 0x12345678;
    }
    
    bool isEqual(const Expression& other) const override {
        const Subtraction* sub = dynamic_cast<const Subtraction*>(&other);
        return sub && left_->isEqual(*sub->left_) && right_->isEqual(*sub->right_);
    }
};

class Multiplication : public Expression {
private:
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
    
public:
    Multiplication(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : left_(std::move(left)), right_(std::move(right)) {}
    
    double evaluate(const std::map<std::string, double>& context) const override {
        return left_->evaluate(context) * right_->evaluate(context);
    }
    
    void print(std::ostream& os = std::cout, int depth = 0) const override {
        os << "(";
        left_->print(os, depth + 1);
        os << " * ";
        right_->print(os, depth + 1);
        os << ")";
    }
    
    size_t getHash() const override {
        return left_->getHash() ^ (right_->getHash() << 2);
    }
    
    bool isEqual(const Expression& other) const override {
        const Multiplication* mul = dynamic_cast<const Multiplication*>(&other);
        return mul && left_->isEqual(*mul->left_) && right_->isEqual(*mul->right_);
    }
};

class Division : public Expression {
private:
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
    
public:
    Division(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : left_(std::move(left)), right_(std::move(right)) {}
    
    double evaluate(const std::map<std::string, double>& context) const override {
        double divisor = right_->evaluate(context);
        if (divisor == 0) {
            throw std::runtime_error("Division by zero");
        }
        return left_->evaluate(context) / divisor;
    }
    
    void print(std::ostream& os = std::cout, int depth = 0) const override {
        os << "(";
        left_->print(os, depth + 1);
        os << " / ";
        right_->print(os, depth + 1);
        os << ")";
    }
    
    size_t getHash() const override {
        return left_->getHash() ^ (right_->getHash() << 3);
    }
    
    bool isEqual(const Expression& other) const override {
        const Division* div = dynamic_cast<const Division*>(&other);
        return div && left_->isEqual(*div->left_) && right_->isEqual(*div->right_);
    }
};

// Фабрика приспособленцев
class ExpressionFactory {
private:
    // Предсозданные константы от -5 до 256
    std::vector<std::shared_ptr<Constant>> predefinedConstants_;
    
    // Кеш для динамически создаваемых констант
    std::unordered_map<double, std::weak_ptr<Constant>> constantsCache_;
    
    // Кеш для переменных
    std::unordered_map<std::string, std::weak_ptr<Variable>> variablesCache_;
    
    ExpressionFactory() {
        for (int i = -5; i <= 256; ++i) {
            auto constant = std::shared_ptr<Constant>(new Constant(i));
            predefinedConstants_.push_back(constant);
            constantsCache_[i] = constant;
        }
    }
    
public:
    // Singleton
    static ExpressionFactory& getInstance() {    // Создается один раз при первом вызове (статическое поле внутри статической функции)
        static ExpressionFactory instance;
        return instance;
    }
    
    ExpressionFactory(const ExpressionFactory&) = delete;
    ExpressionFactory& operator=(const ExpressionFactory&) = delete;
    
    // Создание константы (shared_ptr)
    std::shared_ptr<Constant> createConstant(double value) {
        // Проверяем, находится ли значение в диапазоне предсозданных констант
        if (value >= -5 && value <= 256 && value == static_cast<int>(value)) {
            int index = static_cast<int>(value) + 5;
            return predefinedConstants_[index];
        }
        
        // Проверяем кеш
        auto it = constantsCache_.find(value);
        if (it != constantsCache_.end()) {
            auto shared = it->second.lock();
            if (shared) {
                return shared;
            }
        }
        
        // Создаём новую константу
        auto constant = std::shared_ptr<Constant>(new Constant(value));
        constantsCache_[value] = constant;
        return constant;
    }
    
    std::shared_ptr<Variable> createVariable(const std::string& name) {
        auto it = variablesCache_.find(name);
        if (it != variablesCache_.end()) {
            auto shared = it->second.lock();
            if (shared) {
                return shared;
            }
        }
        
        auto variable = std::shared_ptr<Variable>(new Variable(name));
        variablesCache_[name] = variable;
        return variable;
    }
    
    void cleanup() {
        for (auto it = constantsCache_.begin(); it != constantsCache_.end();) {
            if (it->second.expired()) {
                it = constantsCache_.erase(it);
            } else {
                ++it;
            }
        }
        
        for (auto it = variablesCache_.begin(); it != variablesCache_.end();) {
            if (it->second.expired()) {
                it = variablesCache_.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    // Получение статистики использования
    size_t getConstantCount() const {
        size_t count = 0;
        for (const auto& pair : constantsCache_) {
            if (!pair.second.expired()) {
                ++count;
            }
        }
        return count;
    }
    
    size_t getVariableCount() const {
        size_t count = 0;
        for (const auto& pair : variablesCache_) {
            if (!pair.second.expired()) {
                ++count;
            }
        }
        return count;
    }
};

int main() {
    auto& factory = ExpressionFactory::getInstance();
    
    // 2 + x при x = 3
    auto c = factory.createConstant(2);
    auto v = factory.createVariable("x");
    auto expression = std::make_unique<Addition>(c, v);
    
    std::map<std::string, double> context;
    context["x"] = 3;
    
    std::cout << "Expression: " << *expression << std::endl;
    std::cout << "Result: " << expression->evaluate(context) << std::endl;
    
    auto c2 = factory.createConstant(2);
    auto v2 = factory.createVariable("x");
    
    std::cout << "\nSame constant: " << (c == c2 ? "yes" : "no") << std::endl;
    std::cout << "Same variable: " << (v == v2 ? "yes" : "no") << std::endl;
    
    auto c5 = factory.createConstant(5);
    auto y = factory.createVariable("y");
    auto c10 = factory.createConstant(10);
    auto z = factory.createVariable("z");
    auto c3 = factory.createConstant(3);
    
    auto mult = std::make_shared<Multiplication>(c5, y);
    auto add = std::make_shared<Addition>(mult, c10);
    auto sub = std::make_shared<Subtraction>(z, c3);
    auto div = std::make_shared<Division>(add, sub);
    
    context["y"] = 2;
    context["z"] = 5;
    
    std::cout << "\nComplex expression: " << *div << std::endl;
    std::cout << "Result: " << div->evaluate(context) << std::endl;
    
    // Демонстрация использования констант из предсозданного диапазона
    auto c100 = factory.createConstant(100);
    auto cMinus5 = factory.createConstant(-5);
    auto c256 = factory.createConstant(256);
    
    std::cout << "\nPredefined constants: " << c100->getValue() 
              << ", " << cMinus5->getValue() 
              << ", " << c256->getValue() << std::endl;
    
    // Демонстрация создания константы вне диапазона
    auto c300 = factory.createConstant(300);
    std::cout << "Custom constant: " << c300->getValue() << std::endl;
    
    // Статистика использования
    std::cout << "\nActive constants: " << factory.getConstantCount() << std::endl;
    std::cout << "Active variables: " << factory.getVariableCount() << std::endl;
    
    // Принудительная очистка
    factory.cleanup();
    
    return 0;
}