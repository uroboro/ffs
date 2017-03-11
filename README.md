# ffs
Small text based file system.

Built for [Axle](https://github.com/codyd51/axle).

# How it works:

Given a directory, `ffs` will add it recursively to a file arxiv in plain text.

`ffs` arxiv files contain a header section and a content section. The header is a dictionary-like recursive structure detailing the name of a file and either the length of its contents or the number of files within. The name is preceded by its length. The content section is simply a concatenation of file's contents.

The example system is structured as follows:

    test
    +-- resources
    |   +-- bbundle.txt
    +-- aaa.txt
    +-- hello.txt
    +-- zero

And the result arxiv contains the following:

    {0:0;1{4test:0;5{7aaa.txt:4}{5empty:4;0}{9hello.txt:6}{9resources:10;1{10bundle.txt:7}}{4zero:0}}}aaa
    hello
    bundle
