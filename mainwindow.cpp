#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cstdlib>   // For rand() function
#include <ctime>     // For seeding random number generator

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    std::srand(std::time(nullptr));  // Seed the random number generator
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::computerChoice()
{
    int choice = std::rand() % 3;  // 0: Rock, 1: Paper, 2: Scissors
    if (choice == 0) return "Rock";
    else if (choice == 1) return "Paper";
    else return "Scissors";
}

QString MainWindow::determineWinner(const QString &player, const QString &computer)
{
    if (player == computer) return "It's a tie!";
    if ((player == "Rock" && computer == "Scissors") ||
        (player == "Paper" && computer == "Rock") ||
        (player == "Scissors" && computer == "Paper"))
        return "You win!";
    else
        return "You lose!";
}

void MainWindow::playGame(const QString &playerChoice)
{
    QString compChoice = computerChoice();
    QString result = determineWinner(playerChoice, compChoice);
    ui->lblResult->setText("Computer chose: " + compChoice + "\n" + result);
}

void MainWindow::on_btnRock_clicked()
{
    playGame("Rock");
}

void MainWindow::on_btnPaper_clicked()
{
    playGame("Paper");
}

void MainWindow::on_btnScissors_clicked()
{
    playGame("Scissors");
}

void MainWindow::on_btnPlayAgain_clicked()
{
    ui->lblResult->setText("Choose your move:");
}
