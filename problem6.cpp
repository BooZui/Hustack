#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stack>

using namespace std;

// Định nghĩa cấu trúc để lưu trữ thuộc tính của thẻ
typedef struct Attribute
{
  char *name;
  char *value;
  struct Attribute *next;
} Attribute;

// Định nghĩa cấu trúc nút cây tổng quát cho thẻ HTML
typedef struct TreeNode
{
  char *tag_name;
  Attribute *attributes;
  char *text;
  struct TreeNode *first_child;
  struct TreeNode *next_sibling;
  struct TreeNode *parent;
} TreeNode;

// Hàm tạo một thuộc tính mới
Attribute *create_attribute(char *name, char *value)
{
  Attribute *attr = (Attribute *)malloc(sizeof(Attribute));
  attr->name = strdup(name);
  attr->value = strdup(value);
  attr->next = NULL;
  return attr;
}

// Hàm tạo một nút cây mới
TreeNode *create_tree_node(char *tag_name, char *text, TreeNode *parent)
{
  TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
  node->tag_name = strdup(tag_name);
  node->attributes = NULL;
  node->text = text ? strdup(text) : NULL;
  node->first_child = NULL;
  node->next_sibling = NULL;
  node->parent = parent;
  return node;
}

// Hàm thêm một thuộc tính vào nút
void add_attribute(TreeNode *node, char *name, char *value)
{
  Attribute *attr = create_attribute(name, value);
  attr->next = node->attributes;
  node->attributes = attr;
}

// Hàm thêm một nút con vào nút cha
void add_child(TreeNode *parent, TreeNode *child)
{
  if (parent->first_child == NULL)
  {
    parent->first_child = child;
  }
  else
  {
    TreeNode *sibling = parent->first_child;
    while (sibling->next_sibling != NULL)
    {
      sibling = sibling->next_sibling;
    }
    sibling->next_sibling = child;
  }
}

// Hàm giải phóng bộ nhớ của cây
void free_tree(TreeNode *node)
{
  if (node == NULL)
    return;
  free_tree(node->first_child);
  free_tree(node->next_sibling);
  free(node->tag_name);
  if (node->text)
    free(node->text);
  Attribute *attr = node->attributes;
  while (attr != NULL)
  {
    Attribute *next = attr->next;
    free(attr->name);
    free(attr->value);
    free(attr);
    attr = next;
  }
  free(node);
}

// Hàm đọc đến ký tự delimiter và trả về chuỗi kết quả
char *read_until(const char **str, char delimiter)
{
  int capacity = 10;
  char *buffer = (char *)malloc(capacity);
  int length = 0;
  char ch;
  while ((ch = **str) != '\0' && ch != delimiter)
  {
    if (length + 1 >= capacity)
    {
      capacity *= 2;
      buffer = (char *)realloc(buffer, capacity);
    }
    buffer[length++] = ch;
    (*str)++;
  }
  if (ch == delimiter)
  {
    (*str)++;
  }

  buffer[length] = '\0';
  return buffer;
}

// Hàm xoá các ký tự xuống dòng và các ký tự cách trống đầu, cuối trong chuỗi
void remove_newlines(char *str)
{
  if (str == NULL)
    return;

  int start = 0;
  while (isspace(str[start]))
  {
    start++;
  }

  int end = strlen(str) - 1;
  while (end >= start && (isspace(str[end]) || str[end] == '\n'))
  {
    str[end] = '\0';
    end--;
  }
}

// Hàm kiểm tra thẻ tự đóng
int is_self_closing_tag(const char *tag_name)
{
  // Hàm này code cho có nên có thể để
  // return 0;
  const char *selfClosingTags[] = {
      "area", "base", "br", "col", "embed", "hr", "img", "input",
      "link", "meta", "param", "wbr", NULL};

  for (int i = 0; selfClosingTags[i] != NULL; i++)
  {
    if (strcmp(tag_name, selfClosingTags[i]) == 0)
    {
      return 1;
    }
  }
  return 0;
}

