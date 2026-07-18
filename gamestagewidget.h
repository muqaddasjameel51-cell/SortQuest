#ifndef GAMESTAGEWIDGET_H
#define GAMESTAGEWIDGET_H
#include <QWidget>
#include <QVector>
#include <QTimer>

class GameStageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameStageWidget(QWidget *parent = nullptr);

    void setArray(const QVector<int> &values);

    void highlightCompare(int i, int j);

    void animateSwap(int i, int j);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<int> numbers;
    int compareIndex1 = -1;
    int compareIndex2 = -1;
    QTimer *animationTimer;

    bool animating = false;

    int swapIndex1 = -1;
    int swapIndex2 = -1;

    int moveOffset = 0;
};

#endif