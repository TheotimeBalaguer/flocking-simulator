// MIT License
// Copyright (c) 2018 Eotvos Lorand University, Budapest

/* vim:set ts=4 sw=4 sts=4 et: */

/*
 * This file contains unversal interaction terms.
 */

#include <limits.h>
#include "interactions.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void FrictionLinSqrt(double *OutputVelocity, phase_t *Phase,
                     const double C_Frict_l, const double V_Frict_l,
                     const double Acc_l, const double p_l, const double R_0_l,
                     const int WhichAgent, const int Dim_l)
{

    NullVect(OutputVelocity, 3);

    int i;
    int n = 0;

    double *AgentsCoordinates;
    double *AgentsVelocity;
    double *NeighboursCoordinates;
    double *NeighboursVelocity;

    AgentsCoordinates = Phase->Coordinates[WhichAgent];
    AgentsVelocity = Phase->Velocities[WhichAgent];

    static double DifferenceVector[3];
    static double DistanceFromNeighbour;
    static double VelDiff;
    static double MaxVelDiff;

    /* Friction-like term */
    for (i = 0; i < Phase->NumberOfAgents; i++)
    {

        if (i == WhichAgent)
            continue;
        /* Get distance from neighbor */
        NeighboursCoordinates = Phase->Coordinates[i];
        NullVect(DifferenceVector, 3);
        VectDifference(DifferenceVector, NeighboursCoordinates,
                       AgentsCoordinates);
        DistanceFromNeighbour = VectAbs(DifferenceVector);
        /* Get velocity difference from neighbor */
        NeighboursVelocity = Phase->Velocities[i];
        VectDifference(DifferenceVector, NeighboursVelocity, AgentsVelocity);
        if (2 == Dim_l)
        {
            DifferenceVector[2] = 0.0;
        }
        VelDiff = VectAbs(DifferenceVector);
        UnitVect(DifferenceVector, DifferenceVector);
        // calculate max allowed velocity difference at a given distance based
        // on an optimal linsqrt breaking curve and allow for V_Frict slack
        MaxVelDiff =
            MAX(V_Frict_l,
                VelDecayLinSqrt(DistanceFromNeighbour, p_l, Acc_l, VelDiff,
                                R_0_l));
        // if velocity difference is larger than allowed, we compensate it
        if (VelDiff > MaxVelDiff)
        {
            MultiplicateWithScalar(DifferenceVector, DifferenceVector,
                                   C_Frict_l * (VelDiff - MaxVelDiff), Dim_l);
            VectSum(OutputVelocity, OutputVelocity, DifferenceVector);
        }
    }
}

void RepulsionLin(double *OutputVelocity,
                  phase_t *Phase, const double V_Rep_l, const double p_l,
                  const double R_0_l, const int WhichAgent, const int Dim_l,
                  const bool normalize)
{

    NullVect(OutputVelocity, 3);

    int i;
    int n = 0;

    double *AgentsCoordinates;
    double *NeighboursCoordinates;

    AgentsCoordinates = Phase->Coordinates[WhichAgent];

    static double DifferenceVector[3];
    static double DistanceFromNeighbour;
    /* Repulsive interaction term */
    for (i = 0; i < Phase->NumberOfAgents; i++)
    {
        if (i == WhichAgent)
            continue;
        NeighboursCoordinates = Phase->Coordinates[i];
        VectDifference(DifferenceVector, AgentsCoordinates,
                       NeighboursCoordinates);
        if (2 == Dim_l)
        {
            DifferenceVector[2] = 0.0;
        }
        DistanceFromNeighbour = VectAbs(DifferenceVector);
        /* Check if we interact at all */
        if (DistanceFromNeighbour >= R_0_l)
            continue;
        n += 1;

        UnitVect(DifferenceVector, DifferenceVector);

        MultiplicateWithScalar(DifferenceVector, DifferenceVector,
                               SigmoidLin(DistanceFromNeighbour, p_l, V_Rep_l, R_0_l), Dim_l);
        VectSum(OutputVelocity, OutputVelocity, DifferenceVector);
    }

    /* divide result by number of interacting units */
    if (normalize && n > 1)
    {
        double length = VectAbs(OutputVelocity) / n;
        UnitVect(OutputVelocity, OutputVelocity);
        MultiplicateWithScalar(OutputVelocity, OutputVelocity, length, Dim_l);
    }
    // printf("Number of Repulsive neighbours: %d Norm of repulsive term relative to max repulsion velocity: %f\n", n, VectAbs (OutputVelocity)/V_Rep_l);
}

