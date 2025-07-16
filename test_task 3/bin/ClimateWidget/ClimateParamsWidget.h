/**
 * @file ClimateParamsWidget.h
 * @brief Виджет для редактирования климатических параметров
 *
 * Предоставляет интерфейс для изменения и отправки параметров температуры,
 * давления и влажности в систему кондиционирования.
 */

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QDoubleValidator>
#include <QMessageBox>
#include "dimension.h"
#include "SystemsConditioning.h"

// Предварительное объявление класса
class SystemsConditioning;

/**
 * @class ClimateParamsWidget
 * @brief Виджет для редактирования климатических параметров
 *
 * Этот виджет предоставляет поля ввода для значений температуры, давления и влажности,
 * а также выпадающие списки для выбора единиц измерения. Параметры могут быть
 * отправлены для обновления системы кондиционирования.
 */
class ClimateParamsWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор ClimateParamsWidget
     * @param system Указатель на систему кондиционирования
     * @param parent Родительский виджет
     */
    ClimateParamsWidget(SystemsConditioning *system, QWidget *parent = nullptr);

    /**
     * @brief Деструктор ClimateParamsWidget
     */
    ~ClimateParamsWidget() override = default;

private:
    SystemsConditioning *m_system; ///< Указатель на систему кондиционирования
    Parametrs params_;             ///< Текущие параметры системы

    // Элементы интерфейса
    QLineEdit *temperatureEdit;   ///< Поле ввода температуры
    QLineEdit *pressureEdit;      ///< Поле ввода давления
    QLineEdit *humidityEdit;      ///< Поле ввода влажности
    QComboBox *tempUnitCombo;     ///< Выбор единиц измерения температуры
    QComboBox *pressureUnitCombo; ///< Выбор единиц измерения давления
    QPushButton *submitButton;    ///< Кнопка подтверждения изменений

    /**
     * @brief Инициализация пользовательского интерфейса
     */
    void initUI();

    /**
     * @brief Настройка соединений сигналов и слотов
     */
    void setupConnections();

    /**
     * @brief Создание виджета для ввода температуры
     * @param mainLayout Основной вертикальный layout
     */
    void createTemperatureWidget(QVBoxLayout *mainLayout);

    /**
     * @brief Создание виджета для ввода давления
     * @param mainLayout Основной вертикальный layout
     */
    void createPressureWidget(QVBoxLayout *mainLayout);

    /**
     * @brief Создание виджета для ввода влажности
     * @param mainLayout Основной вертикальный layout
     */
    void createHumidityWidget(QVBoxLayout *mainLayout);

private slots:
    /**
     * @brief Обработчик нажатия кнопки подтверждения
     *
     * Собирает введенные параметры и отправляет их в систему кондиционирования
     */
    void onSubmitClicked();
};