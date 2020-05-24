#pragma once
#include <exception>
#include <vector>
#include <initializer_list>
#include <utility>
#include <cmath>
#include <iostream>


template <typename T>
class KDTree;

//***WHEN COMPARING DISCRIMS NEED TO TAKE APPROPRIATE MEASURES FOR FLOATING POINT TYPES***

template <typename T>
class Node
{
public:
	Node(const T& val, std::initializer_list<double> il) : mVal(val), mDiscrims(il), mLeft(nullptr), mRight(nullptr) { }
	Node(const T& val, std::vector<double> discrimVec) : mVal(val), mDiscrims(discrimVec), mLeft(nullptr), mRight(nullptr) { }
	//checking for appropriate size is done by insert in KDTree

	const T& getVal() const { return mVal; }
	const std::vector<double>& getDiscrims() const { return mDiscrims; }

	friend class KDTree<T>;
private:
	T mVal;					//value stored in node that isn't used as a discriminator
	std::vector<double> mDiscrims;		//vector with number of elements equal to respective KDTree's mNumDiscrims
	Node<T>* mLeft, * mRight;		//child pointers
};

template <typename T>
class KDTree
{
public:
	KDTree() = delete;
	KDTree(const int& numDiscrims) : mRoot(nullptr), mNumDiscrims(numDiscrims) { }
	~KDTree() { cleanup(mRoot); }
	void print() const { printhelp(mRoot); }
	void regionQuery(const double& dist, const std::initializer_list<double>& il) const
	{
		if (il.size() != mNumDiscrims)
			throw std::length_error("Incorrect number of coordinates given.");

		std::vector<double> temp(il);
		queryhelp(mRoot, temp, dist, 0);
	}
	void regionQuery(double dist, const std::vector<double>& originCoords) const
	{
		if (originCoords.size() != mNumDiscrims)
			throw std::length_error("Incorrect number of coordinates given.");

		queryHelp(mRoot, originCoords, dist, 0);
	}
	
	void insert(const std::initializer_list<double>& il, const T& val)
	{
		if (il.size() != mNumDiscrims)
			throw std::length_error("Incorrect number of coordinates given.");

		std::vector<double> temp(il);
		mRoot = inserthelp(val, temp, 0, mRoot);
	}

	void insert(const std::vector<double>& discrimVec, const T& val)
	{
		if (discrimVec.size() != mNumDiscrims)
			throw std::length_error("Incorrect number of coordinates given.");

		mRoot = inserthelp(val, discrimVec, 0, mRoot);
	}

	void remove(const T& val)
	{
		if (mRoot == nullptr)
			return;
		mRoot = removeByVal(val, mRoot, 0);
	}
	void remove(const std::initializer_list<double> il)
	{
		if (il.size() != mNumDiscrims)
			throw std::length_error("Incorrect number of coordinates given.");

		std::vector<double> temp(il);
		mRoot = removeByDiscrim(temp, 0, mRoot);
	}
	void remove(const std::vector<double>& discrimVec)
	{
		if (discrimVec.size() != mNumDiscrims)
			throw std::length_error("Incorrect number of coordinates given.");

		mRoot = removeByDiscrim(discrimVec, 0, mRoot);
	}

	Node<T>* search(const T& val)
		//returns a matching node or nullptr
	{
		if (mRoot == nullptr)
			return nullptr;

		return searchByVal(val, mRoot);
	}

	Node<T>* search(const std::initializer_list<double>& il)
		//returns a matching node or nullptr
	{
		if (il.size() != mNumDiscrims)
			throw std::length_error("Incorrect number of coordinates given.");

		std::vector<double> temp(il);
		return searchByDiscrim(temp, 0, mRoot);
	}
	Node<T>* search(const std::vector<double>& discrimVec)
		//returns a matching node or nullptr
	{
		if (discrimVec.size() != mNumDiscrims)
			throw std::length_error("Incorrect number of coordinates given.");

		return searchByDiscrim(discrimVec, 0, mRoot);
	}
private:
	Node<T>* mRoot;
	int mNumDiscrims;
	void cleanup(Node<T>* root);
	void printhelp(Node<T>* root) const;
	void queryhelp(Node<T>* root, const std::vector<double>& coords, const double& dist, const int& currDiscrim) const;
	Node<T>* inserthelp(const T& val, const std::vector<double>& discrimVec, const int& currDiscrim, Node<T>* root);
	Node<T>* removeByVal(const T& val, Node<T>* root, const int& currDiscrim);
	Node<T>* removeByDiscrim(const std::vector<double>& discrimVec, const int& currDiscrim, Node<T>* root);
	std::pair<Node<T>*, int> findMin(const std::vector<double>& discrimVec, const int& discrim, Node<T>* root, const int& currLevel);
	Node<T>* searchByVal(const T& val, Node<T>* root);
	Node<T>* searchByDiscrim(const std::vector<double>& discrimVec, const int& currDiscrim, Node<T>* root);

};



