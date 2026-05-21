#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <memory>

class SetImplementation {
public:
    virtual ~SetImplementation() = default;
    
    virtual void add(int element) = 0;
    virtual void remove(int element) = 0;
    virtual bool contains(int element) const = 0;
    virtual std::vector<int> getElements() const = 0;
    virtual size_t size() const = 0;
    virtual std::unique_ptr<SetImplementation> clone() const = 0;
};

// Массив для маленьких множеств
class ArraySet : public SetImplementation {
private:
    std::vector<int> elements;
    static const size_t MAX_SIZE = 10;
    
    void sortAndUnique() {
        std::sort(elements.begin(), elements.end());
        elements.erase(std::unique(elements.begin(), elements.end()), elements.end());
    }
    
public:
    void add(int element) override {
        if (std::find(elements.begin(), elements.end(), element) == elements.end()) {
            elements.push_back(element);
            sortAndUnique();
        }
    }
    
    void remove(int element) override {
        auto it = std::find(elements.begin(), elements.end(), element);
        if (it != elements.end()) {
            elements.erase(it);
        }
    }
    
    bool contains(int element) const override {
        return std::find(elements.begin(), elements.end(), element) != elements.end();
    }
    
    std::vector<int> getElements() const override {
        return elements;
    }
    
    size_t size() const override {
        return elements.size();
    }
    
    std::unique_ptr<SetImplementation> clone() const override {
        auto newSet = std::make_unique<ArraySet>();
        newSet->elements = this->elements;
        return newSet;
    }
    
    static size_t getMaxSize() { return MAX_SIZE; }
};

//  Хеш-таблица для больших множеств
class HashSet : public SetImplementation {
private:
    std::unordered_set<int> elements;
    
public:
    void add(int element) override {
        elements.insert(element);
    }
    
    void remove(int element) override {
        elements.erase(element);
    }
    
    bool contains(int element) const override {
        return elements.find(element) != elements.end();
    }
    
    std::vector<int> getElements() const override {
        return std::vector<int>(elements.begin(), elements.end());
    }
    
    size_t size() const override {
        return elements.size();
    }
    
    std::unique_ptr<SetImplementation> clone() const override {
        auto newSet = std::make_unique<HashSet>();
        newSet->elements = this->elements;
        return newSet;
    }
};

class Set {
private:
    std::unique_ptr<SetImplementation> implementation;
    
    void checkAndSwitchImplementation() {
        size_t currentSize = implementation->size();
        
        if (dynamic_cast<ArraySet*>(implementation.get()) && // пытаемся преобразовать указатель implementation на конкретный класс ArraySet
            currentSize > ArraySet::getMaxSize()) {
            switchToHashSet();
        }
        // Если используется HashSet и размер маленький
        else if (dynamic_cast<HashSet*>(implementation.get()) && 
                 currentSize <= ArraySet::getMaxSize()) {
            switchToArraySet();
        }
    }
    
    void switchToHashSet() {
        auto hashSet = std::make_unique<HashSet>();
        auto elements = implementation->getElements();
        for (int element : elements) {
            hashSet->add(element);
        }
        implementation = std::move(hashSet);
        std::cout << "Переключено на HashSet (элементов: " << implementation->size() << ")\n";
    }
    
    void switchToArraySet() {
        auto arraySet = std::make_unique<ArraySet>();
        auto elements = implementation->getElements();
        for (int element : elements) {
            arraySet->add(element);
        }
        implementation = std::move(arraySet);
        std::cout << "Переключено на ArraySet (элементов: " << implementation->size() << ")\n";
    }
    
public:
    Set() : implementation(std::make_unique<ArraySet>()) {}
    
    Set(const Set& other) 
        : implementation(other.implementation->clone()) {}
    
    Set& operator=(const Set& other) {
        if (this != &other) {
            implementation = other.implementation->clone();
        }
        return *this;
    }
    
    void add(int element) {
        implementation->add(element);
        checkAndSwitchImplementation();
    }
    
    void remove(int element) {
        implementation->remove(element);
        checkAndSwitchImplementation();
    }
    
    bool contains(int element) const {
        return implementation->contains(element);
    }
    
    size_t size() const {
        return implementation->size();
    }
    
    std::vector<int> getElements() const {
        return implementation->getElements();
    }
    
    Set unionWith(const Set& other) const {
        Set result = *this;
        auto otherElements = other.getElements();
        for (int element : otherElements) {
            result.add(element);
        }
        return result;
    }
    
    Set intersectWith(const Set& other) const {
        Set result;
        auto elements = getElements();
        for (int element : elements) {
            if (other.contains(element)) {
                result.add(element);
            }
        }
        return result;
    }
    
    void print() const {
        auto elements = getElements();
        std::cout << "{ ";
        for (size_t i = 0; i < elements.size(); ++i) {
            std::cout << elements[i];
            if (i < elements.size() - 1) std::cout << ", ";
        }
        std::cout << " }";
        
        // Показываем тип реализации
        if (dynamic_cast<ArraySet*>(implementation.get())) {
            std::cout << " [ArraySet]";
        } else if (dynamic_cast<HashSet*>(implementation.get())) {
            std::cout << " [HashSet]";
        }
        std::cout << std::endl;
    }
};

int main() {
    std::cout << "=== Демонстрация паттерна Bridge для множества ===\n\n";
    
    // Создаем множество и добавляем элементы
    Set set1;
    std::cout << "Добавляем элементы в set1 (маленькое множество):\n";
    for (int i = 1; i <= 5; ++i) {
        set1.add(i);
        std::cout << "  Добавлен " << i << ": ";
        set1.print();
    }
    
    // Создаем второе множество
    Set set2;
    std::cout << "\nДобавляем элементы в set2:\n";
    for (int i = 3; i <= 12; ++i) {
        set2.add(i);
        std::cout << "  Добавлен " << i << ": ";
        set2.print();
    }
    
    // Демонстрация автоматического переключения
    std::cout << "\n=== Демонстрация автоматического переключения ===\n";
    Set set3;
    std::cout << "Добавляем много элементов в set3:\n";
    for (int i = 1; i <= 15; ++i) {
        set3.add(i);
        if (i == 11) {
            std::cout << "  (ожидается переключение на HashSet)\n";
        }
    }
    set3.print();
    
    // Операции над множествами
    std::cout << "\n=== Операции над множествами ===\n";
    
    std::cout << "Объединение set1 и set2: ";
    Set unionSet = set1.unionWith(set2);
    unionSet.print();
    
    std::cout << "Пересечение set1 и set2: ";
    Set intersectSet = set1.intersectWith(set2);
    intersectSet.print();
    
    // Проверка наличия элементов
    std::cout << "\n=== Проверка элементов ===\n";
    std::cout << "set1 содержит 3? " << (set1.contains(3) ? "Да" : "Нет") << std::endl;
    std::cout << "set1 содержит 15? " << (set1.contains(15) ? "Да" : "Нет") << std::endl;
    
    // Удаление элементов
    std::cout << "\n=== Удаление элементов ===\n";
    std::cout << "Удаляем элементы из set3:\n";
    for (int i = 1; i <= 10; ++i) {
        set3.remove(i);
        if (i == 10) {
            std::cout << "  (ожидается переключение обратно на ArraySet)\n";
        }
    }
    set3.print();
    
    return 0;
}