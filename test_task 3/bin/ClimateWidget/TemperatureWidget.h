#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QString>
#include <QList>

#include "SystemsConditioning.h"

class TemperatureWidget : public QWidget
{
    Q_OBJECT
public:
    TemperatureWidget(SystemsConditioning *system, QWidget *parent = nullptr);
    void update(Temperature temp_);
    ~TemperatureWidget();
signals:
    void Signalupdate(Temperature temp_);
private slots:
    void increaseTemperature();
    void decreaseTemperature();
    void changeTemperatureUnit(int index);

private:
    void setupUI();
    void connectSignals();

    SystemsConditioning *m_system = nullptr;
    QComboBox *tempUnitCombo;
    QLabel *tempValueLabel;
};