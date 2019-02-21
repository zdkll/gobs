#include "segy.h"
#include <QDebug>
GobsInterface *GobsInterface::GobsInface = NULL;

GobsInterface::GobsInterface()
{
}

//====打开SEGY数据并返回3200、400字节卷头
int GobsInterface::OpenSEGYFileFunc(OpenTrace *openTr, QString SEGYFName, int Mode, char *Bna3200, BinaryHead *BinHead, SegyInfo *DtInfo)
{
    if(Mode < Read_Only || Mode > Read_Write)
        return -1;
    if(NULL == Bna3200 || NULL == BinHead || NULL == DtInfo)
        return -2;
    if(SEGYFName.isEmpty())
        return -3;

    if(Read_Only == Mode)
    {
        QFileInfo SgyInfo(SEGYFName);//===当文件不存在返回错误
        if(!SgyInfo.exists())
            return -4;
        openTr->DatafName = SEGYFName;

        FILE *fpSEGY = fopen(SEGYFName.toStdString().c_str(), "rb");
        if(NULL == fpSEGY)
            return -5;
        openTr->fpData = fpSEGY;

        fread(Bna3200, CHAR_1, BINARY3200, fpSEGY);
        EBD2QStr(Bna3200);//==转换编码

        fread(BinHead, CHAR_1, BINARY400,  fpSEGY);

        openTr->g_Endian = LITENDI;
        if(BinHead->nt < 0 || BinHead->mudt < 0 || BinHead->samptype > 10)
        {
            openTr->g_Endian = BIGENDI;
            CreateBinHead(BinHead);

        }
        openTr->ns              = BinHead->nt;
        DtInfo->ns              = (int)BinHead->nt;
        DtInfo->sampleInterval  = (int)BinHead->mudt;

        switch(BinHead->samptype)
        {
        case 1:
        case 2:
        case 4:
        case 5: openTr->DataLen = FLOAT_4; break;
        case 3: openTr->DataLen = SHORT_2; break;
        case 8: openTr->DataLen = CHAR_1;  break;
        default:openTr->DataLen = FLOAT_4;
        }
        openTr->dtype       = BinHead->samptype;
        openTr->DataLen     = openTr->DataLen*DtInfo->ns;
        DtInfo->traces      = (SgyInfo.size()-3600)/(HEADLENTH+openTr->DataLen);
        openTr->TotalTrace  = DtInfo->traces;
        return 0;
    }
    else if(Write_Only == Mode)
    {
        //===获取当前数据信息
        openTr->ns          = DtInfo->ns;
        openTr->dtype       = DtInfo->sampleInterval;
        openTr->TotalTrace   = DtInfo->traces;
        switch(openTr->dtype)
        {
        case 1:
        case 2:
        case 4:
        case 5: openTr->DataLen = FLOAT_4; break;
        case 3: openTr->DataLen = SHORT_2; break;
        case 8: openTr->DataLen = CHAR_1;  break;
        default:openTr->DataLen = FLOAT_4;
        }
        openTr->DataLen = DtInfo->ns*openTr->DataLen;

        char *tmpBin3200 = new char[BINARY3200];
        BinaryHead tmpBin400;;
        strcpy(tmpBin3200, Bna3200);
        memcpy(&tmpBin400, BinHead, BINARY400);

        QStr2EBD(tmpBin3200);

        FILE *fpSEGY = fopen(SEGYFName.toStdString().c_str(), "wb");
        if(NULL == fpSEGY)
        {
            delete [] tmpBin3200;
            qDebug() << "这里返回的错误？？？";
            return -1;
        }
        openTr->DatafName = SEGYFName;
        openTr->fpData    = fpSEGY;
        openTr->TraceNum  = 0;

        //===将400字节卷头转换为大端/IBM数据
        tmpBin400.nt   = (short)DtInfo->ns;
        tmpBin400.mudt = (short)DtInfo->sampleInterval;
        WriteGetBinHead400(&tmpBin400);

        //====写卷头
        fwrite(tmpBin3200, CHAR_1, BINARY3200, fpSEGY);//==写3200字节卷头
        fwrite(&tmpBin400, CHAR_1, BINARY400,  fpSEGY);//===写400字节卷头

        delete [] tmpBin3200;
        return 0;
    }
    return -10;
}

