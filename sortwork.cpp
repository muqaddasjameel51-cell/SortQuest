#include "sortwork.h"
#include <QThread>
#include <algorithm>

SortWork::SortWork(QObject *parent) : QObject(parent) {}

void SortWork::pause() { QThread::msleep(delay); }

void SortWork::doSwap(int i, int j) {
    emit onSwap(i, j, arr[i], arr[j]);
    std::swap(arr[i], arr[j]);
    pause();
    emit onResetState(i);
    emit onResetState(j);
}

void SortWork::doCompare(int i, int j) {
    emit onCompare(i, j);
    pause();
}

void SortWork::runSort(QVector<int> a, QString algo) {
    arr = a;
    running = true;
    if      (algo == "bubble")    bubbleSort();
    else if (algo == "selection") selectionSort();
    else if (algo == "insertion") insertionSort();
    else if (algo == "quick")     quickSort(0, arr.size()-1);
    else if (algo == "merge")     mergeSort(0, arr.size()-1);
    if (running) {
        // mark all remaining unsorted cards as sorted
        for (int i = 0; i < arr.size(); i++)
            emit onMarkSorted(i);
        QThread::msleep(400);
        emit onFinished();
    }
}

void SortWork::bubbleSort() {
    int n = arr.size();
    for (int i = 0; i < n && running; i++) {
        for (int j = 0; j < n-i-1 && running; j++) {
            doCompare(j, j+1);
            if (arr[j] > arr[j+1])
                doSwap(j, j+1);
            else {
                emit onResetState(j);
                emit onResetState(j+1);
            }
        }
        emit onMarkSorted(n-i-1);
    }
}

void SortWork::selectionSort() {
    int n = arr.size();
    for (int i = 0; i < n && running; i++) {
        int mn = i;
        for (int j = i+1; j < n && running; j++) {
            doCompare(mn, j);
            if (arr[j] < arr[mn]) mn = j;
            emit onResetState(j);
        }
        if (mn != i) doSwap(i, mn);
        emit onMarkSorted(i);
    }
}

void SortWork::insertionSort() {
    int n = arr.size();
    emit onMarkSorted(0);
    for (int i = 1; i < n && running; i++) {
        int j = i;
        while (j > 0 && running) {
            doCompare(j-1, j);
            if (arr[j] < arr[j-1]) { doSwap(j-1, j); j--; }
            else { emit onResetState(j-1); emit onResetState(j); break; }
        }
        emit onMarkSorted(j);
    }
}

void SortWork::quickSort(int lo, int hi) {
    if (lo >= hi || !running) return;
    int pivot = arr[hi], k = lo-1;
    for (int j = lo; j < hi && running; j++) {
        doCompare(j, hi);
        if (arr[j] <= pivot) {
            k++;
            if (k != j) doSwap(k, j);
            else { emit onResetState(j); emit onResetState(hi); }
        } else {
            emit onResetState(j);
            emit onResetState(hi);
        }
    }
    if (k+1 != hi) doSwap(k+1, hi);
    int p = k+1;
    emit onMarkSorted(p);
    quickSort(lo, p-1);
    quickSort(p+1, hi);
}

void SortWork::mergeSort(int lo, int hi) {
    if (lo >= hi || !running) return;
    int mid = (lo+hi)/2;
    mergeSort(lo, mid);
    mergeSort(mid+1, hi);
    doMerge(lo, mid, hi);
}
void SortWork::doMerge(int lo, int mid, int hi) {
    QVector<int> L = arr.mid(lo, mid-lo+1);
    QVector<int> R = arr.mid(mid+1, hi-mid);
    int i=0, j=0, k=lo;

    while (i<L.size() && j<R.size() && running) {
        emit onCompare(lo+i, mid+1+j);
        pause();
        if (L[i] <= R[j]) {
            arr[k] = L[i++];
        } else {
            arr[k] = R[j++];
        }
        emit onSwap(k, k, arr[k], arr[k]);
        pause();
        emit onResetState(k);
        k++;
    }
    while (i<L.size() && running) {
        arr[k] = L[i++];
        emit onSwap(k, k, arr[k], arr[k]);
        pause();
        emit onResetState(k);
        k++;
    }
    while (j<R.size() && running) {
        arr[k] = R[j++];
        emit onSwap(k, k, arr[k], arr[k]);
        pause();
        emit onResetState(k);
        k++;
    }
    for (int m=lo; m<=hi; m++)
        emit onMarkSorted(m);
}
