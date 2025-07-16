/**
 * @file MainWindow.h
 * @brief Заголовочный файл класса главного окна приложения
 */

#pragma once
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QApplication>
#include <QGroupBox> 
#include "dimension.h"
#include "SystemsConditioning.h"
#include "ArrowControlWidget.h"


// Предварительные объявления классов
class TemperatureWidget;
class PressureWidget;
class HumidityWidget;
class ClimateParamsWidget;
class QPushButton;
class QGraphicsRectItem;
class QGraphicsTextItem;

/**
 * @class MainWindow
 * @brief Класс главного окна приложения управления системой
 *
 * Класс предоставляет графический интерфейс для управления системой кондиционирования,
 * отображения состояния блоков и параметров климата. Также служит средством общения со системой
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор главного окна
     * @param system Указатель на систему кондиционирования
     * @param parent Родительский виджет
     */
    explicit MainWindow(SystemsConditioning *system, QWidget *parent = nullptr);

protected:
    /**
     * @brief Обработчик события закрытия окна
     * @param event Событие закрытия
     */
    void closeEvent(QCloseEvent *event) override;

private slots:
    /**
     * 
     * @brief Сохраняет данные системы в XML файл
     */
    void SaveSystemData();

    /**
     * @brief Переключает состояние системы (вкл/выкл)
     */
    void toggleSystemPower();

    /**
     * @brief Переключает тему интерфейса (темная/светлая)
     */
    void toggleTheme();

    /**
     * @brief Открывает окно для ввода климатических параметров
     */
    void openInputDataWindow();

    /**
     * @brief Устанавливает состояние блока системы
     * @param blockIndex Индекс блока (0-2)
     * @param state Новое состояние блока (вкл/выкл)
     */
    void setBlockState(int blockIndex, bool state);

private:
    SystemsConditioning *m_system; ///< Указатель на систему кондиционирования
    bool m_darkTheme = false;      ///< Флаг темной темы интерфейса
    QPushButton *powerButton;      ///< Кнопка включения/выключения системы
    ArrowControlWidget *airDirectionWidget; ///< стрелки для изменения направления воздуха
    QGraphicsScene *m_blocksScene;       ///< Сцена для отображения блоков системы
    QGraphicsView *m_blocksView;         ///< Виджет для отображения сцены блоков
    QGraphicsRectItem *m_blockItems[3];  ///< Графические элементы блоков
    QGraphicsTextItem *m_blockLabels[3]; ///< Текстовые метки блоков

    /**
     * @brief Загружает данные системы из XML файла
     */
    void loadSystemData();
    void ConnectSignals();
    /**
     * @brief Инициализирует пользовательский интерфейс
     */
    void initUI();

    /**
     * @brief Инициализирует отображение блоков системы
     */
    void initBlocksDisplay();

    /**
     * @brief Обновляет отображение состояния блоков системы
     */
    void updateBlocksDisplay();

    /**
     * @brief Создает меню приложения
     */
    void createMenuBar();

    /**
     * @brief Применяет текущую тему (темную или светлую) к интерфейсу
     */
    void applyTheme();

    /**
     * @brief Обновляет состояние кнопки питания системы
     */
    void updatePowerButton();

    /**
     * @brief Создает групповой элемент с заголовком
     * @param title Заголовок группы
     * @param content Содержимое группы
     * @return Указатель на созданный групповой элемент
     */
    QGroupBox *createGroupBox(const QString &title, QWidget *content);
};