//====写数据时将3200字节卷头转换编码
void GobsInterface::QStr2EBD(char *AscData)
{
    char *OutEbcDic = new char[3200];
    for( int i = 0; i < 3200; i ++)
        OutEbcDic[i] = a2e[(unsigned char)AscData[i]];
    memcpy(AscData, OutEbcDic, 3200);
    delete [] OutEbcDic; OutEbcDic = NULL;
}

//====读数据时将3200字节卷头转换编码
void GobsInterface::EBD2QStr(char *source)
{
    char *tmpS = source;
    char *dest = new char[3200];
    memset(dest, 0, 3200);
    for(int i = 0; i < BINARY3200; i++, source++)
    {
        *dest = e2a[(unsigned char)(*source)];
        dest ++;
    }
    dest = dest - 3200;
    memcpy(tmpS, dest, BINARY3200);
    delete [] dest; dest = NULL;
}

//====将400字节接卷头小端转换为大端
void GobsInterface::WriteGetBinHead400(BinaryHead *BinHead_T)
{
    BinHead_T->ntrace    = htons(BinHead_T->ntrace);
    BinHead_T->naux      = htons(BinHead_T->naux);
    BinHead_T->mudt      = htons(BinHead_T->mudt);
    BinHead_T->omudt     = htons(BinHead_T->omudt);
    BinHead_T->nt        = htons(BinHead_T->nt);
    BinHead_T->ont       = htons(BinHead_T->ont);
    BinHead_T->samptype  = htons(BinHead_T->samptype);
    BinHead_T->fold      = htons(BinHead_T->fold);
    BinHead_T->sort      = htons(BinHead_T->sort);
    BinHead_T->sums      = htons(BinHead_T->sums);
    BinHead_T->sweep0    = htons(BinHead_T->sweep0);
    BinHead_T->sweepf    = htons(BinHead_T->sweepf);
    BinHead_T->sweepl    = htons(BinHead_T->sweepl);
    BinHead_T->sweept    = htons(BinHead_T->sweept);
    BinHead_T->sweeptr   = htons(BinHead_T->sweeptr);
    BinHead_T->taper0    = htons(BinHead_T->taper0);
    BinHead_T->taperf    = htons(BinHead_T->taperf);
    BinHead_T->tapert    = htons(BinHead_T->tapert);
    BinHead_T->cor       = htons(BinHead_T->cor);
    BinHead_T->gain      = htons(BinHead_T->gain);
    BinHead_T->recov     = htons(BinHead_T->recov);
    BinHead_T->units     = htons(BinHead_T->units);
    BinHead_T->pol       = htons(BinHead_T->pol);
    BinHead_T->vibpol    = htons(BinHead_T->vibpol);
}

//====将400字节卷头大端转换为小端
void GobsInterface::CreateBinHead(BinaryHead *BinHead_T)
{
    BinHead_T->jobid     = ntohl(BinHead_T->jobid);
    BinHead_T->lineno    = ntohl(BinHead_T->lineno);
    BinHead_T->reelno    = ntohl(BinHead_T->reelno);
    BinHead_T->ntrace    = ntohs(BinHead_T->ntrace);
    BinHead_T->naux      = ntohs(BinHead_T->naux);
    BinHead_T->mudt      = ntohs(BinHead_T->mudt);
    BinHead_T->omudt     = ntohs(BinHead_T->omudt);
    BinHead_T->nt        = ntohs(BinHead_T->nt);
    BinHead_T->ont       = ntohs(BinHead_T->ont);
    BinHead_T->samptype  = ntohs(BinHead_T->samptype);
    BinHead_T->fold      = ntohs(BinHead_T->fold);
    BinHead_T->sort      = ntohs(BinHead_T->sort);
    BinHead_T->sums      = ntohs(BinHead_T->sums);
    BinHead_T->sweep0    = ntohs(BinHead_T->sweep0);
    BinHead_T->sweepf    = ntohs(BinHead_T->sweepf);
    BinHead_T->sweepl    = ntohs(BinHead_T->sweepl);
    BinHead_T->sweept    = ntohs(BinHead_T->sweept);
    BinHead_T->sweeptr   = ntohs(BinHead_T->sweeptr);
    BinHead_T->taper0    = ntohs(BinHead_T->taper0);
    BinHead_T->taperf    = ntohs(BinHead_T->taperf);
    BinHead_T->tapert    = ntohs(BinHead_T->tapert);
    BinHead_T->cor       = ntohs(BinHead_T->cor);
    BinHead_T->gain      = ntohs(BinHead_T->gain);
    BinHead_T->recov     = ntohs(BinHead_T->recov);
    BinHead_T->units     = ntohs(BinHead_T->units);
    BinHead_T->pol       = ntohs(BinHead_T->pol);
    BinHead_T->vibpol    = ntohs(BinHead_T->vibpol);
    BinHead_T->num_samps = ntohl(BinHead_T->num_samps);
    for( int i = 0; i < 84; i++ )
        BinHead_T->unass[i] = ntohl(BinHead_T->unass[i]);
}



