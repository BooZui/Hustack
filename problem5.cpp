#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue>
using namespace std;
typedef struct TNode
{
  char label;
  struct TNode *left, *right;
} TN;

int isBSTUtil(const TN *node, const TN *minNode, const TN *maxNode);

// tao nut moi
TN *makeNewNode(char label)
{
  TN *newNode = (TN *)malloc(sizeof(TN));
  newNode->left = newNode->right = NULL;
  newNode->label = label;
  return newNode;
}
// then nut moi vao cay
// root la nut cha va nNode la nut moi them
// tham so rtype = L --> them vao con trai va R thi them vao con phai
void addNodeToTree(TN *root, TN *nNode, char rtype)
{
  if (rtype == 'L')
    root->left = nNode;
  else
    root->right = nNode;
}

// duyet cay theo thu tu giua
void inOrderTravel(const TN *root)
{
  if (NULL == root)
    return;
  inOrderTravel(root->left);
  printf("%c ", root->label);
  inOrderTravel(root->right);
}

void printInOrderTravel(const TN *root)
{
  inOrderTravel(root);
  printf("\n");
}

TN *buildTree()
{
  TN *root, *nNode, *parent;
  int level, i;
  char path[20], label, nextLine[50];
  while (1)
  {
    fgets(nextLine, sizeof(nextLine), stdin);
    if (strstr(nextLine, "-1") != NULL)
      break;
    sscanf(nextLine, "%d %s %c", &level, path, &label);
    if (level == 0)
    {
      root = makeNewNode(label);
      continue;
    }

    // add decendent nodes
    nNode = makeNewNode(label);
    parent = root;
    for (i = 0; i < int(strlen(path) - 1); i++)
      if (path[i] == 'R')
        parent = parent->right;
      else
        parent = parent->left;
    addNodeToTree(parent, nNode, path[i]);
  }
  return root;
}

// Hàm kiểm tra xem cây có phải cây nhị phân tìm kiếm hay không
// nhãn các nút có theo thứ tự A-Z
// Hàm trả về 1 nếu là BST và 0 nếu ngược lại
int isBinarySearchTree(const TN *root)
{
  int result = isBSTUtil(root, NULL, NULL);
  return result;
}

int isBSTUtil(const TN *node, const TN *minNode, const TN *maxNode)
{
  if (node == NULL)
    return 1;

  if (minNode != NULL && node->label <= minNode->label)
    return 0;
  if (maxNode != NULL && node->label >= maxNode->label)
    return 0;

  return isBSTUtil(node->left, minNode, node) && isBSTUtil(node->right, node, maxNode);
}

void checkIsaBST(const TN *root)
{
  if (isBinarySearchTree(root))
    printf("is a BST\n");
  else
    printf("is not a BST\n");
}

// hàm đếm độ sâu của cây
// chú ý. cây có 1 nút gốc là lá thì độ sâu là 0
int countLevel(const TN *root)
{
  if (root == NULL)
  {
    return 0;
  }

  int leftLevel = countLevel(root->left);
  int rightLevel = countLevel(root->right);

  return 1 + (leftLevel > rightLevel ? leftLevel : rightLevel);
}

// Hàm đếm và trả về số lượng nút đầy đủ trên cây
// nút đầy đủ là các nút có đủ 2 con
int countFullNodes(const TN *root)
{
  if (root == NULL)
    return 0;

  int count = 0;
  if (root->left != NULL && root->right != NULL)
  {
    count = 1;
  }

  count += countFullNodes(root->left);
  count += countFullNodes(root->right);

  return count;
}

int main()
{
  TN *root = NULL;
  char nextCommand[50];
  while (1)
  {
    fgets(nextCommand, sizeof(nextCommand), stdin);
    nextCommand[strcspn(nextCommand, "\r\n")] = 0;
    if (nextCommand[0] != '?')
      break;
    if (strcmp(&nextCommand[2], "buildTree") == 0)
      root = buildTree();
    else if (strcmp(&nextCommand[2], "printInOrderTravel") == 0)
      printInOrderTravel(root);
    else if (strcmp(&nextCommand[2], "checkIsaBST") == 0)
      checkIsaBST(root);
    else if (strcmp(&nextCommand[2], "countLevel") == 0)
      printf("Total levels: %d\n", countLevel(root) - 1);
    else if (strcmp(&nextCommand[2], "countFullNodes") == 0)
      printf("Total full nodes: %d\n", countFullNodes(root));
  }

  return 0;
}