pathToImages = './to_eval/output1/';
pathToReferences = './references/';

addpath('./code_forMetrics');
addpath('./code_forMetrics/FastEMD');

categories = {};
categories{1} = 'Action/';
categories{2} = 'Affective/';
categories{3} = 'Art/';
categories{4} = 'BlackWhite/';
categories{5} = 'Cartoon/';
categories{6} = 'Fractal/';
categories{7} = 'Indoor/';
categories{8} = 'Inverted/';
categories{9} = 'Jumbled/';
categories{10} = 'LineDrawing/';
categories{11} = 'LowResolution/';
categories{12} = 'Noisy/';
categories{13} = 'Object/';
categories{14} = 'OutdoorManMade/';
categories{15} = 'OutdoorNatural/';
categories{16} = 'Pattern/';
categories{17} = 'Random/';
categories{18} = 'Satelite/';
categories{19} = 'Sketch/';
categories{20} = 'Social/';

results = zeros(0, 8);
allfilenames = {};

for catId = 1:length(categories)

   filelist=dir([pathToImages categories{catId} '*.jpg']);

   metrics_label=struct('AUC_Judd', 1, 'SIM', 2, 'EMD', 3, 'AUC_Borji', 4, 'sAUC', 5, 'CC', 6, 'NSS', 7, 'KL', 8);

   results = [results ; zeros(length(filelist), 8)];


   for k=1:length(filelist) 
      disp(filelist(k).name);
      imgEval = imread([pathToImages categories{catId} filelist(k).name]);
      imgRef  = imread([pathToReferences 'FIXATIONMAPS/' categories{catId} filelist(k).name]);

      imgBinRef  = load([pathToReferences 'FIXATIONLOCS/' categories{catId} filelist(k).name(1:(length(filelist(k).name)-3)) 'mat']);
      imgBinRef = imgBinRef.fixLocs;

      % imgRef = real(antonioGaussian(imgBinRef, 8)); % provide the same result as loading the image from the file...

      allfilenames{length(allfilenames)+1} = [pathToImages categories{catId} filelist(k).name];

      fprintf('[');
      results(k, metrics_label.SIM) = similarity(imgEval, imgRef);
      fprintf('.');
      results(k, metrics_label.EMD) = EMD(imgEval, imgBinRef);
      fprintf('.');
      results(k, metrics_label.AUC_Judd) = AUC_Judd(imgEval, imgBinRef);
      fprintf('.');
      results(k, metrics_label.AUC_Borji) = AUC_Borji(imgEval, imgBinRef);
      fprintf('.');
      % results(k, metrics_label.sAUC) = AUC_shuffled(imgEval, imgBinRef);
      % fprintf('.');
      results(k, metrics_label.CC) = CC(imgEval, imgRef);
      fprintf('.');
      results(k, metrics_label.NSS) = NSS(imgEval, imgRef);
      fprintf('.');
      results(k, metrics_label.KL) = KLdiv(imgEval, imgRef);
      fprintf(']\n');
   end

end

% Output the results to a file
csvwrite('evaluation.csv', results);

file = fopen('image_names.csv', 'w');
for k = 1:length(allfilenames)
   fprintf(file, '%s\n', allfilenames{k});
end
fclose(file);



