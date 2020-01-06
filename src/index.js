import {ImgPadResample} from 'img_pad_resample';
import _ from 'underscore';

class ImgPadResampleLib {
	constructor(){
		this.img_pad_resample = new ImgPadResample();
		this.in_img = {};
		this.fit_spacing_to_output_size = 0;
		this.iso_spacing = 0;
		this.center_image = 0;
	}
	SetFitSpacingToOutputSizeOn(){
		this.fit_spacing_to_output_size = 1;
	}
	SetFitSpacingToOutputSizeOff(){
		this.fit_spacing_to_output_size = 0;
	}
	SetIsoSpacingOn(){
		this.iso_spacing = 1;
	}
	SetIsoSpacingOff(){
		this.iso_spacing = 0;
	}
	SetImage(in_img){
		this.in_img = in_img;
		this.img_pad_resample.SetImage(in_img);
	}
	SetInterpolationTypeToNearest(){
		this.img_pad_resample.SetInterpolationTypeToNearest();
	}
	SetInterpolationTypeToLinear(){
		this.img_pad_resample.SetInterpolationTypeToLinear();
	}
	SetOutputSize(output_size){
		this.output_size = output_size;
		this.img_pad_resample.SetOutputSize(output_size);	
	}
	SetOutputSpacing(output_spacing){
		this.output_spacing = output_spacing;
		this.img_pad_resample.SetOutputSpacing(output_spacing);	
	}
	SetOutputPad(output_pad){
		this.img_pad_resample.SetOutputPad(output_pad);	
	}
	SetCenterImageOn(){
		this.center_image = 1;
	}
	SetCenterImageOff(){
		this.center_image = 0;
	}
	Update(){
		if(this.fit_spacing_to_output_size && this.in_img.spacing && this.in_img.size.length == this.output_size.length){
			var out_size = this.output_size;
			var out_spacing = _.map(this.in_img.spacing, (sp, index)=>{
				return sp * this.in_img.size[index]/this.output_size[index];
			});
			this.SetOutputSpacing(out_spacing);
		}
		if(this.iso_spacing && this.in_img.spacing){
			var out_spacing = this.output_spacing && this.output_spacing.length == this.in_img.spacing.length? this.output_spacing: this.in_img.spacing;
			var max_spacing = _.max(out_spacing);
			out_spacing.fill(max_spacing);
			this.SetOutputSpacing(out_spacing);
		}
		if(this.center_image && this.in_img.size && this.in_img.spacing){
			
			var out_spacing = this.output_spacing && this.output_spacing.length == this.in_img.spacing.length? this.output_spacing: this.in_img.spacing;
			var out_size = this.output_size && this.output_size.length == this.in_img.size.length? this.output_size: this.in_img.size;
						
			var current_length = _.map(this.in_img.size, (s, i)=>{return s*this.in_img.spacing[i]});
			var target_length = _.map(out_size, (s, i)=>{return s*out_spacing[i]});
			
			var output_pad = _.map(target_length, (l, index)=>{ 
				var o_p = (l - current_length[index])/2;
				return o_p > 0? Math.round(o_p/out_spacing[index]): 0;
			});
			this.SetOutputPad(output_pad);
		}
		this.img_pad_resample.Update();
	}
	GetOutput(){
		// image = {
		//   imageType: {
		//     dimension: 2,
		//     componentType: 'uint16_t',
		//     pixelType: 1,
		//     components: 1
		//   },
		//   name: 'Image',
		//   origin: [ 0, 0 ],
		//   spacing: [ 0.148489, 0.148489 ],
		//   direction: { rows: 2, columns: 2, data: [ 1, 0, 0, 1 ] },
		//   size: [ 1136, 852 ],
		//   data: Uint16Array []
		// }
		
		var img_out = this.in_img;
		img_out.size = this.output_size && this.output_size.length == img_out.size.length? this.output_size: img_out.size;
		img_out.spacing = this.output_spacing && this.output_spacing.length == img_out.spacing.length? this.output_spacing: img_out.spacing;

		// This stuff comes from itk.js
		// Int8 = 'int8_t'
		// UInt8 = 'uint8_t'
		// Int16 = 'int16_t'
		// UInt16 = 'uint16_t'
		// Int32 = 'int32_t'
		// UInt32 = 'uint32_t'
		// Int64 = 'int64_t'
		// UInt64 = 'uint64_t'
		// Float32 = 'float'
		// Float64 = 'double'

		var img_buffer_out = this.img_pad_resample.GetOutputBuffer();

		switch (img_out.imageType.componentType) {
			case 'int8_t':
				img_out.data = new Int8Array(img_buffer_out);
				break;
			case 'uint8_t':
				img_out.data = new Uint8Array(img_buffer_out);
				break;
			case 'int16_t':
				img_out.data = new Int16Array(img_buffer_out);
				break;
			case 'uint16_t':
				img_out.data = new Uint16Array(img_buffer_out);
				break;
			case 'int32_t':
				img_out.data = new Int32Array(img_buffer_out);
				break;
			case 'uint32_t':
				img_out.data = new Uint32Array(img_buffer_out);
				break;
			case 'int64_t':
				img_out.data = new BigInt64Array(img_buffer_out);
				break;
			case 'uint64_t':
				img_out.data = new BigUint64Array(img_buffer_out);
				break;
			case 'float':
				img_out.data = new Float32Array(img_buffer_out);
				break;
			case 'double':
				img_out.data = new Float64Array(img_buffer_out);
				break;
			default:
				img_out.data = new Uint16Array(img_buffer_out);
		}
		
		return img_out;
	}	
}

export default ImgPadResampleLib;