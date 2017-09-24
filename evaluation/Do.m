
pathToEstimation = '/Volumes/SSD/Salient360/FTP/Model/GBVS360_DIOP_mx32/';
pathToFTP = '/Volumes/SSD/Salient360/FTP/';
result_file_name = 'evaluation_GBVS360_DIOP_mx32.csv';

imageSet = [10 66 22 27 31 35 39 43 47 52 57 63 6 80 84 11 15 23 28 32 36 3 44 49 54 58 64 76 81 87 12 17 24 29 33 37 41 45 4 55 5 14 77 82 13 21 25 2 34 38 42 46 51 56 62 68 7 83];




scores = zeros(length(imageSet), 4);



for k = 1:length(imageSet)

    imgNum = imageSet(k);

disp(imgNum);
    estSal = im2double(imread([pathToEstimation 'P' num2str(imgNum) '.jpg']));

    [scoreKL,scoreCC,scoreNSS,scoreROC] = CompareHeadEyeSalMaps(estSal, pathToFTP, imgNum, imgNum > 28);

    scores(k, 1) = scoreKL;
    scores(k, 2) = scoreCC;
    scores(k, 3) = scoreNSS;
    scores(k, 4) = scoreROC;
end

file = fopen(result_file_name, 'w');
for k = 1:length(imageSet)
   fprintf(file, '%d', imageSet(k));
   for c = 1:4
      fprintf(file, ', %f', scores(k,c));
   end
   fprintf(file, '\n');
end
fclose(file);

