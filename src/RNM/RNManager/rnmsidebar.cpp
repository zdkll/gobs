#include "rnmsidebar.h"
#include "ui_rnmsidebar.h"

#include <QBoxLayout>
#include <QPixmap>

//statics Box-----------------------------------------
StatisticsBox::StatisticsBox(QWidget *parent)
    :QGroupBox(parent)
{
    this->setTitle("Statistics");
    initBox();
}

void StatisticsBox::initBox()
{
    QGridLayout  *mainLayout = new QGridLayout;
    //total nodes
    QLabel *totalNodeslbl = new QLabel("Total Nodes:");
    m_totalNodeslbl = new QLabel;
    mainLayout->addWidget(totalNodeslbl,0,0);
    mainLayout->addWidget(m_totalNodeslbl,0,1);

    //avalible nodes
    QLabel *validNodeslbl = new QLabel("Avaliable Nodes:");
    m_validNodeslbl = new QLabel;
    mainLayout->addWidget(validNodeslbl,1,0);
    mainLayout->addWidget(m_validNodeslbl,1,1);

    //Node Boxes
    QLabel *nodeBoxlbl = new QLabel("Node Boxes:");
    m_nodeBoxlbl = new QLabel;
    mainLayout->addWidget(nodeBoxlbl,2,0);
    mainLayout->addWidget(m_nodeBoxlbl,2,1);

    //warning nodes
    QLabel *warningNodeslbl = new QLabel("Update Interval");
    m_updateIntervallbl = new QLabel;
    mainLayout->addWidget(warningNodeslbl,3,0);
    mainLayout->addWidget(m_updateIntervallbl,3,1);

    //download nodes
    QLabel *downloadNodeslbl = new QLabel("Downloading Nodes:");
    m_downloadNodeslbl = new QLabel;
    mainLayout->addWidget(downloadNodeslbl,4,0);
    mainLayout->addWidget(m_downloadNodeslbl,4,1);

    this->setLayout(mainLayout);
}

void StatisticsBox::setStatisticsData(const StatisticsData &data)
{
    m_totalNodeslbl->setText(QString::number(data.totalNodes));
    m_validNodeslbl->setText(QString::number(data.avaliableNodes));
    m_nodeBoxlbl->setText(QString::number(data.nodeBoxes));

    m_updateIntervallbl->setText(QString::number(data.updateInterval)+" s");
    m_downloadNodeslbl->setText(QString::number(data.downloadingNode));
}

//Display Filters---------------------------------------
FiltersBox::FiltersBox(QWidget *parent)
    :QGroupBox(parent)
{
    this->setTitle("Display Filters");
    initBox();
}

void FiltersBox::setDisplayedNodes(const bool &displayed)
{
    reportBtn->setEnabled(displayed);
}

void FiltersBox::setDisplayFilters(const int &status)
{
    groupBox->button(status)->setChecked(true);
}

void FiltersBox::initBox()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    groupBox = new QButtonGroup(this);

    //Status
    QHBoxLayout *statusLayout = new QHBoxLayout;
    QRadioButton  *nodeStatusBtn = new QRadioButton("Node Status");
    reportBtn = new QPushButton("Report");
    reportBtn->setEnabled(false);
    statusLayout->addWidget(nodeStatusBtn);
    statusLayout->setSpacing(12);
    statusLayout->addWidget(reportBtn);
    mainLayout->addLayout(statusLayout);
    groupBox->addButton(nodeStatusBtn,Status);

    nodeStatusBtn->setChecked(true);

    //Voltage
    QRadioButton  *voltageBtn = new QRadioButton("Voltage");
    mainLayout->addWidget(voltageBtn);
    groupBox->addButton(voltageBtn,Voltage);

    //Temperature
    QRadioButton  *temperatureBtn = new QRadioButton("Temperature");
    mainLayout->addWidget(temperatureBtn);
    groupBox->addButton(temperatureBtn,Temperature);

    //Pressure
    QRadioButton  *pressureBtn = new QRadioButton("Pressure");
    mainLayout->addWidget(pressureBtn);
    groupBox->addButton(pressureBtn,Pressure);

