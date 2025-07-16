/**
 * @file HumidityWidget.h
 * @brief Заголовочный файл класса виджета влажности
 *
 * Виджет предоставляет интерфейс для отображения текущего значения влажности
 * и управления параметрами системы кондиционирования.
 */

#pragma once

#include <QWidget>     
#include <QVBoxLayout> 
#include <QHBoxLayout> 
#include <QLabel>     
#include <QPushButton>
#include <QComboBox>  
#include <QString>  
#include <QList>    
#include"dimension.h"

// Предварительное объявление класса
class SystemsConditioning;

/**
 * @class HumidityWidget
 * @brief Виджет для отображения и управления параметрами влажности
 */
class HumidityWidget : public QWidget
{

public:
    /**
     * @brief Конструктор виджета влажности
     * @param system Указатель на систему кондиционирования
     * @param parent Родительский виджет
     */
    HumidityWidget(SystemsConditioning *system, QWidget *parent = nullptr)
        : QWidget(parent),
          m_system(system)
    {
        setupUI();
    }

    /**
     * @brief Обновляет отображаемое значение влажности
     * @tparam T Тип параметра (должен быть Humidity)
     * @param temp_ Объект с данными о влажности
     */
    template <class T>
    void update(T temp_)
    {
        // Форматирование значения влажности с 2 знаками после запятой
        tempValueLabel->setText(
            QString::number(temp_.getValue(), 'f', 2) +
            " " +
            QString::fromStdString(temp_.Get_Dimension()));
    }

private:
    /**
     * @brief Настраивает пользовательский интерфейс виджета
     */
    void setupUI()
    {
        // Основной вертикальный layout
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // Горизонтальный layout для значения влажности
        QHBoxLayout *tempLayout = new QHBoxLayout();

        // Вертикальный layout для отображения значения
        QVBoxLayout *tempValueLayout = new QVBoxLayout();
        tempValueLabel = new QLabel();

        // Инициализация начального значения
        update(m_system->Get_Parametr<Pressure>());

        tempValueLayout->addWidget(tempValueLabel);
        tempLayout->addLayout(tempValueLayout);

        // Добавление растягивающего элемента
        tempLayout->addStretch();

        // Добавление в основной layout
        mainLayout->addLayout(tempLayout);
    }

    SystemsConditioning *m_system = nullptr; ///< Указатель на систему кондиционирования
    QLabel *tempValueLabel;                  ///< Метка для отображения значения влажности
};