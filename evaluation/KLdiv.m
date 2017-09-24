% created: Zoya Bylinskii, Aug 2014

% This finds the KL-divergence between two different saliency maps when
% viewed as distributions: it is a non-symmetric measure of the information 
% lost when saliencyMap is used to estimate fixationMap.


function score = KLdiv(map1, map2)
% saliencyMap is the saliency map
% fixationMap is the human fixation map

% map1 = im2double(imresize(saliencyMap, size(fixationMap)));
% map2 = im2double(fixationMap);

% make sure map1 and map2 sum to 1
if any(map1(:))
    map1 = map1/sum(map1(:));
end
map1(map1==0)=1e-50;

if any(map2(:))
    map2 = map2/sum(map2(:));
end
map2(map2==0)=1e-50;

% compute KL-divergence
score1 = sum(sum(map2 .* log(eps + map2./(map1+eps))));
score2 = sum(sum(map1 .* log(eps + map1./(map2+eps))));
score = (score1+score2)/2;


