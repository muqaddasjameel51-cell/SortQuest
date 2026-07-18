#ifndef WELCOMEWINDOW_H
#define WELCOMEWINDOW_H

#include <QWidget>

class QPushButton;
class QLabel;

class WelcomeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeWindow(QWidget *parent = nullptr);

private slots:
    void openVisualizer();
    void openGame();

private:
    QLabel *titleLabel;
    QLabel *subtitleLabel;

    QPushButton *visualizerBtn;
    QPushButton *gameBtn;
    QPushButton *exitBtn;
};

#endif // WELCOMEWINDOW_H