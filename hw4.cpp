#include <iostream>
#include <list>
#include <stack>
#include <vector>
#include <algorithm>
#include <math.h>
#include <fstream>
#include <sstream>
#include <string>


using namespace std;

struct vertex {
    int x;
    int y;
};

struct edge {
    float distance;
    int from;
    int to;
};

struct scc_ {
    list<int> scc;
};

bool compare_dist(edge e1, edge e2) {
    return e1.distance < e2.distance;
}


class Graph {
    private:
        int num_vertex;
        vector<vertex> vertexList;
        list<int> *adj;
        stack<int> fin;
        int extra_;  // The number of extra edge should be added
        int *color,  // 0:white, 1:gray, 2:black
            *predecessor,
            *discover,
            *finish;
            
    public:
        Graph();
        Graph(int num_vertex);
        void addVertex(int x_, int y_);
        void addEdge(int v, int w);
        void DFS(int start);
        void DFSVisit(int vertex, int &time);
        void Initialization();
        Graph getTranspose();
        void SetCollapsing(int vertex);
        void PrintSCCs(int start, ofstream &output);
};

Graph::Graph () {
    this->num_vertex = 0;
}

Graph::Graph(int num_vertex) {
    this->num_vertex = num_vertex;
    adj = new list<int>[num_vertex];
    vertexList.reserve(num_vertex);
}

void Graph::addVertex(int x_, int y_) {
    vertex temp_vertex;
    temp_vertex.x = x_;
    temp_vertex.y = y_;
    vertexList.push_back(temp_vertex);
}

void Graph::addEdge(int v, int w) {
    adj[v].push_back(w);  // Add w to v's list.
}


void Graph::Initialization() {
    color = new int[num_vertex];
    discover = new int[num_vertex];
    finish = new int[num_vertex];
    predecessor = new int[num_vertex];

    
    for (int i = 0; i < num_vertex; i++) { 
        color[i] = 0;
        discover[i] = 0;
        finish[i] = 0;
        predecessor[i] = -1;
    }
}


Graph Graph::getTranspose() {
    Graph gT(num_vertex);
    for (int i = 0; i < num_vertex; i++) {
        for (list<int>::iterator itr = adj[i].begin();
             itr != adj[i].end(); itr++) {
            gT.adj[*itr].push_back(i);
        }
    }
    return gT;
}


void Graph::SetCollapsing(int vertex) {
    int root;  // root
    for (root = vertex; predecessor[root] >= 0; root = predecessor[root]);

    while (vertex != root) {
        int parent = predecessor[vertex];
        predecessor[vertex] = root;
        vertex = parent;
    }
}


void Graph::DFS(int start) {
    int counter = 0;
    color = new int[num_vertex];
    discover = new int[num_vertex];
    finish = new int[num_vertex];
    predecessor = new int[num_vertex];
    
    
    for (int i = 0; i < num_vertex; i++) { 
        color[i] = 0;
        discover[i] = 0;
        finish[i] = 0;
        predecessor[i] = -1;
    }

    int time = 0;
    int i = start;
    for (int j = 0; j <= num_vertex; j++) { // All vertices should be searched
        if (color[i] == 0) {                // If the vertex i is white, DFSVisit(i)
            DFSVisit(i, time);
        }
        i = j;                             // Make sure that every vertex was searched
    }
}


void Graph::DFSVisit(int vertex, int &time){   // A white vertex was searched, DFSVisit()
    color[vertex] = 1;                         // Change color from white to gray
    discover[vertex] = ++time;                 // Update the discover time
    for (list<int>::iterator itr = adj[vertex].begin();
         itr != adj[vertex].end(); itr++) {
        if (color[*itr] == 0) {                // If the searched vertex is white
            predecessor[*itr] = vertex;        // Update its predecessor
            DFSVisit(*itr, time);              // Use the vertex as a new start, DFSVisit()
        }
    }
    color[vertex] = 2;                         // After searching all neightbors, change the color from gray to black
    finish[vertex] = ++time;                   // Update the finish time
    fin.push(vertex);                          // Push into stack
}



