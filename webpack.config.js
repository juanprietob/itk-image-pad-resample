const path = require('path');

module.exports = {
  entry: './src/index.js',
  target: 'node',
  resolve: {
  	alias: {
  		img_pad_resample: path.resolve(__dirname, 'build/ImgPadResample')
  	}
  },
  output: {
  	library: 'img_pad_resample_lib',
  	libraryTarget: 'umd',
  	libraryExport: 'default',
    filename: 'index.js',
    path: path.resolve(__dirname, 'dist'),
  }
};