// Hàm in cây theo cấu trúc cây tổng quát
void print_tree(TreeNode *node, int depth)
{
  if (node == NULL)
    return;
  for (int i = 0; i < depth; i++)
    printf("  ");
  printf("<%s", node->tag_name);
  Attribute *attr = node->attributes;
  while (attr != NULL)
  {
    printf(" %s=\"%s\"", attr->name, attr->value);
    attr = attr->next;
  }
  printf(">");
  bool isContainText = false;
  if (node->text)
  {
    printf("%s", node->text);
    isContainText = true;
  }
  else
    printf("\n");

  print_tree(node->first_child, depth + 1);

  if (!is_self_closing_tag(node->tag_name))
  {
    if (!isContainText)
      for (int i = 0; i < depth; i++)
        printf("  ");
    printf("</%s>\n", node->tag_name);
  }

  print_tree(node->next_sibling, depth);
}

// Hàm phân tích thẻ HTML và tạo cây
TreeNode *parse_html(const char *str)
{
  TreeNode *root = NULL;
  TreeNode *current_node = NULL;
  char *tag_name;
  char *attr_name;
  char *attr_value;
  stack<TreeNode *> tagStack;

  while (*str != '\0' && *str != '^')
  {
    while (isspace(*str))
      str++;

    if (*str == '<')
    {
      str++;

      if (strncmp(str, "!--", 3) == 0)
      {
        str += 3;
        while (strncmp(str, "-->", 3) != 0)
          str++;
        str += 3;
        continue;
      }

      if (strncmp(str, "?xml", 4) == 0)
      {
        while (*str != '>')
          str++;
        str++;
        continue;
      }

      tag_name = read_until(&str, '>');

      int i = 0;

      if (tag_name[strlen(tag_name) - 1] == '"') {
        while (tag_name[i] != ' ') {
          i++;
        }
        str -= strlen(tag_name) - i;
        tag_name[i] = '\0';
      }
      
      if (tag_name[0] == '/')
      {
        if (!tagStack.empty() && strcmp(tagStack.top()->tag_name, tag_name + 1) == 0)
        {
          tagStack.pop();
          current_node = current_node->parent;
        }
        else
        {
          fprintf(stderr, "Error: Mismatched closing tag </%s>\n", tag_name + 1);
          free_tree(root);
          return NULL;
        }
      }
      else
      {
        TreeNode *new_node = create_tree_node(tag_name, NULL, current_node);
        if (root == NULL)
        {
          root = new_node;
        }
        else
        {
          add_child(current_node, new_node);
        }
        current_node = new_node;
        tagStack.push(new_node);
      }
      free(tag_name);

      while (isspace(*str))
        str++;
      while (*str != '>' && *str != '/' && *str != '<' && i != 0)
      {
        attr_name = read_until(&str, '=');
        str++;
        attr_value = read_until(&str, '"');
        str++;

        add_attribute(current_node, attr_name, attr_value);

        free(attr_name);
        free(attr_value);
        while (isspace(*str))
          str++;
      }

      if (*str == '/')
      {
        str += 2;
        if (!tagStack.empty())
        {
          tagStack.pop();
        }
        current_node = current_node->parent;
      }
    }
    else
    {
      char *text = read_until(&str, '<');
      remove_newlines(text);

      if (strlen(text) > 0 && current_node != NULL)
      {
        current_node->text = text;
      }
      else
      {
        free(text);
      }
      str--;
    }
  }

  if (!tagStack.empty())
  {
    fprintf(stderr, "Error: Missing closing tag for <%s>\n", tagStack.top()->tag_name);
    free_tree(root);
    return NULL;
  }

  return root;
}

// Hàm đọc toàn bộ đầu vào từ bàn phím cho đến ^
char *read_all_input()
{
  size_t capacity = 1024;
  char *buffer = (char *)malloc(capacity);
  size_t length = 0;

  int ch;
  while ((ch = getchar()) != '^')
  {
    if (length + 1 >= capacity)
    {
      capacity *= 2;
      buffer = (char *)realloc(buffer, capacity);
    }
    buffer[length++] = ch;
  }
  buffer[length] = '\0';
  return buffer;
}

int main()
{

  char *html_content = read_all_input();
  TreeNode *root = parse_html(html_content);

  if (root)
  {
    print_tree(root, 0);
    free_tree(root);
  }
  else
  {
    printf("Failed to parse XML.\n");
  }

  return EXIT_SUCCESS;
}