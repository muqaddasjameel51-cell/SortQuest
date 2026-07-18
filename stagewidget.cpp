#include "stagewidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <cmath>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const char* RANKS[] = {
    "","A","2","3","4","5","6","7","8","9","10","J","Q","K","A"
};

struct Theme { QColor bg, border, fg, shadow; };
static Theme themeFor(const QString &state) {
    if (state=="comparing") return {QColor("#F2EBCC"),QColor("#C8A800"),QColor("#5A4000"),QColor("#C8A800")};
    if (state=="swapping")  return {QColor("#FFCCE1"),QColor("#D04070"),QColor("#6A0020"),QColor("#D04070")};
    if (state=="sorted")    return {QColor("#C6E2E7"),QColor("#2090A0"),QColor("#004050"),QColor("#2090A0")};
    return {QColor("#CDE5D9"),QColor("#3AAA70"),QColor("#0A4020"),QColor("#3AAA70")};
}

StageWidget::StageWidget(QWidget *parent) : QWidget(parent) {
    setMinimumHeight(340);
    setMinimumWidth(400);
    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &StageWidget::tick);
    animTimer->start(15);
    particleTimer = new QTimer(this);
    connect(particleTimer, &QTimer::timeout, this, &StageWidget::tickParticles);
    particleTimer->start(15);
}

int StageWidget::xFor(int i) {
    if (N <= 1) return width()/2 - 28;
    int margin = qMax(10, (width() - N*58) / 2);
    return margin + i * (width() - 2*margin) / (N-1) - 28;
}
int StageWidget::yBase() { return height() - 68 - 20; }
int StageWidget::yJump() { return height() - 68*3 - 10; }

float StageWidget::noteFor(int value) {
    float scale[] = {262,294,330,349,392,440,494,523,587,659,698,784,880,988,1047};
    int count = 15;
    int idx = qBound(0, (int)((value-1.0f)/(qMax(N,2)-1.0f)*(count-1)), count-1);
    return scale[idx];
}

void StageWidget::playTone(float freq, int ms, float vol) {
    QAudioFormat fmt;
    fmt.setSampleRate(44100);
    fmt.setChannelCount(1);
    fmt.setSampleFormat(QAudioFormat::Int16);
    int samples = 44100 * ms / 1000;
    QByteArray buf(samples * 2, 0);
    short *d = reinterpret_cast<short*>(buf.data());
    for (int i = 0; i < samples; i++) {
        float t   = float(i)/44100.f;
        float env = 1.f - float(i)/float(samples);
        float w   = sinf(2.f*M_PI*freq*t)
                  + 0.3f*sinf(4.f*M_PI*freq*t)
                  + 0.08f*sinf(6.f*M_PI*freq*t);
        d[i] = short(26000*vol*env*w*0.5f);
    }
    auto *sink = new QAudioSink(fmt, this);
    auto *buf2 = new QBuffer(this);
    buf2->setData(buf); buf2->open(QIODevice::ReadOnly);
    sink->start(buf2);
    QTimer::singleShot(ms+60, this, [sink,buf2]{ sink->stop(); delete sink; delete buf2; });
}

void StageWidget::setArray(const QVector<int> &arr) {
    N = arr.size();
    cards.clear();
    showBanner  = false;
    bannerAlpha = 0.f;
    bannerScale = 0.3f;
    bannerTick  = 0;
    particles.clear();
    for (int i = 0; i < N; i++) {
        CardState c;
        c.value   = arr[i];
        c.state   = "idle";
        c.drawX   = c.targetX = xFor(i);
        c.drawY   = c.targetY = yBase();
        c.bobOffset = 0; c.bobPhase = i*8;
        cards.append(c);
    }
    update();
}

void StageWidget::onCompare(int i, int j) {
    if (i<cards.size()){ cards[i].state="comparing"; cards[i].targetY=yJump()+40; }
    if (j<cards.size()){ cards[j].state="comparing"; cards[j].targetY=yJump()+40; }
    if (i<cards.size()) playTone(noteFor(cards[i].value), 75, 0.18f);
    update();
}

