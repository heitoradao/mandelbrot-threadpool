#include "mandelbrotwidget.h"
#include <QResizeEvent>
#include <QImage>
#include <QPainter>
#include <QtMath>

using namespace std;

const int ITERATION_MAX = 4000;
const double DEFAULT_SCALE = 0.005;
const double DEFAULT_OFFSET_X = -0.74364390249094747;
const double DEFAULT_OFFSET_Y = 0.13182589977450967;

MandelbrotWidget::MandelbrotWidget(QWidget *parent)
    : QWidget(parent)
    //, mThreadCalculator(this)
    , mScaleFactor(DEFAULT_SCALE)
    , mMoveOffset(DEFAULT_OFFSET_X, DEFAULT_OFFSET_Y)
    , mIterationMax(ITERATION_MAX)
{
    mMandelbrotCalculator.moveToThread(&mThreadCalculator);

    connect(this, &MandelbrotWidget::requestPicture,
            &mMandelbrotCalculator, &MandelbrotCalculator::generatePicture);

    connect(&mMandelbrotCalculator, &MandelbrotCalculator::pictureLinesGenerated,
            this, &MandelbrotWidget::processJobResults);

    mThreadCalculator.start();
}

MandelbrotWidget::~MandelbrotWidget()
{
    mThreadCalculator.quit();
    mThreadCalculator.wait(1000);
    if (!mThreadCalculator.isFinished()) {
        mThreadCalculator.terminate();
    }
}

void MandelbrotWidget::processJobResults(QList<JobResult> jobResults)
{
    int yMin = height();
    int yMax = 0;

    for (JobResult &jobResult : jobResults) {

        if (mImage->size() != jobResult.areaSize) {
            continue;
        }

        int y = jobResult.pixelPositionY;
        QRgb *scanLine = reinterpret_cast<QRgb*>(mImage->scanLine(y));

        for (int x = 0; x < mAreaSize.width(); ++x) {
            scanLine[x] = generateColorFromIteration(jobResult.values[x]);
        }

        if (y < yMin) {
            yMin = y;
        }

        if (y > yMax) {
            yMax = y;
        }
    }

    repaint(0, yMin, width(), yMax);
}

void MandelbrotWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.save();

    QRect imageRect = event->region().boundingRect();
    painter.drawImage(imageRect, *mImage, imageRect);

    painter.setPen(Qt::white);

    painter.drawText(10, 20, tr("Size: %1 x %2").arg(mImage->width())
                                                     .arg(mImage->height()));
    painter.drawText(10, 40, tr("Offset: %1 x %2").arg(mMoveOffset.x())
                                                       .arg(mMoveOffset.y()));
    painter.drawText(10, 60, tr("Scale: %1").arg(mScaleFactor));
    painter.drawText(10, 80, tr("Max iteration: %1").arg(ITERATION_MAX));

    painter.restore();
}

void MandelbrotWidget::resizeEvent(QResizeEvent *event)
{
    mAreaSize = event->size();
    mImage = make_unique<QImage>(mAreaSize, QImage::Format_RGB32);
    mImage->fill(Qt::black);
    emit requestPicture(mAreaSize, mMoveOffset, mScaleFactor, mIterationMax);
}

void MandelbrotWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();
    mScaleFactor *= qPow(0.75, delta / 120.0);
    emit requestPicture(mAreaSize, mMoveOffset, mScaleFactor, mIterationMax);
}

void MandelbrotWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        mLastMouseMovePosition = event->pos();
    }
}

void MandelbrotWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPointF offset = event->pos() - mLastMouseMovePosition;
        mLastMouseMovePosition = event->pos();
        offset.setY(offset.y());
        mMoveOffset += offset * mScaleFactor;
        emit requestPicture(mAreaSize, mMoveOffset, mScaleFactor, mIterationMax);
    }
}

QRgb MandelbrotWidget::generateColorFromIteration(int iteration)
{
    if (iteration == mIterationMax) {
        return qRgb(50, 50, 255);
    }
    return qRgb(0, 0, 255.0 * iteration / mIterationMax);
}
