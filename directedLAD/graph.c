// This software has been written by Chrstine Solnon.
// It is distributed under the CeCILL-B FREE SOFTWARE LICENSE
// see http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html for more details

typedef struct{
	bool isDirected; // false iff for each edge (i,j), there exists an edge (j,i)
	bool isLabelled; // true if labels are associetd with vertices and edges
    bool* isLoop; // isLoop[i] = true if there is a loop on vertex i
	int nbVertices; // Number of vertices
	int* vertexLabel; // if isLabelled then vertexLabel[i] = label associated with vertex i
	int* nbAdj;    // nbAdj[i] = number of vertices j such that (i,j) or (j,i) is an edge
	int* nbPred;   // nbPred[i] = number of vertices j such that (j,i) is an edge and (i,j) is not an edge
	int* nbSucc;   // nbSucc[i] = number of vertices j such that (i,j) is an edge and (j,i) is not an edge
	int** adj;     // forall j in [0..nbAdj[i]-1], adj[i][j] = jth vertex adjacent to i 
	char** edgeDirection;	// if both (i,j) and (j,i) are edges then edgeDirection[i][j] = 3
							// else if (i,j) is an edge then edgeDirection[i][j] = 1
							// else if (j,i) is an edge then edgeDirection[i][j] = 2
							// else (neither (i,j) nor (j,i) is an edge) edgeDirection[i][j] = 0
	int** edgeLabel; // if isLabelled then edgeLabel[i][j] = label associated with edge (i,j)
} Tgraph;

Tgraph* createGraph(char* fileName, bool isLabelled){
	// reads data in fileName and create the corresponding graph
	
	FILE* fd;
	int i, j, k;
	Tgraph* graph = (Tgraph*)malloc(sizeof(Tgraph));
	
	if ((fd=fopen(fileName, "r"))==NULL){
		printf("ERROR: Cannot open ascii input file %s\n", fileName); 
		exit(1);
	}
	if (fscanf(fd,"%d",&(graph->nbVertices)) != 1){
		printf("ERROR while reading input file %s\n", fileName); 
		exit(1);
	}
	graph->isLabelled = isLabelled;
	if (isLabelled){
		graph->vertexLabel = (int*)calloc(graph->nbVertices,sizeof(int));
		memset(graph->vertexLabel,0,graph->nbVertices*sizeof(int));
		graph->edgeLabel = (int**)calloc(graph->nbVertices,sizeof(int*));
	}
    graph->isLoop = (bool*)calloc(graph->nbVertices,sizeof(bool));
    graph->nbAdj = (int*)calloc(graph->nbVertices,sizeof(int));
	graph->nbPred = (int*)calloc(graph->nbVertices,sizeof(int));
	graph->nbSucc = (int*)calloc(graph->nbVertices,sizeof(int));
	graph->edgeDirection = (char**)malloc(graph->nbVertices*sizeof(char*));
	graph->adj = (int**)malloc(graph->nbVertices*sizeof(int*));
	for (i=0; i<graph->nbVertices; i++){
        graph->isLoop[i] = false;
		graph->adj[i] = (int*)malloc(graph->nbVertices*sizeof(int));
		graph->edgeDirection[i] = (char*)malloc(graph->nbVertices*sizeof(char));
		memset(graph->edgeDirection[i],0,graph->nbVertices*sizeof(char));
		memset(graph->nbAdj,0,graph->nbVertices*sizeof(int));
		memset(graph->nbPred,0,graph->nbVertices*sizeof(int));
		if (isLabelled){
			graph->edgeLabel[i] = (int*)malloc(graph->nbVertices*sizeof(int));
			memset(graph->edgeLabel[i],0,graph->nbVertices*sizeof(int));
		}
	}
	for (i=0; i<graph->nbVertices; i++){
		if (isLabelled){// read the label of vertex i
			if ((fscanf(fd,"%d",&(graph->vertexLabel[i])) != 1) || (feof(fd))) {
				printf("ERROR while reading input file %s: Vertex %d should have an integer valued label\n", 
					   fileName, i); 
				exit(1);
			}
		}
		// read degree of vertex i
		if ((fscanf(fd,"%d",&(graph->nbSucc[i])) != 1) || (graph->nbSucc[i] < 0) || (graph->nbSucc[i]>=graph->nbVertices) || (feof(fd))) {
			printf("ERROR while reading input file %s: Vertex %d has an illegal number of successors (%d should be between 0 and %d)\n", 
				   fileName, i, graph->nbSucc[i], graph->nbVertices); 
			exit(1);
		}
        for (j=graph->nbSucc[i]; j>0; j--){
			// read jth successor of i
			if ((fscanf(fd,"%d",&k) != 1) || (k<0) || (k>=graph->nbVertices) || (feof(fd))){
				printf("ERROR while reading input file %s: Successor %d of vertex %d has an illegal value %d (should be between 0 and %d)\n", 
					   fileName, j, i, k, graph->nbVertices); 
				exit(1);
			}
			if (isLabelled){// read the label of edge (i,k)
				if ((fscanf(fd,"%d",&(graph->edgeLabel[i][k])) != 1) || (feof(fd))) {
					printf("ERROR while reading input file %s: Edge (%d,%d) should have an integer valued label\n", 
						   fileName, i, k); 
					exit(1);
				}
			}
            if (i == k){ // The edge is a loop
                graph->isLoop[i] = true;
            }
            else{
                if (graph->edgeDirection[i][k] == 1){
                    printf("ERROR while reading input file %s (the successors of node %d should be all different)\n",fileName, i);
                    exit(1);
                }
                if (graph->edgeDirection[i][k] == 2){
                    // i is a successor of k and k is a successor of i
                    graph->edgeDirection[k][i] = 3;
                    graph->edgeDirection[i][k] = 3;
                    graph->nbPred[i]--;
                    graph->nbSucc[i]--;
                    graph->nbSucc[k]--;
                }
                else{
                    graph->nbPred[k]++;
                    graph->adj[i][graph->nbAdj[i]++] = k;
                    graph->adj[k][graph->nbAdj[k]++] = i;
                    graph->edgeDirection[i][k] = 1;
                    graph->edgeDirection[k][i] = 2;
                }
            }
		}
	}
	fclose(fd);
	
	graph->isDirected = false;
	for (i=0; i<graph->nbVertices; i++){
		for (j=0; j<graph->nbAdj[i]; j++){
			graph->isDirected = graph->isDirected || (graph->edgeDirection[i][graph->adj[i][j]]!=3);
		}
	}
	return graph;
}

