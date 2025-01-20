#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void filelogic(QString inputdir, QString filemaks, uint16_t xorvalue, bool deleteinputfiles, QString outputdir, QString overwrite);
    void selectInputDirectory();
    void selectOutputDirectory();

public slots:
    void startProcessing();
    void stopProcessing();
    void exit();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
};
#endif // MAINWINDOW_H
