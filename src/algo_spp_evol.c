// MIT License
// Copyright (c) 2018 Eotvos Lorand University, Budapest

/* vim:set ts=4 sw=4 sts=4 et: */

/*

    3D SPP algorithm for testing evolution algorithms.

*/

#include "algo.h"
#include "algo_spp_evol.h"

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a > b ? b : a)

/* *INDENT-OFF* */

/* Initialization of flocking model parameters */
void InitializeFlockingParams(flocking_model_params_t *FlockingParams)
{
    /* Desired flocking velocity */
    CREATE_FLOCKING_PARAM(V_Flock,
                          .Name = "Preferred SPP velocity",
                          .UnitOfMeas = "m/s",
                          .Value = 400.0,
                          .Digits = 2,
                          .SizeOfStep = 10.0,
                          .Mult = 0.01,
                          .Min = 0.0,
                          .Max = 2e222);
    /* Velocity threshold of pairwise repulsion */
    CREATE_FLOCKING_PARAM(V_Rep,
                          .Name = "Maximum repulsion velocity",
                          .UnitOfMeas = "m/s",
                          .Value = 400.0,
                          .Digits = 2,
                          .SizeOfStep = 10,
                          .Mult = 0.01,
                          .Min = 0.0,
                          .Max = 2e222);
    /* Velocity slack for pairwise friction */
    CREATE_FLOCKING_PARAM(V_Frict,
                          .Name = "Friction velocity slack",
                          .UnitOfMeas = "m/s",
                          .Value = 50.0,
                          .Digits = 2,
                          .SizeOfStep = 10,
                          .Mult = 0.01,
                          .Min = 0.0,
                          .Max = 2e222);
    /* Maximal velocity */
    CREATE_FLOCKING_PARAM(V_Max,
                          .Name = "Maximum velocity",
                          .UnitOfMeas = "m/s",
                          .Value = 600.0,
                          .Digits = 2,
                          .SizeOfStep = 10.0,
                          .Mult = 0.01,
                          .Min = 0.0,
                          .Max = 2e222);
    /* Eq distance of the pair potential */
    CREATE_FLOCKING_PARAM(R_0,
                          .Name = "Equilibrium distance",
                          .UnitOfMeas = "m",
                          .Value = 1000.0,
                          .Digits = 2,
                          .SizeOfStep = 10.0,
                          .Mult = 0.01,
                          .Min = 0.0,
                          .Max = 2e222);

    /* Linear coefficient for the pressure repulsion */
    CREATE_FLOCKING_PARAM(K_Press,
                          .Name = "Slope of pressure repulsion",
                          .UnitOfMeas = "-",
                          .Value = 0.01,
                          .Digits = 3,
                          .SizeOfStep = 0.001,
                          .Mult = 1,
                          .Min = 0,
                          .Max = 2e222);

    /* Distance of friction relative to repulsion below which we only allow
       friction velocity slack and above which we use the linsqrt breaking
       curve to determine maximal allowed velocity difference */
    CREATE_FLOCKING_PARAM(R_0_Offset_Frict,
                          .Name = "Friction range",
                          .UnitOfMeas = "m",
                          .Value = 1000.0,
                          .Digits = 2,
                          .SizeOfStep = 10.0,
                          .Mult = 0.01,
                          .Min = -2e222,
                          .Max = 2e222);
    /* Distance offset of shill wall */
    CREATE_FLOCKING_PARAM(R_0_Shill,
                          .Name = "Shill dist offset",
                          .UnitOfMeas = "m",
                          .Value = 0.0,
                          .Digits = 2,
                          .SizeOfStep = 10.0,
                          .Mult = 0.01,
                          .Min = -2e222,
                          .Max = 2e222);
    /* Linear v-x coefficient of repulsion */
    CREATE_FLOCKING_PARAM(Slope_Rep,
                          .Name = "Slope of repulsion",
                          .UnitOfMeas = "1/s",
                          .Value = 0.15,
                          .Digits = 2,
                          .SizeOfStep = 0.01,
                          .Mult = 1,
                          .Min = 0,
                          .Max = 2e222);
    /* Linear v-x coefficient of attraction */
    CREATE_FLOCKING_PARAM(Slope_Att,
                          .Name = "Slope of attraction",
                          .UnitOfMeas = "1/s",
                          .Value = 0.005,
                          .Digits = 3,
                          .SizeOfStep = 0.001,
                          .Mult = 1,
                          .Min = 0,
                          .Max = 2e222);
    /* Linear v-x coefficient of friction breaking curve */
    CREATE_FLOCKING_PARAM(Slope_Frict,
                          .Name = "Slope of friction",
                          .UnitOfMeas = "1/s",
                          .Value = 0.4,
                          .Digits = 2,
                          .SizeOfStep = 0.01,
                          .Mult = 1,
                          .Min = 0,
                          .Max = 2e222);
    /* Max acceleration allowed in alignment term error calculation */
    CREATE_FLOCKING_PARAM(Acc_Frict,
                          .Name = "Acc limit of Friction",
                          .UnitOfMeas = "m/s^2",
                          .Value = 250,
                          .Digits = 1,
                          .SizeOfStep = 10,
                          .Mult = 0.01,
                          .Min = 0,
                          .Max = 2e222);

    /* Slope of wall */
    CREATE_FLOCKING_PARAM(Slope_Shill,
                          .Name = "Slope of wall",
                          .UnitOfMeas = "1/s",
                          .Value = 0.4,
                          .Digits = 2,
                          .SizeOfStep = 0.01,
                          .Mult = 1,
                          .Min = 0,
                          .Max = 2e222);
    /* Velocity of shill agent */
    CREATE_FLOCKING_PARAM(V_Shill,
                          .Name = "Velocity of shill agents",
                          .UnitOfMeas = "m/s",
                          .Value = 600.0,
                          .Digits = 2,
                          .SizeOfStep = 10.0,
                          .Mult = 0.01,
                          .Min = 0.0,
                          .Max = 2e222);
    CREATE_FLOCKING_PARAM(Acc_Shill,
                          .Name = "Acc limit of shill",
                          .UnitOfMeas = "m/s^2",
                          .Value = 250,
                          .Digits = 1,
                          .SizeOfStep = 10,
                          .Mult = 0.01,
                          .Min = 0,
                          .Max = 2e222);

    /* Olfati Saber gradient component parameters */

    /* Param of the bump function, shifting the decaying starting point to the "right" when h rises */
    CREATE_FLOCKING_PARAM(H_Bump,
                          .Name = "Decay point",
                          .UnitOfMeas = "-",
                          .Value = 0.3,
                          .Digits = 2,
                          .SizeOfStep = 0.01,
                          .Mult = 1.0,
                          .Min = 0,
                          .Max = 1);

    /* Param of the sigma norm */
    CREATE_FLOCKING_PARAM(Epsilon,
                          .Name = "Epsilon",
                          .UnitOfMeas = "-",
                          .Value = 0.1,
                          .Digits = 2,
                          .SizeOfStep = 0.01,
                          .Mult = 1.0,
                          .Min = 0,
                          .Max = 10);

    CREATE_FLOCKING_PARAM(A_Action_Function,
                          .Name = "A Action Function",
                          .UnitOfMeas = "m",
                          .Value = 50,
                          .Digits = 1,
                          .SizeOfStep = 1,
                          .Mult = 1,
                          .Min = 0,
                          .Max = 2e222);

    CREATE_FLOCKING_PARAM(B_Action_Function,
                          .Name = "B Action Function",
                          .UnitOfMeas = "m",
                          .Value = 250,
                          .Digits = 1,
                          .SizeOfStep = 1,
                          .Mult = 1,
                          .Min = 0,
                          .Max = 2e222);

    /* Hyper parameters */
    CREATE_FLOCKING_PARAM(Size_Neighbourhood,
                          .Name = "Size of neighbourhood",
                          .UnitOfMeas = "-",
                          .Value = 4,
                          .Digits = 1,
                          .SizeOfStep = 1,
                          .Mult = 1.0,
                          .Min = 0,
                          .Max = 2e222);

    /* Size of the arena */
    /* Diameter of circle or Length of the edges of the square */
    CREATE_FLOCKING_PARAM(ArenaRadius,
                          .Name = "Arena Radius",
                          .UnitOfMeas = "m",
                          .Value = 62500.0,
                          .Digits = 1,
                          .SizeOfStep = 10.0,
                          .Mult = 0.01,
                          .Min = 0.0,
                          .Max = 2e222);

    /* Size of the arena */
    /* Diameter of circle or Length of the edges of the square */
    CREATE_FLOCKING_PARAM(Lambda2,
                          .Name = "Target lambda2",
                          .UnitOfMeas = "-",
                          .Value = 4.0,
                          .Digits = 1,
                          .SizeOfStep = 1.0,
                          .Mult = 1.0,
                          .Min = 0.0,
                          .Max = 2e222);


    /* Hidden parameters */

    /* Linear scaler of velocity difference error in friction term */
    CREATE_HIDDEN_FLOCKING_PARAM(C_Frict,
                                 .Name = "Friction coefficient",
                                 .UnitOfMeas = "-",
                                 .Value = 1.0,
                                 .Digits = 2,
                                 .SizeOfStep = 0.01,
                                 .Mult = 1.0,
                                 .Min = 0.0,
                                 .Max = 2e222);
    /*Arena center X and Y */
    CREATE_HIDDEN_FLOCKING_PARAM(ArenaCenterX,
                                 .Name = "Arena Center X",
                                 .UnitOfMeas = "m",
                                 .Value = 0.0,
                                 .Digits = 2,
                                 .SizeOfStep = 10.0,
                                 .Mult = 0.01,
                                 .Min = -2e222,
                                 .Max = 2e222);
    CREATE_HIDDEN_FLOCKING_PARAM(ArenaCenterY,
                                 .Name = "Arena Center Y",
                                 .UnitOfMeas = "m",
                                 .Value = 0.0,
                                 .Digits = 2,
                                 .SizeOfStep = 10.0,
                                 .Mult = 0.01,
                                 .Min = -2e222,
                                 .Max = 2e222);
    /* Shape of the arena (0 means sphere, 1 means cube) */
    CREATE_HIDDEN_FLOCKING_PARAM(ArenaShape,
                                 .Name = "Shape of the arena",
                                 .UnitOfMeas = "",
                                 .Value = 0.0,
                                 .Digits = 0.0,
                                 .SizeOfStep = 0.0,
                                 .Mult = 0.0,
                                 .Min = 0.0,
                                 .Max = 1.0);

    /* 2D or 3D? */
    CREATE_HIDDEN_FLOCKING_PARAM(Dim,
                                 .Name = "Number of dimensions in the simulation (2 or 3)",
                                 .UnitOfMeas = "",
                                 .Value = 2,
                                 .Digits = 0,
                                 .SizeOfStep = 11,
                                 .Mult = 1,
                                 .Min = 2,
                                 .Max = 3);

    // FlockingParams->NumberOfInnerStates = 3; // Columns 1 and 2 handle some target tracking functionalities and column 3 is for the agent own neighbourhood
    FlockingParams->NumberOfInnerStates = 17; // Column 1 is for att/rep neighbourhood ratio and column 2 is for the alignment. (For ex, \\
    0.5 for column 1 and agent 1 means that this agent will only use half of its available neighbourhood to compute its att/rep forces) \\
    This is to prepare the RL framework.
    // Column 3 is to know which agent is the leader (1 if yes).
    //  Columns 4, 5, 6, 7, 8, 9, 10, 11 are to save and plot the interaction components (Two for one arrow x, y).
    //  4, 5 for repulsion x, y ; 6,7 attraction
    //  8,9 alignment, 10, 11, obstacles
}