void AttractionLin(double *OutputVelocity,
                   phase_t *Phase, const double V_Rep_l, const double p_l,
                   const double R_0_l, const int WhichAgent, const int Dim_l,
                   const bool normalize, double **Jacard)
{

    NullVect(OutputVelocity, 3);

    int i;
    int n = 0;

    double *AgentsCoordinates;
    double *NeighboursCoordinates;
    AgentsCoordinates = Phase->Coordinates[WhichAgent];

    static double DifferenceVector[3];
    static double DistanceFromNeighbour;
    /* Attractive interaction term */
    // int vois = Phase->NumberOfAgents / 2;
    // for (i = 0; i < vois; i++) {
    for (i = 0; i < Phase->NumberOfAgents; i++)
    {
        if (i == WhichAgent || Jacard[Phase->RealIDs[WhichAgent]][Phase->RealIDs[i]] > 0)
        {
            continue;
        }

        NeighboursCoordinates = Phase->Coordinates[i];
        VectDifference(DifferenceVector, AgentsCoordinates,
                       NeighboursCoordinates);
        if (2 == Dim_l)
        {
            DifferenceVector[2] = 0.0;
        }
        DistanceFromNeighbour = VectAbs(DifferenceVector);
        /* Check if we interact at all */
        if (DistanceFromNeighbour <= R_0_l)
            continue;
        n += 1;

        UnitVect(DifferenceVector, DifferenceVector);

        // printf("coef of agent %d with agent %d is %f\n", Phase->RealIDs[WhichAgent], Phase->RealIDs[i],2 * (1 + Jacard[Phase->RealIDs[WhichAgent]][Phase->RealIDs[i]]));

        MultiplicateWithScalar(DifferenceVector, DifferenceVector,
                               SigmoidLin(DistanceFromNeighbour * log(DistanceFromNeighbour), p_l, V_Rep_l, R_0_l), Dim_l);
        // MultiplicateWithScalar(DifferenceVector, DifferenceVector, BumpFunction(DistanceFromNeighbour / (3 * R_0_l), 0.3), Dim_l);
        MultiplicateWithScalar(DifferenceVector, DifferenceVector, 2 * (1 + Jacard[Phase->RealIDs[WhichAgent]][Phase->RealIDs[i]]), Dim_l);
        VectSum(OutputVelocity, OutputVelocity, DifferenceVector);
    }

    /* divide result by number of interacting units */
    if (normalize && n > 1)
    {
        double length = VectAbs(OutputVelocity) / n;
        UnitVect(OutputVelocity, OutputVelocity);
        MultiplicateWithScalar(OutputVelocity, OutputVelocity, length, Dim_l);
    }
    // printf("Number of Attractive neighbours: %d Norm of attractive term relative to max repulsion velocity: %f\n", n, VectAbs (OutputVelocity)/V_Rep_l);
}

