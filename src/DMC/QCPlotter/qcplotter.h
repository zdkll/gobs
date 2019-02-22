#ifndef QCPLOTTER_H
#define QCPLOTTER_H

#include <QWidget>

#include <QScrollBar>
#include <QStatusBar>

#include "qcgraphbase.h"
#include "gsegy.h"
#include "qcplotter_global.h"

class PlotParamDlg;
class QCPLOTTERSHARED_EXPORT QCPlotter : public QWidget
{
    Q_OBJECT
public:
    explicit QCPlotter(QWidget *parent = 0);
    ~QCPlotter();

    void Initial(QStatusBar *statusBar,GatherInfo *gatherInfo);
    void setData(TraceHead *headers,char *data);
    void zoomOut();

    //图像属性设置-GraphAttribute----
    void setMouseFunction(eMouseFunction mf);

    void setGraphMode(eGraphicMode mode);
    void setInterType(int interType);
    void setBrushMode(eBrushMode brushMode);
    void setScaleScope(eScale scale);
    void setGain(float gainPercent);
    void setTraceInc(int inc);
    void setGridShow(GridPosition pos);
    void setMinAxesLength(Qt::Orientation orientation,int len);
    void setVertScaleType(VertScaleType type);
    void setShowHeaderFielPos(HeaderFieldPos headerFieldPos);

    void setFixedTimeScope(QPair<int,int>);
    void setComponents(const int &coms);

private slots:
    void horiScrollValueChanged(int);
    void vertScrollValueChanged(int);

private:
    void initWg();

    QScrollBar     *m_horizBar,*m_vertBar;
    QStatusBar     *m_statusBar;
    QCGraphBase    *m_graphic;
    GraphAttribute *m_grapAttribute;
    PlotParamDlg   *m_paramDlg;
};




#endif // QCPLOTTER_H
