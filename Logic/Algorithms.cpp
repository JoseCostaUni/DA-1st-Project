#include <climits>
#include "Algorithms.h"
#include "stdafx.h"

/**
 * @brief Finds the minimum residual capacity along an augmenting path from a source vertex to a sink vertex.
 * @param source The source vertex.
 * @param sink The sink vertex.
 * @return The minimum residual capacity along the augmenting path.
 * @details Time Complexity: O(N), where N is the number of vertices along the path.
 */
double findMinResidualAlongPath(Vertex<DeliverySite> *source, Vertex<DeliverySite> *sink) {
    double f = DBL_MAX; // Traverse the augmenting path to find the minimum residual capacity

    for (Vertex<DeliverySite>* v = sink; v != source;) {
        Edge<DeliverySite>* e = v->getPath();
        if (e->getDest() == v){
            f = std::min(f, e->getWeight() - e->getFlow());
            v = e->getOrig();
        } else {
            f = std::min(f, e->getFlow());
            v = e->getDest();
        }
    }
    return f; // Return the minimum residual capacity
}

/**
 * @brief Finds an augmenting path in the graph using BFS.
 * @param g Pointer to the graph.
 * @param source Pointer to the source vertex.
 * @param sink Pointer to the sink vertex.
 * @param removed Pointer to the removed vertex (optional).
 * @return True if an augmenting path is found, otherwise false.
 * @details Time Complexity: O(V + E), where:
 * - V is the number of vertices in the graph.
 * - E is the number of edges in the graph.
 */
bool findAugmentingPath(Graph<DeliverySite> *g, Vertex<DeliverySite> *source, Vertex<DeliverySite> *sink, Vertex<DeliverySite> *removed) {
// Mark all vertices as not visited
    for(Vertex<DeliverySite>* v : g->getVertexSet()) {
        v->setVisited(false);
    }
// Mark the source vertex as visited and enqueue it

    if (removed != nullptr) removed->setVisited(true);

    std::queue<Vertex<DeliverySite> *> q;
    q.push(source);
    source->setVisited(true);

// BFS to find an augmenting path
    while(!q.empty() && !sink->isVisited()) {
        Vertex<DeliverySite>* v = q.front();
        q.pop();
        for (Edge<DeliverySite>* e : v->getAdj()) {
            Vertex<DeliverySite>* dest = e->getDest();
            if (!dest->isVisited() && (e->getWeight() - e->getFlow() > 0)) {
                dest->setVisited(true);
                dest->setPath(e);
                q.push(dest);
            }
        }
        for (Edge<DeliverySite>* e: v->getIncoming()) {
            Vertex<DeliverySite>* origin = e->getOrig();
            if (!origin->isVisited() && (e->getFlow() > 0)) {
                origin->setVisited(true);
                origin->setPath(e);
                q.push(origin);
            }
        }
    }
    return sink->isVisited();
}

/**
 * @brief Augments the flow along the augmenting path found using BFS.
 * @param source Pointer to the source vertex.
 * @param sink Pointer to the sink vertex.
 * @param f The flow to augment along the path.
 * @details Time Complexity: O(N), where N is the number of vertices along the path.
 */
void augmentFlowAlongPath(Vertex<DeliverySite> *source, Vertex<DeliverySite> *sink, double f) {
// Traverse the augmenting path and update the flow values accordingly

    for (Vertex<DeliverySite>* v = sink; v != source;) {
        Edge<DeliverySite>* e = v->getPath();

        double flow = e->getFlow();
        if (e->getDest() == v) {
            e->setFlow(flow + f);
            v = e->getOrig();
        }
        else {
            e->setFlow(flow - f);
            v = e->getDest();
        }
    }


}

/**
 * @brief Implements the Edmonds-Karp algorithm to find the maximum flow in the graph.
 * @param g Pointer to the graph.
 * @param source The source delivery site.
 * @param target The target delivery site.
 * @param removed The removed delivery site.
 * @return The maximum flow in the graph.
 * @details Time Complexity: O((V^2) * E), where:
 * - V is the number of vertices in the graph.
 * - E is the number of edges in the graph.
 */
double edmondsKarp(Graph<DeliverySite> *g, const DeliverySite& source, const DeliverySite& target,const DeliverySite& removed) {
    double maxFlow = 0;
// Find source and target vertices in the graph
    Vertex<DeliverySite>* s = g->findVertex(source);
    Vertex<DeliverySite>* t = g->findVertex(target);
    Vertex<DeliverySite>* remove = g->findVertex(removed);
// Validate source and target vertices
    if (s == nullptr || t == nullptr || s == t)
        throw std::logic_error("Invalid source and/or target vertex");
// Initialize flow on all edges to 0
    for (auto v : g->getVertexSet()) {
        for (auto e: v->getAdj()) {
            e->setFlow(0);
            e->setSelected(false);
        }
    }

// While there is an augmenting path, augment the flow along the path
    while(findAugmentingPath(g, s, t, remove) ) {
        double f = findMinResidualAlongPath(s, t);
        maxFlow += f;
        augmentFlowAlongPath(s, t, f);
    }
    return maxFlow;
}

