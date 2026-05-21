#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>
#include <sstream>

// Модель данных
class Checkpoint {
public:
    enum class Type { MANDATORY, OPTIONAL };

private:
    std::string name_;
    double latitude_;
    double longitude_;
    Type type_;
    double penalty_;  // значим только для OPTIONAL

public:
    Checkpoint(std::string name, double lat, double lon, Type type, double penalty = 0.0)
        : name_(std::move(name)), latitude_(lat), longitude_(lon), type_(type), penalty_(penalty) {}

    const std::string& getName() const { return name_; }
    double getLatitude() const { return latitude_; }
    double getLongitude() const { return longitude_; }
    Type getType() const { return type_; }

    // Возвращает строковое представление штрафа
    std::string getPenaltyDisplay() const {
        if (type_ == Type::MANDATORY) {
            return "незачёт СУ";
        }
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << penalty_ << " ч";
        return oss.str();
    }

    // Возвращает числовое значение штрафа (0 для обязательных)
    double getNumericPenalty() const {
        return (type_ == Type::OPTIONAL) ? penalty_ : 0.0;
    }
};

// =============================================
// Абстрактный Строитель
// =============================================
class ReportBuilder {
public:
    virtual ~ReportBuilder() = default;

    virtual void startReport() = 0;
    virtual void addCheckpoint(int index, const Checkpoint& cp) = 0;
    virtual void finishReport() = 0;
};

// =============================================
// Распорядитель
// =============================================
class RallyReportDirector {
public:
    void setBuilder(ReportBuilder* builder) {
        builder_ = builder;
    }

    void buildReport(const std::vector<Checkpoint>& checkpoints) {
        if (!builder_) return;

        builder_->startReport();
        for (size_t i = 0; i < checkpoints.size(); ++i) {
            builder_->addCheckpoint(static_cast<int>(i + 1), checkpoints[i]);
        }
        builder_->finishReport();
    }

private:
    ReportBuilder* builder_ = nullptr;
};

// =============================================
// ConcreteBuilder 1: Текстовый отчёт
// =============================================
class TextReportBuilder : public ReportBuilder {
private:
    std::ostringstream report_;

public:
    void startReport() override {
        report_.str("");  // очистка
        report_ << "=== Трасса трофи-рейда ===\n";
        report_ << std::left
                << std::setw(5)  << "№"
                << std::setw(18) << "Имя"
                << std::setw(28) << "Координаты"
                << "Штраф\n";
        report_ << std::string(65, '-') << "\n";
    }

    void addCheckpoint(int index, const Checkpoint& cp) override {
        std::ostringstream coords;
        coords << std::fixed << std::setprecision(4)
               << std::showpos << cp.getLatitude() << "°, "
               << cp.getLongitude() << "°";

        report_ << std::left
                << std::setw(5)  << index
                << std::setw(18) << cp.getName()
                << std::setw(28) << coords.str()
                << cp.getPenaltyDisplay() << "\n";
    }

    void finishReport() override {
        report_ << std::string(65, '=') << "\n";
    }

    std::string getResult() const {
        return report_.str();
    }
};

// =============================================
// ConcreteBuilder 2: Подсчёт суммарного штрафа
// =============================================
class PenaltyCalculator : public ReportBuilder {
private:
    double totalPenalty_ = 0.0;

public:
    void startReport() override {
        totalPenalty_ = 0.0;
    }

    void addCheckpoint(int index, const Checkpoint& cp) override {
        (void)index;  // не используется в этом строителе
        totalPenalty_ += cp.getNumericPenalty();
    }

    void finishReport() override {
        // ничего не делаем
    }

    double getResult() const {
        return totalPenalty_;
    }
};

// =============================================
// Клиентский код (демонстрация)
// =============================================
int main() {
    // Тестовые данные
    std::vector<Checkpoint> checkpoints = {
        {"Старт",   55.7558, 37.6176, Checkpoint::Type::MANDATORY},
        {"Брод",    55.8122, 37.5023, Checkpoint::Type::OPTIONAL,  1.5},
        {"Лесной",  55.9233, 37.9876, Checkpoint::Type::MANDATORY},
        {"Грязь",   55.6789, 38.1234, Checkpoint::Type::OPTIONAL,  0.5},
        {"Финиш",   55.6543, 37.8765, Checkpoint::Type::MANDATORY}
    };

    RallyReportDirector director;

    // --- Вариант 1: текстовый отчёт ---
    TextReportBuilder textBuilder;
    director.setBuilder(&textBuilder);
    director.buildReport(checkpoints);
    std::cout << textBuilder.getResult() << "\n";

    // --- Вариант 2: суммарный штраф ---
    PenaltyCalculator penaltyCalc;
    director.setBuilder(&penaltyCalc);
    director.buildReport(checkpoints);
    std::cout << "Суммарный штраф: " 
              << std::fixed << std::setprecision(2) 
              << penaltyCalc.getResult() << " ч\n";

    return 0;
}