/* *INDENT-ON* */

void InitializePhase(phase_t *Phase, flocking_model_params_t *FlockingParams,
                     sit_parameters_t *SitParams, int Verbose)
{

    char ArenaFilePath[512];
    char ObstaclesFilePath[512];
    int i;

    /* Load arenas from arena file */
    getcwd(ArenaFilePath, sizeof(ArenaFilePath));
    strcat(ArenaFilePath, "/parameters/arenas.default");
    // parse arguments for user defined arena file
    for (i = 0; i < FlockingParams->NumberOfInputs - 1; i++)
    {
        if (strcmp(FlockingParams->Inputs[i], "-arena") == 0)
            strcpy(ArenaFilePath, FlockingParams->Inputs[i + 1]);
    }
    if (Verbose != 0)
    {
        printf("Using arena file: %s\n", ArenaFilePath);
    }
    ParseArenaFile(ArenaFilePath, &Arenas, 1, Verbose);

    /* Load obstacles from obstacle file */
    obstacles.o_count = 0;
    getcwd(ObstaclesFilePath, sizeof(ObstaclesFilePath));
    strcat(ObstaclesFilePath, "/parameters/obstacles.default");
    // parse arguments for user defined obstacle file
    for (i = 0; i < FlockingParams->NumberOfInputs - 1; i++)
    {
        if (strcmp(FlockingParams->Inputs[i], "-obst") == 0)
            strcpy(ObstaclesFilePath, FlockingParams->Inputs[i + 1]);
    }
    if (Verbose != 0)
    {
        printf("Using obstacle file: %s\n", ObstaclesFilePath);
    }
    ParseObstacleFile(ObstaclesFilePath, &obstacles, Verbose);

    /* randomize phase within 2D grid arena */
    // Here we assume 1s delay in V_Flock*2
    // PlaceAgentsOnXYPlane(Phase, 2 * ArenaRadius, 2 * ArenaRadius,
    //         ArenaCenterX, ArenaCenterY, 0,
    //         0, Phase->NumberOfAgents, MAX(SitParams->Radius, V_Flock * 2));

    // PlaceAgentsInsideARing(Phase, 30000, 0, Phase->NumberOfAgents,
    //          ArenaCenterX - 25000, ArenaCenterY, 0, 0, MAX(SitParams->Radius, V_Flock * 2));

    // PlaceAgentsNoRandomInsideARing(Phase, 30000, 0, Phase->NumberOfAgents,
    //         ArenaCenterX - 20000, ArenaCenterY, 0, 0, MAX(SitParams->Radius, V_Flock * 2));
    // Phase->Coordinates[0][0] += 12000;
    //  Phase->Coordinates[0][0] += 20000;

    // int start = ArenaCenterX - 50000;

    // for (i = Phase->NumberOfAgents - 1; i >= 0; i--) {
    //     Phase->Coordinates[i][0] = start;
    //     Phase->Coordinates[i][1] = 0;
    //     start += 2000;
    // }
    // Phase->Coordinates[0][0] = -1000;
    // Phase->Velocities[0][0] = 100;

    // PlaceAgentsInsideARing(Phase, 12000, 0, Phase->NumberOfAgents,
    //                        ArenaCenterX, ArenaCenterY, 0, 0, MAX(SitParams->Radius, V_Flock * 2));

    /* reset z coordinate in two dimensions */
    if (2 == Dim)
    {
        for (i = 0; i < Phase->NumberOfAgents; i++)
        {
            Phase->Coordinates[i][2] = 0;
            Phase->Velocities[i][2] = 0;
        }
    }
}

