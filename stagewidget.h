#pragma once
#include <QWidget>
#include <QVector>
#include <QTimer>
#include <QAudioSink>
#include <QAudioFormat>
#include <QBuffer>

struct Particle {
    float x, y, vx, vy, life, r;
    QColor color;
};

struct CardState {
    int   value;
    QString state;   // idle, comparing, swapping, sorted
    float drawX, drawY;
    float targetX, targetY;
    float bobOffset;   // used for sorted wave
    int   bobPhase;
};

class StageWidget : public QWidget {
    Q_OBJECT
public:
    explicit StageWidget(QWidget *parent = nullptr);
    void setArray(const QVector<int> &arr);
    void setN(int n) { N = n; }

public slots:
    void onCompare(int i, int j);
    void onSwap(int i, int j, int valI, int valJ);
    void onResetState(int i);
    void onMarkSorted(int i);
    void onFinished();

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;

private:
    int N = 12;
    QVector<CardState> cards;
    QVector<Particle>  particles;

    QTimer *animTimer;
    QTimer *particleTimer;

    bool  showBanner    = false;
    float bannerAlpha   = 0.0f;
    float bannerScale   = 0.3f;
    int   bannerTick    = 0;

    int xFor(int i);
    int yBase();
    int yJump();

    void spawnBurst(float cx, float cy, QColor col, int n = 10);
    void spawnStars(float cx, float cy);
    void tick();
    void tickParticles();

    void drawCard(QPainter &p, int idx);
    void drawBanner(QPainter &p);
    void drawBackground(QPainter &p);

    void playTone(float freq, int ms = 110, float vol = 0.28f);
    float noteFor(int value);
};