template <typename T>
void KDTree<T>::cleanup(Node<T>* root)
{
	if (root == nullptr)
		return;
	cleanup(root->mLeft);
	cleanup(root->mRight);
	delete root;
}

template <typename T>
void KDTree<T>::printhelp(Node<T>* root) const
{
	if (root == nullptr)
		return;
	std::cout << root->mVal << std::endl;
	for (auto it = root->mDiscrims.begin(); it != root->mDiscrims.end(); ++it)
		std::cout << *it << " ";
	std::cout << std::endl;
	printhelp(root->mLeft);
	printhelp(root->mRight);
}

template <typename T>
void KDTree<T>::queryhelp(Node<T>* root, const std::vector<double>& coords, const double& dist, const int& currDiscrim) const
{
	if (root == nullptr)
		return;

	double currDist = 0;
	auto it1 = root->mDiscrims.begin(); 
	auto it2 = coords.begin();
	for (; it2 != coords.end(); ++it1, ++it2)	//use Euclidean formula to get distance between points
		currDist += pow(*it2 - *it1, 2);
	currDist = sqrt(currDist);

	if (currDist <= dist)
	{
		std::cout << root->mVal << ": " << std::endl;
		//std:: cout << "Coordinates - (
	}

	double tooBig = coords[currDiscrim] - root->mDiscrims[currDiscrim];
	double tooSmall = root->mDiscrims[currDiscrim] - coords[currDiscrim];
	if (tooBig > dist)
		queryhelp(root->mLeft, coords, dist, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1));
	else if (tooSmall > dist)
		queryhelp(root->mRight, coords, dist, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1));
	else
	{
		queryhelp(root->mLeft, coords, dist, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1));
		queryhelp(root->mRight, coords, dist, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1));
	}
}

template <typename T>
Node<T>* KDTree<T>::inserthelp(const T& val, const std::vector<double>& discrimVec, const int& currDiscrim, Node<T>* root)
{
	if (root == nullptr)
		return new Node<T>(val, discrimVec);

	if (discrimVec[currDiscrim] < root->mDiscrims[currDiscrim])	//go left
	{
		root->mLeft = inserthelp(val, discrimVec, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1), root->mLeft);
		return root;
	}
	else
	{
		root->mRight = inserthelp(val, discrimVec, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1), root->mRight);
		return root;
	}
}

template <typename T>
Node<T>* KDTree<T>::removeByVal(const T& val, Node<T>* root, const int& currDiscrim)
{
	if (root == nullptr)
		return nullptr;
	if (root->mVal == val)
	{
		if (root->mLeft == nullptr && root->mRight == nullptr)	//min is a leaf node, don't need to recursively swap values to maintain tree properties
		{
			delete root;
			return nullptr;
		}
		if (root->mLeft != nullptr && root->mRight == nullptr)
			std::swap(root->mLeft, root->mRight);

		std::pair<Node<T>*, int> min = findMin(root->mDiscrims, currDiscrim, root->mRight, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1));

		//min has children, need to recursively swap values for min node in right tree via remove

		root->mVal = min.first->mVal; root->mDiscrims = min.first->mDiscrims;
		root->mRight = removeByDiscrim(min.first->mDiscrims, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1), root->mRight);	//substitute with removeByDiscrim
		return root;
	}
	else
	{
		root->mLeft = removeByVal(val, root->mLeft, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1));
		root->mRight = removeByVal(val, root->mRight, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1));
		return root;
	}
}

template <typename T>
Node<T>* KDTree<T>::removeByDiscrim(const std::vector<double>& discrimVec, const int& currDiscrim, Node<T>* root)
{
	if (root == nullptr)
		return nullptr;
	double rootDiscrim = root->mDiscrims[currDiscrim];
	double vecDiscrim = discrimVec[currDiscrim];
	if ((rootDiscrim > vecDiscrim - 0.00001) && (rootDiscrim < vecDiscrim + 0.00001))	//considered equal
	{
		bool match = true;
		auto it1 = root->mDiscrims.begin();
		auto it2 = discrimVec.begin();
		for (; it2 != discrimVec.end() && match == true; ++it1, ++it2)
			//compare remaining discriminators for equality
		{
			if (!(*it1 > *it2 - 0.00001) && !(*it1 < *it2 + 0.00001))		//not equal
				match = false;
		}
		if (match)
		{
			if (root->mLeft == nullptr && root->mRight == nullptr)
			{
				delete root;
				return nullptr;
			}
			if (root->mLeft != nullptr && root->mRight == nullptr)
				std::swap(root->mLeft, root->mRight);

			std::pair<Node<T>*, int> min = findMin(root->mDiscrims, currDiscrim, root->mRight, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1));

			//min has children, need to recursively swap values for min node in right tree via remove

			root->mVal = min.first->mVal; root->mDiscrims = min.first->mDiscrims;
			root->mRight = removeByDiscrim(discrimVec, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1), root->mRight);
			return root;

		}
		else	//original discrim was equal, but mismatch found in other discrims - go right
		{
			root->mRight = removeByDiscrim(discrimVec, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1), root->mRight);
			return root;
		}
	}
	else if (vecDiscrim < rootDiscrim)
	{
		root->mLeft = removeByDiscrim(discrimVec, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1), root->mLeft);
		return root;
	}
	else		//vecDiscrim >= rootDiscrim (this vecDiscrim may be equal but the remaining discrims may not have matched
	{
		root->mRight = removeByDiscrim(discrimVec, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1), root->mRight);
		return root;
	}

}

