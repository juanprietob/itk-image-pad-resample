# itk-image-pad-resample

> Resample virtually any type of image 2D, 3D with 1 or multiple components

[![NPM](https://img.shields.io/npm/v/itk-image-pad-resample.svg)](https://www.npmjs.com/package/itk-image-pad-resample) [![JavaScript Style Guide](https://img.shields.io/badge/code_style-standard-brightgreen.svg)](https://standardjs.com)

## Install

Install with the flag '-g' and use it in the command line. 

```bash
	npm install itk-image-pad-resample -g
```

## Usage in the command line

```bash
	img-pad-resample --help
```

```
Help: Resample an image to a specific size.
Required:
--img <input path to image>
--size <sizeX,sizeY,sizeZ>

Optional:
--out <output path> default: out.nrrd
--spacing <spacingX,spacingY,spacingZ> Input image spacing is used. Otherwise, is set to fit the output size.
--pad <padX,padY,padZ> pad output at the top
--iso_spacing If this is set, the spacing of the output image will be isometric, i.e., the same for all dimensions which means the max spacing value is selected and set for all dimensions.
--center_image If this is set, the output image is centered in the resampled space.
--linear Linear interpolation, default is nearest neighbor.
```

```bash
img-pad-resample --img /path/to/input.nii --output_size 250,250,250 --out temp.nrrd 
```

## Usage in your js logic

Use [itk.js](https://insightsoftwareconsortium.github.io/itk-js/) to read an image. 

```js
	const readImageLocalFileSync = require('itk/readImageLocalFileSync');
	const writeImageLocalFileSync = require('itk/writeImageLocalFileSync');
	const ImgPadResampleLib = require('../dist');
```

```js
	const in_img = readImageLocalFileSync('/path/to/input');

	const imgpad = new ImgPadResampleLib();
	imgpad.SetImage(in_img);
	imgpad.SetOutputSize([300, 250]);
	imgpad.SetFitSpacingToOutputSizeOn(); //optional
	imgpad.SetIsoSpacingOn(); //optional 
	imgpad.SetCenterImageOn(); //optional
	imgpad.SetInterpolationTypeToLinear(); //default is nearest
	imgpad.Update();
	var img_out = imgpad.GetOutput();

	writeImageLocalFileSync(true, img_out, outputFileName)
```

## License

MIT © [juanprietob](https://github.com/juanprietob)
