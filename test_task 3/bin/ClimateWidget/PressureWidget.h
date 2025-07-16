#pragma once
#include <QWidget>     // Базовый класс QWidget
#include <QVBoxLayout> // Вертикальная компоновка
#include <QHBoxLayout> // Горизонтальная компоновка
#include <QLabel>      // Текстовые метки
#include <QPushButton> // Кнопки "+" и "-"
#include <QComboBox>   // Выпадающий список (шкалы температуры)
#include <QString>     // Работа со строками QString
#include <QList>       // Для findChildren<QPushButton*>
class PressureWidget : public QWidget
{
public:
    PressureWidget(SystemsConditioning *system, QWidget *parent = nullptr) : QWidget(parent), m_system(system)
    {
        setupUI();
        connectSignals();
    }

    template<class T>
    void update(T temp_)
    {
        tempValueLabel->setText(
            QString::number(temp_.getValue(), 'f', 2) +
            " " +
            QString::fromStdString(temp_.Get_Dimension()));
    }

private slots:

    void changeUnit(int index)
    {
        PressureUnit pressureunit = static_cast<PressureUnit>(index);
        m_system->Update_Dimension(pressureunit);
        update(m_system->Get_Parametr<Pressure>());
    }

private:
    void setupUI()
    {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        QHBoxLayout *tempLayout = new QHBoxLayout();

        QVBoxLayout *tempValueLayout = new QVBoxLayout();
        tempValueLabel = new QLabel();
        update(m_system->Get_Parametr<Pressure>());

        tempValueLayout->addWidget(tempValueLabel);

        QVBoxLayout *tempControlLayout = new QVBoxLayout();

        tempUnitCombo = new QComboBox();
        tempUnitCombo->addItems({"P", "mmHg"});

        tempControlLayout->addWidget(tempUnitCombo);

        tempLayout->addLayout(tempValueLayout);
        tempLayout->addStretch(); 
        tempLayout->addLayout(tempControlLayout);

        mainLayout->addLayout(tempLayout);
    }

    void connectSignals()
    {
        connect(tempUnitCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &PressureWidget::changeUnit);
    }
    SystemsConditioning *m_system = nullptr;
    QComboBox *tempUnitCombo;
    QLabel *tempValueLabel;
    ~PressureWidget()
    {
        delete m_system; 
    }
};