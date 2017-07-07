#include "job.h"
#include "jobresult.h"

Job::Job(QObject *parent)
    : QObject(parent)
    , mAbort(false)
    , mPixelPositionY(0)
    , mMoveOffset(0, 0)
    , mScaleFactor(0.0)
    , mAreaSize(0, 0)
    , mIterationMax(1)
{
}

void Job::run()
{
    JobResult result(mAreaSize.width());
    result.areaSize = mAreaSize;
    result.pixelPositionY = mPixelPositionY;
    result.moveOffset = mMoveOffset;
    result.scaleFactor = mScaleFactor;

    double imageHalfWidth  = mAreaSize.width()  / 2.0;
    double imageHalfHeight = mAreaSize.height() / 2.0;

    for (int imageX = 0; imageX < mAreaSize.width(); ++imageX) {

        int iteration = 0;

        double x0 = (imageX - imageHalfWidth) * mScaleFactor - mMoveOffset.x();
        double y0 = (mPixelPositionY - imageHalfHeight) * mScaleFactor - mMoveOffset.y();
        double x = 0.0;
        double y = 0.0;

        do {
            if (mAbort.load()) {
                return;
            }

            double nextX = (x*x) - (y*y) + x0;
            y = 2.0 * x * y + y0;
            x = nextX;
            iteration++;
        } while (iteration < mIterationMax
                 && x*x + y*y < 4.0);

        result.values[imageX] = iteration;
    }

    emit jobCompleted(result);
}

void Job::setPixelPositionY(int value)
{
    if (value != mPixelPositionY) {
        mPixelPositionY = value;
    }
}

void Job::setMoveOffset(const QPointF &value)
{
    if (value != mMoveOffset) {
        mMoveOffset = value;
    }
}

void Job::setScaleFactor(double value)
{
    if (value != mScaleFactor) {
        mScaleFactor = value;
    }
}

void Job::setAreaSize(const QSize &value)
{
    if (value != mAreaSize) {
        mAreaSize = value;
    }
}

void Job::setIterationMax(int value)
{
    if (value != mIterationMax) {
        mIterationMax = value;
    }
}

void Job::abort()
{
    mAbort.store(true);
}
