function [scoreKL,scoreCC,scoreNSS,scoreROC]=CompareHeadEyeSalMaps(SalMap2,folderName,imgNum,fixMapAvailable)

% This code measures the similarity between two equirectangular format
% saliency maps. It needs 3 parameters as inputs

% SalMap2: The saliency map to be tested against the ground-truth. Ensure
%          that we need this in equirectangular format and it must have 
%          the same dimensions as the input image.
% folderName: The root folder where all scanpaths and head-eye saliency 
%              maps are stored. This folder must have the Scanpaths and 
%              HeadEyeSalMaps nested within it. 
% It outputs 4 different scores.
%imgNum: An integer argument denoting the image Number you want to test.
%scoreKL : The KL-Divergence score between the saliency maps
%scoreCC : The correltion score between the two saliency maps
%scoreNSS : The normalised Scanpath saliency between the two maps
%scoreROC : The ROC score between the two maps

%% read both the groundtruth saliency maps and te fixation vectors from file
sz=size(SalMap2);
height=sz(1);
width=sz(2);
fixationMap=zeros(height,width);
if(fixMapAvailable)
    scanFile = fopen([folderName '/Scanpaths/SP' num2str(imgNum) '.txt'], 'r');
    scanData = fscanf(scanFile,'%d,%f,%d,%d\n');
    fclose(scanFile);
    pers=0; startTime=0;
    for idx=1:floor(numel(scanData)/4)
        if scanData(4*(idx-1)+1)==1
            pers=pers+1;
            startTime=scanData(4*(idx-1)+2);
        end;
        scanPath(idx,1)=pers;
        scanPath(idx,2)=scanData(4*(idx-1)+1);
        time=scanData(4*(idx-1)+2)-startTime;
        if time<0
           time=60+time;
        end;
        scanPath(idx,3)=time;
        scanPath(idx,4)=scanData(4*(idx-1)+3);
        scanPath(idx,5)=scanData(4*(idx-1)+4);
        fixationMap(4,5)=1;
    end;
    fileId = fopen([folderName '/HeadEyeSalMaps/SHE' num2str(imgNum) '.bin'], 'rb');
else
    fileId = fopen([folderName '/HeadSalMaps/SH' num2str(imgNum) '.bin'], 'rb');
end

buf = fread(fileId, width * height, 'single');
SalMap1= reshape(buf, [width, height])';
fclose(fileId);

%% generate regular sampling points over the viewing sphere
N=floor(4*pi*100000);   %we sample the sphere at every 100000 times for every steradian
width=size(SalMap1,2);
height=size(SalMap1,1);
sampPoints=SpiralSampleSphere(N,0);
spOne=zeros(N,1); spTwo=zeros(N,1);
for k=1:N
    ang=atan2(sampPoints(k,2),sampPoints(k,1));
    if ang<0
        ang=ang+(2*pi);
    end;
    xCoord=max(min(1+floor(width*(ang/(2*pi))),width),1);                                                 % mapping the spherical to equirectangular
    yCoord=max(min(1+floor(height*(asin(sampPoints(k,3)/norm([sampPoints(k,:)]))/pi+0.5)),height),1);     % mapping the spherical to equirectangular
    spOne(k)=SalMap1(yCoord,xCoord);
    spTwo(k)=SalMap2(yCoord,xCoord);
end;

%% get the metrics at these sampled points
scoreKL = KLdiv(spOne, spTwo);
scoreCC = CC(spOne, spTwo);

if(fixMapAvailable)
    scoreNSS = NSS(SalMap2, fixationMap);
    [scoreROC,~,~,~] = AUC_Judd(SalMap2, fixationMap);
else
    scoreNSS = 0;
    scoreROC = 0;
end