//root will be the origin of edge with the lowest flow
//this edge will be locked to guarantee that it is not picked during this algorithm
//we can try to tell this algo to
//djikstra picks the paths with full edges
/**
 * @brief Performs Dijkstra's algorithm to find the shortest paths in terms of flow from a root vertex to a target vertex.
 * @param g Pointer to the graph.
 * @param root Pointer to the root vertex.
 * @param target Pointer to the target vertex.
 * @details Time Complexity: O((V + E) * log(V)), where:
 * - V is the number of vertices in the graph.
 * - E is the number of edges in the graph.
 */
void Dijkstra(Graph<DeliverySite>*g , Vertex<DeliverySite>* root , Vertex<DeliverySite>* target) {

    MutablePriorityQueue<Vertex<DeliverySite>> vertexQueue;
    for(Vertex<DeliverySite>* v : g->getVertexSet()){
        vertexQueue.insert(v);
    }

    for (Vertex<DeliverySite> *v: g->getVertexSet()) {
        v->setDist(INF);
        v->setPath(nullptr);
    }

    root->setDist(0);

    while (!vertexQueue.empty()){
        Vertex<DeliverySite>* u = vertexQueue.extractMin();

        for(Edge<DeliverySite>* e : u->getAdj()){
            //we want the minimum distance by flow
            Vertex<DeliverySite>* v = e->getDest();
            if(v->getDist() > (u->getDist() + e->getFlow())){
                v->setDist(u->getDist() + e->getFlow());
                v->setPath(e);
            }
        }
    }

    target->setDist(INF);
}

/**
 * @brief Calculates the minimum left-over capacity along a given path.
 * @param path The path for which to calculate the minimum left-over capacity.
 * @return The minimum left-over capacity.
 * @details Time Complexity: O(N), where N is the number of edges in the path.
 */
double minLeftOverCap(std::vector<Edge<DeliverySite>*>& path){
    auto a = 0;
    for(auto e : path){
        if(e->getFlow() != e->getWeight())
            a++;
    }

    auto min = DBL_MAX;
    for(Edge<DeliverySite>* edge : path){
        if(edge->getWeight() == edge->getFlow())
            return 0;
        if(edge->getWeight() - edge->getFlow() < min){
            min = edge->getWeight() - edge->getFlow();
        }
    }

    return min;
}

/**
 * @brief Computes metrics heuristic for optimizing water delivery.
 * @param g Pointer to the graph of delivery sites.
 * @return Metrics after optimization.
 * @details Time Complexity: O(((V+E) * E +E) * P), where:
 * - N is the number of delivery sites in the graph.
 * - E is the number of edges in the graph.
 * - P is the number of times that the while cycle is repeated
 */
Metrics heuristic(Graph<DeliverySite>*g){
    std::vector<Edge<DeliverySite>*> edges;

    edges = g->getEdges(); //O(V+E)

    Metrics finalMetrics = g->calculateMetrics();
    Metrics initialMetrics = finalMetrics;

    g->printMetrics(initialMetrics);
    initialMetrics = {DBL_MAX , DBL_MAX , DBL_MAX , DBL_MAX};
    while(finalMetrics.variance < initialMetrics.variance || finalMetrics.avg < initialMetrics.avg){

        std::sort(edges.begin(), edges.end(), [](Edge<DeliverySite>* a, Edge<DeliverySite>* b) {

            if(a->getWeight() - a->getFlow() == b->getWeight() - b->getFlow()){
                return a->getWeight() > b->getWeight();
            }

            return a->getWeight() - a->getFlow() < b->getWeight() - b->getFlow();
        });//O(E log E)

        //O(E)
        for(Edge<DeliverySite>* e : edges){
            std::vector<Edge<DeliverySite>*> path;
            std::vector<std::vector<Edge<DeliverySite>*>> allPaths;

            //O(V+E)
            allPaths = g->allPaths(e->getOrig()->getInfo() , e->getDest()->getInfo());

            double maxDiff = -1;
            if(allPaths.empty())
                continue;

            for(std::vector<Edge<DeliverySite>*> tempPath : allPaths){
                double minFlow = minLeftOverCap(tempPath);
                if (minFlow > maxDiff) {
                    maxDiff = minFlow;
                    path = tempPath;
                }
            }

            double waterToPump = maxDiff;
            if(e->getFlow() - waterToPump < 0)
                waterToPump = e->getFlow();

            e->setFlow(e->getFlow() - waterToPump);

            pumpWater(path , waterToPump);
        }

        initialMetrics = finalMetrics;

        finalMetrics = g->calculateMetrics();

    }

    finalMetrics = g->calculateMetrics();

    g->printMetrics(finalMetrics);

    for(auto e : g->getEdges()){
        if(e->getFlow() > e->getWeight()){
            print("SOBRECARGAAAA" , false);
        }
        if(e->getFlow() < 0)
            print("DESCEU O CANOOOOO" , false);
    }
    return  finalMetrics;
}

/**
 * @brief Pumps water along the given path with the specified flow rate.
 * @param path The path along which water is to be pumped.
 * @param flowToPump The flow rate of water to pump.
 * @details Time Complexity: O(N), where N is the number of edges in the path.
 */
void pumpWater(std::vector<Edge<DeliverySite>*>& path , double flowToPump){
    if(flowToPump != 0)
        auto a = 0;
    for(Edge<DeliverySite>* e : path){
            e->setFlow(e->getFlow() + flowToPump);
    }
}

