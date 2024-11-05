struct TNode{
    int spike;
    double time;
    TNode *next;
};

TNode *first, *last;

void insert(int spike, double time){
    TNode *newNode = new TNode{spike, time};
    last->next = newNode;
    last = last->next;
}
