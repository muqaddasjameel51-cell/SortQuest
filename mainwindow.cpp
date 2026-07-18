#include "mainwindow.h"
#include "welcomewindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QElapsedTimer>
#include <QDateTime>
#include <QRandomGenerator>
#include <algorithm>

const QStringList MainWindow::ALGO_NAMES = {"Bubble Sort","Selection Sort","Insertion Sort","Quick Sort","Merge Sort"};
const QStringList MainWindow::ALGO_KEYS  = {"bubble","selection","insertion","quick","merge"};
const QStringList MainWindow::ALGO_DESCS = {
    " Repeatedly swaps adjacent elements if out of order.",
    "Finds minimum element and places it at the front.",
    "Builds a sorted array one element at a time.",
    "Divides around a pivot and conquers recursively.",
    "Splits in half, sorts each half, then merges."
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    applyDarkTheme();
    buildUI();
    clockTimer = new QTimer(this);
    clockTimer->setInterval(100);
    connect(clockTimer, &QTimer::timeout, this, &MainWindow::tickTimer);
    shuffleArray();
}

MainWindow::~MainWindow() {
    if (workerThread) { workerThread->quit(); workerThread->wait(); }
}

void MainWindow::applyDarkTheme() {
    qApp->setStyle("Fusion");
    QPalette p;
    p.setColor(QPalette::Window,          QColor("#F2F1ED"));
    p.setColor(QPalette::WindowText,      QColor("#3A2A1A"));
    p.setColor(QPalette::Base,            QColor("#F7DDE8"));
    p.setColor(QPalette::AlternateBase,   QColor("#FFCCE1"));
    p.setColor(QPalette::Text,            QColor("#3A2A1A"));
    p.setColor(QPalette::Button,          QColor("#F7DDE8"));
    p.setColor(QPalette::ButtonText,      QColor("#3A2A1A"));
    p.setColor(QPalette::Highlight,       QColor("#FFCCE1"));
    p.setColor(QPalette::HighlightedText, QColor("#7A2040"));
    p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#C0A0B0"));
    qApp->setPalette(p);
}

