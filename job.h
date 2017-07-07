#ifndef JOB_H
#define JOB_H

#include <QObject>
#include <QRunnable>
#include <QPointF>
#include <QSize>
#include <QAtomicInteger>
#include "jobresult.h"

class Job : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Job(QObject *parent = nullptr);
    void run() override;

    void setPixelPositionY(int value);
    void setMoveOffset(const QPointF &value);
    void setScaleFactor(double value);
    void setAreaSize(const QSize &value);
    void setIterationMax(int value);

signals:
    void jobCompleted(JobResult result);

public slots:
    void abort();

private:
    QAtomicInteger<bool> mAbort;
    int mPixelPositionY;
    QPointF mMoveOffset;
    double mScaleFactor;
    QSize mAreaSize;
    int mIterationMax;
};

#endif // JOB_H
