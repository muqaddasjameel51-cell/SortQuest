#pragma once
#include <QMainWindow>
#include <QThread>
#include <QVector>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QTimer>
#include "stagewidget.h"
#include "sortwork.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStart();
    void onShuffle();
    void onStop();
    void onSortFinished();
    void onAlgoChanged(int index);
    void onNChanged(int index);
    void onSpeedChanged(int value);
    void tickTimer();

    void onCompare(int i, int j);
    void onSwap(int i, int j, int vi, int vj);
    void onResetState(int i);
    void onMarkSorted(int i);
    void onBack();

private:
    void buildUI();
    void applyDarkTheme();
    void shuffleArray();
    void updateCounters();
    void setControlsEnabled(bool on);
    QPushButton *btnBack;

    // UI
    StageWidget   *stage;
    QPushButton   *btnStart, *btnShuffle, *btnStop;
    QComboBox     *algoBox, *nBox;
    QSlider       *speedSlider;
    QLabel        *lblSpeed, *lblCmp, *lblSwp, *lblTime, *lblAlgoInfo;

    // Worker thread
    QThread       *workerThread = nullptr;
    SortWork    *worker       = nullptr;

    // State
    QVector<int>   arr;
    int            N           = 12;
    int            delayMs     = 180;
    bool           sorting     = false;
    int            cmpCount    = 0;
    int            swpCount    = 0;
    qint64         startMs     = 0;
    QTimer        *clockTimer;

    static const QStringList ALGO_NAMES;
    static const QStringList ALGO_KEYS;
    static const QStringList ALGO_DESCS;
};