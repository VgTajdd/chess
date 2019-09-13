# CHESS
Chess visualizer developed in C++ and OpenGL

---

## SOLVE IT!

Implement a function that draws a chess board with the position of the pieces passed as a parameter.

The parameter to pass will be a string composed only of the following characters:

``` txt
p, r, b, n, q, k, /, 1, 2, 3, 4, 5, 6, 7, 8
```

Where each piece is represented by a character, being the black characters in lowercase and the white characters in uppercase. The characters are:

``` txt
p = pawn
r = tower
b = bishop
n = horse
q = queen
k = king
```

The "/" character is used to separate the rows within the chain, there are exactly 8 rows in the chain.

To indicate empty spaces in the row the numbers from 1 to 8 are placed. For example, row "4p3" is composed of 4 empty cells, a pawn and finally three empty cells. The number of cells per row must always be 8.

Thus, for example, the initial chess position is represented by:

``` txt
"rnbqkbnr / pppppppp / 8/8/8/8 / PPPPPPPP / RNBQKBNR"
```

Implement the function: ```drawChessBoard (initPosition)```

Use any image for the pieces as well as the graphic library of your choice, but you must load and display BMP, PNG or JPG images in your program,

Solutions that only use the text console will not be considered valid.

Verify that the images are located correctly so there are no problems executing your project.