//===读取SEGY数据
int GobsInterface::ReadSEGYDataFunc(OpenTrace *openTr, long long statnum, long long trnum, TraceHead *trhead, char *trdata, long long *currtr)
{
    //===需要读取的道超过了整个数据的总道数
    if(statnum >= openTr->TotalTrace)
        return -15;

    //===道头空间或者数据空间指针为NULL
    if(NULL == trhead || NULL == trdata)
        return 16;

    //===当读取数据位置超过了整个数据
    long long rdTrnum = 0;
    if(statnum+trnum >= openTr->TotalTrace)
        rdTrnum = openTr->TotalTrace-statnum;
    else
        rdTrnum = trnum;


    int TDataLen = HEADLENTH+openTr->DataLen;//===SEGY道头加数据长度
    int CurrRdTr = INT_MAX/TDataLen-1;   //===将一次读取的数据按照2G多次来进行读取，2G读取道数
    int ReadNum, RdRemdNum;
    if(rdTrnum > CurrRdTr)  //===当读取的道数的总长度小于2G
    {
        ReadNum     = rdTrnum/CurrRdTr;//===读取次数
        RdRemdNum   = rdTrnum%CurrRdTr;//===读取余数
        if(0 != RdRemdNum)
            ReadNum ++;
    }
    else
    {
        ReadNum     = 1;
        RdRemdNum   = rdTrnum;
        CurrRdTr    = rdTrnum;
    }

    long long skipLenth   = statnum*TDataLen+BINARY3600;
    fseek(openTr->fpData, skipLenth, SEEK_SET);

    long long CurrReadLen = (long long)CurrRdTr*TDataLen;
    char *ReadBuff = new char[CurrReadLen];//===单次读取数据空间
    if(NULL == ReadBuff)
        return -10;

    long long StaTrNum = 0, CurrHead, CurrData;
    for(int ReadNO = 0; ReadNO < ReadNum; ReadNO ++)
    {
        int TmpReadTrNum;
        if(ReadNO == ReadNum-1 && RdRemdNum != 0)
            TmpReadTrNum = RdRemdNum;
        else
            TmpReadTrNum = CurrRdTr;

        long long TmpReadDtLen = (long long)TmpReadTrNum*TDataLen;
        fread(ReadBuff, CHAR_1, TmpReadDtLen, openTr->fpData);

        long long tmpCurrHead, tmpCurrData;
        for(long long itr = 0; itr < TmpReadTrNum; itr ++)
        {
            CurrHead = StaTrNum*HEADLENTH;
            CurrData = StaTrNum*openTr->DataLen;

            char *tmpHead = ((char*)trhead)+CurrHead;
            char *tmpData = trdata+CurrData;

            tmpCurrHead = itr*TDataLen;
            tmpCurrData = itr*TDataLen+HEADLENTH;

            memcpy(tmpHead, ReadBuff+tmpCurrHead, HEADLENTH);
            memcpy(tmpData, ReadBuff+tmpCurrData, openTr->DataLen);
            StaTrNum ++;
        }
    }
    *currtr = StaTrNum;
    //===转换小端
    if(openTr->g_Endian == BIGENDI)
        ChangeTraceHeadBigEndian(trhead, StaTrNum);
    ChangeTraceDataBigEndian(openTr, trdata, StaTrNum);

    delete [] ReadBuff; ReadBuff = NULL;
    return 0;
}

