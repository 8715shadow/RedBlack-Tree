#ifndef TEST_RBTREE_H
#define TEST_RBTREE_H

#include <iostream>

enum Color
{
    red, black
};

struct Node
{
    int val; //节点信息
    Color color; //节点颜色
    Node *parent; //因为红黑树需要频繁访问父节点，所以加一个回头指针
    Node *lchild;
    Node *rchild;

    Node()
    {
        val = 0;
        color = black; //新生成的节点默认黑色
        parent = lchild = rchild = nullptr;
    }
};

struct RBTree
{
    Node *root;
    Node *nil; //外部空节点统一化

    RBTree();
    //不属于某个对象，设为静态
    void leftRotate(Node *x); //左旋
    void rightRotate(Node *x); //右旋
    void beRoot(Node *z); //成为根
    void insertNode(Node *z); //插入
    void insertFix(Node *z); //插入的维护
    void replaceNode(Node *u, Node* v); //v节点替换u节点
    Node* findMin(Node* p); //找子树中最小的节点
    void deleteNode(Node *z); //删除
    void deleteFix(Node *x); // 删除的维护

    void inorder(Node* rt); //debug用
    void remove(int v); //debug用
};

RBTree::RBTree()
{
    nil = new Node();
    nil->color = black;
    nil->lchild = nil->rchild = root;

    nil->parent = nullptr; //此属性不重要
    nil->val = -1; //此属性不重要

    root = nil; //开始啥都没，也就是root = nil
}

void RBTree::leftRotate(Node* x) //断了三个边，就重组三次
{
    Node* y = x->rchild;

    x->rchild = y->lchild;
    if (y->lchild!= nil) //如果不是nil,就反向连接，nil不能反向连接
        y->lchild->parent = x;

    y->parent = x->parent; //连接y到x的祖先，即y到x的原来位置
    if (x->parent == nil) //如果是根
        beRoot(y);
    else if (x == x->parent->lchild)
        x->parent->lchild = y;
    else // x == x->parent->rchild
        x->parent->rchild = y;

    y->lchild = x;
    x->parent = y;
}
void RBTree::rightRotate(Node* x)
{
    Node* y = x->lchild;
    x->lchild = y->rchild;
    if (y->rchild != nil)
        y->rchild->parent = x;
    y->parent = x->parent;
    if (x->parent == nil)
        beRoot(y);
    else if (x == x->parent->lchild)
        x->parent->lchild = y;
    else // x == x->parent->rchild
        x->parent->rchild = y;
    y->rchild = x;
    x->parent = y;
}

void RBTree::beRoot(Node *z) //处理节点为根，考虑到T.nil也需要不断更新
{
    root = z;
    z->parent = nil;
    nil->lchild = nil->rchild = root;
}

void RBTree::insertNode(Node *z)
{
    z->lchild = z->rchild = nil; //初始化指向nil非nullptr
    Node* x = root; // 从根开始的比较节点
    Node* y = nil;
    while (x != nil) // 找到最后空为止
    {
        y = x; //用于一直记录其父节点
        if (z->val < x->val)
            x = x->lchild;
        else
            x = x->rchild;
    }
    z->parent = y; //反向连认定父亲
    if (y == nil)// 树为空
        beRoot(z);
    else if (z->val < y->val) //正向连判定左孩还是右孩
        y->lchild = z;
    else
        y->rchild = z;
    z->color = red;
    insertFix(z);
}

void RBTree::insertFix(Node *z)
{
    this->root->color = black; //第一次插入为根节点时特殊处理
    while (z->parent->color == red) //插入总是红的，我们只需要双红修正，且不管情况是否需要递归，我们都递归
    {
        if (z->parent == z->parent->parent->lchild) //父亲跟爷爷的关系
        {
            Node* y = z->parent->parent->rchild; //y是叔叔节点
            if (y->color == red)                 //case 1：叔叔和父亲都是红色，且此时爷爷一定为黑色
            {                                    //三次变色即可
                z->parent->color = black;
                y->color = black;
                z->parent->parent->color = red;
                z = z->parent->parent; //会产生递归修复
                continue;
            }
            //下面两种旋转情况类似AVL树，LL&LR
            else if (z == z->parent->rchild)     //case 2：叔叔是黑色节点，自己为右孩子
            {                                    //左旋父亲，变为case3
                z = z->parent;
                leftRotate(z); //注意旋转之后z变了位置
            }
             //z == z->parent->lchild           //case 3：叔叔是黑色节点，自己为左孩子，三点一线
            z->parent->color = black;           //父亲变黑，爷爷变红，右旋爷爷
            z->parent->parent->color = red;
            rightRotate(z->parent->parent);
        }
        else //z->parent == z->parent->parent->rchild 上面一个if的左右孩颠倒
        {
            Node* y = z->parent->parent->lchild;
            if (y->color == red)                 //case 1：同理
            {
                z->parent->color = black;
                y->color = black;
                z->parent->parent->color = red;
                z = z->parent->parent; //会产生递归修复
                continue;
            }
            else if (z == z->parent->lchild)     //case 2：叔叔是黑色节点，自己为左孩子
            {                                    //左旋父亲，变为case3
                z = z->parent;
                rightRotate(z); //注意旋转之后z变了位置
            }
            //z == z->parent->rchild           //case 3：叔叔是黑色节点，自己为右孩子
            z->parent->color = black;           //父亲变黑，爷爷变红，左旋爷爷
            z->parent->parent->color = red;
            leftRotate(z->parent->parent);
        }
        this->root->color = black; //根节点总是需要染成黑色
    }
}