void AttractionVAT(double *OutputVelocity,
                   phase_t *Phase, const double V_Rep_l, const double p_l,
                   const double R_0_l, const int WhichAgent, const int Dim_l,
                   const bool normalize, double **Jacard)
{

    NullVect(OutputVelocity, 3);

    int i;
    int n = 0;

    double *AgentsCoordinates;
    double *NeighboursCoordinates;
    AgentsCoordinates = Phase->Coordinates[WhichAgent];

    static double DifferenceVector[3];
    static double DistanceFromNeighbour;
    /* Attractive interaction term */
    // int vois = Phase->NumberOfAgents / 2;
    // for (i = 0; i < vois; i++) {
    for (i = 0; i < Phase->NumberOfAgents; i++)
    {
        if (i == WhichAgent)
        {
            continue;
        }

        NeighboursCoordinates = Phase->Coordinates[i];
        VectDifference(DifferenceVector, AgentsCoordinates,
                       NeighboursCoordinates);
        if (2 == Dim_l)
        {
            DifferenceVector[2] = 0.0;
        }
        DistanceFromNeighbour = VectAbs(DifferenceVector);
        /* Check if we interact at all */
        if (DistanceFromNeighbour <= R_0_l)
            continue;
        n += 1;

        UnitVect(DifferenceVector, DifferenceVector);

        // printf("coef of agent %d with agent %d is %f\n", Phase->RealIDs[WhichAgent], Phase->RealIDs[i],2 * (1 + Jacard[Phase->RealIDs[WhichAgent]][Phase->RealIDs[i]]));

        MultiplicateWithScalar(DifferenceVector, DifferenceVector,
                               SigmoidLin(DistanceFromNeighbour, p_l, V_Rep_l, R_0_l), Dim_l);
        // MultiplicateWithScalar(DifferenceVector, DifferenceVector, BumpFunction(DistanceFromNeighbour / (3 * R_0_l), 0.3), Dim_l);
        // MultiplicateWithScalar(DifferenceVector, DifferenceVector,2 * (1 + Jacard[Phase->RealIDs[WhichAgent]][Phase->RealIDs[i]]), Dim_l);
        VectSum(OutputVelocity, OutputVelocity, DifferenceVector);
    }

    /* divide result by number of interacting units */
    if (normalize && n > 1)
    {
        double length = VectAbs(OutputVelocity) / n;
        UnitVect(OutputVelocity, OutputVelocity);
        MultiplicateWithScalar(OutputVelocity, OutputVelocity, length, Dim_l);
    }
    // printf("Number of Attractive neighbours: %d Norm of attractive term relative to max repulsion velocity: %f\n", n, VectAbs (OutputVelocity)/V_Rep_l);
}

void AttractionAsym(double *OutputVelocity,
                    phase_t *Phase, const double V_Rep_l, const double p_l,
                    const double R_0_l, const int WhichAgent, const int NeighMax, const int Dim_l,
                    const bool normalize)
{

    NullVect(OutputVelocity, 3);

    int i;
    int n = 0;

    double *AgentsCoordinates;
    double *NeighboursCoordinates;
    // printf("nb agents = %d\n", Phase->NumberOfAgents);
    AgentsCoordinates = Phase->Coordinates[WhichAgent];

    static double DifferenceVector[3];
    static double DistanceFromNeighbour;
    /* Attractive interaction term */
    // int vois = Phase->NumberOfAgents / 2;
    // for (i = 0; i < vois; i++) {
    for (i = 0; i < Phase->NumberOfAgents; i++)
    {
        if (i == WhichAgent)
            continue;

        NeighboursCoordinates = Phase->Coordinates[i];
        VectDifference(DifferenceVector, AgentsCoordinates,
                       NeighboursCoordinates);
        if (2 == Dim_l)
        {
            DifferenceVector[2] = 0.0;
        }
        DistanceFromNeighbour = VectAbs(DifferenceVector);
        /* Check if we interact at all */
        if (DistanceFromNeighbour <= R_0_l)
            continue;
        n += 1;

        UnitVect(DifferenceVector, DifferenceVector);

        MultiplicateWithScalar(DifferenceVector, DifferenceVector,
                               SigmoidLin(DistanceFromNeighbour, p_l, V_Rep_l, R_0_l), Dim_l);
        MultiplicateWithScalar(DifferenceVector, DifferenceVector, NeighMax - Phase->NumberOfAgents + 1, (int)Dim_l);
        // MultiplicateWithScalar(DifferenceVector, DifferenceVector, BumpFunction(DistanceFromNeighbour / (3 * R_0_l), 0.3), Dim_l);
        VectSum(OutputVelocity, OutputVelocity, DifferenceVector);
    }

    /* divide result by number of interacting units */
    if (normalize && n > 1)
    {
        double length = VectAbs(OutputVelocity) / n;
        UnitVect(OutputVelocity, OutputVelocity);
        MultiplicateWithScalar(OutputVelocity, OutputVelocity, length, Dim_l);
    }
    // printf("Number of Attractive neighbours: %d Norm of attractive term relative to max repulsion velocity: %f\n", n, VectAbs (OutputVelocity)/V_Rep_l);
}

