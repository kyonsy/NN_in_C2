#ifndef TRAIN_H
#define TRAIN_H

#include "nn.h"
#include "dataset.h"

double mse_loss(double *y,double *t,double *dy,int m);
void train(NN *net,const Dataset *ds,int epochs,double lr);

#endif
