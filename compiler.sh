bison -v -d -t syntax.y
flex lexical.l
gcc main.c syntax.tab.c gramtree.c semanteme.c intermediate_code.c -lfl -ly
