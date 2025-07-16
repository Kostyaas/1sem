/**
 * @file MainWindow.cpp
 * @brief Файл реализации класса главного окна приложения
 */
#include "window.h"
#include "TemperatureWidget.h"
#include "PressureWidget.h"
#include "HumidityWidget.h"
#include "ClimateParamsWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QApplication>

MainWindow::MainWindow(SystemsConditioning *system, QWidget *parent)
    : QMainWindow(parent), m_system(system)
{
    m_system->OFF();
    initBlocksDisplay();
    initUI();
    updatePowerButton();
    updateBlocksDisplay();
    createMenuBar();
    applyTheme();
    loadSystemData();
    ConnectSignals();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    SaveSystemData();
    event->accept();
}

void MainWindow::SaveSystemData()
{
    try
    {
        m_system->SaveXml("base.xml");
        QMessageBox::information(this, "Сохранение", "Данные успешно сохранены");
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Ошибка", QString("Не удалось сохранить данные: %1").arg(e.what()));
    }
}

void MainWindow::toggleSystemPower()
{
    if (m_system->GetStatus() == Status::on)
    {
        m_system->OFF();
    }
    else
    {
        m_system->ON();
    }
    updatePowerButton();
    updateBlocksDisplay();
}

void MainWindow::toggleTheme()
{
    m_darkTheme = !m_darkTheme;
    applyTheme();
    updateBlocksDisplay();
}

void MainWindow::openInputDataWindow()
{
    auto *window = new ClimateParamsWidget(m_system);
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();
}

void MainWindow::setBlockState(int blockIndex, bool state)
{
    if (blockIndex >= 0 && blockIndex < 3)
    {
        m_system->BlockUpdate(blockIndex, state);
        updateBlocksDisplay();
    }
}

void MainWindow::loadSystemData()
{
    try
    {
        m_system->LoadXml("base.xml");
        qDebug() << "Данные успешно загружены";
    }
    catch (const std::exception &e)
    {
        qDebug() << "Не удалось загрузить данные:" << e.what();
        m_system->OFF();
    }
}

void MainWindow::initUI()
{
    setWindowTitle("Климатическая система");
    resize(900, 600);

    // Кнопка питания
    powerButton = new QPushButton();
    powerButton->setFixedSize(30, 30);
    powerButton->setCheckable(true);
    connect(powerButton, &QPushButton::clicked, this, &MainWindow::toggleSystemPower);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Верхняя панель с кнопкой питания
    QWidget *topPanel = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);
    topLayout->addStretch();
    topLayout->addWidget(powerButton);
    mainLayout->addWidget(topPanel);

    // Основной контент
    QHBoxLayout *contentLayout = new QHBoxLayout();

    // Левая панель с параметрами
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(10, 10, 10, 10);

    //
    auto *tempWidget = new TemperatureWidget(m_system);
    auto *pressureWidget = new PressureWidget(m_system);
    auto *humidityWidget = new HumidityWidget(m_system);

    m_system->Set_Widget(tempWidget, pressureWidget, humidityWidget);

    leftLayout->addWidget(createGroupBox("Температура", tempWidget));
    leftLayout->addWidget(createGroupBox("Давление", pressureWidget));
    leftLayout->addWidget(createGroupBox("Влажность", humidityWidget));

    QGroupBox *directionGroup = new QGroupBox("Направление воздуха");
    QVBoxLayout *directionLayout = new QVBoxLayout();

    airDirectionWidget = new ArrowControlWidget();

    directionLayout->addWidget(airDirectionWidget);
    directionGroup->setLayout(directionLayout);
    leftLayout->addWidget(directionGroup);
    leftLayout->addStretch();

    contentLayout->addWidget(leftPanel, 1);

    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(10, 10, 10, 10);
    rightLayout->addWidget(new QLabel("Состояние блоков"));
    rightLayout->addWidget(m_blocksView);
    rightLayout->addStretch();

    contentLayout->addWidget(rightPanel, 0);

    mainLayout->addLayout(contentLayout, 1);
    setCentralWidget(centralWidget);
}
void MainWindow::ConnectSignals(){
    connect(airDirectionWidget, &ArrowControlWidget::directionChanged,
            [this](int angle)
            {
                qDebug() << "Угол поворота изменен:" << angle << "градусов";
            });
}
void MainWindow::initBlocksDisplay()
{
    m_blocksScene = new QGraphicsScene(this);
    m_blocksView = new QGraphicsView(m_blocksScene);
    m_blocksView->setFixedSize(200, 200);
    m_blocksView->setRenderHint(QPainter::Antialiasing);

    for (int i = 0; i < 3; ++i)
    {
        m_blockItems[i] = new QGraphicsRectItem(0, 0, 150, 40);
        m_blockItems[i]->setPos(25, 10 + i * 50);
        m_blockItems[i]->setPen(QPen(Qt::black, 1));
        m_blocksScene->addItem(m_blockItems[i]);

        m_blockLabels[i] = new QGraphicsTextItem(QString("Блок %1").arg(i + 1));
        m_blockLabels[i]->setPos(30, 15 + i * 50);
        m_blocksScene->addItem(m_blockLabels[i]);

        QPushButton *btn = new QPushButton();
        btn->setProperty("blockIndex", i);
        btn->setFixedSize(20, 20);
        btn->move(130, 20 + i * 50);
        connect(btn, &QPushButton::clicked, [this, i]()
                { setBlockState(i, !m_system->IsBlockActive(i)); });

        m_blocksScene->addWidget(btn);
    }
}

