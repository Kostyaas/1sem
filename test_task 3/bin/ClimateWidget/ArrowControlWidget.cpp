#include "ArrowControlWidget.h"
#include <QFont>
#include <QDebug>

ArrowControlWidget::ArrowControlWidget(QWidget *parent)
    : QWidget(parent), currentAngle(90) 
{
    initUI();
    setAngle(currentAngle);
}

void ArrowControlWidget::initUI()
{
    leftArrow = new QPushButton("←", this);
    upArrow = new QPushButton("↑", this);
    rightArrow = new QPushButton("→", this);

    QFont arrowFont = leftArrow->font();
    arrowFont.setPointSize(16);
    leftArrow->setFont(arrowFont);
    upArrow->setFont(arrowFont);
    rightArrow->setFont(arrowFont);

    leftArrow->setFixedSize(40, 40);
    upArrow->setFixedSize(40, 40);
    rightArrow->setFixedSize(40, 40);

    angleLabel = new QLabel("Поворот: 90°", this);
    angleLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout *arrowsLayout = new QHBoxLayout;
    arrowsLayout->addWidget(leftArrow);
    arrowsLayout->addWidget(upArrow);
    arrowsLayout->addWidget(rightArrow);
    arrowsLayout->setSpacing(10);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(arrowsLayout);
    mainLayout->addWidget(angleLabel);
    mainLayout->setSpacing(5);

    connect(leftArrow, &QPushButton::clicked, this, &ArrowControlWidget::onLeftClicked);
    connect(upArrow, &QPushButton::clicked, this, &ArrowControlWidget::onUpClicked);
    connect(rightArrow, &QPushButton::clicked, this, &ArrowControlWidget::onRightClicked);
}

void ArrowControlWidget::setAngle(int angle)
{
    currentAngle = qBound(0, angle, 180); 
    angleLabel->setText(QString("Поворот: %1°").arg(currentAngle));
    emit directionChanged(currentAngle);
}

void ArrowControlWidget::updateAngle(int delta)
{
    setAngle(currentAngle + delta);
}

void ArrowControlWidget::onLeftClicked()
{
    updateAngle(-5); 
}

void ArrowControlWidget::onUpClicked()
{
    if (currentAngle < 90)
    {
        updateAngle(5);
    }
    else if (currentAngle > 90)
    {
        updateAngle(-5);
    }
}

void ArrowControlWidget::onRightClicked()
{
    updateAngle(5);
}