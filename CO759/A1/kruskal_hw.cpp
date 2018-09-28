/****************************************************************************/
/*                                                                          */
/*              CO759: Model for HW1, Kruskal MST Algorithm                 */
/*              Date:  January 14, 2015                                     */
/*                                                                          */
/****************************************************************************/

#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

#include <gvc.h>

#include "util.h"


using namespace std;

//Filename, vert count, edge count, edge list  [2*i] = edge i end 0 and  [2*i + 1]=edge i end 1, edge length list
static int getprob (char *fname, int *p_ncount, int *p_ecount, int **p_elist, int **p_elen);

//tlist = spanning tree edge list output
static int kruskal_spanningtree (int ncount, int ecount, int *elist, int *elen, int *tlist);

class Graph {
    public:
        Graph (const char *filename);

        int node_count;
        vector< pair<int,int> > edges;
        vector<int> edge_lengths;

        void print_edges();
        void save_graph_image(const char* filename);
};

//Load a graph from a file
Graph::Graph(const char *filename) {
    int edge_count;

    fstream fin;
    fin.open(filename);

    fin >> node_count >> edge_count;

    edges.resize(edge_count);
    edge_lengths.resize(edge_count);

    for(int i = 0; i < edge_count; i++) {
        fin >> edges[i].first >> edges[i].second >> edge_lengths[i];
    }
}

void Graph::print_edges() {
    for (int i = 0; i < edges.size(); ++i)
    {
        pair<int,int> edge = edges[i];
        cout << edge.first << ", " << edge.second << endl;
    }
}

void Graph::save_graph_image(const char* filename) {
    GVC_t *gvc;
    graph_t *g;
    FILE *fp;

    gvc = gvContext();

    if (argc > 1)
    fp = fopen(argv[1], "r");
    else
    fp = stdin;
    g = agread(fp, 0);

    gvLayout(gvc, g, "dot");

    gvRender(gvc, g, "plain", stdout);

    gvFreeLayout(gvc, g);

    agclose(g);

    return (gvFreeContext(gvc));
}

int main (int argc, char **argv)
{
    char *problem_filename;
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " edge_file" << endl;
        return 1;
    } else {
        problem_filename = argv[1]; 
    }


    Graph base_graph(problem_filename);
    base_graph.print_edges();   
    
    return 0;
    // int rval  = 0, i, ncount, ecount;
    // int *elist = (int *) NULL, *elen = (int *) NULL, *tlist = (int *) NULL;
    // double szeit, wt;

    

    // rval = getprob (av[1], &ncount, &ecount, &elist, &elen);
    // if (rval) {
    //     fprintf (stderr, "getprob failed\n");
    //     goto CLEANUP;
    // }

    // tlist = (int *) malloc ((ncount-1)*sizeof (int));
    // if (!tlist) {
    //     fprintf (stderr, "out of memory for tlist\n");
    //     rval = 1;  goto CLEANUP; 
    // }

    // szeit = CO759_zeit ();
    // rval = kruskal_spanningtree (ncount, ecount, elist, elen, tlist);
    // if (rval) {
    //     fprintf (stderr, "kruskal_spanningtree failed\n");
    //     goto CLEANUP;
    // }
    // printf ("Running Time: %.2f seconds\n", CO759_zeit() - szeit);
    // fflush (stdout);

    // wt = 0.0;
    // for (i = 0; i < ncount - 1; i++) wt += elen[tlist[i]];

    // printf ("Tree Length: %.0f\n", wt); fflush (stdout);

}

// static int kruskal_spanningtree (int ncount, int ecount, int *elist,
//         int *elen, int *tlist)
// {
//     int i;

//     /* Place the indices of the spanning tree edges into the array    */
//     /* tlist.  The array should receive ncount-1 numbers, each number */
//     /* being between 0 and ecount-1.                                  */

//     /* The function should return the value 0 and a non-zero value if */
//     /* it fails.                                                      */

//     /* I'll put dummy values into the array.                          */

//     for (i = 0; i < ncount-1; i++) tlist[i] = i;

//     return 0;
// }


// static int getprob (char *fname, int *p_ncount, int *p_ecount, int **p_elist,
//     int **p_elen)
// {
//     FILE *f = (FILE *) NULL;
//     int i, end1, end2, w, rval = 0, ncount, ecount;
//     int *elist = (int *) NULL, *elen = (int *) NULL;

//     if ((f = fopen (fname, "r")) == NULL) {
// 	fprintf (stderr, "Unable to open %s for input\n",fname);
//         rval = 1;  goto CLEANUP;
//     }

//     if (fscanf (f, "%d %d", &ncount, &ecount) != 2) {
// 	fprintf (stderr, "Input file %s has invalid format\n",fname);
//         rval = 1;  goto CLEANUP;
//     }

//     printf ("Nodes: %d  Edges: %d\n", ncount, ecount);
//     fflush (stdout);

//     elist = (int *) malloc (2 * ecount * sizeof (int));
//     if (!elist) {
//         fprintf (stderr, "out of memory for elist\n");
//         rval = 1;  goto CLEANUP;
//     }

//     elen = (int *) malloc (ecount * sizeof (int));
//     if (!elen) {
//         fprintf (stderr, "out of memory for elen\n");
//         rval = 1;  goto CLEANUP;
//     }

//     for (i = 0; i < ecount; i++) {
// 	if (fscanf(f,"%d %d %d",&end1, &end2, &w) != 3) {
// 	    fprintf (stderr, "%s has invalid input format\n",fname);
//             rval = 1;  goto CLEANUP;
// 	}
// 	elist[2*i] = end1;
// 	elist[2*i+1] = end2;
// 	elen[i] = w;
//     }

//     *p_ncount = ncount;
//     *p_ecount = ecount;
//     *p_elist = elist;
//     *p_elen = elen;

// CLEANUP:
//     if (f) fclose (f);
//     return rval;
// }

