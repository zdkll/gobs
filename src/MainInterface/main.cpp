#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QDebug>

#include "logindlg.h"
#include "gpositionsystem.h"

#include "fftw3.h"

using namespace Core;


void FFTtest();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //1 创建程序数据存放目录
    if(!Core::createDocDir()){
        QMessageBox::warning(NULL,"Error","Create Documents File Error.");
        exit(1);
    }
    //    QString icon = QApplication::applicationDirPath()
    //            + "/../icon/gobs.png";
    //    a.setWindowIcon(QIcon(icon));

    //登陆界对话框
    //    LoginDlg dlg;

    //    int ret;
    //    if(dlg.exec()){
    //        MainWindow w;
    //        w.show();
    //      int  ret = a.exec();

    //    GPositionSystem w;

    //    w.show();

    //    }

    //fftw3 test
    FFTtest();

    return a.exec();
}

void FFTtest()
{
    int N = 5;
    fftw_complex *in1_c, *out1_c;//声明复数类型的输入数据in1_c和FFT变换的结果out1_c
    fftw_plan p;//声明变换策略
    in1_c = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* N);//申请动态内存,这里构造二维数组的方式值得学习
    out1_c = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* N);
    p = fftw_plan_dft_1d(N, in1_c, out1_c, FFTW_FORWARD, FFTW_ESTIMATE);//返回变换策略

}