// credit goes to: http://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon/2922778#2922778
bool PointInObstacle(obstacle_t *obstacle, double *Point)
{
    bool c = false;
    int nvert = obstacle->p_count;
    int i, j = 0;
    // Warning: this will crash if for some reason obstacle->p[j][1] == obstacle->p[i][1]
    for (i = 0, j = nvert - 1; i < nvert; j = i++)
    {
        if (((obstacle->p[i][1] > Point[1]) != (obstacle->p[j][1] > Point[1])) && (Point[0] <
                                                                                   (obstacle->p[j][0] - obstacle->p[i][0]) * (Point[1] - obstacle->p[i][1]) / (obstacle->p[j][1] - obstacle->p[i][1]) + obstacle->p[i][0]))
        {
            c = !c;
        }
    }
    return c;
}

// sets NearestPointOfPolygon as expected from its name and returns distance from it
// distance is negative if we are inside obstacle and positive if outside
// so far works only for convex polygons
double DistanceOfNearestPointOfObstacle(double *NearestPointOfPolygon,
                                        obstacle_t *obstacle, double *AgentsCoordinates)
{
    int i, j;
    int NearestVertexIndex, NearestEdgeStartPointIndex;
    double NearestVertexDistance = 1e22, NearestEdgeDistance = 1e22;
    double PolygonDistance, vertexdistance, edgedistance;
    double EdgeVector[3];

    // find the nearest point of the Polygon
    for (j = 0; j < obstacle->p_count; j++)
    {
        // check closest vertex
        vertexdistance = DistanceOfTwoPoints2D(AgentsCoordinates,
                                               obstacle->p[j]);
        if (vertexdistance < NearestVertexDistance)
        {
            NearestVertexDistance = vertexdistance;
            NearestVertexIndex = j;
        }
        // check closest edge
        if (AtShadow(obstacle->p[j], obstacle->p[(j + 1) % obstacle->p_count],
                     AgentsCoordinates))
        {
            edgedistance = DistanceFromLineXY(AgentsCoordinates, obstacle->p[j],
                                              obstacle->p[(j + 1) % obstacle->p_count]);
            if (edgedistance < NearestEdgeDistance)
            {
                NearestEdgeDistance = edgedistance;
                NearestEdgeStartPointIndex = j;
            }
        }
    }
    PolygonDistance = MIN(NearestEdgeDistance, NearestVertexDistance);
    // vertex is the closest point
    if (NearestVertexDistance <= NearestEdgeDistance)
    {
        FillVect(NearestPointOfPolygon, obstacle->p[NearestVertexIndex][0],
                 obstacle->p[NearestVertexIndex][1], 0.0);
    }
    // or the closest point is on an edge
    else
    {
        VectDifference(EdgeVector,
                       obstacle->p[(NearestEdgeStartPointIndex +
                                    1) %
                                   obstacle->p_count],
                       obstacle->p[NearestEdgeStartPointIndex]);
        UnitVect(EdgeVector, EdgeVector);
        VectDifference(NearestPointOfPolygon, AgentsCoordinates,
                       obstacle->p[NearestEdgeStartPointIndex]);
        MultiplicateWithScalar(EdgeVector, EdgeVector, ScalarProduct(EdgeVector, NearestPointOfPolygon, 2), 3);
        VectSum(NearestPointOfPolygon, obstacle->p[NearestEdgeStartPointIndex],
                EdgeVector);
    }

    return (PointInObstacle(obstacle,
                            AgentsCoordinates)
                ? -1
                : 1) *
           PolygonDistance;
}

