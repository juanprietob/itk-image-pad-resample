# itk-image-pad-resample

> Resample virtually any type of image 2D, 3D with 1 or multiple components.

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
--spacing <spacingX,spacingY,spacingZ> If not set, the output spacing is set to match the current image physical length.
--pad <padX,padY,padZ> pad output image at the top
--iso_spacing If this is set, the spacing of the output image will be isometric, i.e., the same for all dimensions which means the max spacing value is selected and set for all dimensions.
--center_image If this is set, the output image is centered in the resampled space.
--linear Linear interpolation, default is nearest neighbor.
```

```bash
img-pad-resample --img /path/to/input.nii --size 250,250,250 --out temp.nrrd 
```

## Usage in your js logic

Use [itk.js](https://insightsoftwareconsortium.github.io/itk-js/) to read an image. 

```js
const readImageLocalFileSync = require('itk/readImageLocalFileSync');
const writeImageLocalFileSync = require('itk/writeImageLocalFileSync');
const ImgPadResampleLib = require('itk-image-pad-resample');
```

```js
const in_img = readImageLocalFileSync('/path/to/input{.png,.jpg,.nrrd,.nii.gz,.dcm}');

const imgpad = new ImgPadResampleLib();
imgpad.SetImage(in_img);
imgpad.SetOutputSize([300, 250]);
imgpad.SetFitSpacingToOutputSizeOn(); //optional
imgpad.SetIsoSpacingOn(); //optional 
imgpad.SetCenterImageOn(); //optional
imgpad.SetInterpolationTypeToLinear(); //default is nearest
imgpad.Update();
var img_out = imgpad.GetOutput();

writeImageLocalFileSync(true, img_out, outputFileName);
```

## Example

Input RGBA image: 
![alt text](https://github.com/juanprietob/itk-image-pad-resample/raw/master/test/brain.png "Brain image with multiple components (RGB)")

```bash
img-pad-resample --img brain.png --size 500,250 --out out_brain.png 
```

Output RGBA image, the image here might look stretched because typical image viewers will not take spacing information in consideration!: 
![alt text](https://github.com/juanprietob/itk-image-pad-resample/raw/master/test/out_brain.png "Image with different spacing")


Use flag '--iso_spacing' to have the same spacing in all dimensions
```bash
img-pad-resample --img brain.png --size 500,250 --iso_spacing --out out_brain_iso.png 
```

Output RGBA image with equal spacing: 
![alt text](https://github.com/juanprietob/itk-image-pad-resample/raw/master/test/out_brain_iso.png "Image with equal spacing")

## License

MIT © [juanprietob](https://github.com/juanprietob)