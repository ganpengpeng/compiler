bison -v -d -t syntax.y
flex lexical.l
gcc main.c syntax.tab.c gramtree.c semanteme.c -lfl -ly
