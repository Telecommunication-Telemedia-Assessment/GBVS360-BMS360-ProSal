Tool for generating ground truth Head-motion saliency maps



Before running the program:
- please make sure you are using a 64bit version of Windows
- If the computer running the program do not have Visual Studio 2015 installed, you may need to install the Visual Studio redistribuable packages for visual studio 2015 64bit. You can find it on the website of Microsoft: ( https://www.microsoft.com/en-us/download/details.aspx?id=48145 ) or there: ( https://drive.google.com/file/d/0B2UDdNMUOQRUVEJXOElCZVF2RkU/view?usp=sharing )


Once everything is setup, the general usage of the tool is as follow (You can also refer to the manual with the option --help):

.\salient-truth.exe -i [folder with .json files]

The saliency map will be generated as a video (mkv container) with a bitstream in h264. 


Example:

.\salient-truth.exe -i data\


Where data\ contains:
user_01.json
user_02.json
user_03.json
... 



Notes: 
- The tool output by default videos with a resolution of 720p. You may choose to change the output resolution, however this is not recommended as it only increase the processing cost and will not result in "higher quality" maps. 



Contact:
In case of any issue, please contact Pierre Lebreton at the following email address: lebreton.pier@gmail.com






