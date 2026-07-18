#include "welcomewindow.h"
#include "mainwindow.h"
#include "gamewindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QApplication>

WelcomeWindow::WelcomeWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("SortQuest");
    setFixedSize(1100,650);

    setStyleSheet(
        "background-color:#F2F1ED;"
        "color:#3A2A1A;"
        );

    titleLabel = new QLabel("SORT QUEST");
    titleLabel->setAlignment(Qt::AlignCenter);

    QFont titleFont;
    titleFont.setPointSize(36);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    titleLabel->setStyleSheet(
        "color:#B06090;"
        );

    subtitleLabel = new QLabel("Learn Sorting Through Interactive Visualizations & Games");
    subtitleLabel->setAlignment(Qt::AlignCenter);

    subtitleLabel->setStyleSheet(
        "font-size:18px;"
        "color:#7A6070;"
        );

    visualizerBtn = new QPushButton("Visualizer Mode");
    gameBtn       = new QPushButton("Game Mode");
    exitBtn       = new QPushButton("Exit");

    visualizerBtn->setFixedSize(320,55);
    gameBtn->setFixedSize(320,55);
    exitBtn->setFixedSize(320,55);

    QString buttonStyle =
        "QPushButton{"
        "background:#FFCCE1;"
        "color:#7A2040;"
        "font-size:17px;"
        "font-weight:bold;"
        "border:2px solid #F7B8D0;"
        "border-radius:10px;"
        "}"
        "QPushButton:hover{"
        "background:#F7B8D0;"
        "}";

    visualizerBtn->setStyleSheet(buttonStyle);
    gameBtn->setStyleSheet(buttonStyle);

    exitBtn->setStyleSheet(
        "QPushButton{"
        "background:#C6E2E7;"
        "color:#1A4A55;"
        "font-size:17px;"
        "font-weight:bold;"
        "border:2px solid #A8D0D8;"
        "border-radius:10px;"
        "}"
        "QPushButton:hover{"
        "background:#A8D0D8;"
        "}"
        );

    QLabel *footer = new QLabel("DSA Sorting Visualizer & Interactive Bubble Sort Game");
    footer->setAlignment(Qt::AlignCenter);
    footer->setStyleSheet(
        "color:#A090A0;"
        "font-size:13px;"
        );

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(40,40,40,40);
    layout->setSpacing(15);
    layout->addStretch();
    layout->addWidget(titleLabel,0,Qt::AlignCenter);
    layout->addSpacing(10);
    layout->addWidget(subtitleLabel,0,Qt::AlignCenter);
    layout->addSpacing(60);
    layout->addWidget(visualizerBtn,0,Qt::AlignCenter);
    layout->addSpacing(15);
    layout->addWidget(gameBtn,0,Qt::AlignCenter);
    layout->addSpacing(25);
    layout->addWidget(exitBtn,0,Qt::AlignCenter);
    layout->addSpacing(40);
    layout->addWidget(footer);
    layout->addStretch();

    connect(visualizerBtn, &QPushButton::clicked, this, &WelcomeWindow::openVisualizer);
    connect(gameBtn,       &QPushButton::clicked, this, &WelcomeWindow::openGame);
    connect(exitBtn,       &QPushButton::clicked, qApp, &QApplication::quit);
}

void WelcomeWindow::openVisualizer()
{
    MainWindow *window = new MainWindow();
    window->show();
    this->hide();
    window->show();
    close();
}

void WelcomeWindow::openGame()
{
    GameWindow *game = new GameWindow();
    game->show();
    close();
}