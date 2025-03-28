Utility program to get the pallette of an image or multiple images. An example:

Input image, 8x8px, 3 channels:

![Example image](resources/example_100px.png)

Output pallette:

```
[0, 0, 1, 1, 2, 2, 3, 3, 
0, 0, 1, 1, 2, 2, 3, 3, 
4, 4, 5, 5, 6, 6, 7, 7, 
4, 4, 5, 5, 6, 6, 7, 7, 
8, 8, 3, 3, 9, 9, 6, 6, 
8, 8, 3, 3, 9, 9, 6, 6, 
10, 10, 7, 7, 11, 11, 12, 12, 
10, 10, 7, 7, 11, 11, 12, 12]

0 -> (255,   0,   0)
10 -> ( 91, 127,   0)
7 -> ( 87,   0, 127)
4 -> (255,   0, 220)
2 -> (  0, 255,  33)
9 -> (255,   0, 110)
12 -> (255, 255, 255)
3 -> (  0, 148, 255)
6 -> ( 72,   0, 255)
8 -> (255, 216,   0)
11 -> (127,   0,   0)
5 -> (255, 127, 182)
1 -> (255, 106,   0)
```

You can provide multiple images of the same size and number of channels:

3 input images, 4x4, 3 channels:

![Example image](resources/example2_layer1_100px.png), ![Example image](resources/example2_layer2_100px.png), ![Example image](resources/example2_layer3_100px.png)

Output pallette:

```
[0, 0, 1, 1, 
0, 0, 1, 1, 
2, 2, 3, 3, 
2, 2, 3, 3]

0 -> (  0, 255,  33, 255), (  0, 255,  33, 255), (  0, 255,  33, 255)
3 -> (  0, 255,  33, 255), (255,   0,   0, 255), (  0,  38, 255, 255)
1 -> (  0, 255,  33, 255), (255,   0,   0, 255), (255,   0,   0, 255)
2 -> (  0, 255,  33, 255), (  0, 255,  33, 255), (  0,  38, 255, 255)
```

There's also an option to export an image as a raylib header file.