void MainWindow::buildUI() {
    setWindowTitle("Sorting Visualizer - DSA Project");
    setFixedSize(1100,650);

    QWidget *root = new QWidget(this);
    setCentralWidget(root);
    QVBoxLayout *rootLayout = new QVBoxLayout(root);
    rootLayout->setContentsMargins(16, 12, 16, 12);
    rootLayout->setSpacing(10);

    // title bar
    QHBoxLayout *titleRow = new QHBoxLayout;

    btnBack = new QPushButton("Back");
    btnBack->setFixedSize(90,35);
    btnBack->setStyleSheet(
        "QPushButton{"
        "background:#C6E2E7;"
        "color:#1A4A55;"
        "border-radius:8px;"
        "font-weight:bold;"
        "border:1px solid #A8D0D8;"
        "}"
        "QPushButton:hover{"
        "background:#A8D0D8;"
        "}"
        );
    connect(btnBack, &QPushButton::clicked, this, &MainWindow::onBack);

    QLabel *title = new QLabel("Sorting Visualizer");
    title->setStyleSheet(
        "font-size:20px;"
        "font-weight:700;"
        "color:#B06090;"
        );

    titleRow->addWidget(btnBack);
    titleRow->addSpacing(20);
    titleRow->addWidget(title);
    titleRow->addStretch();
    rootLayout->addLayout(titleRow);

    // control strip
    QFrame *ctrlFrame = new QFrame;
    ctrlFrame->setStyleSheet(
        "QFrame{background:#FFCCE1;border-radius:10px;border:1px solid #F7B8D0;}"
        );
    QHBoxLayout *ctrl = new QHBoxLayout(ctrlFrame);
    ctrl->setContentsMargins(14,10,14,10);
    ctrl->setSpacing(10);

    algoBox = new QComboBox;
    for (auto &n : ALGO_NAMES) algoBox->addItem(n);
    algoBox->setFixedWidth(148);
    algoBox->setStyleSheet(
        "QComboBox{background:#F2F1ED;border:1px solid #F7B8D0;border-radius:6px;"
        "padding:4px 10px;font-size:13px;color:#3A2A1A;}"
        "QComboBox::drop-down{border:none;}"
        "QComboBox QAbstractItemView{background:#F2F1ED;color:#3A2A1A;selection-background-color:#FFCCE1;}"
        );
    connect(algoBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onAlgoChanged);

    nBox = new QComboBox;
    for (int n : {6,8,10,12,16}) nBox->addItem(QString("%1 elements").arg(n), n);
    nBox->setCurrentIndex(3);
    nBox->setFixedWidth(120);
    nBox->setStyleSheet(algoBox->styleSheet());
    connect(nBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onNChanged);

    QLabel *spdLbl = new QLabel("Speed:");
    spdLbl->setStyleSheet("color:#9A6070;font-size:12px;");

    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(1, 10);
    speedSlider->setValue(6);
    speedSlider->setFixedWidth(100);
    speedSlider->setStyleSheet(
        "QSlider::groove:horizontal{height:4px;background:#F7B8D0;border-radius:2px;}"
        "QSlider::handle:horizontal{width:14px;height:14px;margin:-5px 0;"
        "background:#B06090;border-radius:7px;}"
        "QSlider::sub-page:horizontal{background:#B06090;border-radius:2px;}"
        );

    lblSpeed = new QLabel("6");
    lblSpeed->setStyleSheet("color:#B06090;font-size:12px;min-width:14px;");
    connect(speedSlider, &QSlider::valueChanged, this, &MainWindow::onSpeedChanged);

    auto makeBtn = [](const QString &txt, const QString &bg, const QString &textcol) {
        QPushButton *b = new QPushButton(txt);
        b->setFixedHeight(34);
        b->setStyleSheet(QString(
                             "QPushButton{background:%1;border:none;border-radius:7px;"
                             "font-size:13px;font-weight:600;color:%2;padding:0 18px;}"
                             "QPushButton:hover{opacity:0.85;}"
                             "QPushButton:disabled{background:#E8D8E0;color:#C0A0B0;}"
                             ).arg(bg).arg(textcol));
        return b;
    };

    btnStart   = makeBtn("Start",   "#CDE5D9", "#0A3A20");
    btnShuffle = makeBtn("Shuffle", "#FFCCE1", "#7A2040");
    btnStop    = makeBtn("Stop",    "#C6E2E7", "#1A4A55");
    btnStop->setEnabled(false);

    connect(btnStart,   &QPushButton::clicked, this, &MainWindow::onStart);
    connect(btnShuffle, &QPushButton::clicked, this, &MainWindow::onShuffle);
    connect(btnStop,    &QPushButton::clicked, this, &MainWindow::onStop);

    ctrl->addWidget(algoBox);
    ctrl->addWidget(nBox);
    ctrl->addWidget(spdLbl);
    ctrl->addWidget(speedSlider);
    ctrl->addWidget(lblSpeed);
    ctrl->addStretch();
    ctrl->addWidget(btnShuffle);
    ctrl->addWidget(btnStart);
    ctrl->addWidget(btnStop);
    rootLayout->addWidget(ctrlFrame);

    lblAlgoInfo = new QLabel(ALGO_DESCS[0]);
    lblAlgoInfo->setStyleSheet("color:#9A6070;font-size:12px;padding:2px 4px;");
    rootLayout->addWidget(lblAlgoInfo);

    stage = new StageWidget(this);
    stage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rootLayout->addWidget(stage, 1);

    QFrame *statsFrame = new QFrame;
    statsFrame->setStyleSheet(
        "QFrame{background:#F7DDE8;border-radius:8px;border:1px solid #F7B8D0;}"
        );
    QHBoxLayout *stats = new QHBoxLayout(statsFrame);
    stats->setContentsMargins(16,8,16,8);
    stats->setSpacing(0);

    auto makeStat = [](const QString &label, QLabel *&valLabel, const QString &color) {
        QWidget *w = new QWidget;
        QVBoxLayout *vl = new QVBoxLayout(w);
        vl->setContentsMargins(0,0,0,0);
        vl->setSpacing(1);
        QLabel *lbl = new QLabel(label);
        lbl->setStyleSheet("color:#B090A0;font-size:11px;");
        valLabel = new QLabel("0");
        valLabel->setStyleSheet(
            QString("color:%1;font-size:22px;font-weight:700;").arg(color)
            );
        vl->addWidget(lbl);
        vl->addWidget(valLabel);
        return w;
    };

    stats->addWidget(makeStat("Comparisons", lblCmp,  "#B06090"));
    stats->addSpacing(32);
    stats->addWidget(makeStat("Swaps",       lblSwp,  "#208060"));
    stats->addSpacing(32);
    stats->addWidget(makeStat("Time",        lblTime, "#206080"));
    stats->addStretch();

    auto leg = [](const QString &lbl, const QString &col) {
        QWidget *w = new QWidget;
        QHBoxLayout *h = new QHBoxLayout(w);
        h->setContentsMargins(0,0,0,0); h->setSpacing(5);
        QLabel *dot = new QLabel;
        dot->setFixedSize(10,10);
        dot->setStyleSheet(QString("background:%1;border-radius:5px;").arg(col));
        QLabel *txt = new QLabel(lbl);
        txt->setStyleSheet("color:#9A7080;font-size:11px;");
        h->addWidget(dot); h->addWidget(txt);
        return w;
    };

    rootLayout->addWidget(statsFrame);
}

