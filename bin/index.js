#!/usr/bin/env node

const MedImgReader = require('med-img-reader');
const ImgPadResampleLib = require(__dirname + '/../dist');
const argv = require('minimist')(process.argv.slice(2));
const _ = require('underscore');
const Promise = require('bluebird');
const path = require('path');
const fs = require('fs');

const help = function(){
    console.error("Help: Resample an image to a specific size.");
    console.error("Required:");
    console.error("--img <input path to image> or --dir <directory>");
    console.error("--size <sizeX,sizeY,sizeZ>");
    console.error("");
    console.error("Optional:");
    console.error("--out <output path> default: out.nrrd");
    console.error("--out_ext <output extension> (when using --dir) default: .nrrd");
    console.error("--spacing <spacingX,spacingY,spacingZ> Input image spacing is used. Otherwise, is set to fit the output size.");
    console.error("--pad <padX,padY,padZ> pad output at the top");
    console.error("--iso_spacing If this is set, the spacing of the output image will be isometric, i.e., the same for all dimensions which means the max spacing value is selected and set for all dimensions.");
    console.error("--center_image If this is set, the output image is centered in the resampled space.");
    console.error("--linear Linear interpolation, default is nearest neighbor.");
}

if(argv["h"] || argv["help"] || (!argv["img"] && !argv["dir"])){
    help();
    process.exit(1);
}

const readDir = (dirname)=>{
    return Promise.map(fs.readdirSync(dirname), (filename)=>{
      var full_path = path.join(dirname, filename);
      var stats = fs.statSync(full_path);
      if(stats.isDirectory()){
        return readDir(full_path);
      }else{
        var ext = path.extname(full_path);
        if(ext.match(new RegExp(/nrrd|nii|gz|dcm|jpg|png|mhd/))){
            return full_path;
        }
      }
    })
    .then((files)=>{return _.compact(_.flatten(files))});
}

const mkdirp = (dirname)=>{
    var currentdir = "";
    _.each(dirname.split(path.sep), (d)=>{
        currentdir += d + path.sep;
        if(currentdir != path.sep && !fs.existsSync(currentdir)){
            try{
                fs.mkdirSync(currentdir);   
            }catch(e){
                console.error(e);
            }
        }
    });
    return Promise.resolve();
}

const resampleImage = (inputFileName, outputFileName, argv)=>{
    return new Promise((resolve, reject)=>{
        try{
            var output_size = argv["size"]? _.map(argv["size"].split(","), (n)=>{return Number(n);}) : [];
            var output_spacing = argv["spacing"]? _.map(argv["spacing"].split(","), (n)=>{return Number(n);}) : [];
            var output_pad = argv["pad"]? _.map(argv["pad"].split(","), (n)=>{return Number(n);}) : [];
            var iso_spacing = argv["iso_spacing"];
            var center_image = argv["center_image"];
            var linear = argv["linear"];

            console.log("Reading:", inputFileName);
            const medimgreader = new MedImgReader();
            medimgreader.SetFilename(inputFileName);
            medimgreader.ReadImage();
            var in_img = medimgreader.GetOutput();

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
            if(iso_spacing){
                imgpad.SetIsoSpacingOn();
            }
            if(center_image){
                imgpad.SetCenterImageOn();
            }
            if(linear){
                imgpad.SetInterpolationTypeToLinear();
            }

            imgpad.Update();
            var img_out = imgpad.GetOutput();

            console.log("Writing:", outputFileName);

            const writer = new MedImgReader();
            writer.SetFilename(outputFileName);
            writer.SetInput(img_out)
            writer.WriteImage();

            medimgreader.delete();
            imgpad.delete();
            writer.delete();

            resolve();
        }catch(e){
            console.error(e);
            resolve();
        }
    });
}

const getImages = (argv)=>{
    var inputFileName = argv["img"];
    var inputDirectory = argv["dir"];
    var outputFileName = argv["out"]? argv["out"] : "out.nrrd";
    var out_ext = argv["out_ext"]?  argv["out_ext"] : ".nrrd";

    if(inputFileName){
        return Promise.resolve([{
            img: inputFileName,
            out: outputFileName
        }]);
    }else{
        return readDir(inputDirectory)
        .then((filenames)=>{
            return Promise.map(filenames, (img)=>{
                var outpath = path.normalize(img.replace(inputDirectory, outputFileName + path.sep));
                outpath = outpath.replace(path.extname(outpath), out_ext);
                return mkdirp(path.dirname(outpath))
                .then(()=>{
                    return {img, out: outpath};
                })
            })
        });
    }
}

getImages(argv)
.then((imgs)=>{
    console.log(imgs)
    return Promise.map(imgs, (i)=>{
        return resampleImage(i.img, i.out, argv);
    });
})
.catch((e)=>{
    console.error(e)
})
