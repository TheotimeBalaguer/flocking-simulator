//MIT License
//Copyright (c) 2018 Eotvos Lorand University, Budapest

/* vim:set ts=4 sw=4 sts=4 et: */

/* Tools for handling the robot model
 */

#ifndef ROBOTMODEL_H
#define ROBOTMODEL_H

#include "utilities/param_utils.h"
#include "utilities/math_utils.h"
#include "utilities/datastructs.h"
#include "utilities/dynamics_utils.h"
#include "algo_spp_evol.h"
#include "vizmode.h"
#include "sensors.h"
#include "algo.h"

#ifndef SERVER_MODE
#include "algo_gui.h"
#endif

#include "vizmode.h"
#include "utilities/debug_utils.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

// LAPACK function declaration for dgeev
extern void dgeev_(char* jobvl, char* jobvr, int* n, double* a, int* lda,
                   double* wr, double* wi, double* vl, int* ldvl, double* vr,
                   int* ldvr, double* work, int* lwork, int* info);

void StepTarget(double * TargetPosition,
        sit_parameters_t * SitParams, vizmode_params_t * VizParams,
        flocking_model_params_t * FlockingParams, int TimeStepReal);

/* Step positions and velocities
 */
void Step(phase_t * OutputPhase, phase_t * GPSPhase, phase_t * GPSDelayedPhase,
        phase_t * PhaseData, unit_model_params_t * UnitParams, int WhichTarget,
        flocking_model_params_t * FlockingParams, sit_parameters_t * SitParams,
        vizmode_params_t * VizParams, int TimeStepLooped, int TimeStepReal,
        bool CountCollisions, bool * ConditionsReset, int *Collisions,
        bool * AgentsInDanger, int *CollisionsObst, bool* AgentInObst,
        double *WindVelocityVector, double *Accelerations,
        double ** TargetsArray, double **Polygons, node **Hull, int Verbose);

/*  Initialization and killing
 */
void InitializePreferredVelocities(phase_t * Phase,
        flocking_model_params_t * FlockingParams, sit_parameters_t * SitParams,
        unit_model_params_t * UnitParams, double *WindVelocityVector);
void freePreferredVelocities(phase_t * Phase,
        flocking_model_params_t * FlockingParams, sit_parameters_t * SitParams);

void print_matrix(const char* desc, int m, int n, double* a, int lda);

#endif