// Note that this function is in 2D yet
// Note that output is only ADDED to OutputVelocity
void Shill_Obstacle_LinSqrt(double *OutputVelocity, phase_t *Phase,
                            obstacle_t *obstacle, const double V_Shill,
                            const double R0_Offset_Shill, const double Acc_Shill,
                            const double Slope_Shill, const int WhichAgent,
                            int *CollisionsObst, bool *AgentInObst, int ObstIndex)
{

    int i;
    double *AgentsPosition = Phase->Coordinates[WhichAgent];
    double *AgentsVelocity = Phase->Velocities[WhichAgent];
    static double ToArena[3];
    static double VelDiff;
    static double DistFromWall; // negative inside obstacle, positive outside
    static double MaxVelDiff;
    static int WhichObst = -1;

    // get target point on obstacle wall in ToArena and distance from it in DistFromWall
    // latter will be negative if we are inside obstacle
    DistFromWall =
        DistanceOfNearestPointOfObstacle(ToArena, obstacle, AgentsPosition);

    // inside, shill is going towards arena
    if (DistFromWall < 0)
    {
        VectDifference(ToArena, ToArena, AgentsPosition);
        if (AgentInObst[Phase->RealIDs[WhichAgent]] == false)
        {
            (*CollisionsObst) += 1;
            WhichObst = ObstIndex;
            // printf("%d\n", *CollisionsObst);
            AgentInObst[Phase->RealIDs[WhichAgent]] = true;
        }
    }
    // outside, shill is going away from arena
    else
    {
        VectDifference(ToArena, AgentsPosition, ToArena);
        if (ObstIndex == WhichObst)
        {
            AgentInObst[Phase->RealIDs[WhichAgent]] = false;
        }
    }
    ToArena[2] = 0;
    UnitVect(ToArena, ToArena);
    MultiplicateWithScalar(ToArena, ToArena, V_Shill, 3);
    VectDifference(ToArena, ToArena, AgentsVelocity);
    ToArena[2] = 0;
    VelDiff = VectAbs(ToArena);
    UnitVect(ToArena, ToArena);
    // RotateVectXY(ToArena, ToArena, M_PI_2);
    // calculate max allowed velocity difference at a given distance based
    // on an optimal linsqrt breaking curve
    MaxVelDiff = VelDecayLinSqrt(DistFromWall, Slope_Shill, Acc_Shill,
                                 VelDiff, R0_Offset_Shill);
    // if velocity difference is larger than allowed, we compensate it
    if (VelDiff > MaxVelDiff)
    {
        MultiplicateWithScalar(ToArena, ToArena, VelDiff - MaxVelDiff, 2);
        VectSum(OutputVelocity, OutputVelocity, ToArena);
    }
}

/* Refreshing values of outer variables (e. g. "number of caught agents" in the chasing algorithm) */
void HandleOuterVariables(phase_t *Phase,
                          vizmode_params_t *VizParams,
                          sit_parameters_t *SitParams,
                          unit_model_params_t *UnitParams,
                          const double ActualTime, char *OutputDirectory)
{
}

