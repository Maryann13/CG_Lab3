#ifndef PAINTCANVAS_H
#define PAINTCANVAS_H

#include <QtWidgets>

class PaintCanvas : public QFrame {
    Q_OBJECT

public:
    PaintCanvas(QWidget* parent = nullptr);
    void setColor(QColor c);
    QPixmap getPixmap() const;
    void setFillMethod(bool v);
    void setImagePattern(QImage ppattern);

public slots:
    void setThickness(int);
    void clearArea();

protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void resizeEvent(QResizeEvent*);

private:
    QColor color = Qt::black;
    QColor bgColor = Qt::white;
    bool mousePressed = false;
    QPixmap pixmap;
    QPoint curPos;
    int penThickness = 2;
    void colorFill(QPoint curPos);
    void imageFill(QPoint basePos);
    bool fillMethod = false; // false = color, true = image
    QImage pattern;
    bool patternLoaded;
};

#endif // PAINTCANVAS_H