void PressureRepulsion(double *OutputVelocity,
                       phase_t *Phase, const double k, const int WhichAgent, const int Dim_l, const double R_0, const double v_max)
{

    NullVect(OutputVelocity, 3);

    int i;
    int n = 0;

    double *AgentsCoordinates;
    double *NeighboursCoordinates;
    AgentsCoordinates = Phase->Coordinates[WhichAgent];

    static double DifferenceVector[3];
    static double DistanceFromNeighbour;

    int NeighbourhoodRep;
    NeighbourhoodRep = (int)Phase->NumberOfAgents * Phase->InnerStates[0][0];
    // printf("New neigh of agent %d is %d because its pressure is %f and ration is %f\n", Phase->RealIDs[0], NeighbourhoodRep, Phase->Pressure[0], Phase->InnerStates[0][0]);

    // for (i = 0; i < NeighbourhoodRep; i++) {
    for (i = 0; i < Phase->NumberOfAgents; i++)
    {
        if (i == WhichAgent)
        {
            continue;
        }
        NeighboursCoordinates = Phase->Coordinates[i];
        VectDifference(DifferenceVector, AgentsCoordinates,
                       NeighboursCoordinates);
        if (2 == Dim_l)
        {
            DifferenceVector[2] = 0.0;
        }

        DistanceFromNeighbour = VectAbs(DifferenceVector);
        /* Check if we interact at all */
        if (DistanceFromNeighbour >= 2 * R_0)
            continue;
        n += 1;

        UnitVect(DifferenceVector, DifferenceVector);
        double speed = VectAbs(Phase->Velocities[WhichAgent]);
        double mult = k * Phase->Pressure[i] * speed;
        if (mult >= v_max)
        {
            mult = v_max;
        }
        MultiplicateWithScalar(DifferenceVector, DifferenceVector, mult, Dim_l);
        VectSum(OutputVelocity, OutputVelocity, DifferenceVector);
    }
}

/* Smooth pairwise potential without finite cut-off */
double ActionFunction(double z, double a, double b)
{

    double phi;
    double sigma; // directly sigma1(z +c)
    double c;
    c = fabs(a - b) / sqrt(4 * a * b);
    sigma = (z + c) / sqrt(1 + pow((z + c), 2));
    phi = .5 * ((a + b) * sigma + (a - b));

    return phi;
}

