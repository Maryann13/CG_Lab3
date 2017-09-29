#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sceneOrig = new QGraphicsScene(nullptr);
    sceneEdge = new QGraphicsScene(nullptr);
    pixmapItemOrig = new QGraphicsPixmapItem();
    pixmapItemEdge = new QGraphicsPixmapItem();

    ui->comboBox->addItems
            (QStringList({"Black", "Red", "Green", "Blue", "Yellow"}));

    ui->graphics_orig->setScene(sceneOrig);
    ui->graphics_edge->setScene(sceneEdge);
    sceneOrig->addItem(pixmapItemOrig);
    sceneEdge->addItem(pixmapItemEdge);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete sceneOrig;
    delete sceneEdge;
    delete pixmapItemOrig;
    delete pixmapItemEdge;
}

void showImage(QGraphicsView * view, QGraphicsPixmapItem * pixmapItem,
               QImage const & img)
{
    pixmapItem->setVisible(true);
    QPixmap pixmap = QPixmap::fromImage(img);
    pixmapItem->setPixmap(pixmap);
    view->scene()->setSceneRect(0, 0, pixmap.width(), pixmap.height());
    view->fitInView(pixmapItem, Qt::KeepAspectRatio);
}

void MainWindow::on_load_button_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Load Image", "./", "*.jpg");
    if (!filename.isEmpty()) {
        QImageReader reader(filename);
        reader.setAutoTransform(true);
        imgOrig = reader.read().convertToFormat(QImage::Format_ARGB32);
        isLoaded = true;

        showImage(ui->graphics_orig, pixmapItemOrig, imgOrig);
    }
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    auto d = ui->plainTextEdit->document();

    switch (index) {
        case 0: d->setPlainText("#000000"); break;
        case 1: d->setPlainText("#ff0000"); break;
        case 2: d->setPlainText("#00ff00"); break;
        case 3: d->setPlainText("#0000ff"); break;
        case 4: d->setPlainText("#ffd800"); break;
    }
}

QPoint next_point(int dir, QPoint const & p)
{
    switch (dir) {
        case 0: return p + QPoint(0, 1);
        case 1: return p + QPoint(1, 1);
        case 2: return p + QPoint(1, 0);
        case 3: return p + QPoint(1, -1);
        case 4: return p + QPoint(0, -1);
        case 5: return p + QPoint(-1, -1);
        case 6: return p + QPoint(-1, 0);
        case 7: return p + QPoint(-1, 1);
    }
    throw std::invalid_argument("invalid direction");
}

int direction(QPoint diff)
{
    int i = diff.x() * 2 + diff.y() * 3 + 5;
    switch (i) {
        case 0:  return 5;
        case 2:  return 4;
        case 3:  return 6;
        case 4:  return 3;
        case 6:  return 7;
        case 7:  return 2;
        case 8:  return 0;
        case 10: return 1;
    }
    throw std::invalid_argument("invalid difference");
}

void MainWindow::on_pushButton_clicked()
{
    QString colName = ui->plainTextEdit->document()->toPlainText();

    if (!isLoaded || !QColor::isValidColor(colName))
        return;
    edgeColor.setNamedColor(colName);

    QImage imgEdge = imgOrig.copy();
    quint64 pW, pH, pixCnt;
    pW = imgOrig.width();
    pH = imgOrig.height();
    pixCnt = pW * pH;
    QRgb* iCp = (QRgb*)imgOrig.bits();
    QRgb* iCpE = (QRgb*)imgEdge.bits();

    quint64 i = 0;
    while (i < pixCnt && edgeColor != QColor::fromRgb(iCp[i]))
        ++i;
    if (edgeColor == QColor::fromRgb(iCp[i + pW]))
        do i += pW;
            while (edgeColor == QColor::fromRgb(iCp[i + pW]));
    if (i == pixCnt)
        return;

    QPoint p(i % pW, i / pW);
    QLinkedList<QPoint> edge;
    edge.append(p);
    edge.append(QPoint(0, 0));
    int dir = 2;

    while (edge.first() != edge.last()) {
        int d = dir - 2 < 0 ? dir + 6 : dir - 2;
        QPoint p_next = next_point(d, p);
        int pi = p_next.x() + p_next.y() * pW;
        if (edgeColor != QColor::fromRgb(iCp[pi]))
            for (int i = 1; i < 8; ++i) {
                int di = d + i > 7 ? d + i - 8 : d + i;
                p_next = next_point(di, p);
                pi = p_next.x() + p_next.y() * pW;
                if (edgeColor == QColor::fromRgb(iCp[pi]))
                    break;
            }
        if (edgeColor == QColor::fromRgb(iCp[pi])) {
            edge.append(p_next);
            iCpE[pi] = QColor::fromRgb(0, 255, 0).rgb();
            dir = direction(p_next - p);
            p = p_next;
        }
        else
            break;
    }
    edge.erase(edge.begin() + 1);
    showImage(ui->graphics_edge, pixmapItemEdge, imgEdge);

    imgEdge.save("result.jpg", nullptr, 100);
    //QImageWriter("result1.jpg").write(imgEdge);
}