template <typename T>
std::pair<Node<T>*, int> KDTree<T>::findMin(const std::vector<double>& discrimVec, const int& discrim, Node<T>* root, const int& currLevel)
//only called when root is not a leaf node
{
	std::pair<Node<T>*, int> left, right;

	if (root->mLeft != nullptr && root->mRight != nullptr)	//search both subtrees recursively and compare against root
	{
		left = findMin(discrimVec, discrim, root->mLeft, (currLevel + 1 == mNumDiscrims ? 0 : currLevel + 1));
		right = findMin(discrimVec, discrim, root->mRight, (currLevel + 1 == mNumDiscrims ? 0 : currLevel + 1));
		if (root->mDiscrims[discrim] < left.first->mDiscrims[discrim] && root->mDiscrims[discrim] < right.first->mDiscrims[discrim])
			return std::pair<Node<T>*, int>(root, currLevel);
		else if (left.first->mDiscrims[discrim] < root->mDiscrims[discrim] && left.first->mDiscrims[discrim] < right.first->mDiscrims[discrim])
			return left;
		else
			return right;
	}
	if (root->mLeft != nullptr && root->mRight == nullptr)	//search left recurisvely and compare against root
	{
		left = findMin(discrimVec, discrim, root->mLeft, (currLevel + 1 == mNumDiscrims ? 0 : currLevel + 1));
		if (root->mDiscrims[discrim] < left.first->mDiscrims[discrim])
			return std::pair<Node<T>*, int>(root, currLevel);
		else
			return left;
	}
	if (root->mLeft == nullptr && root->mRight != nullptr)	//search right recursively and compare against root
	{
		right = findMin(discrimVec, discrim, root->mRight, (currLevel + 1 == mNumDiscrims ? 0 : currLevel + 1));
		if (root->mDiscrims[discrim] < right.first->mDiscrims[discrim])
			return std::pair<Node<T>*, int>(root, currLevel);
		else
			return right;
	}
	else		//leaf node - reached end up recursion so return this node
		return std::pair<Node<T>*, int>(root, currLevel);

}

template <typename T>
Node<T>* KDTree<T>::searchByVal(const T& val, Node<T>* root)
//requires type T to be able to be compared for equality with == operator
{
	Node<T>* left, * right;
	if (root->mVal == val)
		return root;
	if (root->mLeft != nullptr)
	{
		left = searchByVal(val, root->mLeft);
		if (left != nullptr)
			return left;
	}
	if (root->mRight != nullptr)
	{
		right = searchByVal(val, root->mRight);
		if (right != nullptr)
			return right;
	}

	/*if (left != nullptr)
		return left;
	if (right != nullptr)
		return right;*/
	return nullptr;
}

template <typename T>
Node<T>* KDTree<T>::searchByDiscrim(const std::vector<double>& discrimVec, const int& currDiscrim, Node<T>* root)
{
	if (root == nullptr)
		return nullptr;

	if (discrimVec[currDiscrim] < root->mDiscrims[currDiscrim])		//go left
		return searchByDiscrim(discrimVec, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1), root->mLeft);
	else if (discrimVec[currDiscrim] > root->mDiscrims[currDiscrim]) 	//go right
		return searchByDiscrim(discrimVec, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1), root->mRight);
	else								//first discriminator matches - compare remaining discriminators
	{
		bool match = true;
		auto it1 = discrimVec.begin(); auto it2 = root->mDiscrims.begin();
		while (match && it1 != discrimVec.end())
		{
			if (*it1 != *it2)
				match = false;
			++it1;
			++it2;
		}
		if (match)		//found it
			return root;
		else			//go right
			return searchByDiscrim(discrimVec, (currDiscrim + 1 == mNumDiscrims ? 0 : currDiscrim + 1), root->mRight);
	}
}