//    //EleCurr
//    QRadioButton  *eleCurrBtn = new QRadioButton("EleCurr");
//    mainLayout->addWidget(eleCurrBtn);
//    groupBox->addButton(eleCurrBtn,EleCurr);

//    //Charge Voltage
//    QRadioButton  *chargeVoltBtn = new QRadioButton("Charge Voltage");
//    mainLayout->addWidget(chargeVoltBtn);
//    groupBox->addButton(chargeVoltBtn,ChargeVolt);

    //Memory_Capacity
    QRadioButton  *storageBtn = new QRadioButton("Memory Capacity");
    mainLayout->addWidget(storageBtn);
    groupBox->addButton(storageBtn,Memory_Capacity);

    //Self Test
    QRadioButton  *selfTestBtn = new QRadioButton("Self Test");
    mainLayout->addWidget(selfTestBtn);
    groupBox->addButton(selfTestBtn,SelfTest);

    //download
    QRadioButton  *downloadBtn = new QRadioButton("Downloaded");
    mainLayout->addWidget(downloadBtn);
    groupBox->addButton(downloadBtn,DownLoad);

    groupBox->setExclusive(true);

    connect(groupBox,SIGNAL(buttonClicked(int)),this,SIGNAL(displayFiltersChanged(int)));

    connect(reportBtn,SIGNAL(clicked()),this,SIGNAL(reportNodeStatus()));

    this->setLayout(mainLayout);
}

//Legend-----------------------------------------------
LegendBox::LegendBox(LimitsDialog *dlg,QWidget *parent)
    :QGroupBox(parent),m_status(Status),m_LimitsDlg(dlg)
{
    this->setTitle("Legend");
    //m_LimitsDlg->getLimits(m_status);

    //获取参信息，初始化面板
    initBox();

    //根据LimitDlg 获取对应状态的限制值信息，显示在图例中,初始为 0 状态
    LimitInfo  limitInfo = m_LimitsDlg->getLimitInfo(m_status);
    setLimitInfo(limitInfo);
}

void LegendBox::setLimitInfo(const LimitInfo &limitInfo)
{
    qDeleteAll(m_childWidgets);
    m_childWidgets.clear();

    LimitOperation  operate = limitInfo.operate;
    QList<Limit> limits = limitInfo.limits;

    if(operate == Equal){
        for(int i = 0;i<limits.size();i++){
            ColorFrame *frameColor = new ColorFrame();
            frameColor->setColor(limits[i].color);
            QLabel     *legendLbl  = new QLabel(limits[i].suffix);

            m_childWidgets.append(frameColor);
            m_childWidgets.append(legendLbl);
            mainLayout->addWidget(frameColor,i,0);
            mainLayout->addWidget(legendLbl,i,1);
        }
    }else if(operate == Big)
    {
        QString legendText;
        float   val1,val2;
        legendText  = logicText(Less) + " ";
        val1    =     limits[0].value;
        legendText += QString::number(val1) + " ";
        legendText += limits[0].suffix;
        ColorFrame *frameColor = new ColorFrame();
        frameColor->setColor(Qt::green);
        QLabel     *legendLbl  = new QLabel(legendText);
        m_childWidgets.append(frameColor);
        m_childWidgets.append(legendLbl);
        mainLayout->addWidget(frameColor,0,0);
        mainLayout->addWidget(legendLbl,0,1);
        for(int i = 0;i<limits.size();i++){
            val1 = limits[i].value;

            frameColor  = new ColorFrame();
            frameColor->setColor(limits[i].color);
            legendText  = logicText(limits[i].operation) + " ";
            legendText += QString::number(val1);

            if(i+1<limits.size()){
                val2 = limits[i+1].value;
                legendText +=",";
                legendText += logicText(Less)+" ";
                legendText += QString::number(val2);
            }
            legendText += " "+limits[i].suffix;
            legendLbl  = new QLabel(legendText);

            m_childWidgets.append(frameColor);
            m_childWidgets.append(legendLbl);
            mainLayout->addWidget(frameColor,i+1,0);
            mainLayout->addWidget(legendLbl,i+1,1);
        }
    }
    else if(operate == Less)
    {
        ColorFrame *frameColor;
        QLabel     *legendLbl;
        QString     legendText;
        float val1,val2;

        legendText  = logicText(Big) + " ";
        val1    =     limits.first().value;
        legendText += QString::number(val1)+ " ";
        legendText += limits.first().suffix;
        frameColor = new ColorFrame();
        frameColor->setColor(Qt::green);
        legendLbl  = new QLabel(legendText);
        m_childWidgets.append(frameColor);
        m_childWidgets.append(legendLbl);
        mainLayout->addWidget(frameColor,0,0);
        mainLayout->addWidget(legendLbl,0,1);
        for(int i = 0;i<limits.size();i++){
            legendText.clear();
            val1 = limits[i].value;

            if(i+1<limits.size()){
                val2 = limits[i+1].value;
                legendText += logicText(Big)+" ";
                legendText += QString::number(val2);
                legendText +=",";
            }

            frameColor  = new ColorFrame();
            frameColor->setColor(limits[i].color);
            legendText += logicText(limits[i].operation) + " ";
            legendText += QString::number(val1)+" ";

            legendText += limits[i].suffix;
            legendLbl  = new QLabel(legendText);

            m_childWidgets.append(frameColor);
            m_childWidgets.append(legendLbl);
            mainLayout->addWidget(frameColor,i+1,0);
            mainLayout->addWidget(legendLbl,i+1,1);
        }
    }
}