void GradientBased(double *OutputVelocity,
                   phase_t *Phase, const double epsilon, const double a,
                   const double b, const double h,
                   const double d, const double r,
                   const int WhichAgent, const int Dim_l)
{

    NullVect(OutputVelocity, 3);

    int i;

    double *AgentsCoordinates;
    double *NeighboursCoordinates;

    static double SigmaDistance;
    static double PhiAlpha;
    static double SigmaR;
    static double SigmaD;

    SigmaR = (1 / epsilon) * (sqrt(1 + epsilon * pow(r, 2)) - 1);
    SigmaD = (1 / epsilon) * (sqrt(1 + epsilon * pow(d, 2)) - 1);

    AgentsCoordinates = Phase->Coordinates[WhichAgent];
    // MultiplicateWithScalar(AgentsCoordinates, AgentsCoordinates, 0.01, Dim_l);

    static double DifferenceVector[3];
    double GradVector[3];

    for (i = 1; i < Phase->NumberOfAgents; i++)
    { // i = 0 is the WhichAgent

        NeighboursCoordinates = Phase->Coordinates[i];
        // MultiplicateWithScalar(NeighboursCoordinates, NeighboursCoordinates, 0.01, Dim_l);

        VectDifference(DifferenceVector, NeighboursCoordinates, AgentsCoordinates);
        if (2 == Dim_l)
        {
            DifferenceVector[2] = 0.0;
        }

        SigmaGrad(GradVector, DifferenceVector, epsilon, Dim_l);

        SigmaDistance = SigmaNorm(DifferenceVector, epsilon);

        PhiAlpha = BumpFunction(SigmaDistance / SigmaR, h) * ActionFunction(SigmaDistance - SigmaD, a, b);
        // PhiAlpha = BumpFunction(SigmaDistance / SigmaR, h) * ActionFunction(SigmaDistance - SigmaD, a, b) + exp(0.5 * cos((2 * M_PI / SigmaD) * SigmaDistance)) - 1;
        // printf("%d\t%f\n",Phase->RealIDs[i], PhiAlpha);
        MultiplicateWithScalar(DifferenceVector, GradVector, PhiAlpha, Dim_l);
        VectSum(OutputVelocity, OutputVelocity, DifferenceVector);
    }
    // MultiplicateWithScalar(OutputVelocity, OutputVelocity, 8, Dim_l); // x100 to have the speed in cm/s
}

void AlignmentOlfati(double *OutputVelocity,
                     phase_t *Phase, const double h,
                     const double r, const int WhichAgent,
                     const int Dim_l, const double epsilon)
{

    NullVect(OutputVelocity, 3);

    int i;

    double *AgentsCoordinates;
    double *NeighboursCoordinates;

    AgentsCoordinates = Phase->Coordinates[WhichAgent];

    double *AgentsVelocity;
    double *NeighboursVelocity;

    AgentsVelocity = Phase->Velocities[WhichAgent];

    static double SigmaDistance;
    static double SigmaR;
    static double aij;

    SigmaR = (1 / epsilon) * (sqrt(1 + epsilon * pow(r, 2)) - 1);

    static double DifferenceVector[3];
    static double DifferenceVelocities[3];

    for (i = 1; i < Phase->NumberOfAgents; i++)
    { // i = 0 is the WhichAgent

        NeighboursCoordinates = Phase->Coordinates[i];
        NeighboursVelocity = Phase->Velocities[i];

        VectDifference(DifferenceVector, NeighboursCoordinates, AgentsCoordinates);
        if (2 == Dim_l)
        {
            DifferenceVector[2] = 0.0;
        }

        VectDifference(DifferenceVelocities, NeighboursVelocity, AgentsVelocity);
        if (2 == Dim_l)
        {
            DifferenceVelocities[2] = 0.0;
        }

        SigmaDistance = SigmaNorm(DifferenceVector, epsilon);

        aij = BumpFunction(SigmaDistance / SigmaR, h);

        MultiplicateWithScalar(DifferenceVector, DifferenceVelocities, aij, Dim_l);

        VectSum(OutputVelocity, OutputVelocity, DifferenceVector);
    }
}

/* Olfati tracking */
void TrackingOlfati(double *OutputVelocity, double *TargetPosition,
                    double *TargetVelocity, phase_t *Phase, const int WhichAgent,
                    const int Dim_l)
{

    NullVect(OutputVelocity, 3);

    double *AgentsCoordinates;
    double *AgentsVelocity;

    double PositionComponent[3];
    double VelocityComponent[3];
    double PositionDiff[3];
    double VelocityDiff[3];

    AgentsCoordinates = Phase->Coordinates[WhichAgent];
    AgentsVelocity = Phase->Velocities[WhichAgent];

    VectDifference(PositionDiff, AgentsCoordinates, TargetPosition);
    // SigmaGrad(PositionComponent, PositionDiff, 1, Dim_l);
    SigmaGrad(OutputVelocity, PositionDiff, 1, Dim_l);

    // VectDifference(VelocityDiff, AgentsVelocity, TargetVelocity);

    // VectSum(OutputVelocity, VelocityDiff, PositionComponent);
    MultiplicateWithScalar(OutputVelocity, OutputVelocity, -3, Dim_l);
}

