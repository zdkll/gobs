#ifndef EXPORTSPSFILESDLG_H
#define EXPORTSPSFILESDLG_H

#include <QDialog>
#include "gcore.h"

namespace Ui {
class ExportSPSFilesDlg;
}
class ExportSPSFilesDlg : public QDialog
{
    Q_OBJECT
public:
    enum SPSFileType
    {
        SFile =0,
        RFile = 1,
        XFile = 1
    };
    ExportSPSFilesDlg(QWidget *parent = 0);

private slots:
    void on_shotRbtn_clicked();

    void on_recvRbtn_clicked();

    void on_addFilesBtn_clicked();

    void on_exportBtn_clicked();

    void on_closeBtn_clicked();

private:
    //生成S File
    bool createSFile(const QStringList &fileList,const QString &outFile);
    //生成R File
    bool createRFile(const QStringList &fileList,const QString &outFile);

    //获取线号
    int  lineNo(const QString &name);

private:
    Ui::ExportSPSFilesDlg *ui;
    SPSFileType  m_FileType;
    QString      m_currentPath;
    QStringList  m_sFileList,m_rFileList;
};

#endif // EXPORTSPSFILESDLG_H
