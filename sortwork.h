#pragma once
#include <QObject>
#include <QVector>

class SortWork : public QObject {
    Q_OBJECT
public:
    explicit SortWork(QObject *parent = nullptr);
    void setDelay(int ms) { delay = ms; }
    void stop() { running = false; }

public slots:
    void runSort(QVector<int> arr, QString algo);

signals:
    void onCompare(int i, int j);
    void onSwap(int i, int j, int valI, int valJ);
    void onResetState(int i);
    void onMarkSorted(int i);
    void onFinished();

private:
    int delay = 300;
    bool running = false;
    QVector<int> arr;

    void pause();
    void doSwap(int i, int j);
    void doCompare(int i, int j);
    void bubbleSort();
    void selectionSort();
    void insertionSort();
    void quickSort(int lo, int hi);
    void mergeSort(int lo, int hi);
    void doMerge(int lo, int mid, int hi);
};