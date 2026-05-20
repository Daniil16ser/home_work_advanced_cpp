#include <iostream>
#include <map>
#include <string>
#include <memory>
#include <stdexcept>

// Базовый класс выражения (Компоновщик)
class Expression {
public:
    virtual ~Expression() = default;
    virtual double calculate(const std::map<std::string, double>& context) const = 0;
    virtual void print(std::ostream& os = std::cout) const = 0;
    virtual bool isLeaf() const { return false; } // Для определения приспособленцев
    
    friend std::ostream& operator<<(std::ostream& os, const Expression& expr) {
        expr.print(os);
        return os;
    }
};

// Константа (Приспособленец)
class Constant : public Expression {
private:
    double value;
    int refCount; // Счетчик ссылок
    
    friend class ExpressionFactory;
    Constant(double val) : value(val), refCount(0) {}
    
public:
    double getValue() const { return value; }
    bool isLeaf() const override { return true; }
    
    void addRef() { refCount++; }
    void release() { refCount--; }
    int getRefCount() const { return refCount; }
    
    double calculate(const std::map<std::string, double>& context) const override {
        return value;
    }
    
    void print(std::ostream& os = std::cout) const override {
        os << value;
    }
};

// Переменная (Приспособленец)
class Variable : public Expression {
private:
    std::string name;
    int refCount; // Счетчик ссылок
    
    friend class ExpressionFactory;
    Variable(const std::string& varName) : name(varName), refCount(0) {}
    
public:
    const std::string& getName() const { return name; }
    bool isLeaf() const override { return true; }
    
    void addRef() { refCount++; }
    void release() { refCount--; }
    int getRefCount() const { return refCount; }
    
    double calculate(const std::map<std::string, double>& context) const override {
        auto it = context.find(name);
        if (it != context.end()) {
            return it->second;
        }
        throw std::runtime_error("Variable '" + name + "' not found in context");
    }
    
    void print(std::ostream& os = std::cout) const override {
        os << name;
    }
};

// Фабрика приспособленцев
class ExpressionFactory {
private:
    std::map<double, Constant*> constants;
    std::map<std::string, Variable*> variables;
    
    static const int PRECREATED_MIN = -5;
    static const int PRECREATED_MAX = 256;
    
public:
    ExpressionFactory() {
        // Предсоздаем константы от -5 до 256
        for (int i = PRECREATED_MIN; i <= PRECREATED_MAX; ++i) {
            double value = static_cast<double>(i);
            Constant* c = new Constant(value);
            constants[value] = c;
            c->addRef(); // Предсозданные константы всегда имеют ссылку
        }
    }
    
    ~ExpressionFactory() {
        // Очищаем все константы
        for (auto& pair : constants) {
            delete pair.second;
        }
        // Очищаем все переменные
        for (auto& pair : variables) {
            delete pair.second;
        }
    }
    
    Constant* createConstant(double value) {
        auto it = constants.find(value);
        if (it != constants.end()) {
            it->second->addRef();
            return it->second;
        }
        
        Constant* c = new Constant(value);
        constants[value] = c;
        c->addRef();
        return c;
    }
    
    Variable* createVariable(const std::string& name) {
        auto it = variables.find(name);
        if (it != variables.end()) {
            it->second->addRef();
            return it->second;
        }
        
        Variable* v = new Variable(name);
        variables[name] = v;
        v->addRef();
        return v;
    }
    
    void releaseConstant(Constant* c) {
        if (!c) return;
        
        double value = c->getValue();
        // Предсозданные константы не удаляем
        if (value >= PRECREATED_MIN && value <= PRECREATED_MAX && 
            value == static_cast<int>(value)) {
            c->release();
            return;
        }
        
        c->release();
        if (c->getRefCount() <= 0) {
            constants.erase(c->getValue());
            delete c;
        }
    }
    
    void releaseVariable(Variable* v) {
        if (!v) return;
        
        v->release();
        if (v->getRefCount() <= 0) {
            variables.erase(v->getName());
            delete v;
        }
    }
    
    size_t getConstantCount() const { return constants.size(); }
    size_t getVariableCount() const { return variables.size(); }
};

// Бинарный оператор (Компоновщик)
class BinaryOperation : public Expression {
protected:
    Expression* left;
    Expression* right;
    ExpressionFactory* factory; // Добавляем ссылку на фабрику
    
public:
    BinaryOperation(Expression* l, Expression* r, ExpressionFactory* f) 
        : left(l), right(r), factory(f) {}
    
    virtual ~BinaryOperation() {
        // Вместо прямого delete, освобождаем через фабрику для приспособленцев
        releaseExpression(left);
        releaseExpression(right);
    }
    
private:
    void releaseExpression(Expression* expr) {
        if (!expr) return;
        
        // Проверяем, является ли выражение приспособленцем
        if (expr->isLeaf()) {
            if (auto* constant = dynamic_cast<Constant*>(expr)) {
                factory->releaseConstant(constant);
            } else if (auto* variable = dynamic_cast<Variable*>(expr)) {
                factory->releaseVariable(variable);
            }
        } else {
            // Для составных выражений - удаляем через деструктор
            delete expr;
        }
    }
};

