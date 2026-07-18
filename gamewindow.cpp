#include "gamewindow.h"
#include "welcomewindow.h"
#include "gameplaywindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QFrame>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Sort Quest");
    setFixedSize(1100,700);

    setStyleSheet(
        "background:#F2F1ED;"
        "color:#3A2A1A;"
        );

    titleLabel = new QLabel("SORT QUEST");
    titleLabel->setAlignment(Qt::AlignCenter);

    QFont titleFont;
    titleFont.setPointSize(28);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color:#B06090;");

    QLabel *iconLabel = new QLabel("* ~ * ~ *");
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size:18px;color:#D04070;");

    QFrame *line = new QFrame;
    line->setFixedHeight(3);
    line->setStyleSheet(
        "QFrame{"
        "border:none;"
        "border-radius:2px;"
        "background:qlineargradient("
        "x1:0,y1:0,x2:1,y2:0,"
        "stop:0 #FFCCE1,"
        "stop:0.33 #CDE5D9,"
        "stop:0.66 #F2EBCC,"
        "stop:1 #C6E2E7);"
        "}"
        );

    modeLabel = new QLabel("Bubble Sort Challenge");
    modeLabel->setAlignment(Qt::AlignCenter);
    modeLabel->setStyleSheet("font-size:20px;font-weight:bold;color:#B06090;");

    difficultyLabel = new QLabel("Difficulty : Easy");
    difficultyLabel->setAlignment(Qt::AlignCenter);
    difficultyLabel->setStyleSheet("font-size:17px;color:#7A5060;");

    highScoreLabel = new QLabel("High Score : 0");
    highScoreLabel->setAlignment(Qt::AlignCenter);
    highScoreLabel->setStyleSheet("font-size:17px;color:#7A5060;");

    startButton = new QPushButton("Start Game");
    backButton  = new QPushButton("Back");

    startButton->setMinimumHeight(55);
    backButton->setMinimumHeight(55);

    startButton->setStyleSheet(
        "QPushButton{"
        "background:#CDE5D9;"
        "color:#0A3A20;"
        "font-size:16px;"
        "font-weight:bold;"
        "padding:12px;"
        "border-radius:10px;"
        "border:2px solid #3AAA70;"
        "}"
        "QPushButton:hover{"
        "background:#3AAA70;"
        "color:white;"
        "}"
        );

    backButton->setStyleSheet(
        "QPushButton{"
        "background:#C6E2E7;"
        "color:#1A4A55;"
        "font-size:16px;"
        "font-weight:bold;"
        "padding:12px;"
        "border-radius:10px;"
        "border:2px solid #2090A0;"
        "}"
        "QPushButton:hover{"
        "background:#2090A0;"
        "color:white;"
        "}"
        );

    QLabel *footer = new QLabel("Interactive Learning Through Visualization");
    footer->setAlignment(Qt::AlignCenter);
    footer->setStyleSheet("font-size:13px;color:#A090A0;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(260,45,260,35);
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addSpacing(8);
    layout->addWidget(iconLabel);
    layout->addSpacing(10);
    layout->addWidget(line);
    layout->addSpacing(25);
    layout->addWidget(modeLabel);
    layout->addSpacing(35);
    layout->addWidget(difficultyLabel);
    layout->addSpacing(12);
    layout->addWidget(highScoreLabel);
    layout->addSpacing(50);
    layout->addWidget(startButton);
    layout->addSpacing(18);
    layout->addWidget(backButton);
    layout->addStretch();
    layout->addWidget(footer);

    connect(startButton, &QPushButton::clicked, this, &GameWindow::startGame);
    connect(backButton,  &QPushButton::clicked, this, &GameWindow::goBack);
}

void GameWindow::startGame()
{
    GamePlayWindow *game = new GamePlayWindow();
    game->show();
    close();
}

void GameWindow::goBack()
{
    WelcomeWindow *w = new WelcomeWindow();
    w->show();
    close();
}