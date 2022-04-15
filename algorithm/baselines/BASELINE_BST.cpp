#include "BASELINE_BST.h"
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

void select(Node_t *node, vector <int> &cand, int ary) {
    if (cand.size() <= ary) {
        for (int i = 0; i < cand.size(); ++ i) {
            (node->pivot).push_back(sgnode(cand[i]));
        }
        cand.clear();
        return;
    }

    for (int i = 0; i < ary; ++ i) {
        (node->pivot).push_back(sgnode(cand.back()));
        cand.pop_back();
    }
}

void build(Node_t *node, vector <int> &cand, int ary) {
    select(node, cand, ary);
    if (cand.size() == 0) {
        return;
    }
    vector <int> childcand[10];
    for (int i = 0; i < ary; ++ i) {
        node->child.push_back(new Node_t());
    }

    for (int i = 0; i < cand.size(); ++ i) {
        double neardist = INF;
        int nearchild = -1;
        for (int j = 0; j < ary; ++ j) {
            double dis = dist(V, cand[i], node->pivot[j].idx, tag);
            if (nearchild == -1 || dis < neardist) {
                neardist = dis;
                nearchild = j;
            }
        }
        childcand[nearchild].push_back(cand[i]);
        if (node->pivot[nearchild].rad < neardist) {
            node->pivot[nearchild].rad = neardist;
        }
    }
    for (int i = 0; i < ary; ++ i) {
        build(node->child[i], childcand[i], ary);
    }
}

void buildBST(int ary) {
    vector <int> cand;
    for (int i = 0; i < nV; ++ i) {
        cand.push_back(i);
    } 
    random_shuffle(cand.begin(), cand.end());
    rt = new Node_t();
    build(rt, cand, ary);
}

void RangeQuery(location_t &loc, double radius, vector<int> &answers) {
    answers.clear();
    if (rt != NULL) {
        _RangeQuery(rt, loc, radius, answers);
    }
}

void _RangeQuery(Node_t *node, location_t &loc, double radius, vector<int> &answers) {
    vector <double> disloc;
    int nearchild = -1;
    double neardist = INF;
    for (int i = 0; i < node->pivot.size(); ++ i) {
        disloc.push_back(dist(loc, V[node->pivot[i].idx], tag));
        if (disloc[i] < radius) {
            answers.push_back(node->pivot[i].idx);
        }
        if (nearchild == -1 || neardist > disloc[i]) {
            neardist = disloc[i];
            nearchild = i;
        }
    }
    if (node->child.size() == 0) {
        return;
    }
    for (int i = 0; i < node->child.size(); ++ i) {
        if (disloc[i] - radius < neardist + radius && disloc[i] - radius < node->pivot[i].rad){
            _RangeQuery(node->child[i], loc, radius, answers);
        }
    }
}

void KnnQuery(location_t &loc, int kth, vector<int> &answers) {
    answers.clear();
    priority_queue<pair<double, int> > q;
    if (rt != NULL) {
        _KnnQuery(rt, loc, kth, q);
        while (q.size() > kth) {
            q.pop();
        }
        while (!q.empty()) {
            answers.push_back(q.top().second);
            q.pop();
        }
    }
}

void _KnnQuery(Node_t *node, location_t &loc, int kth, priority_queue<pair<double, int> > &q) {
    vector <pair<double, int> > disloc;
    for (int i = 0; i < node->pivot.size(); ++ i) {
        disloc.push_back(make_pair(dist(loc, V[node->pivot[i].idx], tag), i));
        addCand(q, kth, make_pair(disloc[i].first, node->pivot[i].idx));
    }
    if (node->child.size() == 0) {
        return;
    }
    sort(disloc.begin(), disloc.end());
    double neardist = disloc[0].first;
    for (int i = 0; i < node->child.size(); ++ i) {
        if (outCand(q, kth, (disloc[i].first - neardist) / 2.)) {
            break;
        }
        if (q.size() < kth || disloc[i].first - q.top().first < node->pivot[disloc[i].second].rad) {
            _KnnQuery(node->child[disloc[i].second], loc, kth, q);
        }
    }
}

void addCand(priority_queue<pair<double, int> > &q, int kth, pair<double, int> tmp) {
    q.push(tmp);
    while (q.size() > kth) {
        q.pop();
    }
}

bool outCand(priority_queue<pair<double, int> > &q, int kth, double dis) {
    return q.size() == kth && dis > q.top().first;
}

void freeBST(Node_t *node) {
    for (int i = 0; i < node->child.size(); ++ i) {
        freeBST(node->child[i]);
    }
    delete node;
}
