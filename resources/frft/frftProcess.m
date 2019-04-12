clc; 
clear;
close all;
N1=42*1024;%N1ΪÿСʱһ�εĲ�������
fs=46544;%������
%  fs=23272;
B=4000; 
f0=12000;  %����Ƶ��
timedef=3600;
time=0.01;%length(raw)/fs;%�ź�
k=B/time; %��Ƶ��
% p2=2/pi*(acot(-k*(N1/(fs*fs)))+pi);%������������Ž�
%��ԭʼ����
 dir = 'E:\MatlabCode\FRFT_matlab';
 filename = '4B2CC9B6.ACU';
fid = fopen(fullfile(dir, filename), 'r');
fseek(fid,0,'eof');
fsize=ftell(fid);
datasize=(fsize/3)*2;%���ݸ���
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

t=(0:length(Data(:,1))-1)/fs;%�ɼ�ʱ��
data_mean=detrend(Data);%ȥֱ������

%��ͨ�˲�10k-14k 
Hd=bandpass10to14;
output_cut=filter(Hd,data_mean);
% figure(1)
% plot(output_cut);

% %��ѡchirp�ź�
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
 output_cut=hilbert(output_cut);%ϣ�����ر任��ʵ�ź�ת���ɸ����ź�
% output_cut=output_cut(72*N1+1:end);
%������������Ž�
a0=atan(-fs^2/(N1*k));
p0=2*a0/pi;
p1=[];
h=0;
r=0.01;%��ά��������
a=p0+[-0.2:r:0.2];%fractional power
idatacount=length(output_cut)/N1;
for i=1:5
    for j=1:length(a)
       y1=output_cut((i-1)*N1+1:i*N1);
       T=frft(y1,a(j)); 
       if(h<=max(abs(T(:))))
           h=max(abs(T(:)));
           pfind=a(j);%ʵ�ʽ���
       end
    end
    p1=[p1,pfind];
end
popt=mean(p1);%ȡ��������ѽ�����ƽ��ֵ
aopt=pi*popt/2;%�����ת�Ƕ�


icount=length(output_cut)/N1;%N1=43008��ÿ�βɼ�һ�����ݸ��� 
for i=1:icount
    y1=output_cut((i-1)*N1+1:i*N1);%ɨ���Ķ����۲��ź�����
     q((i-1)*N1+1:i*N1)=frft(y1,popt);%�Ȱ����۽�
%      q1((i-1)*N1+1:i*N1)=sum(abs(q(:)).^2);%������
end


figure(5)
subplot(2,1,1);%�۲��ź�
plot(real(output_cut(:)));
title('�۲��ź�')
subplot(2,1,2);
plot(abs(q(:)));% ���ﻭ��ģ���ֵ
title('���')

%����frft����������λ�úͶ�Ӧʱ��

% signtime=zeros(icount,3);%�źŵ�ʱ����
signindex=1;%�ź�����
signtime=[];
number=0;
for i=0:icount-1 
     x_cut=abs(q(i*N1+1:(i+1)*N1));%ÿ���յ����ź������任
    [amax,m]=max(x_cut);%�ҵ���������Ӧλ��
    if (amax>400)%�������400��Ϊ��chirp�źţ������ֵ�Ĵ�С��������ȸı�
        number=number+1;
        m1=N1-m;
         signtime(signindex,1)=m1;
%           signtime(signindex,1)=m;
        if signindex==1
           signtime(signindex,2)=0;
        else
            s0=(m1- signtime(signindex-1,1));
            signdiff=fix(s0/abs(cos(aopt)));%�õ���һСʱ���ݲ�ֵ����
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


