#pragma once
#include <iostream>

using namespace std;

//利用递归思想实现的平衡二叉树插入与删除，很具有参考意义，部分算法已优化
template <class _Key, class _Data>
class BTree
{
private:
	struct Node
	{
	public:
		Node() : m_Key(), m_Data(), m_pLeft(nullptr), m_pRight(nullptr), m_nHeight(1) {};
		Node(const _Key& key, const _Data& data) : m_Key(key), m_Data(data), m_pLeft(nullptr), m_pRight(nullptr), m_nHeight(1) {};
		~Node() = default;
	public:
		_Key m_Key;
		_Data m_Data;
		Node* m_pLeft;
		Node* m_pRight;
		int m_nHeight;
	};
public:
	BTree() :m_pRoot(nullptr) {};
	virtual ~BTree();
	//BTree(const BTree& btr);
	//BTree& operator=(const BTree& btr);
public:
	void Insert(const _Key& key, const _Data& data);
	void Delete(const _Key& key);
	void Clear();
	//bool Delete(const _Key& key, const _Data& data = nullptr);
	//bool Find(const _Key& key, const _Data& data = nullptr);
	void Mid();
	void Pre();
	void Pos();
	//void NoRecursionPre();
	//void NoRecursionMid();
	//void NoRecursionPos();
private:
	int getHeight(Node* pNode);
	int getBalance(Node* pNode);
	void updateHeight(Node* pNode);
	Node* minValueNode(Node* node);

	Node* LL_Rotate(Node* pNode);
	Node* LR_Rotate(Node* pNode);
	Node* RR_Rotate(Node* pNode);
	Node* RL_Rotate(Node* pNode);

	Node* Insert(Node* pNode, const _Key& key, const _Data& data);
	Node* Delete(Node* pNode, const _Key& key);

	void Mid(Node* pNode);
	void Pre(Node* pNode);
	void Pos(Node* pNode);
	void DeleteStruct(Node* pNode);
private:
	Node* m_pRoot;
};

template <class _Key, class _Data>
void BTree<_Key, _Data>::Clear()
{
	DeleteStruct(m_pRoot);
	m_pRoot = nullptr;
}

template <class _Key, class _Data>
typename BTree<_Key, _Data>::Node* BTree<_Key, _Data>::minValueNode(Node* pNode)
{
	Node* current = pNode;

	while (current->m_pLeft != nullptr)
	{
		current = current->m_pLeft;
	}
	return current;
}

template <class _Key, class _Data>
typename BTree<_Key, _Data>::Node* BTree<_Key, _Data>::Delete(Node* pNode, const _Key& key)
{
	if (pNode == nullptr)
	{
		return pNode;
	}

	if (key < pNode->m_Key)
	{
		pNode->m_pLeft = Delete(pNode->m_pLeft, key);
	}
	else if (key > pNode->m_Key)
	{
		pNode->m_pRight = Delete(pNode->m_pRight, key);
	}
	else
	{
		if ((pNode->m_pLeft == nullptr) || (pNode->m_pRight == nullptr))
		{
			Node* temp = pNode->m_pLeft ? pNode->m_pLeft : pNode->m_pRight;
			delete pNode;
			if (temp == nullptr)		//无子节点（叶子节点）
			{
				pNode = nullptr;
			}
			else						//有单个子节点
			{
				pNode = temp;
			}
			return pNode;
		}
		else							//有两个子节点
		{
			Node* temp = minValueNode(pNode->m_pRight);
			pNode->m_Key = temp->m_Key;					//类内部要实现运算符重载执行深拷贝
			pNode->m_Data = temp->m_Data;					//同上
			pNode->m_pRight = Delete(pNode->m_pRight, temp->m_Key);	//递归目标变更为删除替换节点
		}
	}

	updateHeight(pNode);

	int balance = getBalance(pNode);

	if (balance > 1 && key < pNode->m_pLeft->m_Key)
	{
		return LL_Rotate(pNode);
	}
	if (balance < -1 && key > pNode->m_pRight->m_Key)
	{
		return RR_Rotate(pNode);
	}
	if (balance > 1 && key > pNode->m_pLeft->m_Key)
	{
		return LR_Rotate(pNode);
	}
	if (balance < -1 && key < pNode->m_pRight->m_Key)
	{
		return RL_Rotate(pNode);
	}

	return pNode;
}

template <class _Key, class _Data>
void BTree<_Key, _Data>::Delete(const _Key& key)
{
	m_pRoot = Delete(m_pRoot, key);
}

template <class _Key, class _Data>
void BTree<_Key, _Data>::updateHeight(Node* pNode)
{
	pNode->m_nHeight = max(getHeight(pNode->m_pLeft), getHeight(pNode->m_pRight)) + 1;
}

template <class _Key, class _Data>
typename BTree<_Key, _Data>::Node* BTree<_Key, _Data>::RL_Rotate(Node* pNode)
{
	pNode->m_pRight = LL_Rotate(pNode->m_pRight);
	return RR_Rotate(pNode);
}