//===数据转换大小端、IBM2IEEE
void GobsInterface::ChangeTraceDataBigEndian(OpenTrace *openTr, char *tracedata, long long CurrTrNum)
{
    switch(openTr->dtype)
    {
    case 1:
    {
        int *tmpDt = new int[openTr->ns];
        int *tmpInt= NULL;
        for(long long itr = 0; itr < CurrTrNum; itr ++)
        {
            tmpInt = (int*)(tracedata+itr*openTr->DataLen);
            if(BIGENDI == openTr->g_Endian)
            {
                for(int ins = 0; ins < openTr->ns; ins ++)
                    tmpInt[ins] = ntohl(tmpInt[ins]);
            }
            memcpy(tmpDt, tracedata+itr*openTr->DataLen, openTr->DataLen);
            ibm_to_ieee(tmpDt, tmpInt, openTr->ns);
        }
        delete [] tmpDt; tmpDt = NULL;
    }break;
    case 2:
    {
        int * tmpInt = NULL;
        for(long long itr = 0; itr < CurrTrNum; itr ++)
        {
            tmpInt = (int*)(tracedata+itr*openTr->DataLen);
            if(BIGENDI == openTr->g_Endian)
            {
                for(int ins = 0; ins < openTr->ns; ins ++)
                    tmpInt[ins] = ntohl(tmpInt[ins]);
            }
        }
    }break;
    case 3:
    {
        short *tmpShort = NULL;
        for(long long itr = 0; itr < CurrTrNum; itr ++)
        {
            tmpShort = (short *)(tracedata+itr*openTr->DataLen);
            if(BIGENDI == openTr->g_Endian)
            {
                for(int ins = 0; ins < openTr->ns; ins ++)
                    tmpShort[ins] = ntohs(tmpShort[ins]);
            }
        }
    }break;
    case 8:
        break;
    default:
    {
        int *tmpInt= NULL;
        for(long long itr = 0; itr < CurrTrNum; itr ++)
        {
            tmpInt = (int*)(tracedata+itr*openTr->DataLen);
            if(BIGENDI == openTr->g_Endian)
            {
                for(int ins = 0; ins < openTr->ns; ins ++)
                    tmpInt[ins] = ntohl(tmpInt[ins]);
            }
        }
    }
    }
}

//===将IBM数据转换为IEEE数据
void GobsInterface::ibm_to_ieee( int *from, int *to, int n )
{
    register int i, t, fconv, fmant;
    for( i = 0; i < n; ++i )
    {
        fconv = from[i];
        if( fconv )
        {
            fmant = 0x00ffffff & fconv;
            /* The next two lines were added by Toralf Foerster */
            /* to trap non-IBM format data i.e. conv=0 data  */
            if( fmant == 0 ) break;
            t = (int) ((0x7f000000 & fconv) >> 22) - 130;
            while( !(fmant & 0x00800000) )
            {
                --t;
                fmant <<= 1;
            }
            if( t > 254 )
                fconv = (0x80000000 & fconv) | 0x7f7fffff;
            else if( t <= 0 ) fconv = 0;
            else
                fconv = (0x80000000 & fconv) |(t << 23)|(0x007fffff & fmant);
        }
        to[i] = fconv;
    }
    return;
}