// Сложение
class Addition : public BinaryOperation {
public:
    Addition(Expression* l, Expression* r, ExpressionFactory* f) 
        : BinaryOperation(l, r, f) {}
    
    double calculate(const std::map<std::string, double>& context) const override {
        return left->calculate(context) + right->calculate(context);
    }
    
    void print(std::ostream& os = std::cout) const override {
        os << "(";
        left->print(os);
        os << " + ";
        right->print(os);
        os << ")";
    }
};

// Вычитание
class Subtraction : public BinaryOperation {
public:
    Subtraction(Expression* l, Expression* r, ExpressionFactory* f) 
        : BinaryOperation(l, r, f) {}
    
    double calculate(const std::map<std::string, double>& context) const override {
        return left->calculate(context) - right->calculate(context);
    }
    
    void print(std::ostream& os = std::cout) const override {
        os << "(";
        left->print(os);
        os << " - ";
        right->print(os);
        os << ")";
    }
};

// Умножение
class Multiplication : public BinaryOperation {
public:
    Multiplication(Expression* l, Expression* r, ExpressionFactory* f) 
        : BinaryOperation(l, r, f) {}
    
    double calculate(const std::map<std::string, double>& context) const override {
        return left->calculate(context) * right->calculate(context);
    }
    
    void print(std::ostream& os = std::cout) const override {
        os << "(";
        left->print(os);
        os << " * ";
        right->print(os);
        os << ")";
    }
};

// Деление
class Division : public BinaryOperation {
public:
    Division(Expression* l, Expression* r, ExpressionFactory* f) 
        : BinaryOperation(l, r, f) {}
    
    double calculate(const std::map<std::string, double>& context) const override {
        double divisor = right->calculate(context);
        if (divisor == 0) {
            throw std::runtime_error("Division by zero");
        }
        return left->calculate(context) / divisor;
    }
    
    void print(std::ostream& os = std::cout) const override {
        os << "(";
        left->print(os);
        os << " / ";
        right->print(os);
        os << ")";
    }
};

// Пример использования
int main() {
    ExpressionFactory factory;
    
    std::cout << "=== Пример 1: 2 + x при x = 3 ===" << std::endl;
    Constant* c = factory.createConstant(2);
    Variable* v = factory.createVariable("x");
    Addition* expression = new Addition(c, v, &factory);
    
    std::map<std::string, double> context;
    context["x"] = 3;
    
    std::cout << "Выражение: " << *expression << std::endl;
    std::cout << "Результат: " << expression->calculate(context) << std::endl;
    
    delete expression;
    
    std::cout << "\n=== Пример 2: Сложное выражение ===" << std::endl;
    Variable* x = factory.createVariable("x");
    Variable* y = factory.createVariable("y");
    Constant* c5 = factory.createConstant(5);
    Constant* c10 = factory.createConstant(10);
    
    // (x + 5) * (y - 10)
    Addition* add = new Addition(x, c5, &factory);
    Subtraction* sub = new Subtraction(y, c10, &factory);
    Multiplication* mul = new Multiplication(add, sub, &factory);
    
    context["x"] = 10;
    context["y"] = 20;
    
    std::cout << "Выражение: " << *mul << std::endl;
    std::cout << "Результат: " << mul->calculate(context) << std::endl;
    
    delete mul;
    
    std::cout << "\n=== Пример 3: Сложное выражение с делением ===" << std::endl;
    Variable* a = factory.createVariable("a");
    Variable* b = factory.createVariable("b");
    Constant* c3 = factory.createConstant(3);
    Constant* c7 = factory.createConstant(7);
    
    // (a + 3) / (b - 7)
    Addition* add2 = new Addition(a, c3, &factory);
    Subtraction* sub2 = new Subtraction(b, c7, &factory);
    Division* div = new Division(add2, sub2, &factory);
    
    context["a"] = 17;
    context["b"] = 27;
    
    std::cout << "Выражение: " << *div << std::endl;
    std::cout << "Результат: " << div->calculate(context) << std::endl;
    
    delete div;
    
    std::cout << "\n=== Пример 4: Демонстрация приспособленца ===" << std::endl;
    Variable* v1 = factory.createVariable("z");
    Variable* v2 = factory.createVariable("z");
    
    std::cout << "v1 и v2 - один объект? " << (v1 == v2 ? "Да" : "Нет") << std::endl;
    std::cout << "Адрес v1: " << v1 << ", Адрес v2: " << v2 << std::endl;
    std::cout << "Счетчик ссылок z: " << v1->getRefCount() << std::endl;
    
    factory.releaseVariable(v1);
    factory.releaseVariable(v2);
    
    std::cout << "\nСтатистика фабрики после освобождения:" << std::endl;
    std::cout << "Констант: " << factory.getConstantCount() << std::endl;
    std::cout << "Переменных: " << factory.getVariableCount() << std::endl;
    
    return 0;
}