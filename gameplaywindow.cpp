#include "gameplaywindow.h"
#include "gamestagewidget.h"
#include "welcomewindow.h"
#include <QRandomGenerator>
#include <algorithm>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QFont>
#include <QTimer>

GamePlayWindow::GamePlayWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Sort Quest");
    setFixedSize(1100,650);

    setStyleSheet(
        "background:#F2F1ED;"
        "color:#3A2A1A;"
        );

    backButton = new QPushButton("Back");
    backButton->setFixedSize(100,40);
    backButton->setStyleSheet(
        "QPushButton{"
        "background:#C6E2E7;"
        "color:#1A4A55;"
        "font-size:15px;"
        "font-weight:bold;"
        "border:1px solid #2090A0;"
        "border-radius:8px;"
        "}"
        "QPushButton:hover{"
        "background:#2090A0;"
        "color:white;"
        "}"
        );
    connect(backButton, &QPushButton::clicked, this, &GamePlayWindow::goBack);

    levelLabel = new QLabel("Level : 1");
    scoreLabel = new QLabel("Score : 0");
    livesLabel = new QLabel("Lives : ❤️❤️❤️");

    levelLabel->setStyleSheet("font-size:18px;color:#B06090;font-weight:bold;");
    scoreLabel->setStyleSheet("font-size:18px;color:#208060;font-weight:bold;");
    livesLabel->setStyleSheet("font-size:18px;color:#D04070;font-weight:bold;");

    questionLabel = new QLabel("Will Bubble Sort swap these two highlighted elements?");
    questionLabel->setAlignment(Qt::AlignCenter);
    questionLabel->setStyleSheet(
        "font-size:18px;"
        "font-weight:bold;"
        "color:#B06090;"
        );

    resultLabel = new QLabel("");
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setStyleSheet("font-size:20px;font-weight:bold;");

    stage  = new GameStageWidget(this);
    engine = new BubbleGameEngine(this);

    connect(engine, &BubbleGameEngine::compareChanged, stage,  &GameStageWidget::highlightCompare);
    connect(engine, &BubbleGameEngine::arrayChanged,   stage,  &GameStageWidget::setArray);
    connect(engine, &BubbleGameEngine::swapRequired,   this,   &GamePlayWindow::handleSwapRequired);
    connect(engine, &BubbleGameEngine::levelCompleted, this,   &GamePlayWindow::levelCompleted);

    generateRandomArray();
    stage->setMinimumHeight(280);

    swapOption   = new QRadioButton("Swap");
    noSwapOption = new QRadioButton("No Swap");

    swapOption->setStyleSheet("font-size:16px;color:#3A2A1A;");
    noSwapOption->setStyleSheet("font-size:16px;color:#3A2A1A;");

    submitButton = new QPushButton("Submit");
    submitButton->setFixedHeight(45);
    submitButton->setStyleSheet(
        "QPushButton{"
        "background:#FFCCE1;"
        "color:#7A2040;"
        "font-size:16px;"
        "font-weight:bold;"
        "border:2px solid #D04070;"
        "border-radius:8px;"
        "}"
        "QPushButton:hover{"
        "background:#D04070;"
        "color:white;"
        "}"
        );

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20,20,20,20);

    QHBoxLayout *topBar = new QHBoxLayout;
    topBar->addWidget(backButton);
    topBar->addSpacing(20);
    topBar->addWidget(levelLabel);
    topBar->addStretch();
    topBar->addWidget(scoreLabel);
    topBar->addStretch();
    topBar->addWidget(livesLabel);
    layout->addLayout(topBar);

    layout->addSpacing(30);
    layout->addWidget(questionLabel);
    layout->addWidget(resultLabel);
    layout->addSpacing(20);
    layout->addWidget(stage);
    layout->addSpacing(20);
    layout->addWidget(swapOption);
    layout->addWidget(noSwapOption);
    layout->addSpacing(15);
    layout->addWidget(submitButton);

    connect(submitButton, &QPushButton::clicked, this, &GamePlayWindow::submitAnswer);
}