void MainWindow::shuffleArray() {
    arr.resize(N);
    for (int i = 0; i < N; i++) arr[i] = i+1;
    for (int i = N-1; i > 0; i--) {
        int j = QRandomGenerator::global()->bounded(i+1);
        std::swap(arr[i], arr[j]);
    }
    stage->setArray(arr);
    cmpCount = swpCount = 0;
    lblCmp->setText("0");
    lblSwp->setText("0");
    lblTime->setText("0.0s");
}

void MainWindow::onAlgoChanged(int index) {
    if (index >= 0 && index < ALGO_DESCS.size())
        lblAlgoInfo->setText(ALGO_DESCS[index]);
}

void MainWindow::onNChanged(int) {
    if (sorting) return;
    N = nBox->currentData().toInt();
    stage->setN(N);
    shuffleArray();
}

void MainWindow::onSpeedChanged(int v) {
    lblSpeed->setText(QString::number(v));
    delayMs = 940 - v*90;
    if (worker) worker->setDelay(delayMs);
}

void MainWindow::onShuffle() {
    if (sorting) return;
    shuffleArray();
}

void MainWindow::onStart() {
    if (sorting) return;
    sorting = true;
    cmpCount = swpCount = 0;
    lblCmp->setText("0"); lblSwp->setText("0"); lblTime->setText("0.0s");
    setControlsEnabled(false);

    startMs = QDateTime::currentMSecsSinceEpoch();
    clockTimer->start();

    workerThread = new QThread(this);
    worker       = new SortWork;
    worker->moveToThread(workerThread);
    worker->setDelay(delayMs);

    connect(workerThread, &QThread::finished,  worker, &QObject::deleteLater);
    connect(worker, &SortWork::onCompare,    this,   &MainWindow::onCompare);
    connect(worker, &SortWork::onSwap,       this,   &MainWindow::onSwap);
    connect(worker, &SortWork::onResetState, this,   &MainWindow::onResetState);
    connect(worker, &SortWork::onMarkSorted, this,   &MainWindow::onMarkSorted);
    connect(worker, &SortWork::onFinished,   this,   &MainWindow::onSortFinished);

    connect(worker, &SortWork::onCompare,    stage, &StageWidget::onCompare);
    connect(worker, &SortWork::onSwap,       stage, &StageWidget::onSwap);
    connect(worker, &SortWork::onResetState, stage, &StageWidget::onResetState);
    connect(worker, &SortWork::onMarkSorted, stage, &StageWidget::onMarkSorted);
    connect(worker, &SortWork::onFinished,   stage, &StageWidget::onFinished);

    workerThread->start();

    QString key = ALGO_KEYS[algoBox->currentIndex()];
    QMetaObject::invokeMethod(worker, "runSort",
                              Qt::QueuedConnection,
                              Q_ARG(QVector<int>, arr),
                              Q_ARG(QString, key));
}

void MainWindow::onStop() {
    if (!sorting) return;
    if (worker)       worker->stop();
    if (workerThread) { workerThread->quit(); workerThread->wait(); workerThread = nullptr; worker = nullptr; }
    clockTimer->stop();
    sorting = false;
    setControlsEnabled(true);
}

void MainWindow::onSortFinished() {
    clockTimer->stop();
    qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - startMs;
    lblTime->setText(QString::number(elapsed / 1000.0, 'f', 1) + "s");
    if (workerThread) { workerThread->quit(); workerThread->wait(); workerThread = nullptr; worker = nullptr; }
    sorting = false;
    setControlsEnabled(true);
}

void MainWindow::onCompare(int, int) { cmpCount++; lblCmp->setText(QString::number(cmpCount)); }
void MainWindow::onSwap(int, int, int, int) { swpCount++; lblSwp->setText(QString::number(swpCount)); }
void MainWindow::onResetState(int) {}
void MainWindow::onMarkSorted(int) {}

void MainWindow::tickTimer() {
    if (!sorting) return;
    qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - startMs;
    lblTime->setText(QString::number(elapsed / 1000.0, 'f', 1) + "s");
}

void MainWindow::setControlsEnabled(bool on) {
    btnStart->setEnabled(on);
    btnShuffle->setEnabled(on);
    btnStop->setEnabled(!on);
    algoBox->setEnabled(on);
    nBox->setEnabled(on);
}

void MainWindow::onBack()
{
    WelcomeWindow *w = new WelcomeWindow;
    w->show();
    close();
}