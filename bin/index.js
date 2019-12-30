#!/usr/bin/env node

const readImageLocalFileSync = require('itk/readImageLocalFileSync');
const writeImageLocalFileSync = require('itk/writeImageLocalFileSync');
const ImgPadResampleLib = require('../dist');
const argv = require('minimist')(process.argv.slice(2));
const _ = require('underscore');

const help = function(){
    console.error("Help: Resample an image to a specific size.");
    console.error("Required:");
    console.error("--img <input path to image>");
    console.error("--size <sizeX,sizeY,sizeZ>");
    console.error("");
    console.error("Optional:");
    console.error("--out <output path> default: out.nrrd");
    console.error("--spacing <spacingX,spacingY,spacingZ> Input image spacing is used. Otherwise, is set to fit the output size.");
    console.error("--pad <padX,padY,padZ> pad output at the top");
    console.error("--iso_spacing If this is set, the spacing of the output image will be isometric, i.e., the same for all dimensions which means the max spacing value is selected and set for all dimensions.");
    console.error("--center_image If this is set, the output image is centered in the resampled space.");
    console.error("--linear Linear interpolation, default is nearest neighbor.");
}

if(argv["h"] || argv["help"] || !argv["img"]){
    help();
    process.exit(1);
}

var inputFileName = argv["img"];
var outputFileName = argv["out"]? argv["out"] : "out.nrrd";
var output_size = argv["size"]? _.map(argv["size"].split(","), (n)=>{return Number(n);}) : [];
var output_spacing = argv["spacing"]? _.map(argv["spacing"].split(","), (n)=>{return Number(n);}) : [];
var output_pad = argv["pad"]? _.map(argv["pad"].split(","), (n)=>{return Number(n);}) : [];

var in_img = readImageLocalFileSync(inputFileName);

var imgpad = new ImgPadResampleLib();
imgpad.SetImage(in_img);
imgpad.SetOutputSize(output_size);

if(output_spacing.length == 0){
	imgpad.SetFitSpacingToOutputSizeOn();
}else{
    imgpad.SetOutputSpacing(output_spacing);
}
if(output_pad.length > 0){
    imgpad.SetOutputPad(output_pad);
}
if(argv["iso_spacing"]){
	imgpad.SetIsoSpacingOn();
}
if(argv["center_image"]){
	imgpad.SetCenterImageOn();
}
if(argv["linear"]){
	imgpad.SetInterpolationTypeToLinear();
}

imgpad.Update();
var img_out = imgpad.GetOutput();

console.log("Writing:", outputFileName);
writeImageLocalFileSync(true, img_out, outputFileName)