void GamePlayWindow::submitAnswer()
{
    bool answer  = swapOption->isChecked();
    bool correct = engine->submitAnswer(answer);

    if(correct) {
        totalQuestions++; score += 10; correctAnswers++;
        scoreLabel->setText("Score : " + QString::number(score));
        resultLabel->setText("Correct!");
        resultLabel->setStyleSheet("font-size:20px;font-weight:bold;color:#208060;");
    } else {
        lives--;
        resultLabel->setText("Wrong!");
        resultLabel->setStyleSheet("font-size:20px;font-weight:bold;color:#D04070;");
        QString hearts;
        for(int i=0;i<lives;i++) hearts += "❤️ ";
        livesLabel->setText("Lives : " + hearts);
        if(lives == 0){ gameOver(); return; }
    }

    swapOption->setAutoExclusive(false);
    noSwapOption->setAutoExclusive(false);
    swapOption->setChecked(false);
    noSwapOption->setChecked(false);
    swapOption->setAutoExclusive(true);
    noSwapOption->setAutoExclusive(true);
}

void GamePlayWindow::handleSwapRequired(int i, int j)
{
    submitButton->setEnabled(false);
    stage->animateSwap(i, j);
    QTimer::singleShot(500, this, [=](){
        submitButton->setEnabled(true);
        engine->animationFinished();
    });
}

void GamePlayWindow::generateRandomArray()
{
    arr.clear();
    int totalCards = qMin(level + 4, 10);
    for(int i = 1; i <= totalCards; i++) arr.append(i);
    for(int i = arr.size()-1; i > 0; i--) {
        int j = QRandomGenerator::global()->bounded(i+1);
        std::swap(arr[i], arr[j]);
    }
    stage->setArray(arr);
    engine->setArray(arr);
    engine->start();
}

void GamePlayWindow::levelCompleted()
{
    double accuracy = 0;
    if(totalQuestions > 0) accuracy = (correctAnswers * 100.0) / totalQuestions;
    QString stars;
    if(accuracy >= 90)      stars = "★★★";
    else if(accuracy >= 70) stars = "★★☆";
    else                    stars = "★☆☆";

    QMessageBox msg(this);
    msg.setWindowTitle("Level Complete!");
    msg.setText(
        "LEVEL COMPLETED!\n\n" + stars +
        "\n\nExcellent Work!\n\n"
        "Accuracy : " + QString::number(accuracy,'f',1) + "%\n"
                                              "Score : "    + QString::number(score) +
        "\n\nReady for the next challenge?"
        );
    msg.setStyleSheet(
        "QMessageBox{background:#F2F1ED;color:#3A2A1A;}"
        "QLabel{color:#3A2A1A;font-size:14px;}"
        "QPushButton{background:#CDE5D9;color:#0A3A20;border-radius:6px;padding:6px 16px;border:1px solid #3AAA70;}"
        "QPushButton:hover{background:#3AAA70;color:white;}"
        );
    QPushButton *nextButton = msg.addButton("Next Level", QMessageBox::AcceptRole);
    msg.exec();

    if(msg.clickedButton() == nextButton) {
        level++;
        levelLabel->setText("Level : " + QString::number(level));
        correctAnswers = 0; totalQuestions = 0;
        resultLabel->clear();
        swapOption->setChecked(false);
        noSwapOption->setChecked(false);
        generateRandomArray();
    }
}

void GamePlayWindow::gameOver()
{
    QMessageBox msg(this);
    msg.setWindowTitle("Game Over");
    msg.setText(
        "GAME OVER\n\n"
        "Final Score : "   + QString::number(score) +
        "\nLevel Reached : " + QString::number(level) +
        "\n\nTry again and beat your score!"
        );
    msg.setStyleSheet(
        "QMessageBox{background:#F2F1ED;color:#3A2A1A;}"
        "QLabel{color:#3A2A1A;font-size:14px;}"
        "QPushButton{background:#FFCCE1;color:#7A2040;border-radius:6px;padding:6px 16px;border:1px solid #D04070;}"
        "QPushButton:hover{background:#D04070;color:white;}"
        );
    QPushButton *playAgain = msg.addButton("Play Again", QMessageBox::AcceptRole);
    QPushButton *quit      = msg.addButton("Quit",       QMessageBox::RejectRole);
    msg.exec();

    if(msg.clickedButton() == playAgain) {
        score=0; lives=3; level=1;
        correctAnswers=0; totalQuestions=0;
        scoreLabel->setText("Score : 0");
        levelLabel->setText("Level : 1");
        livesLabel->setText("Lives : ❤️ ❤️ ❤️");
        resultLabel->clear();
        questionLabel->setText("Will Bubble Sort swap these two highlighted elements?");
        submitButton->setEnabled(true);
        swapOption->setChecked(false);
        noSwapOption->setChecked(false);
        generateRandomArray();
    } else {
        close();
    }
}

void GamePlayWindow::goBack()
{
    WelcomeWindow *w = new WelcomeWindow();
    w->show();
    close();
}