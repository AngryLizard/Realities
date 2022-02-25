#pragma once


#include "ParabolaTree.h"


namespace Voronoi
{
	ParabolaTree::ParabolaTree(Cell* cell)
		:	_parent(nullptr),
			_right(nullptr),
			_left(nullptr),
			_edge(nullptr), 
			_cell(cell)
	{
	}

	ParabolaTree::ParabolaTree(ParabolaTree* left, Edge* edge)
		:	_parent(nullptr),
			_left(left),
			_cell(edge->getOut())
	{
		// Set parent
		_left->_parent = this;

		// Set other child
		_right = new ParabolaTree(_cell);
		_right->_parent = this;
		
		// Compute edge
		_edge = new Edge(edge);
	}


	ParabolaTree::~ParabolaTree()
	{
		// Delete children
		if (_left != nullptr) 
			delete(_left);

		if (_right != nullptr) 
			delete(_right);
	}

	Edge* ParabolaTree::getEdge()
	{
		return(_edge);
	}

	void ParabolaTree::update(Intersection* meet, CmpHeap<Event*>*& events)
	{
		// Get edge left and right of intersection
		ParabolaTree* left = meet->left->neighbour;
		ParabolaTree* right = meet->right->intersection.right;
		
		// Remove intersections from events
		if (left != nullptr)
			left->intersection.extract(&Event::slt);

		if (meet->right != nullptr)
			meet->right->intersection.extract(&Event::slt);

		// Actually intersect edges
		_edge = meet->connect();

		// Connect left and right edges to intersection edge
		link(left, right, events);
	}

	void ParabolaTree::replace(ParabolaTree* child, ParabolaTree* tree)
	{
		// Set parent parameters
		child->_parent = nullptr;
		tree->_parent = this;
		
		// Replace right child
		if (_left == child)
			_left = tree;
		else _right = tree;
	}

	void ParabolaTree::detach(ParabolaTree* tree, CmpHeap<Event*>*& events)
	{
		// Find right leaf
		if (this->_right->_edge == nullptr)
		{
			// Move node and add edge from intersection
			this->_parent->replace(this, this->_left);
			tree->update(&intersection, events);
			intersection.extract(&Event::slt);
			this->_left = nullptr;
			delete(this);
		}
		else
		{
			// Move node and add edge from intersection
			tree->_parent->replace(tree, tree->_right);
			this->update(&intersection, events);
			tree->intersection.extract(&Event::slt);
			tree->_right = nullptr;
			delete(tree);
		}
	}

	void ParabolaTree::link(ParabolaTree* left, ParabolaTree* right, CmpHeap<Event*>*& events)
	{
		neighbour = nullptr;

		if (left != nullptr)
		{
			// link left intersection
			left->intersection.right = this;
			left->intersection.left = left;

			// Set neighbour
			neighbour = left;

			// Look for intersection
			if (left->intersection.intersect())
			{
				left->intersection.extract(&Event::slt);
				if (events == nullptr) left->intersection.attach(&events);
				else events->insert(&left->intersection, &Event::slt);
			}
		}

		if (right != nullptr)
		{
			// link right intersection
			intersection.left = this;
			intersection.right = right;

			// Set right neighbour
			right->neighbour = this;

			// Look for intersection
			if (intersection.intersect())
			{
				intersection.extract(&Event::slt);
				if (events == nullptr) intersection.attach(&events);
				else events->insert(&intersection, &Event::slt);
			}
		}
	}

	Vector ParabolaTree::getAnchor(Cell* left, Cell* right)
	{
		// Get sweep position when intersection occurs
		Vector center = left->center.p;
		float sweep = right->parabola(center);

		// Custom behaviour on vertical edges
		if (isnan(sweep))
		{
			// Put position far down
			float x = right->center.p.x;
			float median = x + (center.x - x) / 2;
			return(Vector(median, center.y - 1e5f));
		}
		return(Vector(center.x, sweep));
	}
	
	void ParabolaTree::insert(ParabolaTree* tree, CmpHeap<Event*>*& events)
	{
		// Check if child is an edge or a cell
		if (_edge == nullptr)
		{
			// Get neighbours
			ParabolaTree* left = tree->intersection.left;
			ParabolaTree* right = tree->intersection.right;
			
			// Create left edge
			Vector anchor = getAnchor(tree->_cell, _cell);
			_edge = new Edge(anchor, tree->_cell, _cell);

			// Create cell
			_left = new ParabolaTree(_cell);
			_left->_parent = this;

			// Link intersections
			link(left, right, events);

			// Only create right edge if not vertical
			if (_edge->isVertical())
			{
				// Also create cell to the right
				_right = new ParabolaTree(tree->_cell);
				_right->_parent = this;
				// Create open edge facing down
				new Edge(_edge);
				delete(tree);
			}
			else
			{
				// Create right edge
				_right = new ParabolaTree(tree, _edge);
				_right->_parent = this;
				// Link intersections
				_right->link(this, right, events);
			}
		}
		else
		{
			// Check if left of the intersection
			if (_edge->disparity(tree->_cell->center.p) >= 0)
			{
				// Set neighbours
				tree->_parent = this;
				tree->intersection.left = this;
				tree->intersection.right = intersection.right;
				_right->insert(tree, events);
			}
			else
			{
				// Set neighbours
				tree->_parent = this;
				tree->intersection.right = this;
				tree->intersection.left = neighbour;
				_left->insert(tree, events);
			}
		}
	}


	ParabolaTreeRoot::ParabolaTreeRoot()
		: _tree(nullptr)
	{
	}

	ParabolaTreeRoot::~ParabolaTreeRoot()
	{
		if(_tree != nullptr)
			delete(_tree);
	}

	void ParabolaTreeRoot::insert(ParabolaTree* tree, CmpHeap<Event*>*& events)
	{
		if (_tree == nullptr) _tree = tree;
		else _tree->insert(tree, events);
	}

}