void MainWindow::updateBlocksDisplay()
{
    if (!m_system || !m_blockItems[0] || !m_blockLabels[0])
        return;

    for (int i = 0; i < 3; ++i)
    {
        QColor blockColor;
        if (m_system->GetStatus() != Status::on)
        {
            blockColor = m_darkTheme ? Qt::darkGray : Qt::lightGray;
        }
        else
        {
            blockColor = m_system->IsBlockActive(i) ? Qt::green : Qt::red;
        }

        if (m_blockItems[i])
        {
            m_blockItems[i]->setBrush(QBrush(blockColor));
        }

        if (m_blockLabels[i])
        {
            QColor textColor = m_darkTheme ? Qt::white : Qt::black;
            m_blockLabels[i]->setDefaultTextColor(textColor);
        }
    }
}

void MainWindow::createMenuBar()
{
    QMenuBar *menuBar = new QMenuBar(this);

    QMenu *viewMenu = menuBar->addMenu("Вид");
    QAction *themeAction = new QAction("Переключить тему", this);
    connect(themeAction, &QAction::triggered, this, &MainWindow::toggleTheme);
    viewMenu->addAction(themeAction);

    QMenu *dataMenu = menuBar->addMenu("Данные");
    QAction *inputDataAction = new QAction("Открыть окно входных данных", this);
    connect(inputDataAction, &QAction::triggered, this, &MainWindow::openInputDataWindow);
    dataMenu->addAction(inputDataAction);

    QAction *saveDataAction = new QAction("Сохранить", this);
    connect(saveDataAction, &QAction::triggered, this, &MainWindow::SaveSystemData);
    dataMenu->addAction(saveDataAction);

    setMenuBar(menuBar);
}

void MainWindow::applyTheme()
{
    if (m_darkTheme)
    {
        qApp->setStyleSheet(
            "QMainWindow { background-color: #252525; }"
            "QWidget { background-color: #252525; color: #e0e0e0; }"
            "QGroupBox { "
            "   border: 1px solid #555555; "
            "   margin-top: 1em; "
            "   padding-top: 10px; "
            "}"
            "QGroupBox::title { "
            "   subcontrol-origin: margin; "
            "   left: 10px; "
            "   padding: 0 3px; "
            "   color: #ffffff; "
            "}"
            "QPushButton { "
            "   background-color: #505050; "
            "   color: white; "
            "   border: 1px solid #606060; "
            "   padding: 5px; "
            "   min-width: 80px; "
            "}"
            "QPushButton:hover { background-color: #606060; }"
            "QPushButton:pressed { background-color: #404040; }"
            "QMenuBar { background-color: #353535; }"
            "QMenuBar::item { background: transparent; color: white; }"
            "QMenuBar::item:selected { background: #505050; }"
            "QMenu { background-color: #353535; color: white; }"
            "QMenu::item:selected { background-color: #505050; }");
    }
    else
    {
        qApp->setStyleSheet(
            "QMainWindow { background-color: #f5f5f5; }"
            "QWidget { background-color: #f5f5f5; color: #333333; }"
            "QGroupBox { "
            "   border: 1px solid #cccccc; "
            "   margin-top: 1em; "
            "   padding-top: 10px; "
            "}"
            "QGroupBox::title { "
            "   subcontrol-origin: margin; "
            "   left: 10px; "
            "   padding: 0 3px; "
            "   color: #000000; "
            "}"
            "QPushButton { "
            "   background-color: #e0e0e0; "
            "   color: black; "
            "   border: 1px solid #cccccc; "
            "   padding: 5px; "
            "   min-width: 80px; "
            "}"
            "QPushButton:hover { background-color: #d0d0d0; }"
            "QPushButton:pressed { background-color: #c0c0c0; }"
            "QMenuBar { background-color: #e8e8e8; }"
            "QMenuBar::item { background: transparent; color: black; }"
            "QMenuBar::item:selected { background: #d0d0d0; }"
            "QMenu { background-color: #e8e8e8; color: black; }"
            "QMenu::item:selected { background-color: #d0d0d0; }");
    }
}

void MainWindow::updatePowerButton()
{
    bool isOn = (m_system->GetStatus() == Status::on);
    powerButton->setChecked(isOn);
    powerButton->setStyleSheet(
        QString("QPushButton {"
                "   border-radius: 15px;"
                "   border: 2px solid %1;"
                "   background-color: %1;"
                "}"
                "QPushButton:pressed {"
                "   background-color: %2;"
                "}")
            .arg(isOn ? "#4CAF50" : "#F44336")
            .arg(isOn ? "#388E3C" : "#D32F2F"));
    powerButton->setToolTip(isOn ? "Выключить систему" : "Включить систему");
}

QGroupBox *MainWindow::createGroupBox(const QString &title, QWidget *content)
{
    auto *groupBox = new QGroupBox(title);
    auto *layout = new QVBoxLayout(groupBox);
    layout->addWidget(content);
    return groupBox;
}