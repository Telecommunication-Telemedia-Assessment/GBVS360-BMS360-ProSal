
function [result] = parseSalMapScanpaths(path,imgNum,typ)

% 
%This function "parseSalMapScanpaths.m" parses the scan-paths or the 
%saliency maps thst is provide in the FTP. The function has a signature 
%"function [result] = parseSalMapScanpaths(path,imgNum,typ)" and takes 
%in three input arguments path,imgNum,typ and gives an output argument 
%result.
% path       : This is the file path on your server or local machine that 
%              contains the data that was downloaded. This must be the root folder 
%              containg the 4 subfolders : Scanpaths, Images, HeadSalMaps and HeadEyeSalMaps
% imgNum     : Use a plain integer 0-10 here to indicate the image number 
%              which you want to examine. Please remember that images 1-5 can be 
%              used for "head-only" condition and images 6-10 can be used for the 
%              other models.
% typ        : This can be either 0/1/2 based on whether you want to test the 
%              "head only","head+eye condition" or the "scan-paths" respectively.
% result     : in case the typ=0 or typ=1, then then the result contains an MxN 
%              equirectangular format saliency map where MxN is equal to the 
%              dimensions of the equirectangular image.
%              in case the typ=3, then we obtin a scanpath as the resut. This is a
%              :x5 vector where the first column indicates UserID, the cond 
%              column indicates the fixation number(counted for every observer serially), 
%              the third column holds the fixation start time 
%              (counted from the instant the stimuli was shown), 
%              the third and fourth column indcates the fixation point 
%              in X,Y coordinates of the equirectangular image.
% A sample call can be [img] =  parseSalMapScanpaths('F:\VR\GazeData',2,1);

result=[]; 
HeadImageList=[2,3,4,5,6,7,10,11,12,13,14,15,17,21,22,23,24,25,27,28];
EyeImageList=[29,31,32,33,34,35,36,37,38,39,41,42,43,44,45,46,47,49,51,52,54,55,56,57,58,62,63,64,66,68,76,77,80,81,82,83,84,87,88,89];
if typ<1 || typ>3
    disp('Sorry! I dont under stand that type.');
    return;
elseif (typ==1 && sum(HeadImageList==imgNum)==0)
    disp('We have head maps only for images <<2,3,4,5,6,7,10,11,12,13,14,15,17,21,22,23,24,25,27,28>>. Please enter valid inputs.');
    return;   
elseif (typ>1 && sum(EyeImageList==imgNum)==0)
    disp('We have eye maps only for images <<29,31,32,33,34,35,36,37,38,39,41,42,43,44,45,46,47,49,51,52,54,55,56,57,58,62,63,64,66,68,76,77,80,81,82,83,84,87,88,89>>. Please enter valid inputs.');
    return; 
end;
imgRGB=imread([path '\Images\P' num2str(imgNum) '.jpg']);
width=size(imgRGB,2);
height=size(imgRGB,1);
if typ==1
    fileId = fopen([path '\HeadSalMaps\SH' num2str(imgNum) '.bin'], 'rb');
    buf = fread(fileId, width * height, 'single');
    SalMap= reshape(buf, [width, height])';
    fclose(fileId);
    figure; imshow(SalMap);colormap(jet);
    result=SalMap;
elseif typ==2
    fileId = fopen([path '\HeadEyeSalMaps\SHE' num2str(imgNum) '.bin'], 'rb');
    buf = fread(fileId, width * height, 'single');
    SalMap= reshape(buf, [width, height])';
    fclose(fileId);
    figure; imshow(SalMap);colormap(jet);
    result=SalMap;
elseif typ==3
    imgScanPaths=imread([path '\Scanpaths\SP' num2str(imgNum) '.jpg']);
    imshow(imgScanPaths);
    scanFile = fopen([path '\Scanpaths\SP' num2str(imgNum) '.txt'], 'r');
    scanData = fscanf(scanFile,'%d,%f,%d,%d\n');
    %scanData=reshape(gazeData,4,[]);
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
    end;
    result=scanPath;
end;

figure; imshow(imgRGB);
