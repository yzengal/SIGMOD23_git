#include "BASELINE_MVPT.h"
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

void build(Node_mvpt *node, vector <int> &cand, int ary, int bsize) {
    if (cand.size() < bsize) {
        node->isleaf = true;
        for (int i = 0; i < cand.size(); ++ i) {
            node->bucket.push_back(cand[i]);
        }
        return;
    }

    int pi = rand() % cand.size();
    node->idx = cand[pi];
    cand[pi] = cand.back();
    cand.pop_back();
    vector <pair<double, int> > disloc;
    for (int i = 0; i < cand.size(); ++ i) {
        disloc.push_back(make_pair(dist(V, node->idx, cand[i], tag), cand[i]));
    }
    sort(disloc.begin(), disloc.end());
    double prev = disloc[0].first;
    int head = 0;
    int tail = 0;
    for (int i = 0; i < ary; ++ i) {
        node->child.push_back(new Node_mvpt());
        node->lower.push_back(prev);
        tail = head + disloc.size() / ary;
        if (i >= ary - disloc.size() % ary) {
            tail ++;
        }
        
        vector <int> tmp;
        for (int i = head; i < tail; ++ i) {
            tmp.push_back(disloc[i].second);
        }
        build(node->child[i], tmp, ary, bsize);
        if (tail < disloc.size()) {
            prev = disloc[tail].first;
            head = tail;
        }
    }
}

void buildMVPT(int ary, int bsize) {
    vector <int> cand;
    for (int i = 0; i < nV; ++ i) {
        cand.push_back(i);
    }
    rt = new Node_mvpt();
    build(rt, cand, ary, bsize);
}

void RangeQuery(location_t &loc, double radius, vector<int> &answers) {
    answers.clear();
    if (rt != NULL) {
        _RangeQuery(rt, loc, radius, answers);
    }
}

void _RangeQuery(Node_mvpt *node, location_t &loc, double radius, vector<int> &answers) {
    if (node->isleaf) {
        for (int i = 0; i < node->bucket.size(); ++ i) {
            if (dist(loc, V[node->bucket[i]], tag) < radius) {
                answers.push_back(node->bucket[i]);
            }
        }
        return;
    } 
    double neardist = dist(loc, V[node->idx], tag);
    if (neardist < radius) {
        answers.push_back(node->idx);
    }
    for (int i = 0; i < node->child.size(); ++ i) {
        if (i == node->child.size() - 1 || (node->lower[i+1] > neardist - radius && node->lower[i] < neardist + radius)) {
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

void _KnnQuery(Node_mvpt *node, location_t &loc, int kth, priority_queue<pair<double, int> > &q) {
    if (node->isleaf) {
        for (int i = 0; i < node->bucket.size(); ++ i) {
            pair<double, int> tmp = make_pair(dist(loc, V[node->bucket[i]], tag), node->bucket[i]);
            addCand(q, kth, tmp);
        }
        return;
    }
    double ndis = dist(loc, V[node->idx], tag);
    pair<double, int> tmp = make_pair(ndis, node->idx);
    addCand(q, kth, tmp);

    int cur = 0;
    for (; cur < node->lower.size(); ++ cur) {
        if (node->lower[cur] > ndis) {
            break;
        }
    }
    cur --;
    bool endfront = false, endbehind = false;
    for (int d = 0; d <= node->lower.size(); ++ d) {
        int i = cur - d;
        if (i < 0) {
            endfront = true;
        }
        if (!endfront) {
            if (i == node->lower.size() - 1 || q.size() < kth || ndis - node->lower[i+1] < q.top().first) {
                _KnnQuery(node->child[i], loc, kth, q);
            } else {
                endfront = true;
            }
        }
        if (d == 0) continue;
        i = cur + d;
        if (i >= node->lower.size()) {
            endbehind = true;
        }
        if (!endbehind) {
            if (q.size() < kth || node->lower[i] - ndis < q.top().first) {
                _KnnQuery(node->child[i], loc, kth, q);
            } else {
                endbehind = true;
            }
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

void freeMVPT(Node_mvpt *node) {
    for (int i = 0; i < node->child.size(); ++ i) {
        freeMVPT(node->child[i]);
    }
    delete node;
}