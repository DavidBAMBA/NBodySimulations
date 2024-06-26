/*-----------------------------------------------------------------------------
The N-Body problem using MPI
-----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structures.h"
#include "Tree.h"

const double THETA  = 0.75;
const double G      = 1.0;


void read_data(const char *File_address, double *Pos, double *Vel, double *Mass) {
    /*---------------------------------------------------------------------------
    Reads data from bodies: position, velocity and mass.
    -----------------------------------------------------------------------------
    Arguments:
      File_address:   File address from where the data is read.
      Pos         :   Position (1D vector) [xi, yi, zi].
      Vel         :   Velocity (1D vector) [vxi, vyi, vzi].
      Mass        :   Mass (1D vector) [mi].
    ---------------------------------------------------------------------------*/

    FILE *File;
    File = fopen(File_address, "r");
    char line[256];
    int row = 0;
    int ii;
    while (fgets(line, sizeof(line), File)) {
        if (line[0] == '\n' || line[0] == '#')
            continue;
        else {
            char *token;
            token = strtok(line, "\t");
            for (ii = 0; ii < 3; ii++) {
                Pos[ii + 3 * row] = atof(token);
                token = strtok(NULL, "\t");
            }
            for (ii = 0; ii < 3; ii++) {
                Vel[ii + 3 * row] = atof(token);
                token = strtok(NULL, "\t");
            }
            Mass[row] = atof(token);
            row += 1;
        }
    }
    fclose(File);
}

int separated(const double * bdr, const Node* node) {
    double xrel = node->CoM[0]-bdr[0];
    double yrel = node->CoM[1]-bdr[1];
    double zrel = node->CoM[2]-bdr[2];
    double d = sqrt(xrel*xrel + yrel*yrel + zrel*zrel);

    double side[3] = {node->max[0]-node->min[0], node->max[1]-node->min[1], node->max[2]-node->min[2]};

    double l =  side[0];
    if (side[1]>l) l = side[1];
    if (side[2]>l) l = side[2];

    if (l/d<THETA) {
        return 1;
    } else {
        return 0;
    }
}

void force(double * bda, const double * bdr, const double * bdm, const Node* node) {
    double xrel = node->CoM[0]-bdr[0];
    double yrel = node->CoM[1]-bdr[1];
    double zrel = node->CoM[2]-bdr[2];
    double F = - G * *node->Mass / pow(xrel*xrel + yrel*yrel + zrel*zrel, 1.5);
    bda[0] += F*xrel;
    bda[1] += F*yrel;
    bda[2] += F*zrel;
}


// !!! Calculate the path in which the tree is walked. 
// void treeforce(Node* tree, double* bda, const double * bdr, const double * bdm) {
//     Node* node = tree;
//     while (node != NULL) {
//         if (separated(bdr, node)){
//             force(bda, bdr, bdm, node);
//         } else{
//             node = nextnode(node, pstnode);
//         }
//     }
// }

void printtree(Node* node) {
    if (node->type) {
        printf("%d \t %f \t %f \t %f \t %f \t %f \t %f \n", *node->deep, node->min[0], node->min[1], node->min[2],
                                     node->max[0],  node->max[1], node->max[2]);
    } else {
        printtree(node->child);
        printtree(node->sibling);

    }
}


int main(int argc, char** argv) {
    
    int N = 100;     // Total number of bodies
    body bd;                        // Bodies
    
    bd.r = (double *) malloc(3*N*sizeof(double));        // [x, y, z]
    bd.rtemp = (double *) malloc(3*N*sizeof(double));    // [x, y, z]
    bd.v = (double *) malloc(3*N*sizeof(double));        // [vx, vy, vz]
    bd.vtemp = (double *) malloc(3*N*sizeof(double));    // [vx, vy, vz]
    bd.a = (double *) malloc(3*N*sizeof(double));        // [ax, ay, az]
    bd.m = (double *) malloc(N*sizeof(double));          // [m]

    int ii;
    
    for (ii = 0; ii < 3*N; ii++) {
        bd.rtemp[ii] = 0.0;
        bd.vtemp[ii] = 0.0;
        bd.a[ii] = 0.0;
    }

    // Read local particles information
    char input[32] = "data0.txt";
    read_data(input, bd.r, bd.v, bd.m);

    double * rootmin = (double *) malloc(3*sizeof(double));        // [x, y, z]
    double * rootmax = (double *) malloc(3*sizeof(double));        // [x, y, z]

    for (ii = 0; ii<3; ii++) {
        rootmin[ii] = -20;
        rootmax[ii] = 20;
    }
    Node* Tree = BuiltTree(bd.r, bd.m, N, rootmin, rootmax);
    printtree(Tree);

    // Save positions
    free (bd.r);
    free (bd.rtemp);
    free (bd.v);
    free (bd.vtemp);
    free (bd.a);
    free (bd.m);

    return 0;
}