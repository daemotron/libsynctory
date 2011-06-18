#include "tree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Node
{
  char			*word;
  TREE_ENTRY(_Node)	 linkage;
} Node;

typedef TREE_HEAD(_Tree, _Node) Tree;

TREE_DEFINE(_Node, linkage);

int fence = 0;

Node *Node_new(char *word)
{
  Node *self = (Node *)calloc(1, sizeof(Node));
  self->word = strdup(word);
  return self;
}

int Node_compare(Node *lhs, Node *rhs)
{
  return strcmp(lhs->word, rhs->word);
}

void Node_print(Node *self, FILE *stream)
{
  fprintf(stream, "%s", self->word);
}

void Node_printer(Node *self, void *stream)
{
  if (fence)
    {
      Node_print(self, (FILE *)stream);
      --fence;
    }
}

int main(int argc, char **argv)
{
  int   count = 0;
  Tree  tree = TREE_INITIALIZER(Node_compare);
  char  line[80];

  while (fgets(line, sizeof(line), stdin))
    {
      Node test = { line };
      Node *ptr = TREE_FIND(&tree, _Node, linkage, &test);
      if (ptr)
	printf("ignoring duplicate line: %s", line);
      else
	{
	  TREE_INSERT(&tree, _Node, linkage, Node_new(line));
	  ++count;
	}
    }

  fence = 20;
  printf("first %d elements, forwards:\n", fence);
  TREE_FORWARD_APPLY(&tree, _Node, linkage, Node_printer, stdout);
  printf("\n");

  fence = 20;
  printf("last %d elements, backwards:\n", fence);
  TREE_REVERSE_APPLY(&tree, _Node, linkage, Node_printer, stdout);
  printf("\n");

  printf("inserted %d elements into a tree of depth %d\n", count, TREE_DEPTH(&tree, linkage));

  return 0;
}
