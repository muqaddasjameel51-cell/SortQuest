#include "bubblegameengine.h"

BubbleGameEngine::BubbleGameEngine(QObject *parent)
    : QObject(parent)
{

}

void BubbleGameEngine::setArray(const QVector<int> &arr)
{
    numbers = arr;
}

void BubbleGameEngine::start()
{
    i = 0;
    j = 0;

    emit compareChanged(0, 1);
}

bool BubbleGameEngine::submitAnswer(bool userSaysSwap)
{
    bool actualSwap = numbers[j] > numbers[j + 1];

    bool correct = (userSaysSwap == actualSwap);

    // ------------------------
    // Swap Required
    // ------------------------

    if(actualSwap)
    {
        emit swapRequired(j, j + 1);

        return correct;
    }

    // ------------------------
    // No Swap
    // ------------------------

    j++;

    if(j >= numbers.size() - i - 1)
    {
        j = 0;
        i++;
    }

    if(i >= numbers.size() - 1)
    {
        emit levelCompleted();
    }
    else
    {
        emit compareChanged(j, j + 1);
    }

    return correct;
}

void BubbleGameEngine::animationFinished()
{
    numbers.swapItemsAt(j, j + 1);

    emit arrayChanged(numbers);

    j++;

    if(j >= numbers.size() - i - 1)
    {
        j = 0;
        i++;
    }

    if(i >= numbers.size() - 1)
    {
        emit levelCompleted();
    }
    else
    {
        emit compareChanged(j, j + 1);
    }
}