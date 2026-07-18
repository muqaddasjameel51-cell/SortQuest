#ifndef BUBBLEGAMEENGINE_H
#define BUBBLEGAMEENGINE_H

#include <QObject>
#include <QVector>

class BubbleGameEngine : public QObject
{
    Q_OBJECT

public:
    explicit BubbleGameEngine(QObject *parent = nullptr);

    void setArray(const QVector<int> &arr);

    void start();

    bool submitAnswer(bool userSaysSwap);

    // Called after animation finishes
    void animationFinished();

signals:

    void compareChanged(int i, int j);

    // NEW: Tell UI that a swap animation is needed
    void swapRequired(int i, int j);

    void arrayChanged(QVector<int> arr);

    void levelCompleted();

private:

    QVector<int> numbers;

    int i = 0;
    int j = 0;
};

#endif