#ifndef GAMEPLAYWINDOW_H
#define GAMEPLAYWINDOW_H

#include <QWidget>
#include "gamestagewidget.h"
#include "bubblegameengine.h"

class QLabel;
class QPushButton;
class QRadioButton;

class GamePlayWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GamePlayWindow(QWidget *parent = nullptr);

private slots:
    void submitAnswer();
    void handleSwapRequired(int i, int j);
    void gameOver();
    void goBack();

private:

    QLabel *levelLabel;
    QLabel *scoreLabel;
    QLabel *livesLabel;
    QLabel *questionLabel;
    QLabel *arrayLabel;
    GameStageWidget *stage;
    BubbleGameEngine *engine;
    QLabel *resultLabel;
    void generateRandomArray();
    void levelCompleted();
    int correctAnswers = 0;
    int totalQuestions = 0;
    QPushButton *backButton;

    QVector<int> arr;

    QRadioButton *swapOption;
    QRadioButton *noSwapOption;

    QPushButton *submitButton;

    int score = 0;
    int lives = 3;
    int level = 1;
};

#endif