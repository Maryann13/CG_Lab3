#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCharts>
#include <QtWidgets>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void on_load_button_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_clicked();

private:
    Ui::MainWindow* ui;
    QGraphicsScene* sceneOrig;
    QGraphicsScene* sceneEdge;
    QGraphicsPixmapItem* pixmapItemOrig;
    QGraphicsPixmapItem* pixmapItemEdge;
    QImage imgOrig;
    QColor edgeColor;
    bool isLoaded = false;
};

#endif // MAINWINDOW_H