//====对道头数据将大端转换为小端
void GobsInterface::ChangeTraceHeadBigEndian(TraceHead *g_TrHead_240, long long CurrTrNum)
{
    for(long long TrNO = 0; TrNO < CurrTrNum; TrNO ++)
    {
        g_TrHead_240[TrNO].tracl  = ntohl(g_TrHead_240[TrNO].tracl );
        g_TrHead_240[TrNO].tracr  = ntohl(g_TrHead_240[TrNO].tracr );
        g_TrHead_240[TrNO].fldr   = ntohl(g_TrHead_240[TrNO].fldr );
        g_TrHead_240[TrNO].tracf  = ntohl(g_TrHead_240[TrNO].tracf );
        g_TrHead_240[TrNO].ep     = ntohl(g_TrHead_240[TrNO].ep );
        g_TrHead_240[TrNO].cdp    = ntohl(g_TrHead_240[TrNO].cdp );
        g_TrHead_240[TrNO].cdpt   = ntohl(g_TrHead_240[TrNO].cdpt);
        g_TrHead_240[TrNO].trid   = ntohs(g_TrHead_240[TrNO].trid );
        g_TrHead_240[TrNO].nvs    = ntohs(g_TrHead_240[TrNO].nvs );
        g_TrHead_240[TrNO].nhs    = ntohs(g_TrHead_240[TrNO].nhs );
        g_TrHead_240[TrNO].duse   = ntohs(g_TrHead_240[TrNO].duse );
        g_TrHead_240[TrNO].offset = ntohl(g_TrHead_240[TrNO].offset );
        g_TrHead_240[TrNO].gelev  = ntohl(g_TrHead_240[TrNO].gelev );
        g_TrHead_240[TrNO].selev  = ntohl(g_TrHead_240[TrNO].selev );
        g_TrHead_240[TrNO].sdepth = ntohl(g_TrHead_240[TrNO].sdepth );
        g_TrHead_240[TrNO].gdel   = ntohl(g_TrHead_240[TrNO].gdel );
        g_TrHead_240[TrNO].sdel   = ntohl(g_TrHead_240[TrNO].sdel );
        g_TrHead_240[TrNO].swdep  = ntohl(g_TrHead_240[TrNO].swdep );
        g_TrHead_240[TrNO].gwdep  = ntohl(g_TrHead_240[TrNO].gwdep );
        g_TrHead_240[TrNO].scalel = ntohs(g_TrHead_240[TrNO].scalel );
        g_TrHead_240[TrNO].scalco = ntohs(g_TrHead_240[TrNO].scalco );
        g_TrHead_240[TrNO].sx     = ntohl(g_TrHead_240[TrNO].sx );
        g_TrHead_240[TrNO].sy     = ntohl(g_TrHead_240[TrNO].sy );
        g_TrHead_240[TrNO].gx     = ntohl(g_TrHead_240[TrNO].gx );
        g_TrHead_240[TrNO].gy     = ntohl( g_TrHead_240[TrNO].gy );
        g_TrHead_240[TrNO].counit = ntohs( g_TrHead_240[TrNO].counit );
        g_TrHead_240[TrNO].wevel  = ntohs( g_TrHead_240[TrNO].wevel );
        g_TrHead_240[TrNO].swevel = ntohs( g_TrHead_240[TrNO].swevel );
        g_TrHead_240[TrNO].sut    = ntohs( g_TrHead_240[TrNO].sut );
        g_TrHead_240[TrNO].gut    = ntohs( g_TrHead_240[TrNO].gut );
        g_TrHead_240[TrNO].sstat  = ntohs( g_TrHead_240[TrNO].sstat );
        g_TrHead_240[TrNO].gstat  = ntohs( g_TrHead_240[TrNO].gstat );
        g_TrHead_240[TrNO].tstat  = ntohs( g_TrHead_240[TrNO].tstat );
        g_TrHead_240[TrNO].laga   = ntohs( g_TrHead_240[TrNO].laga );
        g_TrHead_240[TrNO].lagb   = ntohs( g_TrHead_240[TrNO].lagb );
        g_TrHead_240[TrNO].delrt  = ntohs( g_TrHead_240[TrNO].delrt );
        g_TrHead_240[TrNO].muts   = ntohs( g_TrHead_240[TrNO].muts );
        g_TrHead_240[TrNO].mute   = ntohs( g_TrHead_240[TrNO].mute );
        g_TrHead_240[TrNO].ns     = ntohs( g_TrHead_240[TrNO].ns );
        g_TrHead_240[TrNO].dt     = ntohs( g_TrHead_240[TrNO].dt );
        g_TrHead_240[TrNO].gain   = ntohs( g_TrHead_240[TrNO].gain );
        g_TrHead_240[TrNO].igc    = ntohs( g_TrHead_240[TrNO].igc );
        g_TrHead_240[TrNO].igi    = ntohs( g_TrHead_240[TrNO].igi );
        g_TrHead_240[TrNO].corr   = ntohs( g_TrHead_240[TrNO].corr );
        g_TrHead_240[TrNO].sfs    = ntohs( g_TrHead_240[TrNO].sfs );
        g_TrHead_240[TrNO].sfe    = ntohs( g_TrHead_240[TrNO].sfe );
        g_TrHead_240[TrNO].slen   = ntohs( g_TrHead_240[TrNO].slen );
        g_TrHead_240[TrNO].styp   = ntohs( g_TrHead_240[TrNO].styp );
        g_TrHead_240[TrNO].stas   = ntohs( g_TrHead_240[TrNO].stas );
        g_TrHead_240[TrNO].stae   = ntohs( g_TrHead_240[TrNO].stae );
        g_TrHead_240[TrNO].tatyp  = ntohs( g_TrHead_240[TrNO].tatyp );
        g_TrHead_240[TrNO].afilf  = ntohs( g_TrHead_240[TrNO].afilf );
        g_TrHead_240[TrNO].afils  = ntohs( g_TrHead_240[TrNO].afils );
        g_TrHead_240[TrNO].nofilf = ntohs( g_TrHead_240[TrNO].nofilf );
        g_TrHead_240[TrNO].nofils = ntohs( g_TrHead_240[TrNO].nofils );
        g_TrHead_240[TrNO].lcf    = ntohs( g_TrHead_240[TrNO].lcf );
        g_TrHead_240[TrNO].hcf    = ntohs( g_TrHead_240[TrNO].hcf );
        g_TrHead_240[TrNO].lcs    = ntohs( g_TrHead_240[TrNO].lcs );
        g_TrHead_240[TrNO].hcs    = ntohs( g_TrHead_240[TrNO].hcs );
        g_TrHead_240[TrNO].year   = ntohs( g_TrHead_240[TrNO].year );
        g_TrHead_240[TrNO].day    = ntohs( g_TrHead_240[TrNO].day );
        g_TrHead_240[TrNO].hour   = ntohs( g_TrHead_240[TrNO].hour );
        g_TrHead_240[TrNO].minute = ntohs( g_TrHead_240[TrNO].minute );
        g_TrHead_240[TrNO].sec    = ntohs( g_TrHead_240[TrNO].sec );
        g_TrHead_240[TrNO].timbas = ntohs( g_TrHead_240[TrNO].timbas );
        g_TrHead_240[TrNO].trwf   = ntohs( g_TrHead_240[TrNO].trwf );
        g_TrHead_240[TrNO].grnors = ntohs( g_TrHead_240[TrNO].grnors );
        g_TrHead_240[TrNO].grnofr = ntohs( g_TrHead_240[TrNO].grnofr );
        g_TrHead_240[TrNO].grnlof = ntohs( g_TrHead_240[TrNO].grnlof );
        g_TrHead_240[TrNO].gaps   = ntohs( g_TrHead_240[TrNO].gaps );
        g_TrHead_240[TrNO].otrav  = ntohs( g_TrHead_240[TrNO].otrav );
        for( int i = 0; i < 15; i++ )
            g_TrHead_240[TrNO].unass[i] = ntohl(g_TrHead_240[TrNO].unass[i]);
    }
}


