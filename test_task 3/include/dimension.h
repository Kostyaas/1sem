/**
 * @file dimension.h
 * @brief Заголовочный файл с определениями типов для работы с климатическими параметрами
 *
 * Содержит определения структур для работы с температурой, давлением и влажностью,
 * включая конвертацию между различными единицами измерения.
 */

#pragma once

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <QDebug>

/**
 * @enum Status
 * @brief Состояние системы (вкл/выкл)
 */
enum class Status
{
    Off, ///< Система выключена
    on   ///< Система включена
};

/**
 * @enum TempScale
 * @brief Шкалы измерения температуры
 */
enum class TempScale
{
    Celsius,    ///< Градусы Цельсия
    Fahrenheit, ///< Градусы Фаренгейта
    Kelvin      ///< Кельвины
};

/**
 * @enum PressureUnit
 * @brief Единицы измерения давления
 */
enum class PressureUnit
{
    Pascal, ///< Паскали
    mmHg    ///< Миллиметры ртутного столба
};

/**
 * @struct Type_Parametr
 * @brief Шаблонная структура для хранения параметров с единицами измерения
 * @tparam Dimension Тип единиц измерения (TempScale или PressureUnit)
 */
template <class Dimension>
struct Type_Parametr
{
    float value;          ///< Значение параметра
    Dimension dimension_; ///< Единицы измерения

    /**
     * @brief Получить значение параметра
     * @return Текущее значение
     */
    float getValue() const { return value; }

    /**
     * @brief Получить единицы измерения
     * @return Текущие единицы измерения
     */
    Dimension getDimension() const { return dimension_; }
};

/**
 * @struct Pressure
 * @brief Структура для работы с давлением
 */
struct Pressure : Type_Parametr<PressureUnit>
{
    /**
     * @brief Конвертировать в другие единицы измерения
     * @param newUnit Новые единицы измерения
     */
    void convertTo(PressureUnit newUnit)
    {
        if (dimension_ == newUnit)
            return;

        if (dimension_ == PressureUnit::Pascal && newUnit == PressureUnit::mmHg)
        {
            value /= 133.322f;
        }
        else if (dimension_ == PressureUnit::mmHg && newUnit == PressureUnit::Pascal)
        {
            value *= 133.322f;
        }

        dimension_ = newUnit;
    }

    /**
     * @brief Установить единицы измерения по строке
     * @param unit Строковое представление единиц измерения
     * @throws std::runtime_error При неизвестных единицах измерения
     */
    void Set_Dimension(const std::string &unit)
    {
        if (unit == "Pa" || unit == "Pascal")
        {
            dimension_ = PressureUnit::Pascal;
        }
        else if (unit == "mmHg" || unit == "mm Hg")
        {
            dimension_ = PressureUnit::mmHg;
        }
        else
        {
            throw std::runtime_error("Unknown pressure unit: " + unit);
        }
    }

    /**
     * @brief Получить строковое представление единиц измерения
     * @return Строка с единицами измерения
     * @throws std::runtime_error При неизвестных единицах измерения
     */
    std::string Get_Dimension() const
    {
        switch (dimension_)
        {
        case PressureUnit::Pascal:
            return "Pa";
        case PressureUnit::mmHg:
            return "mmHg";
        default:
            throw std::runtime_error("Unknown pressure unit");
        }
    }
};

/**
 * @struct Temperature
 * @brief Структура для работы с температурой
 */
struct Temperature : Type_Parametr<TempScale>
{
    /**
     * @brief Конструктор температуры
     * @param val Значение температуры
     * @param scale Шкала измерения
     */
    Temperature(float val = 0.0f, TempScale scale = TempScale::Celsius)
    {
        value = val;
        dimension_ = scale;
    }

