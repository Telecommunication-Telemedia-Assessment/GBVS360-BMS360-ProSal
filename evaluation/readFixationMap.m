function fixationMap=readFixationMap(folderName, imgNum, height, width)

fixationMap=zeros(height,width);
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