//====关闭文件
int GobsInterface::CloseSEGYDataFunc(OpenTrace *openTr)
{
    if(NULL != openTr->fpData)
        fclose(openTr->fpData);
    return 0;
}

//====写数据
int GobsInterface::WriteSEGYDataFunc(OpenTrace *openTr, TraceHead *trhead, char *trdata)
{
    openTr->TraceNum ++;
    trhead->tracr = openTr->TraceNum;
    TraceHead tmpHead;
    char     *tmpData = new char[openTr->DataLen];
    memcpy(&tmpHead, trhead, sizeof(TraceHead));
    memcpy(tmpData,  trdata, openTr->DataLen);

    trhead->offset= sqrt(pow((trhead->sx-trhead->gx), 2) + pow((trhead->sy-trhead->gy), 2));

    //===将小端数据转换为大端
    ChangeTraceHeadLittleEndian(&tmpHead);
    ieee_to_ibm((int*)tmpData, (int*)tmpData, openTr->ns);
    changeEnd(tmpData, openTr->ns);

    //====将数据复制到内存中其他空间中,方便写
    int DataSize    = openTr->DataLen+HEADLENTH;
    char *tmpDtBuff = new char[DataSize];
    if(NULL == tmpDtBuff)
    {
        qDebug() << "1111111111111111111111";
        return -10;
    }
    memcpy(tmpDtBuff, &tmpHead, HEADLENTH);
    memcpy(tmpDtBuff+HEADLENTH, tmpData, openTr->DataLen);
    delete [] tmpData;

    //===写数据
    fwrite(tmpDtBuff, CHAR_1, DataSize, openTr->fpData);

    delete [] tmpDtBuff;
    return 0;
}

