#include"ClimateParamsWidget.h"

ClimateParamsWidget::ClimateParamsWidget(SystemsConditioning* system, QWidget* parent)
    : QWidget(parent), m_system(system)
{
    initUI();
    setupConnections();
}

void ClimateParamsWidget::initUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    createTemperatureWidget(mainLayout);
    createPressureWidget(mainLayout);
    createHumidityWidget(mainLayout);

    submitButton = new QPushButton("Отправить данные", this);
    mainLayout->addWidget(submitButton);

    setLayout(mainLayout);
    setWindowTitle("Параметры системы кондиционирования");
}

void ClimateParamsWidget::setupConnections()
{
    connect(submitButton, &QPushButton::clicked, this, &ClimateParamsWidget::onSubmitClicked);
}

void ClimateParamsWidget::onSubmitClicked()
{
    bool ok;
    
    float tempValue = temperatureEdit->text().toFloat(&ok);
    if (!ok) {
        QMessageBox::critical(this, "Ошибка ввода", "Некорректное значение температуры");
        return;
    }

    float pressureValue = pressureEdit->text().toFloat(&ok);
    if (!ok) {
        QMessageBox::critical(this, "Ошибка ввода", "Некорректное значение давления");
        return;
    }

    float humidityValue = humidityEdit->text().toFloat(&ok);
    if (!ok || humidityValue < 0 || humidityValue > 100) {
        QMessageBox::critical(this, "Ошибка ввода", "Некорректное значение влажности (0-100%)");
        return;
    }

    Temperature temp{
        tempValue,
        static_cast<TempScale>(tempUnitCombo->currentIndex())
    };
    
    Pressure pres{
        pressureValue,
        static_cast<PressureUnit>(pressureUnitCombo->currentIndex())
    };
    
    Humidity hum{humidityValue};

    if (m_system) {
        m_system->Change_Parametrs({pres, temp, hum});
    }
}

void ClimateParamsWidget::createTemperatureWidget(QVBoxLayout* mainLayout)
{
    QHBoxLayout* layout = new QHBoxLayout();

    QLabel* label = new QLabel("Температура:", this);
    layout->addWidget(label);

    temperatureEdit = new QLineEdit(this);
    temperatureEdit->setFixedWidth(100);
    temperatureEdit->setValidator(new QDoubleValidator(this));
    layout->addWidget(temperatureEdit);

    tempUnitCombo = new QComboBox(this);
    tempUnitCombo->addItems({"°C", "°F", "K"});
    tempUnitCombo->setCurrentIndex(static_cast<int>(params_.temperature_.getDimension()));
    layout->addWidget(tempUnitCombo);

    layout->addStretch();
    mainLayout->addLayout(layout);
}

void ClimateParamsWidget::createPressureWidget(QVBoxLayout* mainLayout)
{
    QHBoxLayout* layout = new QHBoxLayout();

    QLabel* label = new QLabel("Давление:", this);
    layout->addWidget(label);

    pressureEdit = new QLineEdit(this);
    pressureEdit->setFixedWidth(100);
    pressureEdit->setValidator(new QDoubleValidator(this));
    layout->addWidget(pressureEdit);

    pressureUnitCombo = new QComboBox(this);
    pressureUnitCombo->addItems({"Па", "мм.рт.ст."});
    pressureUnitCombo->setCurrentIndex(static_cast<int>(params_.pressure_.getDimension()));
    layout->addWidget(pressureUnitCombo);

    layout->addStretch();
    mainLayout->addLayout(layout);
}

void ClimateParamsWidget::createHumidityWidget(QVBoxLayout* mainLayout)
{
    QHBoxLayout* layout = new QHBoxLayout();

    QLabel* label = new QLabel("Влажность:", this);
    layout->addWidget(label);

    humidityEdit = new QLineEdit(this);
    humidityEdit->setFixedWidth(100);
    humidityEdit->setValidator(new QDoubleValidator(0, 100, 2, this));
    layout->addWidget(humidityEdit);

    QLabel* unitLabel = new QLabel("%", this);
    layout->addWidget(unitLabel);

    layout->addStretch();
    mainLayout->addLayout(layout);
}