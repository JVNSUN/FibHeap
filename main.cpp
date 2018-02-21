#include <iostream>
#include <limits>

struct node {
    int key, degree;
    bool mark;
    node *p, *child, *left, *right;
    explicit node(int key) {
        this->key = key;
    }
    void insertToList(node*&);
    void deleteFromList();
    void mergeWithList(node*&);
    node* copyToMakeARoot();
};

void node::insertToList(node *&aNode) {
    if (aNode == nullptr) {
        aNode = this;
        aNode->left = aNode->right = aNode;
        return;
    }
    else {
        this->right = aNode->right;
        aNode->right->left = this;
        aNode->right = this;
        this->left = aNode;
        return;
    }
}

void node::deleteFromList() {
    this->right->left = this->left;
    this->left->right = this->right;
}

void node::mergeWithList(node *&aNode) {
	for (auto i = this->right; i != this; i = this->right) {
		i->deleteFromList();
		i->insertToList(aNode);
	}
	this->insertToList(aNode);
}

node* node::copyToMakeARoot() {
    auto copy = new node(this->key);
    copy->p = nullptr;
    copy->mark = false;
    copy->child = this->child;
	copy->degree = this->degree;
    return copy;
}

class FibHeap {
public:
    node *min;
    int n;
    static FibHeap* makeHeap();
    void insert(int);
    static FibHeap* makeUnion(FibHeap*&, FibHeap*&);
    node* extractMin();
    void consolidate();
    void heapLink(node*& y, node*& x);
    void decreaseKey(node *&x, int k);
    void cut(node *&x, node *&y);
    void cascadingCut(node *&y);
    void heapDelete(node *&x);
};

FibHeap* FibHeap::makeHeap() {
    auto newHeap = new FibHeap;
    newHeap->n = 0;
    newHeap->min = nullptr;
    return newHeap;
}

void FibHeap::insert(int key) {
    auto x = new node(key);
    x->degree = 0;
    x->p = nullptr;
    x->child = nullptr;
    x->mark = false;
    if (this->min == nullptr) {
        x->insertToList(this->min);
        this->min = x;
    }
    else {
        x->insertToList(this->min);
        if (x->key < this->min->key)
            this->min = x;
    }
    (this->n)++;
}

FibHeap* FibHeap::makeUnion(FibHeap *& h1, FibHeap *& h2) {
    auto h = FibHeap::makeHeap();
    h->min = h1->min;
    h2->min->mergeWithList(h->min);
    if (h1->min == nullptr || (h2->min != nullptr && h2->min->key < h1->min->key)) {
        h->min = h2->min;
    }
    h->n = h1->n + h2->n;
    return h;
}

node* FibHeap::extractMin() {
    auto z = this->min;
    if (z != nullptr) {

        if (z->degree > 0) {
            auto c = z->child;
            for (auto i = c->right; i != c; i = c->right) {
                i->deleteFromList();
                i->insertToList(z);
                i->p = nullptr;
            }
            c->insertToList(z);
            c->p = nullptr;
        }

        z->deleteFromList();
        if (z == z->right) {
            this->min = nullptr;
        }
        else {
            this->min = z->right;
            this->consolidate();
        }
        (this->n)--;
    }
    return z;
}

void FibHeap::consolidate() {

    auto A = new node*[this->n];
    for (int i = 0; i < this->n; i++) {
        A[i] = nullptr;
    }

    auto w = this->min;
    for (auto i = w->right; i != w; i = i->right) {
        auto x = i;
        auto d = x->degree;
        while (A[d] != nullptr) {
            auto y = A[d];
            if (x->key > y->key) {
                auto t = x;
                x = y;
                y = t;
            }
            heapLink(y, x);
            A[d] = nullptr;
            d++;
        }
        A[d] = x;
    }
    auto x = w;
    auto d = x->degree;
    while (A[d] != nullptr) {
        auto y = A[d];
        if (x->key > y->key) {
            auto t = x;
            x = y;
            y = t;
        }
        heapLink(y, x);
        A[d] = nullptr;
        d++;
    }
    A[d] = x;
    this->min = nullptr;
    for (int i = 0; i < this->n; i++) {
        if (A[i] != nullptr) {
            if (this->min == nullptr) {
                A[i]->insertToList(this->min);
                this->min = A[i];
            }
            else {
                A[i]->insertToList(this->min);
                if (A[i]->key < this->min->key) {
                    this->min = A[i];
                }
            }
        }
    }
    delete[](A);
}

void FibHeap::heapLink(node *&y, node *&x) {
    y->deleteFromList();
    y->p = x;
    y->insertToList(x->child);
    (x->degree)++;
    y->mark = false;
}

void FibHeap::decreaseKey(node *&x, int k) {
    if (k > x->key) {
        return;
    }
    x->key = k;
    auto y = x->p;
    if ((y != nullptr) && (x->key < y->key)) {
        this->cut(x, y);
        this->cascadingCut(y);
    }
}

void FibHeap::cut(node *&x, node *&y) {
    x->deleteFromList();
    auto copyToInsert = x->copyToMakeARoot();
    if (y->degree == 1) {
        delete(x);
		y->child = nullptr;
    }
    else {
        y->child = x->right;
    }
    (y->degree)--;
    copyToInsert->insertToList(this->min);
	if (copyToInsert->key < this->min->key)
		this->min = copyToInsert;
}

void FibHeap::cascadingCut(node *&y) {
    auto z = y->p;
    if (z != nullptr) {
        if (y->mark == false) {
            y->mark = true;
        }
        else {
            this->cut(y, z);
            cascadingCut(z);
        }
    }
}

void FibHeap::heapDelete(node *&x) {
    this->decreaseKey(x, std::numeric_limits<int>::min());
    this->extractMin();
}

int main() {
    auto fibHeap = FibHeap::makeHeap();
    fibHeap->insert(5);
    fibHeap->insert(2);
    std::cout << fibHeap->extractMin()->key;
    return 0;
}