void StageWidget::onSwap(int i, int j, int valI, int valJ) {
    if (i>=cards.size()||j>=cards.size()) return;
    cards[i].value = valJ;
    cards[j].value = valI;
    cards[i].state = "swapping"; cards[j].state = "swapping";
    cards[i].targetY = yJump();
    cards[j].targetY = yJump();
    float tx = cards[i].targetX;
    cards[i].targetX = cards[j].targetX;
    cards[j].targetX = tx;
    playTone(noteFor(valI), 120, 0.32f);
    QTimer::singleShot(65, this, [this,valJ]{ playTone(noteFor(valJ)*1.5f,120,0.32f); });
    spawnBurst(cards[i].drawX+28, yBase()+34, QColor("#D04070"), 10);
    spawnBurst(cards[j].drawX+28, yBase()+34, QColor("#D04070"), 10);
    QTimer::singleShot(220, this, [this,i,j]{
        if(i<cards.size()) cards[i].targetY = yBase();
        if(j<cards.size()) cards[j].targetY = yBase();
    });
    update();
}

void StageWidget::onResetState(int i) {
    if (i<cards.size() && cards[i].state!="sorted") {
        cards[i].state   = "idle";
        cards[i].targetY = yBase();
        cards[i].targetX = xFor(i);
    }
    update();
}

void StageWidget::onMarkSorted(int i) {
    if (i>=cards.size()) return;
    cards[i].state   = "sorted";
    cards[i].targetY = yBase();
    cards[i].targetX = xFor(i);
    playTone(noteFor(cards[i].value), 160, 0.22f);
    spawnBurst(xFor(i)+28, yBase()+34, QColor("#2090A0"), 12);
    update();
}

void StageWidget::onFinished() {
    float notes[] = {523,659,784,1047,1319};
    for (int k=0;k<5;k++)
        QTimer::singleShot(k*90, this, [this,notes,k]{ playTone(notes[k],220,0.28f); });
    for (int k=0;k<8;k++) {
        QTimer::singleShot(k*60, this, [this,k]{
            spawnBurst(width()*0.5f, height()*0.5f, QColor("#FFCCE1"), 14);
            spawnStars(width()*0.5f, height()*0.5f);
        });
    }
    QTimer::singleShot(350, this, [this]{
        showBanner  = true;
        bannerAlpha = 0.f;
        bannerScale = 0.3f;
    });
    update();
}

void StageWidget::tick() {
    bool dirty = false;
    for (auto &c : cards) {
        float dx = c.targetX - c.drawX;
        float dy = c.targetY - c.drawY;
        float spd = 0.18f;
        if (fabsf(dx)>0.5f){ c.drawX += dx*spd; dirty=true; } else c.drawX=c.targetX;
        if (fabsf(dy)>0.5f){ c.drawY += dy*spd; dirty=true; } else c.drawY=c.targetY;
        if (c.state=="sorted") {
            c.bobPhase++;
            c.bobOffset = sinf(c.bobPhase * 0.07f) * 3.5f;
            dirty = true;
        } else {
            c.bobOffset = 0;
        }
    }
    if (showBanner) {
        bannerTick++;
        if (bannerAlpha < 1.f){ bannerAlpha = qMin(1.f, bannerAlpha+0.055f); dirty=true; }
        if (bannerScale< 1.f){ bannerScale = qMin(1.f, bannerScale+0.07f);  dirty=true; }
    }
    if (dirty) update();
}

void StageWidget::tickParticles() {
    bool any = false;
    for (auto &p : particles) {
        p.x+=p.vx; p.y+=p.vy; p.vy+=0.18f; p.life-=0.032f;
        if(p.life>0) any=true;
    }
    particles.removeIf([](const Particle &p){ return p.life<=0; });
    if(any) update();
}

void StageWidget::spawnBurst(float cx, float cy, QColor col, int n) {
    for (int i=0;i<n;i++) {
        Particle p;
        float a = (rand()%360)*M_PI/180.f;
        float s = 2.f+(rand()%40)/10.f;
        p.x=cx; p.y=cy; p.vx=cosf(a)*s; p.vy=sinf(a)*s-2.5f;
        p.life=1.f; p.r=2+rand()%4; p.color=col;
        particles.append(p);
    }
}

void StageWidget::spawnStars(float cx, float cy) {
    QColor cols[] = {QColor("#FFCCE1"),QColor("#CDE5D9"),QColor("#F2EBCC"),QColor("#C6E2E7"),QColor("#F7DDE8")};
    for (int i=0;i<20;i++) {
        Particle p;
        float a = (rand()%360)*M_PI/180.f;
        float s = 3.f+(rand()%60)/10.f;
        p.x=cx+(rand()%200-100); p.y=cy+(rand()%100-50);
        p.vx=cosf(a)*s; p.vy=sinf(a)*s-4.f;
        p.life=1.f; p.r=3+rand()%5; p.color=cols[rand()%5];
        particles.append(p);
    }
}