void RBTree::replaceNode(Node *u, Node* v) //v换到u的位置
{
    if (u->parent == nil)
        beRoot(v);
    else if (u == u->parent->lchild)
        u->parent->lchild = v;
    else
        u->parent->rchild = v;
    v->parent = u->parent;
}

Node* RBTree::findMin(Node *p)
{
    while (p->lchild != nil)
        p = p->lchild;
    return p;
}

void RBTree::deleteNode(Node *z)
{
    Node* x = nil; //记录用来顶替被删除元素的元素的位置！！！出问题的最低点，根源
    Node* y = z;
    Color yOriginalColor = y->color;
    if (z->lchild == nil) //没有左孩子，让右孩子接替或者两个都没有
    {
        x = z->rchild;
        replaceNode(z, z->rchild);
    }
    else if (z->rchild == nil) //没有右孩子，让左孩子接替
    {
        x = z->lchild;
        replaceNode(z, z->lchild);
    }
    else //有两个孩子
    {
        y = findMin(z->rchild);
        //因为一定有右孩子，所以找出来右子树最小的，放入这个位置，
        //由于中序遍历，所以这个位置也小于本来的所有右子树元素
        yOriginalColor = y->color; //后面让y节点和z节点同色，修正判断等效为右子树删除该颜色
        x = y->rchild;
        // 然后这里也分为两种情况：
        if (y->parent == z) //case 1：y的父亲就是z，说明y没有左孩子。和前面一样
            x->parent = y;
        else        //case 2：y有左孩子，方法是转换为case1
        {
            replaceNode(y, y->rchild);//用y的右孩子替换y，向上拱一格，露出，且y代替z位置
            y->rchild = z->rchild;//此时z右孩子是y右孩子，更新指针关系
            y->rchild->parent = y;
        }
        replaceNode(z, y);
        y->lchild = z->lchild;
        y->lchild->parent = y;
        y->color = z->color;//这里让y和z同色，整体不变，而右子树由于上面改变
    }

    //如果是红色，则没问题，原因如下：
    //1.红色不可能为根节点
    //2.不会出现双红，因为本身没有红色节点相邻
    //3.不会影响树高
    //对于双孩子的情况，相当于从右子树删除y
    if (yOriginalColor == black) //修复，删掉的如果是黑色，会影响黑高！！！
        deleteFix(x);
}

void RBTree::deleteFix(Node *x)// x是删除节点后的相同位置，被替代的节点
{
    while (x != root && x->color == black)
    {
        if (x == x->parent->lchild)
        {
            Node* y = x->parent->rchild; //兄弟
            if (y->color == red)                //case 1：右兄弟为红色，父亲一定黑
            {                                   //交换兄弟和父亲的颜色，左旋父亲，过度到case2
                y->color = black;
                x->parent->color = red;
                leftRotate(x->parent);
                y = x->parent->rchild; //更新兄弟
            }
            if (y->lchild->color == black && y->rchild->color == black) //case 2：y的两个孩子都是黑
            {                                                           //染红自己
                y->color = red;
                x = x->parent;// 更新指向原来的爷爷， 递推
            }
            else if (y->rchild->color == black)   //case 3：y左孩子是红，右孩子是黑
            {                                      //交换颜色，右旋y，转化为case4
                y->lchild->color = black;
                y->color = red;
                rightRotate(y);
                y = x->parent->rchild;// 更新
            }
            // y->lchild->color == black          //case 4：y左孩子是是黑色的
            y->color = x->parent->color;
            x->parent->color = black;
            y->rchild->color = black;
            leftRotate(x->parent);
            x = root;// 结束循环
        }
        else    //x == x->parent->rchild
        {
            Node* y = x->parent->lchild; //兄弟
            if (y->color == red)                //case 1：右兄弟为红色，父亲一定黑
            {                                   //交换兄弟和父亲的颜色，右旋父亲，过度到case2
                y->color = black;
                x->parent->color = red;
                rightRotate(x->parent);
                y = x->parent->lchild; //更新兄弟
            }
            if (y->lchild->color == black && y->rchild->color == black) //case 2：y的两个孩子都是黑
            {                                                           //染红自己
                y->color = red;
                x = x->parent;// 更新指向原来的爷爷， 递推
            }
            else if (y->lchild->color == black)   //case 3：y右孩子是红，左孩子是黑
            {                                      //交换颜色，左旋y，转化为case4
                y->rchild->color = black;
                y->color = red;
                leftRotate(y);
                y = x->parent->lchild;// 更新
            }
            // y->lchild->color == black          //case 4：y左孩子是是黑色的
            y->color = x->parent->color;
            x->parent->color = black;
            y->lchild->color = black;
            rightRotate(x->parent);
            x = root;// 结束循环
        }
    }
    x->color = black; //根节点必须黑色
}

void RBTree::inorder(Node *rt)
{
    if (rt == this->nil)
        return;
    inorder(rt->lchild);
    std::cout << rt->val << ' ';
    inorder(rt->rchild);
}

void RBTree::remove(int v)
{
    Node* p = root;
    while (p != nil)
    {
        if (v > p->val)
            p = p->rchild;
        else if (v < p->val)
            p = p->lchild;
        else
        {
            deleteNode(p);
            return; //一定要return不然一直删，卡死
        }
    }
}

#endif //TEST_RBTREE_H