/* Target tracking function */
void TargetTracking(double *OutputVelocity, double *TargetPosition,
                    phase_t *Phase, const double R_CoM, const double d_CoM,
                    const double R_trg, const double d_trg,
                    const int SizeOfNeighbourhood, const int WhichAgent,
                    const int Dim_l)
{

    NullVect(OutputVelocity, 3);

    double *AgentsCoordinates;
    double TargetComponent[3];

    AgentsCoordinates = Phase->Coordinates[WhichAgent];

    /* CoM component */
    static double CoMDifferenceVector[3];
    double CoMCoef;
    double CoMCoords[3];
    double CoMComponent[3];

    // GetNeighbourhoodSpecificCoM(CoMCoords, Phase, SizeOfNeighbourhood);
    GetNeighbourhoodSpecificCoM(CoMCoords, Phase, Phase->NumberOfAgents);
    VectDifference(CoMDifferenceVector, CoMCoords, AgentsCoordinates);
    UnitVect(CoMComponent, CoMDifferenceVector);

    CoMCoef = SigmoidLike(VectAbs(CoMDifferenceVector), R_CoM, d_CoM);

    MultiplicateWithScalar(CoMComponent, CoMComponent, CoMCoef, Dim_l);

    /* Trg component */
    static double TrgDifferenceVector[3];
    double TrgCoef;
    double TrgComponent[3];

    VectDifference(TrgDifferenceVector, TargetPosition, CoMCoords);
    UnitVect(TrgComponent, TrgDifferenceVector);

    TrgCoef = SigmoidLike(VectAbs(TrgDifferenceVector), R_trg, d_trg);

    MultiplicateWithScalar(TrgComponent, TrgComponent, TrgCoef, Dim_l);

    /* Add and normalize */
    VectSum(OutputVelocity, CoMComponent, TrgComponent);
}

/* Target tracking function without attrcaction to CoM */
void TargetTrackingSimple(double *OutputVelocity, double *TargetPosition,
                          phase_t *Phase, const double R_trg, const double d_trg,
                          const int WhichAgent, const int Dim_l)
{

    NullVect(OutputVelocity, 3);

    double *AgentsCoordinates;
    double TargetComponent[3];

    AgentsCoordinates = Phase->Coordinates[WhichAgent];

    /* Trg component */
    static double TrgDifferenceVector[3];
    double TrgCoef;
    double TrgComponent[3];

    VectDifference(TrgDifferenceVector, TargetPosition, AgentsCoordinates);
    UnitVect(TrgComponent, TrgDifferenceVector);

    TrgCoef = SigmoidLike(VectAbs(TrgDifferenceVector), R_trg, d_trg);

    MultiplicateWithScalar(TrgComponent, TrgComponent, TrgCoef, Dim_l);

    /* Add and normalize */
    VectSum(OutputVelocity, OutputVelocity, TrgComponent);
}

