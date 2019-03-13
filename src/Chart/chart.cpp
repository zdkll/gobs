#include "chart.h"


Chart::Chart(QWidget *parent)
    :QWidget(parent),m_drawer(new ChartDrawer(this))
{

}
