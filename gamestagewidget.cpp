#include "gamestagewidget.h"
#include <QPainter>
#include <QFont>
#include <QTimer>

GameStageWidget::GameStageWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(220);
    animationTimer = new QTimer(this);
    animationTimer->setInterval(25);
    connect(animationTimer, &QTimer::timeout, this, [=](){
        moveOffset += 8;
        if(moveOffset >= 90){
            moveOffset = 90;
            animating  = false;
            animationTimer->stop();
        }
        update();
    });
}

void GameStageWidget::setArray(const QVector<int> &values)
{
    numbers = values;
    update();
}

void GameStageWidget::highlightCompare(int i, int j)
{
    compareIndex1 = i;
    compareIndex2 = j;
    update();
}

void GameStageWidget::animateSwap(int i, int j)
{
    swapIndex1 = i;
    swapIndex2 = j;
    animating  = true;
    moveOffset = 0;
    animationTimer->start();
}

void GameStageWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QColor("#F2F1ED"));

    if(numbers.isEmpty()) return;

    int cardWidth  = 70;
    int cardHeight = 90;
    int spacing    = 20;
    int totalWidth = numbers.size()*cardWidth + (numbers.size()-1)*spacing;
    int startX     = (width()  - totalWidth) / 2;
    int y          = (height() - cardHeight) / 2;

    QFont font;
    font.setPointSize(18);
    font.setBold(true);
    painter.setFont(font);

    for(int i = 0; i < numbers.size(); i++)
    {
        int xx = startX;
        int yy = y;

        if(animating){
            if(i == swapIndex1) xx += moveOffset;
            if(i == swapIndex2) xx -= moveOffset;
        }

        QRect card(xx, yy, cardWidth, cardHeight);

        if(i == compareIndex1 || i == compareIndex2){
            // comparing → soft yellow
            painter.setBrush(QColor("#F2EBCC"));
            painter.setPen(QPen(QColor("#C8A800"), 3));
        } else {
            // idle → mint green
            painter.setBrush(QColor("#CDE5D9"));
            painter.setPen(QPen(QColor("#3AAA70"), 2));
        }

        painter.drawRoundedRect(card, 12, 12);
        painter.setPen(QColor("#3A2A1A"));
        painter.drawText(card, Qt::AlignCenter, QString::number(numbers[i]));

        startX += cardWidth + spacing;
    }
}