void StageWidget::drawBackground(QPainter &p) {
    p.fillRect(rect(), QColor("#F2F1ED"));
    p.setPen(QPen(QColor(180,140,160,30), 1));
    for (int x=0; x<width(); x+=40)
        p.drawLine(x,0,x,height());
    for (int y=0; y<height(); y+=40)
        p.drawLine(0,y,width(),y);
    p.setPen(QPen(QColor(180,140,160,60), 1));
    p.drawLine(0, yBase()+68+4, width(), yBase()+68+4);
}

void StageWidget::drawCard(QPainter &p, int idx) {
    const CardState &c = cards[idx];
    Theme t = themeFor(c.state);

    int x = (int)c.drawX;
    int y = (int)(c.drawY + c.bobOffset);
    int W = 56, H = 68;

    p.setBrush(QColor(180,140,160,40));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(x+4, y+6, W, H, 12, 12);

    if (c.state != "idle") {
        QColor glow = t.shadow; glow.setAlpha(80);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(glow, 8));
        p.drawRoundedRect(x-2, y-2, W+4, H+4, 14, 14);
    }

    p.setBrush(t.bg);
    p.setPen(QPen(t.border, 2.5));
    p.drawRoundedRect(x, y, W, H, 12, 12);

    p.setPen(t.fg);
    QFont small = p.font(); small.setPointSize(8); small.setBold(true); p.setFont(small);
    if (c.value>=1 && c.value<=14)
        p.drawText(QRect(x+4,y+3,20,14), Qt::AlignLeft, QString(RANKS[c.value]));

    QFont big = p.font(); big.setPointSize(16); big.setBold(true); p.setFont(big);
    p.drawText(QRect(x, y+14, W, 36), Qt::AlignCenter, QString::number(c.value));

    QFont sm2 = p.font(); sm2.setPointSize(8); sm2.setBold(false); p.setFont(sm2);
    if (c.value>=1 && c.value<=14)
        p.drawText(QRect(x, y+H-16, W-4, 14), Qt::AlignRight, QString(RANKS[c.value]));
}

void StageWidget::drawBanner(QPainter &p) {
    int cx = width()/2, cy = height()/2 - 20;
    int bw = (int)(360*bannerScale), bh = (int)(120*bannerScale);

    p.setOpacity(bannerAlpha * 0.95f);

    QPainterPath ring;
    ring.addRoundedRect(cx-bw/2-8, cy-bh/2-8, bw+16, bh+16, 24, 24);
    p.fillPath(ring, QColor("#FFCCE1"));

    QPainterPath card;
    card.addRoundedRect(cx-bw/2, cy-bh/2, bw, bh, 18, 18);
    p.fillPath(card, QColor("#F2F1ED"));

    p.setPen(QColor("#B06090"));
    QFont f; f.setPointSize((int)(26*bannerScale)); f.setBold(true); p.setFont(f);
    p.drawText(QRect(cx-bw/2, cy-bh/2, bw, (int)(bh*0.65f)), Qt::AlignCenter, "SORTED!");

    p.setPen(QColor("#9A6070"));
    QFont f2; f2.setPointSize((int)(9*bannerScale)); f2.setBold(false); p.setFont(f2);
    p.drawText(QRect(cx-bw/2, cy+bh/2-(int)(32*bannerScale), bw, (int)(28*bannerScale)),
               Qt::AlignCenter, "Press Shuffle to play again");

    p.setOpacity(1.0);
}

void StageWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawBackground(painter);
    for (int i = 0; i < cards.size(); i++)
        drawCard(painter, i);
    for (const auto &pt : particles) {
        painter.setOpacity(pt.life);
        painter.setBrush(pt.color);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(pt.x, pt.y), pt.r, pt.r);
    }
    painter.setOpacity(1.0);
    if (showBanner)
        drawBanner(painter);
}

void StageWidget::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
    for (int i=0; i<cards.size(); i++) {
        if (cards[i].state=="idle"||cards[i].state=="sorted") {
            cards[i].targetX = xFor(i);
            cards[i].drawX   = xFor(i);
        }
    }
    update();
}