//===
void GobsInterface::changeEnd(char *trace_data, int ns)
{
    int i = 0;
    int *iOut = (int *)trace_data;
    for( i = 0; i < ns; i ++ )
    {
        iOut[i] = htonl(iOut[i]);
    }
}

void GobsInterface::ieee_to_ibm(int from[], int to[], int n)
{
    register int fconv, fmant, ii, t;
    for ( ii = 0; ii < n; ++ii )
    {
        fconv = from[ii];
        if ( fconv )
        {
            fmant = (0x007fffff & fconv) | 0x00800000;
            t = (int) ((0x7f800000 & fconv) >> 23) - 126;
            while (t & 0x3 )
            {
                ++t;
                fmant >>= 1;
            }
            fconv = (0x80000000 & fconv) | (((t>>2) + 64) << 24) | fmant;
        }
        to[ii] = fconv;
    }
    return;
}

//====写数据时将小端数据转换为大端数据====道头
void GobsInterface::ChangeTraceHeadLittleEndian(TraceHead *trhead)
{
    trhead->tracl  = htonl(trhead->tracl );
    trhead->tracr  = htonl(trhead->tracr );
    trhead->fldr   = htonl(trhead->fldr );
    trhead->tracf  = htonl(trhead->tracf );
    trhead->ep     = htonl(trhead->ep );
    trhead->cdp    = htonl(trhead->cdp );
    trhead->cdpt   = htonl(trhead->cdpt );
    trhead->trid   = htons(trhead->trid );
    trhead->nvs    = htons(trhead->nvs );
    trhead->nhs    = htons(trhead->nhs );
    trhead->duse   = htons(trhead->duse );
    trhead->offset = htonl(trhead->offset );
    trhead->gelev  = htonl(trhead->gelev );
    trhead->selev  = htonl(trhead->selev );
    trhead->sdepth = htonl(trhead->sdepth );
    trhead->gdel   = htonl(trhead->gdel );
    trhead->sdel   = htonl(trhead->sdel );
    trhead->swdep  = htonl(trhead->swdep );
    trhead->gwdep  = htonl(trhead->gwdep );
    trhead->scalel = htons(trhead->scalel );
    trhead->scalco = htons(trhead->scalco );
    trhead->sx     = htonl(trhead->sx );
    trhead->sy     = htonl(trhead->sy );
    trhead->gx     = htonl(trhead->gx );
    trhead->gy     = htonl( trhead->gy );
    trhead->counit = htons( trhead->counit );
    trhead->wevel  = htons( trhead->wevel );
    trhead->swevel = htons( trhead->swevel );
    trhead->sut    = htons( trhead->sut );
    trhead->gut    = htons( trhead->gut );
    trhead->sstat  = htons( trhead->sstat );
    trhead->gstat  = htons( trhead->gstat );
    trhead->tstat  = htons( trhead->tstat );
    trhead->laga   = htons( trhead->laga );
    trhead->lagb   = htons( trhead->lagb );
    trhead->delrt  = htons( trhead->delrt );
    trhead->muts   = htons( trhead->muts );
    trhead->mute   = htons( trhead->mute );
    trhead->ns     = htons( trhead->ns );
    trhead->dt     = htons( trhead->dt );
    trhead->gain   = htons( trhead->gain );
    trhead->igc    = htons( trhead->igc );
    trhead->igi    = htons( trhead->igi );
    trhead->corr   = htons( trhead->corr );
    trhead->sfs    = htons( trhead->sfs );
    trhead->sfe    = htons( trhead->sfe );
    trhead->slen   = htons( trhead->slen );
    trhead->styp   = htons( trhead->styp );
    trhead->stas   = htons( trhead->stas );
    trhead->stae   = htons( trhead->stae );
    trhead->tatyp  = htons( trhead->tatyp );
    trhead->afilf  = htons( trhead->afilf );
    trhead->afils  = htons( trhead->afils );
    trhead->nofilf = htons( trhead->nofilf );
    trhead->nofils = htons( trhead->nofils );
    trhead->lcf    = htons( trhead->lcf );
    trhead->hcf    = htons( trhead->hcf );
    trhead->lcs    = htons( trhead->lcs );
    trhead->hcs    = htons( trhead->hcs );
    trhead->year   = htons( trhead->year );
    trhead->day    = htons( trhead->day );
    trhead->hour   = htons( trhead->hour );
    trhead->minute = htons( trhead->minute );
    trhead->sec    = htons( trhead->sec );
    trhead->timbas = htons( trhead->timbas );
    trhead->trwf   = htons( trhead->trwf );
    trhead->grnors = htons( trhead->grnors );
    trhead->grnofr = htons( trhead->grnofr );
    trhead->grnlof = htons( trhead->grnlof );
    trhead->gaps   = htons( trhead->gaps );
    trhead->otrav  = htons( trhead->otrav );
    for( int i = 0; i < 15; i++ )
        trhead->unass[i] = htonl(trhead->unass[i]);
}




