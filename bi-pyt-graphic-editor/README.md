# Graphic editor

Simple graphic editor that loads the input image, processes selected graphic operations and stores the output.

## Graphic operations
`--help`: prints information which options and arguments can be passed to the program  
`--rotate`: rotates image to the **right** by 90° **(optional)**  
`--mirror`: mirrors the image **(optional)**  
`--inverse`: produces a negative of the image **(optional)**  
`--bw`: produces grayscaled image **(optional)**  
`--lighten <percentage>`: lightenes the image by 0 - 100 % **(optional)**  
`--darken <percentage>`: darkens the image by 0 - 100 % **(optional)**  
`--sharpen`: applies the "unsharp mask" filter to the image **(optional)**

## Requirements
- Anaconda or Miniconda

## Usage
`conda env create -f environment.yml`
`conda activate bipyt`  
`python3 graphic_editor.py` `[--rotate]` `[--mirror]` `[--inverse]` `[--bw]` `[--lighten <0; 100>]` `[--darken <0;100>]` `[--sharpen]` `INPUT_IMAGE_PATH` `OUTPUT_IMAGE_PATH`

## Results
Got **30/30** points.