void printGraph(Tgraph* graph){
	int i, j, k;
	if (graph->isDirected)
		printf("Directed ");
	else
		printf("Non directed ");
	if (graph->isLabelled)
		printf("labelled graph with %d vertices\n",graph->nbVertices);
	else
		printf("graph with %d vertices\n",graph->nbVertices);
	for (i=0; i<graph->nbVertices; i++){
		if (graph->isLabelled)
			printf("Vertex %d has label %d and %d adjacent vertices (%d pred and %d succ): ",
				   i,graph->vertexLabel[i],graph->nbAdj[i],graph->nbPred[i],graph->nbSucc[i]);
		else
			printf("Vertex %d has %d adjacent vertices (%d pred and %d succ): ",i,graph->nbAdj[i],graph->nbPred[i],graph->nbSucc[i]);
		for (j=0; j<graph->nbAdj[i]; j++){
			k = graph->adj[i][j];
			if (graph->isLabelled){
				if (graph->edgeDirection[i][k] == 1)
					printf(" %d(succ; label %d)",k, graph->edgeLabel[i][k]);
				else if (graph->edgeDirection[i][k] == 2)
					printf(" %d(pred; label %d)",k, graph->edgeLabel[i][k]);
				else if (graph->edgeDirection[i][k] == 3)
					printf(" %d(succ and pred; label %d)",k, graph->edgeLabel[i][k]);
				else 
					printf("error !");
			}
			else {
				if (graph->edgeDirection[i][k] == 1)
					printf(" %d(succ)",k);
				else if (graph->edgeDirection[i][k] == 2)
					printf(" %d(pred)",k);
				else if (graph->edgeDirection[i][k] == 3)
					printf(" %d(succ and pred)",k);
				else 
					printf("error !");
			}

		}
		printf("\n");
	}
}