//===========================================================================================================================================
//===打开SEGY数据接口
//===参数说明:
//=== path      ： 文件存放路径
//=== filename  ： 文件名
//=== mode      ： 打开文件模式 0:只读;1:只写;2:读写
//=== Bnary3200 : 3200字节卷头
//=== binary_head：读打开是返回400字节卷头信息,写打开时将400字节卷头写入到SEGY数据文件中
//=== datainfo  ： 当前数据部分信息
//=== fileptr   ： 返回值，文件句柄，用于读写
//=== ok        ： 返回值，成功返回0,失败返回非0
OpenTrace *gobs_open_segy_func(const char*filename, int *mode, char *Bnary3200, BinaryHead *binary_head, SegyInfo *datainfo, int *ok)
{
    QString FName = QString(filename);
    GobsInterface *RdSEGY = GobsInterface::getInstance();
    OpenTrace  *openTr = new OpenTrace;
    *ok = RdSEGY->OpenSEGYFileFunc(openTr, FName, *mode, Bnary3200, binary_head, datainfo);
    if(*ok!=0) {
        delete openTr;
        return NULL;
    }
    return openTr;
}

//===读取SEGY数据接口
//===参数说明:
//=== fileptr ： open时返回的文件句柄
//=== statnum ： 读取数据的起始道
//=== trnum   ： 读取数据的道数
//=== headbuff:  存放道头
//=== databuff:  存放数据
//=== currtrnum: 实际读取的道数
//=== ok      ： 返回值，成功返回0，失败返回非0
void gobs_read_segy_func(OpenTrace *openTr, long long statnum, long long trnum, TraceHead *headbuff, char *databuff, long long *currtrnum, int *ok)
{
    if(NULL == openTr)
    {
        qDebug() << "文件信息指针为NULL。";
        *ok = 10;
        return ;
    }
    GobsInterface *RdSEGY = GobsInterface::getInstance();
    *ok = RdSEGY->ReadSEGYDataFunc(openTr, statnum, trnum, headbuff, databuff, currtrnum);
}

//===写SEGY数据接口
//===参数说明:
//=== fileptr ： open时返回的文件句柄
//=== statnum ： 写数据的起始道
//=== headbuff:  道头
//=== databuff:  数据
//=== ok      ： 返回值，成功返回0，失败返回非0
void gobs_write_segy_func(OpenTrace *openTr, TraceHead *headbuff, char *databuff, int *ok)
{
    if(NULL == openTr)
    {
        qDebug() << "文件信息指针为NULL";
        *ok = 11;
        return ;
    }
    GobsInterface *RdSEGY = GobsInterface::getInstance();
    *ok = RdSEGY->WriteSEGYDataFunc(openTr, headbuff, databuff);
}


//===关闭open时打开的文件
//===参数说明
//===   fileptr : open时返回的文件句柄
void gobs_close_segy_func(OpenTrace *openTr , int *ok)
{
    if(NULL == openTr)
    {
        qDebug() << "文件信息指针为NULL。";
        *ok = 12;
        return ;
    }
    GobsInterface *RdSEGY = GobsInterface::getInstance();
    *ok = RdSEGY->CloseSEGYDataFunc(openTr);
    delete openTr;
    openTr = NULL;
}





