void Graph::PrintSCCs(int start, ofstream &output) {
    DFS(start);
    
    /* Transpose of graph */
    Graph gT(num_vertex);
    gT = getTranspose();
      
    int finishLargetoSmall[num_vertex];
    for (int i = 0; i < num_vertex; i++) {
        int v = fin.top();
        finishLargetoSmall[i] = v;
        fin.pop();
    }

    gT.Initialization();
    int time = 0;

    for (int i = 0; i < num_vertex; i++) {
        gT.DFSVisit(finishLargetoSmall[i], time);
    }
 
    for (int i = 0; i< num_vertex; i++) {
        gT.SetCollapsing(i);
    }

    
    /* Count SCC */
    int *SCC_num = new int[num_vertex];  // The No. of SCC
    int num_cc = 0;
    vector<scc_> sccList;   // Store the vertex
    scc_ temp_scc;
    int count = 0;
    for (int i = 0; i < num_vertex; i++) {
        if (gT.predecessor[i] < 0) {
            num_cc++;
            SCC_num[i] = num_cc;
            (temp_scc.scc).push_back(i);
            count++;
            for (int j = 0; j < num_vertex; j++) {
                if (gT.predecessor[j] == i) {
                    SCC_num[j] = num_cc;
                    (temp_scc.scc).push_back(j);
                    ++count;
                }
            }
            sccList.push_back(temp_scc);
            (temp_scc.scc).clear();
        }
    }
    sccList.shrink_to_fit();

    /* Make SCC's adjancency list */
    list<int> *scc_adj = new list<int>[sccList.size()];   // Store the No. of scc
    

    bool *scc_add = new bool[sccList.size()];
    for (int i = 0; i < sccList.size(); i++) {
        scc_add[i] = false;
    }
    
    
    for (int i = 0; i < sccList.size(); i++) {
        for (list<int>::iterator j = sccList[i].scc.begin(); j != sccList[i].scc.end(); j++) {
            for (list<int>::iterator itr = adj[*j].begin(); itr != adj[*j].end(); itr++) {
                if (scc_add[SCC_num[*itr]] == false && SCC_num[*itr] != (i+1)) {
                    scc_adj[i].push_back(SCC_num[*itr]);
                    scc_add[SCC_num[*itr]] = true;
                }
            }
        }
        for (int i = 0; i < sccList.size(); i++) {
            scc_add[i] = false;
        }
    }

    delete [ ] scc_add;

    bool *visited_scc = new bool[sccList.size()];
    for (int i = 0; i < sccList.size(); i++) {
        visited_scc[i] = false;
    }

    visited_scc[0] = true;
    for (int i = 0; i < sccList.size(); i++) {
        for (list<int>::iterator itr = scc_adj[i].begin(); itr != scc_adj[i].end(); itr++) {
            visited_scc[*itr - 1] = true;
        }
    }

    count = 0;
    list<int> not_reached;
    for (int i = 0; i < sccList.size(); i++) {
        if (visited_scc[i] == false) {
            not_reached.push_back(i);
            ++count;
        }
    }
    
    extra_ = count;
    

    vector<edge> extra_edge;
    edge edge_temp;
    for (int l = 0; l < sccList.size(); l++) {
        for (list<int>::iterator i = sccList[l].scc.begin(); i != sccList[l].scc.end(); i++) {
            for (list<int>::iterator j = not_reached.begin(); j != not_reached.end(); j++) {
                for (list<int>::iterator k = sccList[*j].scc.begin(); k != sccList[*j].scc.end(); k++) {
                    edge_temp.from = *i;
                    edge_temp.to = *k;
                    edge_temp.distance = sqrt(pow(((vertexList.at(*i)).x - (vertexList.at(*k)).x),2) +\
                                              pow(((vertexList.at(*i)).y - (vertexList.at(*k)).y),2));
                    extra_edge.push_back(edge_temp);
                }
            }
        }
    }

    extra_edge.shrink_to_fit();
    sort(extra_edge.begin(), extra_edge.end(), compare_dist);

    
    count = 0;
    int scc_idx;
    vector<edge> edgeAddList;
    edgeAddList.reserve(extra_);
    bool add_ = false;
    int from_tmp, to_tmp;
    float dist_tmp;
    while (count < extra_) {
        add_ = false;
        for (int j = 0; j < extra_edge.size(); j++) {
            from_tmp = (extra_edge.at(j)).from;
            to_tmp = (extra_edge.at(j)).to;
            if (visited_scc[SCC_num[from_tmp]-1] == true && visited_scc[SCC_num[to_tmp]-1] == false) {   
                dist_tmp = sqrt(pow(((vertexList.at(from_tmp)).x - (vertexList.at(to_tmp)).x),2) +\
                                pow(((vertexList.at(from_tmp)).y - (vertexList.at(to_tmp)).y),2));
                edge_temp.from = from_tmp;
                edge_temp.to = to_tmp;
                edge_temp.distance = dist_tmp;
                edgeAddList.push_back(edge_temp);

                visited_scc[SCC_num[to_tmp]-1] = true;
                add_ = true;
                ++count;
            }
            if (add_ == true) {
                break;
            }
        }
    }
    
    
    output << extra_ << endl;
    sort(edgeAddList.begin(), edgeAddList.end(), compare_dist);
    for (int i = 0; i < edgeAddList.size(); i++) {
        //cout << (edgeAddList.at(i)).from << " " << (edgeAddList.at(i)).to << " " << (edgeAddList.at(i)).distance << endl; 
        output << (edgeAddList.at(i)).from << " " << (edgeAddList.at(i)).to << endl;
    }

    /* Calculate the total distance added */
    /*
    float temp = 0;
    for (int i = 0; i < edgeAddList.size(); i++) {
        temp = temp + (edgeAddList.at(i)).distance;
    }
    cout << "Add distance total: " << temp << endl;
    */
}


int main(int argc, char *argv[]) {
    fstream input(argv[1]);
    ofstream output(argv[2]);
    string line;
    int from_, to_;
    int input_x, input_y;
    int vertex_number;
    int counter_ = 0;
    
    getline(input, line);
    istringstream in(line);
    in >> vertex_number;
    Graph g(vertex_number);

    while (getline(input, line)) {
        istringstream in(line);
        if (counter_ < vertex_number) {
            in >> input_x >> input_y;
            g.addVertex(input_x, input_y);
            ++counter_;
        } else if (counter_ >= vertex_number) {
            in >> from_ >> to_;
            g.addEdge(from_, to_);
        }    
    }
    g.PrintSCCs(0, output);
    input.close();
    output.close();
    return 0;
}