void CalculatePreferredVelocity(double *OutputVelocity,
                                double *OutputInnerState,
                                phase_t *Phase,
                                double **TargetsArray,
                                int WhichTarget,
                                const int WhichAgent,
                                flocking_model_params_t *FlockingParams,
                                vizmode_params_t *VizParams,
                                const double Delay,
                                const double ActualTime, agent_debug_info_t *DebugInfo,
                                const int Flocking_type, double **Jacard,
                                int *CollisionsObst, bool *AgentInObst)
{

    /* Clear output velocity */
    NullVect(OutputVelocity, 3);

    int i, j;
    double *AgentsCoordinates;
    AgentsCoordinates = Phase->Coordinates[WhichAgent];
    double *AgentsVelocity;
    AgentsVelocity = Phase->Velocities[WhichAgent];
    double velo[3];
    NullVect(velo, 3);
    static int ActualTargetID;

    if (WhichTarget == 0)
    {
        ActualTargetID = 0;
    }

    static double ArenaVelocity[3];
    NullVect(ArenaVelocity, 3);
    static double ObstacleVelocity[3];
    NullVect(ObstacleVelocity, 3);
    static double PotentialVelocity[3];
    NullVect(PotentialVelocity, 3);
    static double PressureVelocity[3];
    NullVect(PressureVelocity, 3);
    static double AttractionVelocity[3];
    NullVect(AttractionVelocity, 3);
    static double GradientAcceleration[3];
    NullVect(GradientAcceleration, 3);
    static double SlipVelocity[3];
    NullVect(SlipVelocity, 3);
    static double TargetTrackingVelocity[3];
    NullVect(TargetTrackingVelocity, 3);
    static double AlignOlfati[3];
    NullVect(AlignOlfati, 3);
    static double TrackOlfati[3];
    NullVect(TrackOlfati, 3);
    static double AdjacencyPotentialVelocity[3];
    NullVect(AdjacencyPotentialVelocity, 3);
    static double GlobalConnectivityVelocity[3];
    NullVect(GlobalConnectivityVelocity, 3);

    static double ActualNeighboursCoordinates[3];
    NullVect(ActualNeighboursCoordinates, 3);
    static double ActualNeighboursVelocity[3];
    NullVect(ActualNeighboursVelocity, 3);
    static double DifferenceVector[3];
    NullVect(DifferenceVector, 3);
    static double DistanceFromNeighbour = 0.0;
    static double DistanceFromObstVect[3];
    NullVect(DistanceFromObstVect, 3);
    static double NormalizedAgentsVelocity[3];
    static double TargetPosition[3];
    NullVect(TargetPosition, 3);
    static double NormalizedTargetTracking[3];
    static double test1[3];
    static double test2[3];
    static double LeaderFollower[3];
    NullVect(LeaderFollower, 3);
    static double NormalizedLF[3];
    NullVect(NormalizedLF, 3);

    /* SPP term */
    FillVect(NormalizedAgentsVelocity, AgentsVelocity[0], AgentsVelocity[1],
             AgentsVelocity[2]);
    UnitVect(NormalizedAgentsVelocity, NormalizedAgentsVelocity);
    MultiplicateWithScalar(NormalizedAgentsVelocity, NormalizedAgentsVelocity,
                           V_Flock, (int)Dim);

    /* test going east term */
    FillVect(test2, 1, 0,
             AgentsVelocity[2]);
    UnitVect(test2, test2);
    MultiplicateWithScalar(test2, test2,
                           0.1 * V_Flock, (int)Dim);

    static double dist;

    if (Flocking_type == 0)
    {
        /* Repulsion */
        // RepulsionLin(PotentialVelocity, Phase, V_Rep,
        //         Slope_Rep, R_0, WhichAgent, (int) Dim, false);
        // printf("%f\t", VectAbs(PotentialVelocity));

        /* Attraction */
        AttractionLin(AttractionVelocity, Phase, 2 * V_Rep,
                      Slope_Att, R_0 + 100, WhichAgent, (int)Dim, false, Jacard);

        // AttractionVAT(AttractionVelocity, Phase, 2 * V_Rep,
        //         Slope_Att, R_0 + 100, WhichAgent, Size_Neighbourhood, (int) Dim, false);

        /* Press Rep */
        PressureRepulsion(PressureVelocity, Phase, K_Press, WhichAgent, (int)Dim, R_0, V_Rep);

        /* Target tracking */
        if (WhichTarget != 0 && Phase->RealIDs[WhichAgent] == 0)
        {
            // printf("%d\n", Phase->RealIDs[WhichAgent]);
            TargetTrackingSimple(TargetTrackingVelocity, TargetsArray[WhichTarget - 1],
                                 Phase, 5000, 10000, WhichAgent, (int)Dim);
            MultiplicateWithScalar(TargetTrackingVelocity, TargetTrackingVelocity,
                                   V_Flock, (int)Dim);
            UnitVect(NormalizedTargetTracking, TargetTrackingVelocity);
            MultiplicateWithScalar(TargetTrackingVelocity, NormalizedTargetTracking,
                                   MIN(V_Flock * 0.5, VectAbs(TargetTrackingVelocity)), (int)Dim);
            // MultiplicateWithScalar(TargetTrackingVelocity, TargetTrackingVelocity, 0.3, (int) Dim);
        }

        bool ILFalgo = true;

        if (Phase->RealIDs[WhichAgent] != 0 && VizParams->DisplayLeader == 1)
        {

            if (ILFalgo)
            {

                int WhoLeads = AssignInnerState(OutputInnerState, Phase);
                if (WhoLeads != 0)
                {
                    TargetTrackingSimple(LeaderFollower, Phase->Coordinates[WhoLeads], Phase, 4000, 6000, WhichAgent, (int)Dim);
                    MultiplicateWithScalar(LeaderFollower, LeaderFollower,
                                           V_Flock, (int)Dim);
                    UnitVect(NormalizedLF, LeaderFollower);
                    MultiplicateWithScalar(LeaderFollower, NormalizedLF, MIN(V_Flock, VectAbs(LeaderFollower)), (int)Dim);
                }
                else
                {
                    OutputInnerState[2] = 0;
                }
            }
            // printf("%d following %d and innner state is %f\n", DebugInfo->AgentsSeqNumber, WhoLeads, OutputInnerState[2]);
            // int WhoLeads = 0;
            // for (i = Phase->NumberOfAgents - 1; i > 0; i --) {

            //     if (Phase->InnerStates[i][2] == 1) {
            //         WhoLeads = i;
            //         OutputInnerState[2] = 2;
            //         break;
            //     }
            //     else if (Phase->InnerStates[i][2] == 2){// || Phase->InnerStates[i][2] == 3) {
            //         WhoLeads = i;
            //         OutputInnerState[2] = 3;
            //     }
            //     else if (Phase->InnerStates[i][2] == 3 && OutputInnerState[2] != 3) {
            //         WhoLeads = i;
            //         OutputInnerState[2] = 4;
            //     }
            //     else if (Phase->InnerStates[i][2] == 4 && OutputInnerState[2] != 4) {
            //         WhoLeads = i;
            //         OutputInnerState[2] = 5;
            //     }
            //     else if (Phase->InnerStates[i][2] == 5 && OutputInnerState[2] != 5) {
            //         WhoLeads = i;
            //         OutputInnerState[2] = 6;
            //     }
            //     else {
            //         OutputInnerState[2] = 0;
            //     }
            // }
            else
            {
                int WhoLeads = 0;
                for (i = 0; i < Phase->NumberOfAgents; i++)
                {
                    if (Phase->RealIDs[i] == 0)
                    {
                        WhoLeads = i;
                        OutputInnerState[2] = 2;
                        break;
                    }
                    else
                    {
                        OutputInnerState[2] = 0;
                    }
                }
            }
            // if (WhoLeads != 0){
            //     TargetTrackingSimple(LeaderFollower, Phase->Coordinates[WhoLeads], Phase, 4000, 6000, WhichAgent, (int)Dim);
            //     MultiplicateWithScalar(LeaderFollower, LeaderFollower,
            //     V_Flock, (int)Dim);
            //     UnitVect(NormalizedLF, LeaderFollower);
            //     MultiplicateWithScalar(LeaderFollower, NormalizedLF, MIN(V_Flock, VectAbs(LeaderFollower)), (int)Dim);
            // }
            // else {
            //     OutputInnerState[2] = 0;
            //     }
        }

        // GradientBased(GradientAcceleration, Phase, Epsilon, A_Action_Function, B_Action_Function, H_Bump,
        //     R_0, 3 * R_0, WhichAgent, (int) Dim);
        // MultiplicateWithScalar(GradientAcceleration, GradientAcceleration, 3, (int)Dim);
        // printf("%f\t%f\t%f\t%f\t%f\n", VectAbs(test1), VectAbs(PotentialVelocity), VectAbs(test2), VectAbs(AttractionVelocity), VectAbs(GradientAcceleration));
    }

    else if (Flocking_type == 1)
    {
        AttractionVAT(AttractionVelocity, Phase, 1.6 * V_Rep,
                      Slope_Att, R_0 + 100, WhichAgent, (int)Dim, false, Jacard);

        RepulsionLin(PotentialVelocity, Phase, V_Rep,
                     Slope_Rep, R_0, WhichAgent, (int)Dim, false);

        if (WhichTarget != 0 && Phase->RealIDs[WhichAgent] == 0)
        {
            TargetTrackingSimple(TargetTrackingVelocity, TargetsArray[WhichTarget - 1],
                                 Phase, 5000, 10000, WhichAgent, (int)Dim);
            MultiplicateWithScalar(TargetTrackingVelocity, TargetTrackingVelocity,
                                   V_Flock, (int)Dim);
            UnitVect(NormalizedTargetTracking, TargetTrackingVelocity);
            MultiplicateWithScalar(TargetTrackingVelocity, NormalizedTargetTracking,
                                   MIN(V_Flock * 0.5, VectAbs(TargetTrackingVelocity)), (int)Dim);
            // MultiplicateWithScalar(TargetTrackingVelocity, TargetTrackingVelocity, 0.4, (int) Dim);
        }

        if (Phase->RealIDs[WhichAgent] != 0 && VizParams->DisplayLeader == 1)
        {

            int WhoLeads = AssignInnerState(OutputInnerState, Phase);

            if (WhoLeads != 0)
            {
                TargetTrackingSimple(LeaderFollower, Phase->Coordinates[WhoLeads], Phase, 4000, 6000, WhichAgent, (int)Dim);
                MultiplicateWithScalar(LeaderFollower, LeaderFollower,
                                       V_Flock, (int)Dim);
                UnitVect(NormalizedLF, LeaderFollower);
                MultiplicateWithScalar(LeaderFollower, NormalizedLF, MIN(V_Flock, VectAbs(LeaderFollower)), (int)Dim);
            }
            else
            {
                OutputInnerState[2] = 0;
            }
        }

        /* Olfati Gradient based term for attraction//repulsion */
        // GradientBased(GradientAcceleration, Phase, Epsilon, A_Action_Function, B_Action_Function, H_Bump,
        //     R_0, (sqrt(2) + 1) * R_0, WhichAgent, (int) Dim);
        // MultiplicateWithScalar(GradientAcceleration, GradientAcceleration, 3, (int)Dim);
        // AlignmentOlfati(AlignOlfati, Phase, H_Bump, 2 * R_0, WhichAgent, (int) Dim, Epsilon);
    }

    else if (Flocking_type == 2)
    {

        /* Olfati Gradient based term for attraction//repulsion */
        GradientBased(GradientAcceleration, Phase, Epsilon, A_Action_Function, B_Action_Function, H_Bump,
                      R_0, (sqrt(2) + 1) * R_0, WhichAgent, (int)Dim);
        MultiplicateWithScalar(GradientAcceleration, GradientAcceleration, 3, (int)Dim);
        AlignmentOlfati(AlignOlfati, Phase, H_Bump, 2 * R_0, WhichAgent, (int)Dim, Epsilon);

        if (WhichTarget != 0 && Phase->RealIDs[WhichAgent] == 0)
        {
            TargetTrackingSimple(TargetTrackingVelocity, TargetsArray[WhichTarget - 1],
                                 Phase, 5000, 10000, WhichAgent, (int)Dim);
            MultiplicateWithScalar(TargetTrackingVelocity, TargetTrackingVelocity,
                                   V_Flock, (int)Dim);
            UnitVect(NormalizedTargetTracking, TargetTrackingVelocity);
            MultiplicateWithScalar(TargetTrackingVelocity, NormalizedTargetTracking,
                                   MIN(V_Flock, VectAbs(TargetTrackingVelocity)), (int)Dim);
            // MultiplicateWithScalar(TargetTrackingVelocity, TargetTrackingVelocity, 0.4, (int) Dim);
        }
        // printf("%f\n", VectAbs(GradientAcceleration));

        if (Phase->RealIDs[WhichAgent] != 0 && VizParams->DisplayLeader == 1)
        {

            int WhoLeads = AssignInnerState(OutputInnerState, Phase);

            if (WhoLeads != 0)
            {
                // TargetTrackingSimple(LeaderFollower, Phase->Coordinates[WhoLeads], Phase, 4000, 6000, WhichAgent, (int)Dim);
                // MultiplicateWithScalar(LeaderFollower, LeaderFollower,
                // V_Flock, (int)Dim);
                // UnitVect(NormalizedLF, LeaderFollower);
                // MultiplicateWithScalar(LeaderFollower, NormalizedLF, MIN(V_Flock, VectAbs(LeaderFollower)), (int)Dim);
                // MultiplicateWithScalar(LeaderFollower, NormalizedLF, 100000, (int)Dim);
                // printf("\t%f\n", VectAbs(LeaderFollower));
                TrackingOlfati(LeaderFollower, Phase->Coordinates[WhoLeads], Phase->Velocities[WhoLeads],
                               Phase, WhichAgent, (int)Dim);
            }
            else
            {
                OutputInnerState[2] = 0;
            }
        }
    }

    else if (Flocking_type == 3)
    {
        /* Target tracking component (doesn't include repulsion so add it) */
        RepulsionLin(PotentialVelocity, Phase, V_Rep,
                     Slope_Rep, R_0, WhichAgent, (int)Dim, false);

        if (WhichTarget != 0)
        {
            TargetTracking(TargetTrackingVelocity, TargetsArray[ActualTargetID], Phase,
                           R_0 + 300, R_0 + 4000, 5000, 10000,
                           Size_Neighbourhood, WhichAgent, (int)Dim);
            MultiplicateWithScalar(TargetTrackingVelocity, TargetTrackingVelocity,
                                   V_Flock, (int)Dim);
            UnitVect(NormalizedTargetTracking, TargetTrackingVelocity);
            MultiplicateWithScalar(TargetTrackingVelocity, NormalizedTargetTracking,
                                   MIN(V_Flock, VectAbs(TargetTrackingVelocity)), (int)Dim);
        }
    }
    // stratégie de flocking avec target tracking automatique si un agent se perd
    else if (Flocking_type == 4)
    {
        AttractionLin(AttractionVelocity, Phase, 1.6 * V_Rep,
                      Slope_Att, R_0 + 100, WhichAgent, (int)Dim, false, Jacard);
        PressureRepulsion(PressureVelocity, Phase, K_Press, WhichAgent, (int)Dim, R_0, V_Rep);

        if (Phase->NumberOfAgents <= 2)
        {
            TargetTrackingSimple(LeaderFollower, Phase->Coordinates[Phase->NumberOfAgents - 1], Phase, R_0, R_0 + 100, WhichAgent, (int)Dim);
            MultiplicateWithScalar(LeaderFollower, LeaderFollower,
                                   V_Flock, (int)Dim);
            UnitVect(NormalizedLF, LeaderFollower);
            MultiplicateWithScalar(LeaderFollower, NormalizedLF, MIN(V_Flock, VectAbs(LeaderFollower)), (int)Dim);
        }
    }
    else if (Flocking_type == 5)
    {

        AttractionLin(AttractionVelocity, Phase, 1.6 * V_Rep,
                      Slope_Att, R_0 + 100, WhichAgent, (int)Dim, false, Jacard);

        RepulsionLin(PotentialVelocity, Phase, V_Rep,
                     Slope_Rep, R_0, WhichAgent, (int)Dim, false);

        if (WhichTarget != 0 && Phase->RealIDs[WhichAgent] == 0)
        {
            TargetTrackingSimple(TargetTrackingVelocity, TargetsArray[WhichTarget - 1],
                                 Phase, 5000, 10000, WhichAgent, (int)Dim);
            MultiplicateWithScalar(TargetTrackingVelocity, TargetTrackingVelocity,
                                   V_Flock, (int)Dim);
            UnitVect(NormalizedTargetTracking, TargetTrackingVelocity);
            MultiplicateWithScalar(TargetTrackingVelocity, NormalizedTargetTracking,
                                   MIN(V_Flock * 0.5, VectAbs(TargetTrackingVelocity)), (int)Dim);
            // MultiplicateWithScalar(TargetTrackingVelocity, TargetTrackingVelocity, 0.4, (int) Dim);
        }
        if (Phase->RealIDs[WhichAgent] != 0 && WhichTarget != 0)
        {

            int WhoLeads = AssignInnerState(OutputInnerState, Phase);

            if (WhoLeads != 0)
            {
                TargetTrackingSimple(LeaderFollower, Phase->Coordinates[WhoLeads], Phase, 4000, 6000, WhichAgent, (int)Dim);
                MultiplicateWithScalar(LeaderFollower, LeaderFollower,
                                       V_Flock, (int)Dim);
                UnitVect(NormalizedLF, LeaderFollower);
                MultiplicateWithScalar(LeaderFollower, NormalizedLF, MIN(V_Flock, VectAbs(LeaderFollower)), (int)Dim);
            }
            else
            {
                OutputInnerState[2] = 0;
            }
        }
    }
    else if (Flocking_type == 6)
    {
        const double R = 30.0;
        double r1 = 0.45 * R;
        double r2 = 0.55 * R;
        const double mu1 = 10;
        const double mu2 = 200;
        const double K0 = 0.05;
        PotentialBased(AdjacencyPotentialVelocity, Phase, K0, R, r1, r2, mu1, mu2, WhichAgent, (int)Dim, false);

        const double Kc0 = 1000;
        const double sigma0 = 1;
        const double lambda_goal = Lambda2;
        GlobalConnectivityController(GlobalConnectivityVelocity, Phase, Kc0, sigma0, lambda_goal, R, WhichAgent, (int)Dim, false);

        if (WhichTarget != 0 && Phase->RealIDs[WhichAgent] == 0)
        {
            TargetTrackingSimple(TargetTrackingVelocity, TargetsArray[WhichTarget - 1],
                                 Phase, 2000, 2000, WhichAgent, (int)Dim);
            MultiplicateWithScalar(TargetTrackingVelocity, TargetTrackingVelocity,
                                   V_Flock, (int)Dim);
            UnitVect(NormalizedTargetTracking, TargetTrackingVelocity);
            MultiplicateWithScalar(TargetTrackingVelocity, NormalizedTargetTracking,
                                   MIN(V_Flock * 0.5, VectAbs(TargetTrackingVelocity)), (int)Dim);
            // MultiplicateWithScalar(TargetTrackingVelocity, TargetTrackingVelocity, 0.4, (int) Dim);
        }

        if (WhichTarget > 1 && Phase->RealIDs[WhichAgent] == 1)
        {
            TargetTrackingSimple(TargetTrackingVelocity, TargetsArray[WhichTarget - 2],
                                 Phase, 5000, 10000, WhichAgent, (int)Dim);
            MultiplicateWithScalar(TargetTrackingVelocity, TargetTrackingVelocity,
                                   V_Flock, (int)Dim);
            UnitVect(NormalizedTargetTracking, TargetTrackingVelocity);
            MultiplicateWithScalar(TargetTrackingVelocity, NormalizedTargetTracking,
                                   MIN(V_Flock * 0.5, VectAbs(TargetTrackingVelocity)), (int)Dim);
            // MultiplicateWithScalar(TargetTrackingVelocity, TargetTrackingVelocity, 0.4, (int) Dim);
        }
    }

    /* (by now far from but better than) Viscous friction-like term */
    FrictionLinSqrt(SlipVelocity, Phase, C_Frict, V_Frict, Acc_Frict,
                    Slope_Frict, R_0 + R_0_Offset_Frict, WhichAgent, (int)Dim);
    // printf("%f\n", VectAbs(AttractionVelocity));

    /* Interaction with walls of the arena (shill agents) */
    Shill_Wall_LinSqrt(ArenaVelocity, Phase, ArenaCenterX, ArenaCenterY,
                       ArenaRadius, &(Arenas.a[(int)ArenaShape]), V_Shill, R_0_Shill,
                       Acc_Shill, Slope_Shill, WhichAgent, Dim);

    /* Interaction with obstacles (shill agents) */
    for (i = 0; i < obstacles.o_count; i++)
    {
        Shill_Obstacle_LinSqrt(ObstacleVelocity, Phase, &obstacles.o[i],
                               V_Shill, R_0_Shill, Acc_Shill, Slope_Shill,
                               WhichAgent, CollisionsObst, AgentInObst, i);
    }

    if (ActualTime < 20)
    {
        *CollisionsObst = 0;
    }

    // Self propulsion term
    // VectSum(OutputVelocity, OutputVelocity, NormalizedAgentsVelocity);
    // VectSum(OutputVelocity, OutputVelocity, test2);

    if (Flocking_type == 0)
    {
        OutputInnerState[3] = PressureVelocity[0];
        OutputInnerState[4] = PressureVelocity[1];
    }
    else if (Flocking_type == 1)
    {
        OutputInnerState[3] = PotentialVelocity[0];
        OutputInnerState[4] = PotentialVelocity[1];
    }
    OutputInnerState[5] = AttractionVelocity[0];
    OutputInnerState[6] = AttractionVelocity[1];

    OutputInnerState[7] = SlipVelocity[0];
    OutputInnerState[8] = SlipVelocity[1];

    OutputInnerState[9] = ObstacleVelocity[0];
    OutputInnerState[10] = ObstacleVelocity[1];

    OutputInnerState[11] = NormalizedAgentsVelocity[0];
    OutputInnerState[12] = NormalizedAgentsVelocity[1];

    OutputInnerState[13] = AdjacencyPotentialVelocity[0];
    OutputInnerState[14] = AdjacencyPotentialVelocity[1];

    OutputInnerState[15] = GlobalConnectivityVelocity[0];
    OutputInnerState[16] = GlobalConnectivityVelocity[1];

    // if (Phase->RealIDs[WhichAgent] == 0) {
    // printf("Agent %d\nPotential = %f \nAttraction = %f\nAlignement = %f\nObstacles = %f\n\n",Phase->RealIDs[WhichAgent], VectAbs(AdjacencyPotentialVelocity),
    //     VectAbs(AttractionVelocity), VectAbs(SlipVelocity), VectAbs(ObstacleVelocity));
    // }

    if (Flocking_type == 0)
    {

        if (WhichTarget == 0 || Phase->RealIDs[WhichAgent] != 0)
        {
            // VectSum(OutputVelocity, OutputVelocity, PotentialVelocity);
            VectSum(OutputVelocity, OutputVelocity, PressureVelocity);
            VectSum(OutputVelocity, OutputVelocity, AttractionVelocity);
            VectSum(OutputVelocity, OutputVelocity, SlipVelocity);
            // VectSum(OutputVelocity, OutputVelocity, TargetTrackingVelocity);
            VectSum(OutputVelocity, OutputVelocity, LeaderFollower);
        }

        else
        {
            NullVect(OutputVelocity, (int)Dim);
            VectSum(OutputVelocity, OutputVelocity, NormalizedAgentsVelocity);
            VectSum(OutputVelocity, OutputVelocity, PressureVelocity);
            // VectSum(OutputVelocity, OutputVelocity, AttractionVelocity);
            // VectSum(OutputVelocity, OutputVelocity, SlipVelocity);
            VectSum(OutputVelocity, OutputVelocity, TargetTrackingVelocity);
            // MultiplicateWithScalar(OutputVelocity, OutputVelocity, .5, (int)Dim);
        }
    }

    else if (Flocking_type == 1)
    {

        if (WhichTarget == 0 || Phase->RealIDs[WhichAgent] != 0)
        {
            VectSum(OutputVelocity, OutputVelocity, PotentialVelocity);
            VectSum(OutputVelocity, OutputVelocity, AttractionVelocity);
            VectSum(OutputVelocity, OutputVelocity, SlipVelocity);
            VectSum(OutputVelocity, OutputVelocity, LeaderFollower);
            // VectSum(OutputVelocity, OutputVelocity, TargetTrackingVelocity);
        }

        else
        {
            NullVect(OutputVelocity, (int)Dim);
            VectSum(OutputVelocity, OutputVelocity, NormalizedAgentsVelocity);
            VectSum(OutputVelocity, OutputVelocity, PotentialVelocity);
            VectSum(OutputVelocity, OutputVelocity, TargetTrackingVelocity);
        }

        // VectSum(OutputVelocity, OutputVelocity, GradientAcceleration);
        // VectSum(OutputVelocity, OutputVelocity, AlignOlfati);
        // MultiplicateWithScalar(OutputVelocity, OutputVelocity, 0.01, (int)Dim);
    }

    else if (Flocking_type == 2)
    {

        if (WhichTarget == 0 || Phase->RealIDs[WhichAgent] != 0)
        {
            VectSum(OutputVelocity, OutputVelocity, GradientAcceleration);
            VectSum(OutputVelocity, OutputVelocity, AlignOlfati);
            MultiplicateWithScalar(OutputVelocity, OutputVelocity, 0.01, (int)Dim);
            VectSum(OutputVelocity, OutputVelocity, LeaderFollower);
        }
        else
        {
            NullVect(OutputVelocity, (int)Dim);
            VectSum(OutputVelocity, OutputVelocity, NormalizedAgentsVelocity);
            VectSum(OutputVelocity, OutputVelocity, GradientAcceleration);
            MultiplicateWithScalar(OutputVelocity, OutputVelocity, 0.01, (int)Dim);
            VectSum(OutputVelocity, OutputVelocity, TargetTrackingVelocity);
        }
    }

    else if (Flocking_type == 3)
    {
        VectSum(OutputVelocity, OutputVelocity, PotentialVelocity);
        VectSum(OutputVelocity, OutputVelocity, TargetTrackingVelocity);
        VectSum(OutputVelocity, OutputVelocity, SlipVelocity);

        if (WhichTarget != 0)
        {
            static double orig;
            VectDifference(DifferenceVector, AgentsCoordinates, TargetsArray[ActualTargetID]);
            if (VectAbs(DifferenceVector) < 10000)
            {
                Phase->InnerStates[WhichAgent][0] = 1;
                // double orig = ActualTime;
                // Phase->InnerStates[WhichAgent][1] = orig;
                // orig++;
            }
            else
            {
                // Phase->InnerStates[WhichAgent][0] = 0;
                Phase->InnerStates[WhichAgent][1] = 0;
                // orig = 0;
            }

            int cnt = 0;
            for (i = 0; i < Phase->NumberOfAgents; i++)
            {
                if (Phase->InnerStates[WhichAgent][0] == 1)
                {
                    cnt += 1;
                }
            }
            if (cnt >= 4)
            {
                R_0 = FlockingParams->Params[4].Value = 15000; // Equilibirum distance increased for spatial coverage
                orig++;
                if (orig > 1000)
                {
                    ActualTargetID++;
                    R_0 = FlockingParams->Params[4].Value = 4000;
                    orig = 0;
                }
            }
        }
    }

    else if (Flocking_type == 4)
    {
        if (Phase->NumberOfAgents < 3)
        {
            NullVect(OutputVelocity, (int)Dim);
        }
        VectSum(OutputVelocity, OutputVelocity, PressureVelocity);
        VectSum(OutputVelocity, OutputVelocity, AttractionVelocity);
        VectSum(OutputVelocity, OutputVelocity, SlipVelocity);
        VectSum(OutputVelocity, OutputVelocity, LeaderFollower);
    }
    else if (Flocking_type == 6)
    {
        // VectSum(OutputVelocity, OutputVelocity, SlipVelocity);
        VectSum(OutputVelocity, OutputVelocity, AdjacencyPotentialVelocity);
        VectSum(OutputVelocity, OutputVelocity, GlobalConnectivityVelocity);
        VectSum(OutputVelocity, OutputVelocity, TargetTrackingVelocity);

        // printf("slip: %f, %f, %f\n", SlipVelocity[0], SlipVelocity[1], SlipVelocity[2]);
        // printf("potential: %f, %f, %f\n", AdjacencyPotentialVelocity[0], AdjacencyPotentialVelocity[1], AdjacencyPotentialVelocity[2]);
        // printf("connectivity: %f, %f, %f\n", GlobalConnectivityVelocity[0], GlobalConnectivityVelocity[1], GlobalConnectivityVelocity[2]);
        // printf("slip: %f\npotential: %f\n", VectAbs(SlipVelocity), VectAbs(AdjacencyPotentialVelocity));
    }

    VectSum(OutputVelocity, OutputVelocity, ArenaVelocity);
    VectSum(OutputVelocity, OutputVelocity, ObstacleVelocity);

    // printf("OutputVelocity (%d) : (%f %f %f)\n", Phase->RealIDs[WhichAgent], OutputVelocity[0], OutputVelocity[1], OutputVelocity[2]);

    /* V_pref saturates at V_Max */
    static bool CutOffMode = false;
    // if (VectAbs(AttractionVelocity) > VectAbs(NormalizedAgentsVelocity)) {
    //     CutOffMode = true;
    //     printf("Over Ride\n");
    // }
    if (false == CutOffMode)
    {
        UnitVect(OutputVelocity, OutputVelocity);
        MultiplicateWithScalar(OutputVelocity, OutputVelocity, V_Flock,
                               (int)Dim);
    }
    else
    {
        if (VectAbs(OutputVelocity) > V_Max)
        {
            UnitVect(OutputVelocity, OutputVelocity);
            MultiplicateWithScalar(OutputVelocity, OutputVelocity, V_Max,
                                   (int)Dim);
        }
    }

    // reset third dim for sure at the end
    if (2 == Dim)
    {
        OutputVelocity[2] = 0;
    }
}

void DestroyPhase(phase_t *Phase, flocking_model_params_t *FlockingParams,
                  sit_parameters_t *SitParams)
{
}