void LegendBox::initBox()
{
    mainLayout = new QGridLayout;

    this->setLayout(mainLayout);
}

void LegendBox::displayFiltersChanged(const int &filters)
{
    if(m_status != filters){
        m_status = filters;
        //根据LimitDlg 获取对应状态的限制值信息，显示在图例中
        LimitInfo  limitInfo = m_LimitsDlg->getLimitInfo(filters);
        setLimitInfo(limitInfo);
    }
}
void LegendBox::limitChanged()
{
    //根据LimitDlg 获取对应状态的限制值信息，显示在图例中
    LimitInfo  limitInfo = m_LimitsDlg->getLimitInfo(m_status);
    setLimitInfo(limitInfo);
}


RNMSideBar::RNMSideBar(LimitsDialog *dlg,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RNMSideBar),m_limitDialog(dlg)
{
    ui->setupUi(this);

    initWg();

    //信号槽---------------------
    //显示状态切换
    connect(m_filtersBox,SIGNAL(displayFiltersChanged(int)),m_legendBox,SLOT(displayFiltersChanged(int)));
    connect(m_filtersBox,SIGNAL(displayFiltersChanged(int)),this,SIGNAL(displayFiltersChanged(int)));

    connect(m_filtersBox,SIGNAL(reportNodeStatus()),this,SIGNAL(reportNodeStatus()));

    connect(m_limitDialog,SIGNAL(limitsChanged()),m_legendBox,SLOT(limitChanged()));

    m_legendBox->displayFiltersChanged(0);
}

RNMSideBar::~RNMSideBar()
{
    delete ui;
}

void RNMSideBar::setDisplayFilters(const int &status)
{
    m_filtersBox->setDisplayFilters(status);
    m_legendBox->displayFiltersChanged(status);
}

void RNMSideBar::setLimitLegend(const LimitInfo &limitInfo)
{
    m_legendBox->setLimitInfo(limitInfo);
}

void RNMSideBar::setDisplayedNode(const bool &displayed)
{
    m_filtersBox->setDisplayedNodes(displayed);
}

void RNMSideBar::setStatisticsData(const StatisticsData &data)
{
    m_statisticsBox->setStatisticsData(data);
}

void RNMSideBar::initWg()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    m_statisticsBox = new StatisticsBox(this);
    m_filtersBox = new  FiltersBox(this);
    m_legendBox = new  LegendBox(m_limitDialog,this);

    mainLayout->addWidget(m_statisticsBox);
    mainLayout->addWidget(m_filtersBox);
    mainLayout->addWidget(m_legendBox);
    mainLayout->addStretch(1);

    this->setLayout(mainLayout);
}



