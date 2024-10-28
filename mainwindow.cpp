#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <random>    // For std::mt19937 and std::uniform_int_distribution
#include <chrono>    // For high_resolution_clock to seed the random generator

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Random choice function using a more precise random engine
QString MainWindow::computerChoice()
{
    // Use the current time with nanosecond precision to seed the generator
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);  // Mersenne Twister engine
    std::uniform_int_distribution<int> distribution(0, 2);  // Random number between 0 and 2

    int choice = distribution(generator);  // Get the random choice
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
    ui->lblResult->setText("Your chose: " + playerChoice + "\n" + "Computer chose: " + compChoice + "\n" + result);
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
