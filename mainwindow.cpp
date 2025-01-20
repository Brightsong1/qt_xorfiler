#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "qdir.h"
#include "QTimer"
#include "QFileDialog"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(650, 500);
    this->timer = NULL;
    // connect buttons
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(startProcessing()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(stopProcessing()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(exit()));

    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::selectInputDirectory);
    connect(ui->pushButton_5, &QPushButton::clicked, this, &MainWindow::selectOutputDirectory);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// explorer
void MainWindow::selectInputDirectory()
{
    QString inputDir = QFileDialog::getExistingDirectory(this, "Выберите входную директорию", QDir::homePath());
    if (!inputDir.isEmpty()) {
        ui->lineEdit_2->setText(inputDir);
    }
}

void MainWindow::selectOutputDirectory()
{
    QString outputDir = QFileDialog::getExistingDirectory(this, "Выберите выходную директорию", QDir::homePath());
    if (!outputDir.isEmpty()) {
        ui->lineEdit_3->setText(outputDir);
    }
}

void MainWindow::filelogic(QString inputdir, QString filemask, uint16_t xorvalue, bool deleteinputfiles, QString outputdir, QString overwrite)
{
    QDir dir(inputdir);
    // first check files in input dir
    QStringList files = dir.entryList(QStringList() << filemask, QDir::Files | QDir::NoDotAndDotDot);

    for (const QString& fileName : files)
    {
        QFile file(dir.filePath(fileName));

        // check open
        if (file.isOpen())
        {
            continue;
        }

        if (file.open(QIODevice::ReadWrite)) {
            QByteArray data = file.readAll();

            // xor with const
            for (int i = 0; i < data.size(); ++i) {
                data[i] ^= xorvalue;
            }

            QString outputFileName = outputdir + "/" + fileName;
            QFile outputFile(outputFileName);

            // rewrite choice
            if (overwrite == "Перезаписать")
            {
                if (outputFile.open(QIODevice::WriteOnly))
                {
                    outputFile.write(data);
                    outputFile.close();
                }
                else
                {
                    QMessageBox::warning(nullptr, "Error", "Ошибка записи файла: " + outputFile.errorString());
                }
            }
            // index save choice
            else if (overwrite == "Сохранить в формате {filename + index}")
            {
                int index = 1;
                QString baseName = QFileInfo(fileName).baseName();
                QString extension = QFileInfo(fileName).suffix();
                QString newFileName;

                do
                {
                    newFileName = outputdir + "/" + baseName + "_" + QString::number(index) + (extension.isEmpty() ? "" : "." + extension);
                    outputFile.setFileName(newFileName);
                    index++;
                } while (outputFile.exists());

                if (outputFile.open(QIODevice::WriteOnly))
                {
                    outputFile.write(data);
                    outputFile.close();
                }
                else
                {
                    QMessageBox::warning(nullptr, "Error", "Ошибка записи файла: " + outputFile.errorString());
                }
            }

            file.close();

            // if delete input files choice
            if (deleteinputfiles)
            {
                file.remove();
            }
        }
        else
        {
            QMessageBox::warning(nullptr, "Error", "Ошибка чтения файла: " + file.errorString());
        }
    }

    // recursive serch in all subdirs
    QStringList directories = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString& dirName : directories) {
        QString subDirPath = dir.filePath(dirName);
        filelogic(subDirPath, filemask, xorvalue, deleteinputfiles, outputdir, overwrite);
    }
}


void MainWindow::startProcessing()
{

    QString mask = ui->lineEdit->text();
    QString directory = ui->lineEdit_2->text();
    QString outputDirectory = ui->lineEdit_3->text();
    QString overwrite = ui->comboBox->currentText();
    QString type = ui->comboBox_2->currentText();
    int interval = ui->lineEdit_4->text().toInt();
    uint16_t xorconst = ui->lineEdit_5->text().toInt();
    bool deleteFiles = ui->checkBox->isChecked();

    // check fields
    if (mask.isEmpty()) {
        QMessageBox::warning(this, "Error", "Впишите маску для файлов.");
        return;
    }

    if (directory.isEmpty()) {
        QMessageBox::warning(this, "Error", "Впишите путь до директории с файлами.");
        return;
    }

    if (outputDirectory.isEmpty()) {
        QMessageBox::warning(this, "Error", "Впишите путь для выходной директории.");
        return;
    }

    if (overwrite.isEmpty()) {
        QMessageBox::warning(this, "Error", "Выберите опцию для повторяющихся файлов.");
        return;
    }

    if (type.isEmpty()) {
        QMessageBox::warning(this, "Error", "Выберети тип запуска.");
        return;
    }

    // type working interval
    if (type == "Через интервал времени")
    {
        QTimer *timer = new QTimer(this);

        // save timer for stop
        this->timer = timer;

        connect(timer, &QTimer::timeout, this, [this, directory, mask, xorconst, deleteFiles, outputDirectory, overwrite]() {
            this->filelogic(directory, mask, xorconst, deleteFiles, outputDirectory, overwrite);
        });
        timer->start(interval * 1000);
    }
    // type work single launch
    else
    {
        this->filelogic(directory, mask, xorconst, deleteFiles, outputDirectory, overwrite);
    }
}

// stop interval work
void MainWindow::stopProcessing()
{
    if(this->timer != NULL)
    {
        this->timer->stop();
        delete this->timer;
        this->timer = NULL;
    }
}

void MainWindow::exit()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Выход", "Вы уверены что хотите выйти?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        // if interval work
        if(this->timer != NULL)
        {
            this->timer->stop();
            delete this->timer;
            this->timer = NULL;
        }

        QApplication::quit();
    }
}