/* Potential based interaction from "Decentralized Robust Connectivity Control in Flocking of Multi-Robot Systems" 2020 */
void PotentialBased(double *OutputVelocity,
                    phase_t *Phase,
                    const double K0, const double R, const double r1, const double r2, const double mu1, const double mu2,
                    const int WhichAgent, const int Dim_l,
                    const bool normalize)
{
    double *AgentsCoordinates;
    double *NeighboursCoordinates;

    static double DistanceFromNeighbour;
    static double DifferenceVector[3];

    int i;
    int n = 0;

    AgentsCoordinates = Phase->Coordinates[WhichAgent];

    for (i = 1; i < Phase->NumberOfAgents; i++)
    { // i = 0 is the WhichAgent

        NeighboursCoordinates = Phase->Coordinates[i];

        VectDifference(DifferenceVector, AgentsCoordinates,
                       NeighboursCoordinates);
        if (2 == Dim_l)
        {
            DifferenceVector[2] = 0.0;
        }
        // Divide the distance by 1000 to have the distance in m
        DistanceFromNeighbour = VectAbs(DifferenceVector) * 0.001;

        if (DistanceFromNeighbour > R)
            continue;
        n += 1;

        UnitVect(DifferenceVector, DifferenceVector);

        MultiplicateWithScalar(DifferenceVector, DifferenceVector,
                               - 1000 * K0 * AdjacencyPotentialGradient(DistanceFromNeighbour, R, r1, r2, mu1, mu2), Dim_l);

        VectSum(OutputVelocity, OutputVelocity, DifferenceVector);
    }

    /* divide result by number of interacting units */
    if (normalize && n > 1)
    {
        double length = VectAbs(OutputVelocity) / n;
        UnitVect(OutputVelocity, OutputVelocity);
        MultiplicateWithScalar(OutputVelocity, OutputVelocity, length, Dim_l);
    }

    // printf("%d\t%f\t%f\t%f\t%f\t%f\t%f\n", Phase->RealIDs[WhichAgent], Phase->Coordinates[WhichAgent][0], Phase->Coordinates[WhichAgent][1], Phase->Velocities[WhichAgent][0], Phase->Velocities[WhichAgent][1], OutputVelocity[0], OutputVelocity[1]);
    // printf("%d\n", n);
}

void GlobalConnectivityController(double *OutputVelocity,
                                  phase_t *Phase,
                                  const double Kc0, const double sigma0, const double lambda_goal, const double R,
                                  const int WhichAgent, const int Dim_l,
                                  const bool normalize)
{
    double *AgentsCoordinates;
    double *NeighboursCoordinates;

    static double DifferenceVector[3];

    int i;
    int n = 0;
    double theta = sqrt(pow(R, 2) / (2.0 * log(2.0)));

    AgentsCoordinates = Phase->Coordinates[WhichAgent];

    // Get the second eigenvalue and eigenvector 
    double SecondEigenvalue = Phase->SecondEigenvalue;
    double *SecondEigenvector = Phase->SecondEigenvector;

    double Kc = Kc0 * sat(lambda_goal - SecondEigenvalue, 0.2);
    double sigma = sigma0 * sat(SecondEigenvalue - lambda_goal, 0.2);
    // printf("Kc: %f\n", Kc);
    // printf("sigma: %f\n", sigma);
    // printf("lambda_2: %f\n", SecondEigenvalue);
    // printf("Eigenvector:\n");
    // PrintVector(SecondEigenvector, Phase->NumberOfAgents);
    // printf("Laplacian\n");
    // PrintMatrix(Phase->Laplacian, Phase->NumberOfAgents, Phase->NumberOfAgents);

    for (i = 1; i < Phase->NumberOfAgents; i++)
    { // i = 0 is the WhichAgent

        // Get the unit vector from the neighbour to the agent
        NeighboursCoordinates = Phase->Coordinates[i];
        VectDifference(DifferenceVector, AgentsCoordinates,
                       NeighboursCoordinates);
        if (2 == Dim_l)
        {
            DifferenceVector[2] = 0.0;
        }
        UnitVect(DifferenceVector, DifferenceVector);

        // Get -a(ij) from the Laplacian stored in the Phase
        double a = Phase->Laplacian[i][WhichAgent];

        double coeff = (Kc * a * pow(SecondEigenvector[WhichAgent] - SecondEigenvector[i], 2) * exp((SecondEigenvalue - lambda_goal)/sigma)) / pow(theta, 2);
        // printf("coeff: %f\n", coeff);

        MultiplicateWithScalar(DifferenceVector, DifferenceVector, coeff, Dim_l);
    
        VectSum(OutputVelocity, OutputVelocity, DifferenceVector);
    }

    /* divide result by number of interacting units */
    if (normalize && n > 1)
    {
        double length = VectAbs(OutputVelocity) / n;
        UnitVect(OutputVelocity, OutputVelocity);
        MultiplicateWithScalar(OutputVelocity, OutputVelocity, length, Dim_l);
    }

}
