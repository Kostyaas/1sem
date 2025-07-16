/**
 * @file SystemsConditioning.h
 * @brief Заголовочный файл класса системы кондиционирования
 */

#pragma once
#include <utility>
#include <QDebug>
#include <QObject>
#include "dimension.h"
#include <vector>

// Предварительные объявления классов виджетов
class TemperatureWidget;
class PressureWidget;
class HumidityWidget;

/**
 * @class SystemsConditioning
 * @brief Класс, представляющий систему кондиционирования
 * 
 * Класс управляет состоянием системы, параметрами климата и взаимодействует с виджетами отображения.
 */
class SystemsConditioning
{
private:
    Status status_;                  ///< Текущее состояние системы (вкл/выкл)
    Parametrs parametr;              ///< Параметры климата (температура, давление, влажность)
    TemperatureWidget *temper_widget; ///< Указатель на виджет температуры
    PressureWidget *pressure_widget;  ///< Указатель на виджет давления
    HumidityWidget *humid_widget;     ///< Указатель на виджет влажности
    std::vector<bool> block{true, true, true}; ///< Состояние блоков системы (активны/неактивны)

public:
    /**
     * @brief Конструктор по умолчанию
     */
    SystemsConditioning() = default;
    
    /**
     * @brief Конструктор с параметрами
     * @param parametr Начальные параметры системы
     */
    SystemsConditioning(const Parametrs &parametr);
    
    /**
     * @brief Конструктор с перемещением параметров
     * @param parametr Начальные параметры системы (r-value)
     */
    SystemsConditioning(Parametrs &&parametr);
    
    /**
     * @brief Конструктор копирования
     * @param other Другой объект SystemsConditioning
     */
    SystemsConditioning(const SystemsConditioning &other);

    /**
     * @brief Устанавливает виджеты для отображения параметров
     * @param widget Виджет температуры
     * @param pressure_widget Виджет давления
     * @param humid_widget Виджет влажности
     */
    void Set_Widget(TemperatureWidget *widget, PressureWidget *pressure_widget, HumidityWidget *humid_widget);

    /**
     * @brief Оператор присваивания
     * @param other Другой объект SystemsConditioning
     * @return Ссылка на текущий объект
     */
    SystemsConditioning &operator=(const SystemsConditioning &other);

    /**
     * @brief Включает систему
     */
    void ON();
    
    /**
     * @brief Выключает систему
     */
    void OFF();
    
    /**
     * @brief Возвращает текущее состояние системы
     * @return Статус системы (вкл/выкл)
     */
    Status GetStatus() const
    {
        return status_;
    }
    
    /**
     * @brief Обновляет значение параметра
     * @tparam T Тип параметра (Temperature, Pressure или Humidity)
     * @param new_value Новое значение параметра
     */
    template <class T>
    void Update_Value(float new_value)
    {
        parametr.update_value<T>(new_value);
    }
    
    /**
     * @brief Проверяет активность блока
     * @param i Индекс блока (0-2)
     * @return true если блок активен, false в противном случае
     */
    bool IsBlockActive(size_t i) { return block[i]; }
    
    /**
     * @brief Обновляет состояние блока
     * @param i Индекс блока (0-2)
     * @param state Новое состояние блока
     */
    void BlockUpdate(size_t i, bool state)
    {
        block[i] = state;
    }
    
    /**
     * @brief Обновляет шкалу измерения температуры
     * @param newscale Новая шкала измерения
     */
    void Update_Dimension(TempScale newscale);
    
    /**
     * @brief Обновляет единицы измерения давления
     * @param newscale Новые единицы измерения
     */
    void Update_Dimension(PressureUnit newscale);

    /**
     * @brief Возвращает параметр указанного типа
     * @tparam T Тип возвращаемого параметра
     * @return Значение параметра
     */
    template <typename T>
    T Get_Parametr() const;

    /// Специализация для получения всех параметров
    template <>
    Parametrs Get_Parametr<Parametrs>() const;

    /// Специализация для получения температуры
    template <>
    Temperature Get_Parametr<Temperature>() const;

    /// Специализация для получения давления
    template <>
    Pressure Get_Parametr<Pressure>() const;

    /// Специализация для получения влажности
    template <>
    Humidity Get_Parametr<Humidity>() const;
    
    /**
     * @brief Сохраняет параметры системы в XML файл
     * @param filename Имя файла для сохранения
     */
    void SaveXml(const QString &filename) const;
    
    /**
     * @brief Загружает параметры системы из XML файла
     * @param filename Имя файла для загрузки
     */
    void LoadXml(const QString &filename);
    
    /**
     * @brief Изменяет параметры системы
     * @param prmtr Новые параметры
     */
    void Change_Parametrs(const Parametrs &prmtr);

    /**
     * @brief Деструктор
     */
    ~SystemsConditioning() = default;
};