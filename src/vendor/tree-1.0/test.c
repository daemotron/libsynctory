#include "tree.h"

#include <stdio.h>
#include <stdlib.h>

#define xnew(T)		((T *)calloc(1, sizeof(T)))
#define xfree(P)	(free(P))

typedef struct _Node Node;

struct _Node
{
  int			key;
  int			value;
  TREE_ENTRY(_Node)	tree;
};

Node *Node_new(int key, int value)
{
  Node *self= xnew(Node);
  self->key= key;
  self->value= value;
  return self;
}

int Node_compare(Node *lhs, Node *rhs)
{
  return rhs->key - lhs->key;
}

void Node_print(Node *self, FILE *stream)
{
  fprintf(stream, "%d is #%d", self->key, self->value);
}

void Node_printer(Node *self, void *stream)
{
  Node_print(self, (FILE *)stream);
  fprintf((FILE *)stream, " ");
}

// ----------------------------------------------------------------

typedef TREE_HEAD(_Tree, _Node) Tree;

TREE_DEFINE(_Node, tree);

Tree *Tree_new(int (*compare)(Node *lhs, Node *rhs))
{
  Tree *self= xnew(Tree);
  TREE_INIT(self, compare);
  return self;
}

// ----------------------------------------------------------------

#include "mersenne.h"

int main(int argc, char **argv)
{
  int i;
  Tree tree= TREE_INITIALIZER(Node_compare);

  mt_random_init();

  for (i= 0;  i < 100;  ++i)
    {
      Node v= { mt_random() % 1000, i };
      Node *vv= TREE_FIND(&tree, _Node, tree, &v);
      if (vv)
	{
	  printf("already inserted ");
	  Node_print(vv, stdout);
	  printf("\n");
	}
      else
        {
	  printf("insert " );
	  Node_print(&v, stdout);
	  printf("\n");
	  TREE_INSERT(&tree, _Node, tree, Node_new(v.key, v.value));
	  TREE_FORWARD_APPLY(&tree, _Node, tree, Node_printer, stdout);
	  printf("\n");
	}
    }

  TREE_FORWARD_APPLY(&tree, _Node, tree, Node_printer, stdout);
  printf("\n");

  for (i= 0;  i < 1000;  ++i)
    {
      Node *v= Node_new(mt_random() % 1000, 0);
      Node *vv= TREE_FIND(&tree, _Node, tree, v);

      printf("looking for %d - ", v->key);
      if (vv)
        {
	  printf("found ");
	  Node_print(vv, stdout);
	  printf("\n");
	  TREE_FORWARD_APPLY(&tree, _Node, tree, Node_printer, stdout);
	  printf("\n");
        }
      else
        {
	  printf("not found\n");
        }
      TREE_REMOVE(&tree, _Node, tree, v);
    }

  TREE_FORWARD_APPLY(&tree, _Node, tree, Node_printer, stdout);
  printf("\n");

  return 0;
}
