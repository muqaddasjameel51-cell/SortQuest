#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>

class QLabel;
class QPushButton;

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GameWindow(QWidget *parent = nullptr);

private slots:
    void startGame();
    void goBack();

private:
    QLabel *titleLabel;
    QLabel *modeLabel;
    QLabel *difficultyLabel;
    QLabel *highScoreLabel;

    QPushButton *startButton;
    QPushButton *backButton;
};

#endif