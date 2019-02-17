#include "exportspsfilesdlg.h"

#include "ui_exportspsfilesdlg.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

ExportSPSFilesDlg::ExportSPSFilesDlg(QWidget *parent)
    :QDialog(parent)
    ,ui(new Ui::ExportSPSFilesDlg)
{
    ui->setupUi(this);

    this->setWindowTitle("Export SPS(SR) Files");
    m_FileType = SFile;

    this->resize(492,570);
}

void ExportSPSFilesDlg::on_shotRbtn_clicked()
{
    if(m_FileType == SFile)
        return;

    m_FileType = SFile;
    ui->InputLbl->setText("Input ShotLine Files:");

    m_rFileList.clear();
    for(int i=0;i<ui->fileListWg->count();i++)
        m_rFileList.append(ui->fileListWg->item(i)->text());

    ui->fileListWg->clear();
    ui->fileListWg->addItems(m_sFileList);
}

void ExportSPSFilesDlg::on_recvRbtn_clicked()
{
    if(m_FileType == RFile)
        return;

    m_FileType = RFile;
    ui->InputLbl->setText("Input RecvLine Files:");

    m_sFileList.clear();
    for(int i=0;i<ui->fileListWg->count();i++)
        m_sFileList.append(ui->fileListWg->item(i)->text());
    ui->fileListWg->clear();
    ui->fileListWg->addItems(m_rFileList);
}

void ExportSPSFilesDlg::on_addFilesBtn_clicked()
{

    //添加文件
    QStringList fileNames = QFileDialog::getOpenFileNames(this,"Input Files",Core::currentDirPath,tr("DAT(*.dat)"));
    if(fileNames.size()<1)
        return;
    Core::currentDirPath = QFileInfo(fileNames[0]).path();

    //判断输入文件是否满足当前要求
    if(m_FileType == SFile)
        foreach (QString file, fileNames) {
            if(!QFileInfo(file).fileName().startsWith("ShotLine"))
                fileNames.removeOne(file);
        }
    if(m_FileType == RFile)
        foreach (QString file, fileNames) {
            if(!QFileInfo(file).fileName().startsWith("RecvLine"))
                fileNames.removeOne(file);
        }
    //
    ui->fileListWg->addItems(fileNames);
    ui->fileListWg->sortItems();
}

void ExportSPSFilesDlg::on_exportBtn_clicked()
{
    if(ui->fileListWg->count()<1)
        return;
    QString filters;
    if(m_FileType == SFile)
        filters = "SFile(*.s *.S)";
    if(m_FileType == RFile)
        filters = "RFile(*.r *.R)";

    //保存文件
    QString outputName = QFileDialog::getSaveFileName(this,"Export File",Core::currentDirPath,filters);
    if(outputName.isEmpty())
        return;
    Core::currentDirPath = QFileInfo(outputName).path();

    bool ok = false;
    if(m_FileType == SFile){
        m_sFileList.clear();
        for(int i=0;i<ui->fileListWg->count();i++)
            m_sFileList.append(ui->fileListWg->item(i)->text());
        ok = createSFile(m_sFileList,outputName);
    }
    if(m_FileType == RFile){
        m_rFileList.clear();
        for(int i=0;i<ui->fileListWg->count();i++)
            m_rFileList.append(ui->fileListWg->item(i)->text());
        ok = createRFile(m_rFileList,outputName);
    }
    if(!ok)
    {
        QMessageBox::warning(this,"Error","Export File failed.");
        return;
    }else
    {
        QMessageBox::warning(this,"Note","Export File success.");
        return;
    }
}

void ExportSPSFilesDlg::on_closeBtn_clicked()
{
    this->close();
}

