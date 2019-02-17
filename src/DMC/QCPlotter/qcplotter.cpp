#include "qcplotter.h"
#include "ui_qcplotter.h"

#include <QBoxLayout>
#include <QScrollBar>

QCPlotter::QCPlotter(QWidget *parent) :
    QWidget(parent)
{
    initWg();

    m_grapAttribute = new GraphAttribute;

    connect(m_horizBar,SIGNAL(sliderMoved(int)),this,SLOT(horiScrollValueChanged(int)));
    connect(m_vertBar, SIGNAL(sliderMoved(int)),this,SLOT(vertScrollValueChanged(int)));
}

QCPlotter::~QCPlotter()
{
    delete m_grapAttribute;
}

void QCPlotter::initWg()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    QHBoxLayout *horiLayout = new QHBoxLayout;
    m_graphic  = new QCGraphBase(this);

    horiLayout->addWidget(m_graphic,1);

    m_vertBar = new QScrollBar(Qt::Vertical,this);
    QVBoxLayout *vertBarLayout = new QVBoxLayout;
    vertBarLayout->addSpacing(Top_Scale_Margin);
    vertBarLayout->addWidget(m_vertBar);
    vertBarLayout->addSpacing(Bottom_Scale_Margin);
    vertBarLayout->setSpacing(0);
    vertBarLayout->setContentsMargins(0,0,0,0);
    horiLayout->addLayout(vertBarLayout);
    horiLayout->setSpacing(0);
    horiLayout->setContentsMargins(0,0,0,0);

    mainLayout->addLayout(horiLayout);

    m_horizBar = new QScrollBar(Qt::Horizontal,this);
    QHBoxLayout  *horiBarLayout = new QHBoxLayout;
    horiBarLayout->addSpacing(Left_Scale_Margin);
    horiBarLayout->addWidget(m_horizBar);
    horiBarLayout->addSpacing(Right_Scale_Margin);
    horiBarLayout->setSpacing(0);
    horiBarLayout->setContentsMargins(0,0,0,0);

    mainLayout->setContentsMargins(0,0,0,4);
    mainLayout->addLayout(horiBarLayout);

    this->setLayout(mainLayout);
}

void QCPlotter::Initial(QStatusBar *statusBar,GatherInfo *gatherInfo)
{
    m_statusBar = statusBar;

    m_graphic->Initial(statusBar,m_horizBar,m_vertBar,gatherInfo,m_grapAttribute);
}

void QCPlotter::setData(TraceHead *headers,char *data)
{
    m_graphic->setData(headers,data);
}

void QCPlotter::zoomOut()
{
    m_graphic->zoomOut();
}


void QCPlotter::setMouseFunction(eMouseFunction mf)
{
    m_grapAttribute->mouseFunction = mf;
}

void QCPlotter::setGraphMode(eGraphicMode mode)
{
    m_grapAttribute->mode = mode;
    m_graphic->objDraw();
}

void QCPlotter::setInterType(int interType)
{
    m_grapAttribute->interType = interType;
    m_graphic->draw();
}

void QCPlotter::setBrushMode(eBrushMode brushMode)
{
    m_grapAttribute->brushMode = brushMode;
    m_graphic->objDraw();
}

void QCPlotter::setScaleScope(eScale scale)
{
    m_grapAttribute->scaleType = scale;
    m_graphic->objDraw();
}

void QCPlotter::setGain(float gainPercent)
{
    m_grapAttribute->gainPercent = gainPercent;
    m_graphic->objDraw();
}

void QCPlotter::setTraceInc(int inc)
{
    m_grapAttribute->dispStep = inc;
    m_graphic->draw();
}
void QCPlotter::setGridShow(GridPosition pos)
{
    m_grapAttribute->gridPosition = pos;
    m_graphic->reDraw();
}
void QCPlotter::setMinAxesLength(Qt::Orientation orientation,int len)
{
    if(orientation == Qt::Horizontal)
    {
        m_grapAttribute->horiAxesMinLen = len;
    }
    if(orientation == Qt::Vertical)
    {
        m_grapAttribute->vertAxesMinLen = len;
    }
    m_graphic->reDraw();
}
void QCPlotter::setVertScaleType(VertScaleType type)
{
    m_grapAttribute->vertScaleType = type;
    m_graphic->reDraw();
}
void QCPlotter::setShowHeaderFielPos(HeaderFieldPos headerFieldPos)
{
    m_grapAttribute->showedHeadFieldPos = headerFieldPos;
    m_graphic->reDraw();
}

void QCPlotter::setFixedTimeScope(QPair<int,int> timeScope)
{
    m_graphic->setFixedTimeScope(timeScope);
}

void QCPlotter::setComponents(const int &coms)
{
    m_grapAttribute->components = coms;
}
void QCPlotter::horiScrollValueChanged(int value)
{
    m_graphic->scrollValueChanged(Qt::Horizontal,value);
}

void QCPlotter::vertScrollValueChanged(int value)
{
    m_graphic->scrollValueChanged(Qt::Vertical,value);
}


