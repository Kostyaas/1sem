#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

class ArrowControlWidget : public QWidget
{
    Q_OBJECT 
public:
    explicit ArrowControlWidget(QWidget *parent = nullptr);
    void setAngle(int angle);

signals: 
    void directionChanged(int angle);

private slots:
    void onLeftClicked();
    void onUpClicked();
    void onRightClicked();

private:
    QPushButton *leftArrow;
    QPushButton *upArrow;
    QPushButton *rightArrow;
    QLabel *angleLabel;
    int currentAngle;

    void initUI();
    void updateAngle(int delta);
};