bool ExportSPSFilesDlg::createSFile(const QStringList &fileList,const QString &outFile)
{
    //打开输出文件---------
    //windows 和 linux 后缀不一样
    //qDebug()<<"create S :"<<outFile;
    QString fileName = outFile;
    if(!outFile.endsWith(".S",Qt::CaseInsensitive))
        fileName += ".S";
    //qDebug()<<fileName;
    QFile outfile(fileName);

    if(!outfile.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        qDebug()<<"open output file failed.";
        return false;
    }
    QTextStream out(&outfile);
    out<<"1234567890123456789012345678901234567890123456789012345678901234567890123456789012\n";
    out<<"         1         2         3         4         5         6         7         8  \n";

    //打开每个输入文件-----
    QFileInfo fileInfo;
    for(int i=0;i<fileList.size();i++)//
    {
        //获取线号
        fileInfo.setFile(fileList[i]);
        int line_no = lineNo(fileInfo.fileName());

        //打开文件,读取每一炮，处理保存
        QFile file(fileList[i]);
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        QStringList strList;
        QString lineStr,shotStr,wdeepStr,x_str,y_str;
        while(!in.atEnd()){
            strList = in.readLine().split(QRegExp("\\s+"));
            //线号---------------
            lineStr = QString::number(line_no);
            out<<"S"<<lineStr+QString(16-lineStr.size(),QChar(' '));//2-17,左对齐

            //炮点号--------------
            shotStr = strList[0]; //18-25，左对齐
            out<<shotStr<<QString(8-shotStr.size(),QChar(' '));

            out<<QString(17,QChar(' '));//26-42
            //水深---------------
            wdeepStr =  strList[3].right(4);//43-46
            out<<wdeepStr<<QString(4-wdeepStr.size(),QChar(' '));

            //坐标-----
            x_str = strList[2].left(9);
            out<<x_str<<QString(9-x_str.size(),QChar(' '));//E x坐标 47-55
            y_str = strList[1].left(10);
            out<<y_str<<QString(10-y_str.size(),QChar(' '));//N y坐标 56-65

            out<<QString(15,QChar(' '))<<"0"<<endl;//80:增加81标识该点是否放炮:0:未放炮;1:放炮
        }
        file.close();
    }

    outfile.close();
    return true;
}

bool ExportSPSFilesDlg::createRFile(const QStringList &fileList,const QString &outFile)
{
    //打开输出文件---------
    QString fileName = outFile;
    if(!outFile.endsWith(".R",Qt::CaseInsensitive))
        fileName += ".R";
    QFile outfile(fileName);
    if(!outfile.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        qDebug()<<"open output file failed.";
        return false;
    }
    QTextStream out(&outfile);
    out<<"1234567890123456789012345678901234567890123456789012345678901234567890123456789012\n";
    out<<"         1         2         3         4         5         6         7         8  \n";

    //打开每个输入文件------
    QFileInfo fileInfo;
    for(int i=0;i<fileList.size();i++)
    {
        //获取线号
        fileInfo.setFile(fileList[i]);
        int line_no = lineNo(fileInfo.fileName());

        //打开文件,读取每一炮，处理保存
        QFile file(fileList[i]);
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        QStringList strList;
        QString lineStr,recvStr,x_str,y_str;//wdeepStr,
        int no = 1;
        while(!in.atEnd()){
            strList = in.readLine().split(QRegExp("\\s+"));
            //线号---------------
            lineStr = QString::number(line_no);
            out<<"R"<<lineStr+QString(16-lineStr.size(),QChar(' '));//2-17,左对齐

            //接收点号--------------
            recvStr = QString::number(no); //18-25，左对齐
            out<<recvStr<<QString(8-recvStr.size(),QChar(' '));

            //out<<QString(17,QChar(' '));//26-42
            //水深---------------
            // wdeepStr =  strList[4].right(4);//43-46
            //out<<wdeepStr<<QString(4-wdeepStr.size(),QChar(' '));

            out<<QString(21,QChar(' '));//26-46
            //坐标-----
            x_str = strList[1].left(9);
            out<<x_str<<QString(9-x_str.size(),QChar(' '));//E x坐标 47-55
            y_str = strList[0].left(10);
            out<<y_str<<QString(10-y_str.size(),QChar(' '));//N y坐标 56-65

            out<<QString(15,QChar(' '))<<endl;//80
            no++;
        }
        file.close();
    }
    outfile.close();

    return true;
}

int  ExportSPSFilesDlg::lineNo(const QString &name)
{
    int left  = name.indexOf("SEQ");
    int right = name.indexOf(".",left);

    return name.mid(left+3,right-left-3).toInt();
}

