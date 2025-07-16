#include "TemperatureWidget.h"

TemperatureWidget::TemperatureWidget(SystemsConditioning *system, QWidget *parent)
    : QWidget(parent), m_system(system)
{
    setupUI();
    connectSignals();
}

void TemperatureWidget::update(Temperature temp_)
{
    tempValueLabel->setText(
        QString::number(temp_.getValue(), 'f', 2) +
        " " +
        QString::fromStdString(temp_.Get_Dimension()));
}

TemperatureWidget::~TemperatureWidget()
{
    delete m_system;
}

void TemperatureWidget::increaseTemperature()
{
    m_system->Update_Value<Temperature>(0.5f);
    update(m_system->Get_Parametr<Temperature>());
}

void TemperatureWidget::decreaseTemperature()
{
    m_system->Update_Value<Temperature>(-0.5f);
    update(m_system->Get_Parametr<Temperature>());
}

void TemperatureWidget::changeTemperatureUnit(int index)
{
    TempScale newScale = static_cast<TempScale>(index);
    m_system->Update_Dimension(newScale);
    update(m_system->Get_Parametr<Temperature>());
}

void TemperatureWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *tempLayout = new QHBoxLayout();

    QVBoxLayout *tempValueLayout = new QVBoxLayout();
    tempValueLabel = new QLabel();
    update(m_system->Get_Parametr<Temperature>());
    tempValueLayout->addWidget(tempValueLabel);

    QVBoxLayout *tempControlLayout = new QVBoxLayout();

    // Кнопки 
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *minusButton = new QPushButton("-");
    QPushButton *plusButton = new QPushButton("+");
    buttonLayout->addWidget(minusButton);
    buttonLayout->addWidget(plusButton);

    //  выбора шкалы
    tempUnitCombo = new QComboBox();
    tempUnitCombo->addItems({"°C", "°F", "K"});

    tempControlLayout->addLayout(buttonLayout);
    tempControlLayout->addWidget(tempUnitCombo);


    tempLayout->addLayout(tempValueLayout);
    tempLayout->addStretch();
    tempLayout->addLayout(tempControlLayout);

    mainLayout->addLayout(tempLayout);
}

void TemperatureWidget::connectSignals()
{
    connect(tempUnitCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TemperatureWidget::changeTemperatureUnit);

    QList<QPushButton*> buttons = findChildren<QPushButton*>();
    
    connect(buttons[0], &QPushButton::clicked, 
            this, &TemperatureWidget::decreaseTemperature);
    connect(buttons[1], &QPushButton::clicked, 
            this, &TemperatureWidget::increaseTemperature);
}