clc; 
clear;
close all;
N1=42*1024;%N1为每小时一次的采样点数
fs=46544;%采样率
%  fs=23272;
B=4000; 
f0=12000;  %中心频率
timedef=3600;
time=0.01;%length(raw)/fs;%信号
k=B/time; %调频率
% p2=2/pi*(acot(-k*(N1/(fs*fs)))+pi);%计算出理论最优阶
%读原始数据
 dir = 'E:\MatlabCode\FRFT_matlab';
 filename = '4B2CC9B6.ACU';
fid = fopen(fullfile(dir, filename), 'r');
fseek(fid,0,'eof');
fsize=ftell(fid);
datasize=(fsize/3)*2;%数据个数
Data=zeros(datasize,1);
idataindex=1;
fseek(fid,0,'bof');
while ~feof(fid)
    raw = fread(fid, 3,'ubit8');  
    if isempty(raw)
        continue
    end
     strhex=dec2hex(raw);  
     a1=bitshift(bitand(raw(2),15),8)+raw(1); %&0x0f
     a2=bitshift(bitand(raw(2),240),4)+raw(3);
     Data(idataindex,1)=a1;
     Data(idataindex+1,1)=a2;
     idataindex=idataindex+2;
    
end
fclose(fid);

t=(0:length(Data(:,1))-1)/fs;%采集时长
data_mean=detrend(Data);%去直流分量

%带通滤波10k-14k 
Hd=bandpass10to14;
output_cut=filter(Hd,data_mean);
% figure(1)
% plot(output_cut);

% %挑选chirp信号
% newdata=[];
% idatacount=length(output_cut)/N1;
% for i=1:idatacount
%     
%     y1=output_cut((i-1)*N1+1:i*N1);
%     if max(y1)>2000
%         if isempty(newdata)==1
%             newdata=y1; 
%             tempdata=y1;
%         else
%             newdata=cat(1,newdata,y1);
%         end
%  
%     end
%     
% end
%  load E:\MatlabCode\FRFT_matlab\outputcut(2).mat;
% load C:\Users\yxhu\Desktop\test-signal.mat
 output_cut=hilbert(output_cut);%希尔伯特变换将实信号转换成复数信号
% output_cut=output_cut(72*N1+1:end);
%计算出理论最优阶
a0=atan(-fs^2/(N1*k));
p0=2*a0/pi;
p1=[];
h=0;
r=0.01;%二维搜索步长
a=p0+[-0.2:r:0.2];%fractional power
idatacount=length(output_cut)/N1;
for i=1:5
    for j=1:length(a)
       y1=output_cut((i-1)*N1+1:i*N1);
       T=frft(y1,a(j)); 
       if(h<=max(abs(T(:))))
           h=max(abs(T(:)));
           pfind=a(j);%实际阶数
       end
    end
    p1=[p1,pfind];
end
popt=mean(p1);%取若干天最佳阶数的平均值
aopt=pi*popt/2;%最佳旋转角度


icount=length(output_cut)/N1;%N1=43008：每次采集一次数据个数 
for i=1:icount
    y1=output_cut((i-1)*N1+1:i*N1);%扫过的定长观测信号序列
     q((i-1)*N1+1:i*N1)=frft(y1,popt);%先按理论解
%      q1((i-1)*N1+1:i*N1)=sum(abs(q(:)).^2);%功率谱
end


figure(5)
subplot(2,1,1);%观测信号
plot(real(output_cut(:)));
title('观测信号')
subplot(2,1,2);
plot(abs(q(:)));% 这里画的模最大值
title('振幅')

%计算frft后最大振幅的位置和对应时间

% signtime=zeros(icount,3);%信号到时估计
signindex=1;%信号索引
signtime=[];
number=0;
for i=0:icount-1 
     x_cut=abs(q(i*N1+1:(i+1)*N1));%每次收到的信号所做变换
    [amax,m]=max(x_cut);%找到最大振幅对应位置
    if (amax>400)%振幅大于400认为是chirp信号，振幅数值的大小根据性噪比改变
        number=number+1;
        m1=N1-m;
         signtime(signindex,1)=m1;
%           signtime(signindex,1)=m;
        if signindex==1
           signtime(signindex,2)=0;
        else
            s0=(m1- signtime(signindex-1,1));
            signdiff=fix(s0/abs(cos(aopt)));%得到这一小时数据插值点数
            signtime(signindex,2)=signdiff;
            signtime(signindex,3)=signdiff/fs;
        end
        signindex=signindex+1;
    else
        signindex=signindex+1;
    end
end
% for i=2:icount
%     signtime(i,3)=signtime(i,2)-signtime(2,2);
% end


