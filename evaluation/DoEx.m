pathToEstimation = './FTP/Model/GBVS360_mx32/';
pathToFTP = './FTP/';
result_file_name = 'evaluation_GBVS360_mx32.csv';

imageSet = [10 66 22 27 31 35 39 43 47 52 57 63 6 80 84 11 15 23 28 32 36 3 44 49 54 58 64 76 81 87 12 17 24 29 33 37 41 45 4 55 5 14 77 82 13 21 25 2 34 38 42 46 51 56 62 68 7 83];


addpath('./code_forMetrics');
addpath('./code_forMetrics/FastEMD');

results = zeros(0, 9);


metrics_label=struct('id', 1, 'AUC_Judd', 2, 'SIM', 3, 'EMD',43, 'AUC_Borji', 5, 'sAUC', 6, 'CC', 7, 'NSS', 8, 'KL', 9);

results = zeros(length(imageSet), 8);

for k=1:length(imageSet) 
    disp(imageSet(k));

    pathToReferences = '';
    namePrefix = '';
    if(imageSet(k) > 28)
        pathToReferences = [pathToFTP 'HeadEyeSalMaps/'];
        namePrefix = 'SHE';
    else
        pathToReferences = [pathToFTP 'HeadSalMaps/'];
        namePrefix = 'SH';
    end

    imgEval = im2double(imread([pathToEstimation 'P' num2str(imageSet(k)) '.jpg']));
    
    height = size(imgEval,1);
    width = size(imgEval,2);
    

    fileId = fopen([pathToReferences namePrefix num2str(imageSet(k)) '.bin'], 'rb');
    buf = fread(fileId, width * height, 'single');
    imgRef = reshape(buf, [width, height])';
    fclose(fileId);

    if(imageSet(k) > 28)
        scanFile = fopen([pathToFTP '/Scanpaths/SP' num2str(imageSet(k)) '.txt'], 'r');
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
    end


    fprintf('[');
    results(k, metrics_label.SIM) = similarity(imgEval, imgRef);
    fprintf('.');
    % results(lIdx, metrics_label.EMD) = EMD(imgEval, fixationMap);
    % fprintf('.');
    if(imageSet(k) > 28)
        results(k, metrics_label.AUC_Judd) = AUC_Judd(imgEval, fixationMap);
        fprintf('.');
        results(k, metrics_label.AUC_Borji) = AUC_Borji(imgEval, fixationMap);
        fprintf('.');
    end
    
    %results(k, metrics_label.sAUC) = AUC_shuffled(imgEval, fixationMap, rand_fix_map);
    fprintf('.');
    results(k, metrics_label.CC) = CC(imgEval, imgRef);
    fprintf('.');
    results(k, metrics_label.NSS) = NSS(imgEval, imgRef);
    fprintf('.');
    results(k, metrics_label.KL) = KLdiv(imgEval, imgRef);
    fprintf(']\n');
end

% Output the results to a file
csvwrite(result_file_name, results);




