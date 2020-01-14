const MedImgReader = require('med-img-reader');
const ImgPadResampleLib = require('../dist/index');
const Lab = require('@hapi/lab');
const lab = exports.lab = Lab.script();
const path = require('path');

lab.experiment("Test image-pad-resample", function(){
	

    lab.test('returns true when a 3D image one component is resampled.', function(){

        var inputFileName = path.join(__dirname, 't1.nrrd');
		var outputFileName = path.join(__dirname, 't1_resampled.nrrd');

		const medimgreader = new MedImgReader();
		medimgreader.SetFilename(inputFileName);
		medimgreader.ReadImage();
		const in_img = medimgreader.GetOutput();

		var imgpad = new ImgPadResampleLib();
		imgpad.SetImage(in_img);
		imgpad.SetOutputSize([500, 500, 500]);
		imgpad.SetOutputSpacing([0.5, 0.5, 0.5]);
		imgpad.Update();
		var img_out = imgpad.GetOutput();

		medimgreader.SetInput(img_out);
		medimgreader.SetFilename(outputFileName);
		medimgreader.WriteImage();
        
        return true;
    });

    lab.test('returns true when a 2D image 3 components is resampled.', function(){

        var inputFileName = path.join(__dirname, 'brain.png');
		var outputFileName = path.join(__dirname, 'brain_resampled.png');
		
		const medimgreader = new MedImgReader();
		medimgreader.SetFilename(inputFileName);
		medimgreader.ReadImage();
		const in_img = medimgreader.GetOutput();

		var imgpad = new ImgPadResampleLib();
		imgpad.SetImage(in_img);
		imgpad.SetOutputSize([1500, 1500]);
		imgpad.SetOutputSpacing([0.25, 0.25]);
		imgpad.SetInterpolationTypeToLinear();
		imgpad.Update();
		var img_out = imgpad.GetOutput();

		medimgreader.SetInput(img_out);
		medimgreader.SetFilename(outputFileName);
		medimgreader.WriteImage();
        
        return true;
    });

	 lab.test('returns true when a 2D image 3 components is resampled to a specific size keeping the dimensions to fit.', function(){

        var inputFileName = path.join(__dirname, 'brain.png');
		var outputFileName = path.join(__dirname, 'brain_resampled.png');
		
		const medimgreader = new MedImgReader();
		medimgreader.SetFilename(inputFileName);
		medimgreader.ReadImage();
		const in_img = medimgreader.GetOutput();

		var imgpad = new ImgPadResampleLib();
		imgpad.SetImage(in_img);
		imgpad.SetOutputSize([500, 250]);
		imgpad.SetFitSpacingToOutputSizeOn();
		imgpad.SetIsoSpacingOn();
		imgpad.SetCenterImageOn();
		imgpad.SetInterpolationTypeToLinear();
		imgpad.Update();
		var img_out = imgpad.GetOutput();

		medimgreader.SetInput(img_out);
		medimgreader.SetFilename(outputFileName);
		medimgreader.WriteImage();
        
        return true;
    });


})