    /**
     * @brief Конвертировать в другую шкалу измерения
     * @param newScale Новая шкала измерения
     * @return Ссылка на текущий объект
     */
    Temperature &convertTo(TempScale newScale)
    {
        if (dimension_ == newScale)
            return *this;

        float inCelsius = value;
        switch (dimension_)
        {
        case TempScale::Fahrenheit:
            inCelsius = (value - 32.0f) * 5.0f / 9.0f;
            break;
        case TempScale::Kelvin:
            inCelsius = value - 273.15f;
            break;
        case TempScale::Celsius:
            break;
        }

        dimension_ = newScale;

        switch (dimension_)
        {
        case TempScale::Celsius:
            value = inCelsius;
            break;
        case TempScale::Fahrenheit:
            value = inCelsius * 9.0f / 5.0f + 32.0f;
            break;
        case TempScale::Kelvin:
            value = inCelsius + 273.15f;
            break;
        }

        return *this;
    }

    /**
     * @brief Установить шкалу измерения по строке
     * @param unit Строковое представление шкалы измерения
     * @throws std::runtime_error При неизвестной шкале измерения
     */
    void Set_Dimension(const std::string &unit)
    {
        if (unit == "°C" || unit == "C" || unit == "Celsius")
        {
            dimension_ = TempScale::Celsius;
        }
        else if (unit == "°F" || unit == "F" || unit == "Fahrenheit")
        {
            dimension_ = TempScale::Fahrenheit;
        }
        else if (unit == "K" || unit == "Kelvin")
        {
            dimension_ = TempScale::Kelvin;
        }
        else
        {
            throw std::runtime_error("Unknown temperature unit: " + unit);
        }
    }

    /**
     * @brief Получить строковое представление шкалы измерения
     * @return Строка с единицами измерения
     * @throws std::runtime_error При неизвестной шкале измерения
     */
    std::string Get_Dimension() const
    {
        switch (dimension_)
        {
        case TempScale::Celsius:
            return "°C";
        case TempScale::Fahrenheit:
            return "°F";
        case TempScale::Kelvin:
            return "K";
        default:
            throw std::runtime_error("Unknown temperature scale");
        }
    }
};

/**
 * @struct Humidity
 * @brief Структура для работы с влажностью
 */
struct Humidity
{
    float value; ///< Значение влажности в процентах

    /**
     * @brief Получить значение влажности
     * @return Текущее значение влажности
     */
    float getValue() { return value; }

    /**
     * @brief Получить единицы измерения влажности
     * @return Строка с символом процента
     */
    std::string Get_Dimension() const { return "%"; }
};

/**
 * @struct Parametrs
 * @brief Структура для хранения всех климатических параметров
 */
struct Parametrs
{
    Pressure pressure_;       ///< Давление
    Temperature temperature_; ///< Температура
    Humidity humidity_;       ///< Влажность

    /**
     * @brief Оператор присваивания
     * @param other Другой объект Parametrs
     * @return Ссылка на текущий объект
     */
    Parametrs &operator=(const Parametrs &other)
    {
        if (this != &other)
        {
            pressure_ = other.pressure_;
            temperature_ = other.temperature_;
            humidity_ = other.humidity_;
        }
        return *this;
    }

    /**
     * @brief Обновить значение параметра
     * @tparam T Тип параметра (Temperature, Pressure или Humidity)
     * @param value_ Значение для добавления
     */
    template <class T>
    void update_value(float value_)
    {
        static_assert(sizeof(T) == 0, "Unsupported type for update_value");
    }

    /// Специализация для температуры
    template <>
    void update_value<Temperature>(float value_)
    {
        temperature_.value += value_;
    }

    /// Специализация для давления
    template <>
    void update_value<Pressure>(float value_)
    {
        pressure_.value += value_;
    }

    /// Специализация для влажности
    template <>
    void update_value<Humidity>(float value_)
    {
        humidity_.value += value_;
    }

    /**
     * @brief Конвертировать температуру в другую шкалу
     * @param newScale Новая шкала измерения
     */
    void convert(TempScale newScale)
    {
        temperature_.convertTo(newScale);
    }

    /**
     * @brief Конвертировать давление в другие единицы
     * @param newScale Новые единицы измерения
     */
    void convert(PressureUnit newScale)
    {
        pressure_.convertTo(newScale);
    }
};