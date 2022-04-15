#include "BASELINE_BKT.h"
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

void addbkt(Node_bkt *node, int cand, int step, int bsize) {
    if (node->isleaf) {
        if (node->bucket.size() < bsize) {
            node->bucket.push_back(cand);
        } else {
            node->isleaf = false;
            node->idx = cand;
            for (int i = 0; i < node->bucket.size(); ++ i) {
                addbkt(node, node->bucket[i], step, bsize);
            }
            node->bucket.clear();
        }
        return;
    }
    double dis = dist(V, cand, node->idx, tag);
    int pos = 0;
    for (; pos < node->child.size(); ++ pos) {
        if (node->lower[pos] < dis + EPS && node->lower[pos] + step > dis) {
            break;
        }
    }
    if (pos == node->child.size()) {
        node->child.push_back(new Node_bkt(step));
        node->lower.push_back(dis);
        node->child[pos]->isleaf = true;
    }
    addbkt(node->child[pos], cand, step, bsize);
}

void buildBKT(int step, int bsize) {
    rt = new Node_bkt(step);
    rt->isleaf = true;
    vector <int> cand;
    for (int i = 0; i < nV; ++ i) {
        cand.push_back(i);
    }
    random_shuffle(cand.begin(), cand.end());
    for (int i = 0; i < nV; ++ i) {
        addbkt(rt, cand[i], step, bsize);
    }
}

void RangeQuery(location_t &loc, double radius, vector<int> &answers) {
    answers.clear();
    if (rt != NULL) {
        _RangeQuery(rt, loc, radius, answers);
    }


}

void _RangeQuery(Node_bkt *node, location_t &loc, double radius, vector<int> &answers) {
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
        if (node->lower[i] + node->step > neardist - radius && node->lower[i] < neardist + radius + EPS) {
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

void _KnnQuery(Node_bkt *node, location_t &loc, int kth, priority_queue<pair<double, int> > &q) {
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

    double maxd = 0;
    for (int i = 0; i < node->lower.size(); ++ i) {
        maxd = max(maxd, node->lower[i]);
    }
    for (int d = 0; (q.size() < kth || d < q.top().first + node->step) && (d < ndis || d < maxd + node->step - ndis); d += node->step) {
        for (int i = 0; i < node->lower.size(); ++ i) {
            if (node->lower[i] < ndis - d + EPS && node->lower[i] + node->step > ndis - d) {
                _KnnQuery(node->child[i], loc, kth, q);
            }
        }
        if (d == 0 || (q.size() >= kth && d > q.top().first + node->step)) {
            continue;
        }
        for (int i = 0; i < node->lower.size(); ++ i) {
            if (node->lower[i] < ndis + d + EPS && node->lower[i] + node->step > ndis + d) {
                _KnnQuery(node->child[i], loc, kth, q);
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

void freeBKT(Node_bkt *node) {
    for (int i = 0; i < node->child.size(); ++ i) {
        freeBKT(node->child[i]);
    }
    delete node;
}