template <class _Key, class _Data>
typename BTree<_Key, _Data>::Node* BTree<_Key, _Data>::RR_Rotate(Node* pNode)
{
	Node* pRNode = pNode->m_pRight;
	pNode->m_pRight = pRNode->m_pLeft;
	pRNode->m_pLeft = pNode;

	updateHeight(pNode);
	updateHeight(pRNode);

	return pRNode;
}

template <class _Key, class _Data>
typename BTree<_Key, _Data>::Node* BTree<_Key, _Data>::LR_Rotate(Node* pNode)
{
	pNode->m_pLeft = RR_Rotate(pNode->m_pLeft);
	return LL_Rotate(pNode);
}

template <class _Key, class _Data>
typename BTree<_Key, _Data>::Node* BTree<_Key, _Data>::LL_Rotate(Node* pNode)
{
	Node* pLNode = pNode->m_pLeft;
	pNode->m_pLeft = pLNode->m_pRight;
	pLNode->m_pRight = pNode;

	updateHeight(pNode);
	updateHeight(pLNode);

	return pLNode;
}

template <class _Key, class _Data>
int BTree<_Key, _Data>::getHeight(Node* pNode)
{
	if (pNode == nullptr)
	{
		return 0;
	}
	return pNode->m_nHeight;
}

template <class _Key, class _Data>
int BTree<_Key, _Data>::getBalance(Node* pNode)
{
	if (pNode == nullptr)
	{
		return 0;
	}
	return getHeight(pNode->m_pLeft) - getHeight(pNode->m_pRight);
}

//[递归算法]
template <class _Key, class _Data>
typename BTree<_Key, _Data>::Node* BTree<_Key, _Data>::Insert(Node* pNode, const _Key& key, const _Data& data)
{
	if (pNode == nullptr)
	{
		return new Node(key, data);
	}

	if (key < pNode->m_Key)
	{
		pNode->m_pLeft = Insert(pNode->m_pLeft, key, data);
	}
	else if (key > pNode->m_Key)
	{
		pNode->m_pRight = Insert(pNode->m_pRight, key, data);
	}
	else
	{
		return pNode;							//相等的情况return自身，后续可作链表处理
	}

	//利用递归的特性，自下而上把高度重刷一边
	//pNode->m_nHeight =  max(getHeight(pNode->m_pLeft), getHeight(pNode->m_pRight)) + 1;
	//多处要用，直接封装函数
	updateHeight(pNode);
	//获取高度差值
	int balance = getBalance(pNode);

	//这里的4个判断可以优化
	if (balance > 1 && key < pNode->m_pLeft->m_Key)			//LL型右旋
	{
		return LL_Rotate(pNode);
	}
	if (balance < -1 && key > pNode->m_pRight->m_Key)		//RR型左旋
	{
		return RR_Rotate(pNode);
	}
	if (balance > 1 && key > pNode->m_pLeft->m_Key)			//LR型左旋再右旋
	{
		return LR_Rotate(pNode);
	}
	if (balance < -1 && key < pNode->m_pRight->m_Key)		//RL型右旋再左旋
	{
		return RL_Rotate(pNode);
	}

	return pNode;
}

template <class _Key, class _Data>
void BTree<_Key, _Data>::Insert(const _Key& key, const _Data& data)
{
	m_pRoot = Insert(m_pRoot, key, data);
}

template <class _Key, class _Data>
void BTree<_Key, _Data>::DeleteStruct(Node* pNode)
{
	if (pNode == nullptr)
	{
		return;
	}
	DeleteStruct(pNode->m_pLeft);
	DeleteStruct(pNode->m_pRight);
	delete pNode;
}

template <class _Key, class _Data>
BTree<_Key, _Data>::~BTree()
{
	DeleteStruct(m_pRoot);
}

template <class _Key, class _Data>
void BTree<_Key, _Data>::Pos(Node* pNode)
{
	if (pNode == nullptr)
	{
		return;
	}
	Pos(pNode->m_pLeft);
	Pos(pNode->m_pRight);
	cout << pNode->m_Key << "：" << pNode->m_Data << endl;
}

template <class _Key, class _Data>
void BTree<_Key, _Data>::Pos()
{
	Pos(m_pRoot);
}

template <class _Key, class _Data>
void BTree<_Key, _Data>::Pre(Node* pNode)
{
	if (pNode == nullptr)
	{
		return;
	}
	cout << pNode->m_Key << "：" << pNode->m_Data << endl;
	Pre(pNode->m_pLeft);
	Pre(pNode->m_pRight);
}

template <class _Key, class _Data>
void BTree<_Key, _Data>::Pre()
{
	Pre(m_pRoot);
}

template <class _Key, class _Data>
void BTree<_Key, _Data>::Mid(Node* pNode)
{
	if (pNode == nullptr)
	{
		return;
	}
	Mid(pNode->m_pLeft);
	cout << pNode->m_Key << "：" << pNode->m_Data << endl;
	Mid(pNode->m_pRight);
}

template <class _Key, class _Data>
void BTree<_Key, _Data>::Mid()
{
	Mid(m_pRoot);
}